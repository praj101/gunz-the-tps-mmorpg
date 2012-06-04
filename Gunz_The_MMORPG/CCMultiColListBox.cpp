#include "stdafx.h"
#include "CCMultiColListBox.h"
#include "CCColorTable.h"
#include "Core.h"

#define MAX_WHEEL_RANGE	2

IMPLEMENT_LOOK(CCMultiColListBox, CCMultiColListBoxLook)

CCMultiColListBox::CCMultiColListBox(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCWidget(szName, pParent, pListener)
, m_numColumn(1)
, m_desiredNumRow(4)
, m_itemHeight(16)
, m_maxRowCanShow(0)
, m_iSelItem(-1)
, m_iOverItem(-1)
, m_bDragAndDrop(false){
	LOOK_IN_CONSTRUCTOR();

	m_pScrollBar = new CCScrollBar(this, this);

	m_FontAlign = CCD_NOTALIGN;
	m_pOnDropFunc = NULL;
	
	SetFocusEnable(true);
}

CCMultiColListBox::~CCMultiColListBox(){
	RemoveAll();
	delete m_pScrollBar;
}

void CCMultiColListBox::SetDesiredNumRow(int n){
	m_desiredNumRow = n;

	CalcItemHeight();
}

void CCMultiColListBox::CalcItemHeight(){
	m_itemHeight = GetInitialClientRect().h / m_desiredNumRow;
}

void CCMultiColListBox::SetNumColumn(int n){
	m_numColumn = n;
	if (m_numColumn < 1)
	{
		_ASSERT(0);
		m_numColumn = 1;
	}

	UpdateScrollBar();
}

void CCMultiColListBox::SetItemHeight(int h){
	m_itemHeight = h;
	if (m_itemHeight < 1)
	{
		_ASSERT(0);	
		m_itemHeight = 1;
	}

	UpdateScrollBar();
}

void CCMultiColListBox::InsertSortedPos(CCMultiColListItem* pItem){
	int nSortHint = pItem->GetSortHint();
	for (ListMultiColListItem::iterator it=m_items.begin(); it!=m_items.end(); ++it)
	{
		if ((*it)->GetSortHint() <= nSortHint)
			continue;

		m_items.insert(it, pItem);
		return;
	}

	m_items.push_back(pItem);
}

void CCMultiColListBox::Add(CCMultiColListItem* pItem){
	InsertSortedPos(pItem);

	if (m_iSelItem == -1)
		m_iSelItem = 0;

	UpdateScrollBar();
}

void CCMultiColListBox::Remove(CCMultiColListItem* pItem){
	for (ItorMultiColListItem it=m_items.begin(); it!=m_items.end(); ++it)
	{
		if ((*it) == pItem)
		{
			m_items.erase(it);
			if (m_iSelItem >= GetNumItem())
				m_iSelItem = GetNumItem()-1;
			UpdateScrollBar();
			break;
		}
	}
	return;
}

void CCMultiColListBox::RemoveAll(){
	for (ItorMultiColListItem it=m_items.begin(); it!=m_items.end(); ++it)
		delete *it;
	m_items.clear();

	m_pScrollBar->SetValue(0);
	UpdateScrollBar();
	
	m_iSelItem = -1;
	m_iOverItem = -1;
}

void CCMultiColListBox::UpdateScrollBar(){
	sRect& r = GetClientRect();

	m_maxRowCanShow = r.h / m_itemHeight;
	
	int numRow = ((int)m_items.size() / m_numColumn);
	if (((int)m_items.size() % m_numColumn) > 0)
		numRow += 1;
	
	int scrollmax = numRow - m_maxRowCanShow;
	if (scrollmax < 0)
		scrollmax = 0;

	m_pScrollBar->SetMinMax(0, scrollmax);

	if (r.h % m_itemHeight > 0)
		m_maxRowCanShow += 1;
}

void CCMultiColListBox::OnSize(int w, int h){
	sRect cr = GetInitialClientRect();
	m_pScrollBar->SetBounds(sRect(cr.x+cr.w-m_pScrollBar->GetDefaultBreadth(), cr.y+1, m_pScrollBar->GetDefaultBreadth(), cr.h-1));

	CalcItemHeight();

	UpdateScrollBar();
}

CCMultiColListItem* CCMultiColListBox::GetItemByIdx(int idx){
	if (idx < 0) return NULL;
	if (idx >= GetNumItem()) return NULL;
	int i=0;
	for (ItorMultiColListItem it=m_items.begin(); it!=m_items.end(); ++it, ++i)
	{
		if (i == idx)
			return *it;
	}
	return NULL;
}

CCMultiColListItem* CCMultiColListBox::GetSelItem(){
	return GetItemByIdx(m_iSelItem);
}

bool CCMultiColListBox::SetSelIndex(int i){
	if (i < 0 || GetNumItem() <= i) return false;

	m_iSelItem = i;
	return true;
}

const char* CCMultiColListBox::GetSelItemString(){
	CCMultiColListItem* pItem = GetItemByIdx(m_iSelItem);
	if (pItem)
		return pItem->GetString();
	return NULL;
}

int CCMultiColListBox::FindItem(sPoint& p){
	if (!GetClientRect().InPoint(p)) return -1;

	int idxItemFirstVisible = GetItemFirstVisible();
	int idxItemLastVisible = GetItemLastVisible();

	sRect r;
	for (int idx=idxItemFirstVisible; idx<=idxItemLastVisible; ++idx)
	{
		CalcItemRect(idx, r);
		if (r.InPoint(p))
			return idx;
	}
	return -1;
}

int CCMultiColListBox::FindNextItem(int i, char c){
	for(int s=0; s<GetNumItem()-1; s++){
		int idx = (i+s+1)%GetNumItem();
		const char* szItem = GetString(idx);

		if (szItem != NULL)
		{
			char a = (char)towupper(szItem[0]);
			char b = (char)towupper(c);
			if(a==b) return idx;
		}
	}
	return -1;
}


bool CCMultiColListBox::CalcItemRect(int idx, sRect& out){
	if (idx < 0 || GetNumItem() <= idx) return false;

	int idxItemFirstShow = GetItemFirstVisible();
	int idxItemLastShow = GetItemLastVisible();

	if (idx < idxItemFirstShow || idxItemLastShow < idx) return false;

	sRect r = GetClientRect();
	int widthItem = r.w / m_numColumn;
	int heightItem = GetItemHeight();

	int col = idx % m_numColumn;
	int row = idx / m_numColumn;

	int xposItem = r.x + (widthItem * col);
	int yposItem = r.y + (heightItem * (row-GetRowFirstVisible()) );

	out.Set(xposItem+2, yposItem+2, widthItem-2, heightItem-2);
	if (out.y >= r.y + r.h) return false;

	return true;
}

bool CCMultiColListBox::GetItemRowCol(int idx, int& out_row, int& out_col){
	if (m_numColumn == 0) return false;
	out_col = (idx % m_numColumn);
	out_row = (idx / m_numColumn);
	return true;
}

bool CCMultiColListBox::OnEvent(CCEvent* pEvent, CCListener* pListener){
	sRect r = GetClientRect();
	if(pEvent->iMessage==CCWM_MOUSEMOVE){
		if(r.InPoint(pEvent->sPos)==false) return false;
		m_iOverItem = FindItem(pEvent->sPos);
	}
	else if(pEvent->iMessage==CCWM_LBUTTONDOWN){
		if(r.InPoint(pEvent->sPos)==false)
		{
			if (pListener)
				pListener->OnCommand(this,CCLB_ITEM_CLICKOUT);
			return false;
		}

		if(m_iDebugType==2){
			int k =0;
		}

		int nSelItem = FindItem(pEvent->sPos);
		if(nSelItem==-1) return true;
		SetSelIndex(nSelItem);

		if(pListener!=NULL) pListener->OnCommand(this, CCLB_ITEM_SEL);

		if ( m_bDragAndDrop)
		{
			CCMultiColListItem* pItem = GetSelItem();
			if(pItem!=NULL)
			{
				CCBitmap* pDragBitmap = NULL;
				char szDragString[256] = "";
				char szDragItemString[256] = "";
				if(pItem->GetDragItem(&pDragBitmap, szDragString, szDragItemString)==true){
					Core::GetInstance()->SetDragObject(this, pDragBitmap, szDragString, szDragItemString);
				}
			}
		}

		return true;
	}
	else if(pEvent->iMessage==CCWM_RBUTTONDOWN){
		if(r.InPoint(pEvent->sPos)==false)
		{
			pListener->OnCommand(this,CCLB_ITEM_CLICKOUT);
			return false;
		}
		int nSelItem = FindItem(pEvent->sPos);
		if(nSelItem==-1) return true;
		SetSelIndex(nSelItem);

		if(m_iSelItem!=-1){
			if(pListener!=NULL) pListener->OnCommand(this, CCLB_ITEM_SEL2);
			return true;
		}
	}
	else if(pEvent->iMessage==CCWM_KEYDOWN){
		if(pEvent->uKey==VK_DELETE){
			if(pListener!=NULL) pListener->OnCommand(this, CCLB_ITEM_DEL);
		}
		else if(pEvent->uKey==VK_UP){
			if(GetSelIndex()>0){
				SetSelIndex(GetSelIndex()-1);
				ShowItem(GetSelIndex());
				if(pListener!=NULL){
					if(m_iSelItem!=-1) pListener->OnCommand(this, CCLB_ITEM_SEL);
					else pListener->OnCommand(this, CCLB_ITEM_SELLOST);
				}
			}
		}
		else if(pEvent->uKey==VK_DOWN){
			if(GetSelIndex()<GetNumItem()-1){
				SetSelIndex(GetSelIndex()+1);
				ShowItem(GetSelIndex());
				if(pListener!=NULL){
					if(m_iSelItem!=-1) pListener->OnCommand(this, CCLB_ITEM_SEL);
					else pListener->OnCommand(this, CCLB_ITEM_SELLOST);
				}
			}
		}
	}
	else if(pEvent->iMessage==CCWM_CHAR){
		int nIndex = FindNextItem(GetSelIndex(), pEvent->uKey);
		if(nIndex>=0){
			SetSelIndex(nIndex);
			ShowItem(nIndex);
			if(pListener!=NULL){
				if(m_iSelItem!=-1) pListener->OnCommand(this, CCLB_ITEM_SEL);
				else pListener->OnCommand(this, CCLB_ITEM_SELLOST);
			}
		}
	}
	else if(pEvent->iMessage==CCWM_LBUTTONDBLCLK){
		if(r.InPoint(pEvent->sPos)==false) return false;
		int nSelItem = FindItem(pEvent->sPos);
		if(nSelItem==-1) return true;
		m_iSelItem = nSelItem;
		if(pListener!=NULL){
			if(m_iSelItem!=-1) pListener->OnCommand(this, CCLB_ITEM_SEL);
			else pListener->OnCommand(this, CCLB_ITEM_SELLOST);
			pListener->OnCommand(this, CCLB_ITEM_DBLCLK);
		}
		return true;
	}
	else if(pEvent->iMessage==CCWM_MOUSEWHEEL){
		if(r.InPoint(pEvent->sPos)==false) 
			return false;

		int newPos = m_pScrollBar->GetValue() + min(max(-pEvent->iDelta, -MAX_WHEEL_RANGE), MAX_WHEEL_RANGE);
		m_pScrollBar->SetValueAdjusted(newPos);
		return true;
	}
	return false;
}

void CCMultiColListBox::MultiplySize( float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight ){
	CCWidget::MultiplySize( byIDLWidth, byIDLHeight, byCurrWidth, byCurrHeight );
}

CCMultiColListItem* CCMultiColListBox::Get( int i ){
	if (i < 0) return NULL;
	if ((int)m_items.size() <= i) return NULL;

	ItorMultiColListItem it=m_items.begin();
	for (int k=0; k<i; ++k)
		++it;
	return *it;

	return NULL;
}

const char* CCMultiColListBox::GetString( int i ){
	CCMultiColListItem* pItem = Get(i);
	if (pItem)
		return pItem->GetString();
	return NULL;
}

void CCMultiColListBox::ShowItem( int i ){
	int row, col;
	if (!GetItemRowCol(i, row, col)) return;

	int rowFirstVisible = GetRowFirstVisible();
	int rowLastVisible = GetRowLastVisible();

	if (rowFirstVisible <= row && row <= rowLastVisible) return;

	m_pScrollBar->SetValueAdjusted(row);
}

bool CCMultiColListBox::OnDrop(CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString){
	if ( m_pOnDropFunc != NULL)
	{
		m_pOnDropFunc(this, pSender, pBitmap, szString, szItemString);
	}

	return true;
}

void CCMultiColListBoxLook::OnDraw(CCMultiColListBox* pListBox, CCDrawContext* pDC){
	m_ItemTextAlignmentMode = pListBox->m_FontAlign;

	sRect rcClient = pListBox->GetInitialClientRect();
	pDC->SetColor(sColor(19,19,19,255));
	pDC->FillRectangle(rcClient);
	pDC->SetColor(sColor(128,128,128,255));
	pDC->Rectangle(rcClient);

	int rowFirstVisible = pListBox->GetRowFirstVisible();
	int rowLastVisible = pListBox->GetRowLastVisible();

	sRect r;
	int idx=0, col=0, row=0;
	ListMultiColListItem& items = pListBox->GetItemList();

	for (ItorMultiColListItem itor=items.begin(); itor!=items.end(); ++itor, ++idx)
	{
		if (false == pListBox->GetItemRowCol(idx, row, col)) continue;

		if (row < rowFirstVisible) continue;
		if (row > rowLastVisible) break;

		if (pListBox->CalcItemRect(idx, r))
			(*itor)->OnDraw(r, pDC, idx==pListBox->GetSelIndex(), idx==pListBox->GetMouseOverIndex());
	}
}

sRect CCMultiColListBoxLook::GetClientRect(CCMultiColListBox* pListBox, sRect& r){
	return sRect(r.x+1, r.y+1, (r.w - pListBox->GetScrollBar()->GetClientRect().w-2), r.h-2);
}

CCMultiColListBoxLook::CCMultiColListBoxLook(){
	m_ItemTextAlignmentMode	= CCD_NOTALIGN;
}

void CCMultiColListItem::OnDraw(sRect& r, CCDrawContext* pDC, bool bSelected, bool bMouseOver){
	pDC->SetColor(sColor(DEFCOLOR_CCLIST_TEXT));
	pDC->Rectangle(r);
	char sz[32];
	sprintf(sz, "%d", (DWORD)this);
	pDC->Text(r, sz, CCD_HCENTER|CCD_VCENTER);
}