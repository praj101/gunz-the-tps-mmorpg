#include "stdafx.h"
#include "CCListBox.h"
#include "CCColorTable.h"
#include "Core.h"

#define CCLISTBOX_MARGIN_X	2
#define CCLISTBOX_MARGIN_Y	2

#define CCLISTBOX_DEFAULT_WIDTH	100
#define CCLISTBOX_DEFAULT_HEIGHT	100
#define CCLISTBOX_ITEM_MARGIN_Y	2
#define CCLISTBOX_ITEM_MARGIN_X	2

#define COLORTEXT_SUPPORT


IMPLEMENT_LOOK(CCListBox, CCListBoxLook)

int CCListBox::GetItemHeight(void)
{
	if(m_iItemHeight>0) return m_iItemHeight;
	return GetFont()->GetHeight()+CCLISTBOX_ITEM_MARGIN_Y;
}

void CCListBox::SetItemHeight(int nHeight)
{
	m_iItemHeight = nHeight;
}

int CCListBox::FindItem(sPoint& p)
{
	int nItemHeight = GetItemHeight();

	int nHeaderHeight = 0;
	if(IsVisibleHeader()==true) nHeaderHeight = nItemHeight;

	sRect r = GetClientRect();
	if (m_ViewStyle == CCVS_LIST)
	{
		for(int i=0; i<GetCount()-m_iStartItemPos; i++){
			if(!IsShowItem(i+m_iStartItemPos)) break;
			if(sRect(r.x, r.y+nHeaderHeight+nItemHeight*i, r.w, nItemHeight).InPoint(p)==true){
				return (i+m_iStartItemPos);
			}
		}
	}
	else if (m_ViewStyle == CCVS_ICON)
	{
		int nColCount = r.w / GetTabSize();
		int nColIndex, nRowIndex;
		nColIndex = p.x / GetTabSize();
		nRowIndex = p.y / GetItemHeight();
		int nItemIndex = m_iStartItemPos + (nRowIndex * nColCount) + nColIndex;
		if (nItemIndex < GetCount()) return nItemIndex;
	}
	return -1;
}

bool CCListBox::GetItemPos(sPoint* p, int i)
{
	int nFontHeight = GetItemHeight();

	int nHeaderHeight = 0;
	if(IsVisibleHeader()==true) nHeaderHeight = nFontHeight;

	sRect r = GetClientRect();
	p->x = r.x;
	p->y = r.y + nHeaderHeight + (i-m_iStartItemPos) * nFontHeight;
	/*
	p->x = m_Rect.x + CCLISTBOX_MARGIN_X;
	p->y = m_Rect.y + (i-m_iStartItemPos) * nFontHeight;
	*/

	if(i>=m_iStartItemPos && i<m_iStartItemPos+m_iShowItemCount) return true;
	return false;
}

bool CCListBox::OnEvent(CCEvent* pEvent, CCListener* pListener)
{
 	sRect r = GetClientRect();
	if(pEvent->iMessage==CCWM_MOUSEMOVE){
		if(r.InPoint(pEvent->sPos)==false) return false;
		m_iOverItem = FindItem(pEvent->sPos);
	}
	else if(pEvent->iMessage==CCWM_LBUTTONDOWN){
		if(r.InPoint(pEvent->sPos)==false)
		{
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

		// 드래그 & 드롭
		if ( m_bDragAndDrop)
		{
			CCListItem* pItem = GetSelItem();
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
			if(GetSelIndex()<GetCount()-1){
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
		if(r.InPoint(pEvent->sPos)==false) return false;
#define MAX_WHEEL_RANGE	4
		if (m_ViewStyle == CCVS_LIST)
			SetStartItem(m_iStartItemPos+min(max(-pEvent->iDelta, -MAX_WHEEL_RANGE), MAX_WHEEL_RANGE));
		else if (m_ViewStyle == CCVS_ICON)
		{
			int nTabSize = GetTabSize();
			int nColCount = r.w / nTabSize;
			int t = (m_iStartItemPos+min(max(-pEvent->iDelta, -MAX_WHEEL_RANGE), MAX_WHEEL_RANGE)) * nColCount;
			SetStartItem((m_iStartItemPos+min(max(-pEvent->iDelta, -MAX_WHEEL_RANGE), MAX_WHEEL_RANGE)) * nColCount);
		}
		return true;
	}
	return false;
}

void CCListBox::RecalcList(void)
{
	int nItemHeight = GetItemHeight();
	sRect r = GetClientRect();

	if (m_ViewStyle == CCVS_LIST)
	{
		int nHeaderHeight = 0;
		if(IsVisibleHeader()==true) nHeaderHeight = nItemHeight;

		m_iShowItemCount = (r.h-nHeaderHeight) / nItemHeight;
	}
	else if (m_ViewStyle == CCVS_ICON)
	{
		int nTabSize = GetTabSize();
		m_iShowItemCount = (r.w / nTabSize) * (r.h / nItemHeight);
	}

	RecalcScrollBar();
}

void CCListBox::RecalcScrollBar(void)
{
	if(m_iShowItemCount<GetCount())
	{
		m_pScrollBar->SetMinMax(0, GetCount()-m_iShowItemCount);
		
		if( !m_bHideScrollBar ) {
			m_pScrollBar->Enable(true);
			m_pScrollBar->Show(true);
		}
		
		if(m_bAlwaysVisibleScrollbar)
			m_pScrollBar->Enable(true);
	}
	else{
		m_pScrollBar->SetMinMax(0, 0);

		if(m_bAlwaysVisibleScrollbar)
			m_pScrollBar->Enable(false);
		else
			m_pScrollBar->Show(false);
	}
}

int CCListBox::FindNextItem(int i, char c)
{
	for(int s=0; s<GetCount()-1; s++){
		int idx = (i+s+1)%GetCount();
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

void CCListBox::OnSize(int w, int h)
{
	sRect cr = GetInitialClientRect();
	sRect rScrollbar = m_pScrollBar->GetRect();
	if(m_pScrollBar->IsVisible()==true)
	{
		//m_pScrollBar->SetBounds(sRect(cr.x+cr.w-m_pScrollBar->GetDefaultBreadth()-5, cr.y+3, m_pScrollBar->GetDefaultBreadth(), cr.h-6));
		//m_pScrollBar->SetBounds(sRect(cr.x+cr.w-m_pScrollBar->GetDefaultBreadth(), cr.y+1, m_pScrollBar->GetDefaultBreadth(), cr.h-1));
		m_pScrollBar->SetBounds(sRect(cr.x+cr.w-rScrollbar.w, cr.y+1, rScrollbar.w, cr.h-1));
	}
	else	// 안보이는 경우 클라이언트 영역이 스크롤바 영역까지 있으므로, 감안해서 계산
	{
		//m_pScrollBar->SetBounds(sRect(cr.x+cr.w-m_pScrollBar->GetDefaultBreadth()-5, cr.y+3, m_pScrollBar->GetDefaultBreadth(), cr.h-6));
		m_pScrollBar->SetBounds(sRect(cr.x+cr.w-rScrollbar.w, cr.y+1, rScrollbar.w, cr.h-1));
	}

	RecalcList();
}

void CCListBox::Initialize(void)
{
	LOOK_IN_CONSTRUCTOR()

	m_iOverItem = -1;
	m_iSelItem = -1;
	m_iStartItemPos = 0;
	m_iShowItemCount = 0;
	m_iItemHeight = -1;
	m_bSelected = true;
	m_pScrollBar = new CCScrollBar(this, this);
	m_ViewStyle = CCVS_LIST;
	m_bDragAndDrop = false;
	m_pOnDropFunc = NULL;

	SetSize(CCLISTBOX_DEFAULT_WIDTH, CCLISTBOX_DEFAULT_HEIGHT);

	SetFocusEnable(true);

	m_bVisibleHeader = true;

	m_bAbsoulteTabSpacing = true;
	m_bAlwaysVisibleScrollbar = false;
	m_bIsListBox = true;
	m_bHideScrollBar = false;
	m_FontColor = sColor(DEFCOLOR_CCLIST_TEXT);
	m_FontAlign = CCD_NOTALIGN;

	m_bNullFrame = false;
	m_bMultiSelect = false;
}


CCListBox::CCListBox(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCWidget(szName, pParent, pListener)
{
	Initialize();
}

CCListBox::CCListBox(CCWidget* pParent, CCListener* pListener)
: CCWidget("CCListBox", pParent, pListener)
{
	Initialize();
}

CCListBox::~CCListBox(void)
{
//	while(m_Items.GetCount()>0) m_Items.Delete(0);
	m_Items.DeleteAll();
	if(m_pScrollBar!=NULL) delete m_pScrollBar;
}

void CCListBox::Add(const char* szItem)
{
	CCListItem* pItem = new CCDefaultListItem(szItem);
	m_Items.Add(pItem);
	/*
	char* pNewItem = new char[strlen(szItem)+1];
	strcpy(pNewItem, szItem);
	m_Items.Add(pNewItem);
	*/
	if(m_Items.GetCount()) {
		if(m_iSelItem == -1)
			m_iSelItem = 0;
//			SetSelIndex(0);
	}

	RecalcList();
	RecalcScrollBar();
}

void CCListBox::Add(const char* szItem, sColor color)
{
	CCListItem* pItem = new CCDefaultListItem(szItem, color);

	m_Items.Add(pItem);

	RecalcList();
	RecalcScrollBar();
}

void CCListBox::Add(CCListItem* pItem)
{
	m_Items.Add(pItem);
	RecalcList();
	RecalcScrollBar();
}

const char* CCListBox::GetString(int i)
{
	return m_Items.Get(i)->GetString();
}

CCListItem* CCListBox::Get(int i)
{
	return m_Items.Get(i);
}

bool CCListBox::Set(int i, const char* szItem)
{
	if(m_Items.GetCount()<=i) return false;
	CCListItem* pItem = m_Items.Get(i);
	pItem->SetString(szItem);
	return true;
}

bool CCListBox::Set(int i, CCListItem* pItem)
{
	if(m_Items.GetCount()<=i) return false;
	m_Items.MoveRecord(i);
	m_Items.Insert(pItem);
	m_Items.Delete(i);
	return true;
}

void CCListBox::Remove(int i)
{
	if(i<0 || i>=m_Items.GetCount()) return;
	m_Items.Delete(i);
	if(i==m_iSelItem) m_iSelItem = -1;

	RecalcScrollBar();
}

void CCListBox::RemoveAll(void)
{
	while(m_Items.GetCount()>0) Remove(0);

	m_iOverItem = -1;
	m_iSelItem = -1;
	m_iStartItemPos = 0;
	m_pScrollBar->SetValue(0);
	RecalcScrollBar();
}

bool CCListBox::Swap(int i, int j)
{
	if(i<0 || j<0 || i>=m_Items.GetCount() || j>=m_Items.GetCount()) return false;
	m_Items.Swap(i, j);
	return true;
}

int CCListBox::GetCount(void)
{
	return m_Items.GetCount();
}

int CCListBox::GetSelIndex(void)
{
	return m_iSelItem;
}

const char* CCListBox::GetSelItemString(void)
{
	if(m_iSelItem==-1) return NULL;
	if(m_iSelItem>=m_Items.GetCount()) return NULL;
	return m_Items.Get(m_iSelItem)->GetString();
}

CCListItem* CCListBox::GetSelItem(void)
{
	if(m_iSelItem==-1) return NULL;
	return m_Items.Get(m_iSelItem);
}

bool CCListBox::SetSelIndex(int i)
{
	if ((i >= m_Items.GetCount()) || (i<0))
		return false;

	if(!m_bMultiSelect) {
		if(0<=m_iSelItem && m_iSelItem<m_Items.GetCount())
			Get(m_iSelItem)->m_bSelected=false;
	}

	m_iSelItem = i;

	if(!m_bMultiSelect) {
		Get(m_iSelItem)->m_bSelected=true;
	}else
		Get(m_iSelItem)->m_bSelected=!Get(m_iSelItem)->m_bSelected;

	return true;
}

bool CCListBox::IsShowItem(int i)
{
	if(i>=m_iStartItemPos && i<m_iStartItemPos+m_iShowItemCount) return true;
	return false;
}

void CCListBox::ShowItem(int i)
{
	if(i<m_iStartItemPos){
		m_iStartItemPos = i;
		m_pScrollBar->SetValue(m_iStartItemPos);
	}
	else if(i>=m_iStartItemPos+m_iShowItemCount){
		m_iStartItemPos = i - m_iShowItemCount + 1;
		if(m_iStartItemPos+m_iShowItemCount>GetCount()) m_iStartItemPos = GetCount()-m_iShowItemCount;
		m_pScrollBar->SetValue(m_iStartItemPos);
	}
}

void CCListBox::SetStartItem(int i)
{
	if(GetCount()<=m_iShowItemCount) return;	// 이미 다 보이면 리턴

	if(i<0) i = 0;
	else if(i+m_iShowItemCount>GetCount()) i = GetCount()-m_iShowItemCount;
	m_iStartItemPos = i;
	m_pScrollBar->SetValue(m_iStartItemPos);
	GetListener()->OnCommand(this, CCLB_ITEM_START);	
}

int CCListBox::GetStartItem(void)
{
	return m_iStartItemPos;
}

int CCListBox::GetShowItemCount(void)
{
	return m_iShowItemCount;
}

CCScrollBar* CCListBox::GetScrollBar(void)
{
	return m_pScrollBar;
}

void CCListBox::Sort(bool bAscend)
{
	m_Items.m_bAscend = bAscend;
	m_Items.Sort();
}

bool CCListBox::OnCommand(CCWidget* pWindow, const char* szMessage)
{
	if(pWindow==m_pScrollBar && strcmp(szMessage, CCLIST_VALUE_CHANGED)==0)
	{
		if (m_ViewStyle == CCVS_LIST)
		{
			m_iStartItemPos = m_pScrollBar->GetValue();
		}
		else if (m_ViewStyle == CCVS_ICON)
		{
			sRect r = GetClientRect();
			int nColCount = r.w / GetTabSize();
			int nValue = m_pScrollBar->GetValue() - (m_pScrollBar->GetValue() % nColCount);
			m_iStartItemPos = nValue;
		}
		return true;
	}
	return false;
}

void CCListBox::AddField(const char* szFieldName, int nTabSize)
{
	bool bVisibleHeader = IsVisibleHeader();
	CCLISTFIELD* pNew = new CCLISTFIELD;
	strcpy(pNew->szFieldName, szFieldName);
	pNew->nTabSize = nTabSize;
	m_Fields.Add(pNew);
	if(bVisibleHeader!=IsVisibleHeader()) RecalcList();	// 헤더가 보이게 되는 경우
}

void CCListBox::RemoveField(const char* szFieldName)
{
	bool bVisibleHeader = IsVisibleHeader();
	for(int i=0; i<m_Fields.GetCount(); i++){
		CCLISTFIELD* pField = m_Fields.Get(i);
		if(strcmp(pField->szFieldName, szFieldName)==0){
			m_Fields.Delete(i);
			if(bVisibleHeader!=IsVisibleHeader()) RecalcList();	// 헤더가 사라지게 되는 경우
			return;
		}
	}
}

void CCListBox::RemoveAllField(void)
{
	while(m_Fields.GetCount()){
		m_Fields.Delete(0);
	}
}

CCLISTFIELD* CCListBox::GetField(int i)
{
	return m_Fields.Get(i);
}

int CCListBox::GetFieldCount(void)
{
	return m_Fields.GetCount();
}

bool CCListBox::IsVisibleHeader(void)
{
	if(GetFieldCount()==0) return false;

	return m_bVisibleHeader;
}

void CCListBox::SetVisibleHeader(bool bVisible)
{
	m_bVisibleHeader = bVisible;
}

void CCListBox::SetViewStyle(CCListViewStyle ViewStyle)
{
	m_ViewStyle = ViewStyle;
	if (ViewStyle == CCVS_ICON) 
	{
		SetVisibleHeader(false);
	}
	RecalcList();
	RecalcScrollBar();
}

int CCListBox::GetTabSize()
{
	if (GetFieldCount() > 0)
	{
		return GetField(0)->nTabSize;
	}

	return GetItemHeight();
}

void CCListBoxLook::OnHeaderDraw(CCDrawContext* pDC, sRect& r, const char* szText)
{
	if(szText==NULL) return;
	pDC->SetColor(sColor(m_SelectedPlaneColor));
//	pDC->FillRectangle(r);			// 안이뻐서 동환이가 지워버림... 걍 비트맵 쓰져... ㅠ.ㅠ

	pDC->SetColor(sColor(m_SelectedTextColor));

	//pDC->Text(r.x,r.y, szText);
	pDC->Text( r, szText );
}

int CCListBoxLook::OnItemDraw(CCDrawContext* pDC, sRect& r, const char* szText, sColor color, bool bSelected, bool bFocus, int nAdjustWidth )
{
	int nLine = 0;

	if(szText==NULL) return nLine;

	if(bSelected==true){
		if(bFocus==true) pDC->SetColor(sColor(m_SelectedPlaneColor));
		else pDC->SetColor(sColor(m_UnfocusedSelectedPlaneColor));
		pDC->FillRectangle(r);
	}

	if(bSelected==true) pDC->SetColor(sColor(m_SelectedTextColor));
	else pDC->SetColor(color);

//	else pDC->SetColor(sColor(DEFCOLOR_CCLIST_TEXT));

	if( m_pItemSlotBitmap != NULL )
	{
		pDC->SetBitmap( m_pItemSlotBitmap );
		pDC->Draw(r.x, r.y, r.w, r.h );
	}

	sRect rtemp, rtemp2;
	rtemp2 = rtemp = pDC->GetClipRect();
	rtemp2.w -= nAdjustWidth;
	pDC->SetClipRect(rtemp2);
#ifdef COLORTEXT_SUPPORT
	if( m_ItemTextAlignmentMode == CCD_NOTALIGN )
	{
		nLine = pDC->TextMultiLine(r, szText,0, m_bItemTextMultiLine );
	}
	else
	{
		nLine = pDC->TextMultiLine2( r, szText, 0, m_bItemTextMultiLine, m_ItemTextAlignmentMode );
	}
#else
	pDC->Text(r.x, r.y+(r.h-pDC->GetFont()->GetHeight())/2, szText);
	nLine	= 1;
#endif
	pDC->SetClipRect(rtemp); 

	return nLine;
}

int CCListBoxLook::OnItemDraw(CCDrawContext* pDC, sRect& r, CCBitmap* pBitmap, bool bSelected, bool bFocus, int nAdjustWidth)
{
	int nLine = 0;

	if(pBitmap==NULL) return nLine;


	if(bSelected==true){
		if(bFocus==true) pDC->SetColor(sColor(m_SelectedPlaneColor));
		else pDC->SetColor(sColor(m_UnfocusedSelectedPlaneColor));
		pDC->FillRectangle(r);
	}

	sRect rtemp, rtemp2;
	rtemp2 = rtemp = pDC->GetClipRect();
	rtemp2.w -= nAdjustWidth;
	pDC->SetClipRect(rtemp2);

	pDC->SetBitmap(pBitmap);
	pDC->Draw(r.x, r.y, nAdjustWidth, nAdjustWidth);

	pDC->SetClipRect(rtemp);

	nLine = 1;

	return nLine;
}

void CCListBoxLook::OnFrameDraw(CCListBox* pListBox, CCDrawContext* pDC)
{
	sRect r = pListBox->GetInitialClientRect();
	pDC->SetColor(sColor(DEFCOLOR_CCLIST_PLANE));
	pDC->FillRectangle(r);
	pDC->SetColor(sColor(DEFCOLOR_CCLIST_OUTLINE));
	pDC->Rectangle(r);
}

void CCListBoxLook::OnDraw(CCListBox* pListBox, CCDrawContext* pDC)
{
	int nLine = 0;

	if(pListBox->m_iDebugType==2){
		int k =0;
	}
	if(!pListBox->m_bNullFrame)
		OnFrameDraw(pListBox, pDC);

	int nItemHeight = pListBox->GetItemHeight();
	int nShowCount = 0;

	sRect r = pListBox->GetClientRect();

	int nHeaderHeight = 0;

	pDC->SetFont( pListBox->GetFont() );	
	m_ItemTextAlignmentMode = pListBox->m_FontAlign;

//	pDC->BeginFont();

	if(pListBox->IsVisibleHeader()==true){
		int nFieldStartX = 0;
		for(int i=0; i<pListBox->GetFieldCount(); i++){
			CCLISTFIELD* pField = pListBox->GetField(i);
			int nWidth = min(pField->nTabSize, r.w-nFieldStartX);
			if(pListBox->m_bAbsoulteTabSpacing==false) nWidth = r.w*pField->nTabSize/100;
			sRect ir(r.x+nFieldStartX, r.y, nWidth, nItemHeight);
			OnHeaderDraw(pDC, ir, pField->szFieldName);
			nFieldStartX += (nWidth+1);
			if(nFieldStartX>=r.w) break;
		}
		nHeaderHeight = nItemHeight;
	}

	if (pListBox->GetViewStyle() == CCVS_LIST)
	{
		for(int i=pListBox->GetStartItem(); i<pListBox->GetCount(); i++){
			sPoint p;
			p.x = r.x;
			p.y = r.y+nHeaderHeight+nItemHeight*nShowCount;

			CCListItem* pItem = pListBox->Get(i);
//			bool bSelected = (pListBox->IsSelected()) ? (pListBox->GetSelIndex()==i) : false;
			bool bSelected = pItem->m_bSelected;
			bool bFocused = (pListBox->IsFocus());

			int nFieldStartX = 0;
			for(int j=0; j<max(pListBox->GetFieldCount(), 1); j++){

				int nTabSize = r.w;
				if(j<pListBox->GetFieldCount()) nTabSize = pListBox->GetField(j)->nTabSize;

				int nWidth = min(nTabSize, r.w-nFieldStartX);
				if(pListBox->m_bAbsoulteTabSpacing==false) nWidth = r.w*nTabSize/100;

				int nAdjustWidth = 0;
				if(pListBox->GetScrollBar()->IsVisible()){
					nAdjustWidth = pListBox->GetScrollBar()->GetRect().w + pListBox->GetScrollBar()->GetRect().w/2;
				}
				sRect ir(p.x+nFieldStartX, p.y, nWidth, nItemHeight);
				const char* szText = pItem->GetString(j);
				CCBitmap* pBitmap = pItem->GetBitmap(j);
							
				if(pBitmap!=NULL) {//이미지 그리고
					if(nWidth > pListBox->GetItemHeight())
						nWidth = pListBox->GetItemHeight();
					OnItemDraw(pDC, ir, pBitmap,  bSelected, bFocused, nWidth);
				}

 				sColor color;
				if( (sColor(pListBox->m_FontColor)).GetARGB() == DEFCOLOR_CCLIST_TEXT )
					color = pItem->GetColor();
				else
					color = pListBox->m_FontColor;

				if(szText!=NULL && szText[0]!=0 )// 텍스트 그리고
				{
					nLine = OnItemDraw(pDC, ir, szText,color,bSelected, bFocused, nAdjustWidth);
				}

/*
				if(szText!=NULL)
					OnItemDraw(pDC, ir, szText, color, bSelected, bFocused, nAdjustWidth);
				else if(pBitmap!=NULL)
					OnItemDraw(pDC, ir, pBitmap,  bSelected, bFocused, nAdjustWidth);
*/
				nFieldStartX += nWidth;
				if(nFieldStartX>=r.w) break;
			}

			nShowCount++;
			//nShowCount += nLine;

			if(nShowCount>=pListBox->GetShowItemCount()) break;
		}
	}
	else if (pListBox->GetViewStyle() == CCVS_ICON)
	{
		sPoint p;
		p.x = r.x;
		p.y = r.y + nHeaderHeight;

		int nStartX = 0, nStartY = 0;
		sSize TabSize(r.w, r.h);

		if(pListBox->GetFieldCount() > 0) 
		{
			TabSize.w = pListBox->GetField(0)->nTabSize;
			TabSize.h = pListBox->GetItemHeight();
		}

		for(int i=pListBox->GetStartItem(); i<pListBox->GetCount(); i++)
		{
			CCListItem* pItem = pListBox->Get(i);
//			bool bSelected = (pListBox->IsSelected()) ? (pListBox->GetSelIndex()==i) : false;
			bool bSelected = pItem->m_bSelected;
			bool bFocused = (pListBox->IsFocus());

			int nWidth = min(TabSize.w, r.w - nStartX);

			int nAdjustWidth = 0;
			if(pListBox->GetScrollBar()->IsVisible())
			{
				nAdjustWidth = pListBox->GetScrollBar()->GetRect().w + pListBox->GetScrollBar()->GetRect().w/2;
			}

			sRect ir(p.x+nStartX, p.y+nStartY, nWidth, nItemHeight);
			const char* szText = pItem->GetString(0);
			CCBitmap* pBitmap = pItem->GetBitmap(0);
			const sColor color = pItem->GetColor(0);
			if(szText!=NULL)
				OnItemDraw(pDC, ir, szText, color, bSelected, bFocused, nAdjustWidth);
			else if(pBitmap!=NULL)
				OnItemDraw(pDC, ir, pBitmap,  bSelected, bFocused, nAdjustWidth);

			nStartX += TabSize.w;
			if(nStartX >= (r.w - TabSize.w )) 
			{
				nStartX = 0;
				nStartY += nItemHeight + 5;

//				if (nStartY >= (r.h - TabSize.h)) break;
			}

			nShowCount++;
			if(nShowCount>=pListBox->GetShowItemCount()) break;
		}
	}

//	pDC->EndFont();
}

sRect CCListBoxLook::GetClientRect(CCListBox* pListBox, sRect& r)
{
	return sRect(r.x+1, r.y+1,
		(pListBox->GetScrollBar()->IsVisible()==true)?(r.w - pListBox->GetScrollBar()->GetClientRect().w-2):(r.w-2),
		r.h-2);
}

CCListBoxLook::CCListBoxLook(void)
{
	m_SelectedPlaneColor = DEFCOLOR_CCLIST_SELECTEDPLANE;
	m_SelectedTextColor = DEFCOLOR_CCLIST_SELECTEDTEXT;
	m_UnfocusedSelectedPlaneColor = DEFCOLOR_DARK;
	m_ItemTextAlignmentMode	= CCD_NOTALIGN;
	m_bItemTextMultiLine			= false;
	m_pItemSlotBitmap				= NULL;
}

bool CCListBox::IsAlwaysVisibleScrollbar(void)
{
	return m_bAlwaysVisibleScrollbar;
}

void CCListBox::SetAlwaysVisibleScrollbar(bool bVisible)
{
	m_bAlwaysVisibleScrollbar=bVisible;
	if(m_bAlwaysVisibleScrollbar)
		m_pScrollBar->Show(true);
}

void CCListBox::EnableDragAndDrop( bool bEnable)
{
	m_bDragAndDrop = bEnable;
}

bool CCListBox::OnDrop(CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString)
{
	if ( m_pOnDropFunc != NULL)
	{
		m_pOnDropFunc(this, pSender, pBitmap, szString, szItemString);
	}

	return true;
}

void CCListBox::MultiplySize( float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight )
{
	CCWidget::MultiplySize( byIDLWidth, byIDLHeight, byCurrWidth, byCurrHeight );

	m_iItemHeight = int(m_iItemHeight * byCurrHeight);

	if (m_bAbsoulteTabSpacing)
	{
		for (int i=0; i<GetFieldCount(); ++i)
			GetField(i)->nTabSize = int(GetField(i)->nTabSize * byCurrWidth);
	}
}

