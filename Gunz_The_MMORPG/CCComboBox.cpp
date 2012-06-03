#include "stdafx.h"
#include "CCComboBox.h"
#include "Core.h"

CCComboListBox::CCComboListBox(CCWidget* pParent, CCListener* pListener)
: CCListBox(pParent, pListener)
{
	m_bClipByParent = false;
	LOOK_IN_CONSTRUCTOR()
}

bool CCComboListBox::OnEvent(CCEvent* pEvent, CCListener* pListener)
{
	if(CCListBox::OnEvent(pEvent, pListener)==true) return true;
	
	switch(pEvent->iMessage){
	case CCWM_LBUTTONDOWN:
	case CCWM_RBUTTONDOWN:
	case CCWM_LBUTTONDBLCLK:
	case CCWM_RBUTTONDBLCLK:
		Show(false);
		return false;	// 다른 Widget에게 넘겨준다.
	}

	return false;
}

IMPLEMENT_LOOK(CCComboListBox, CCListBoxLook)


#define DEFAULT_DROP_HEIGHT	200

CCComboBox::CCComboBox(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCButton(szName, pParent, pListener)
{
	CCButton::SetListener(this);	// Button 메세지는 한번 걸러서 보내준다.

	m_pListBox = new CCComboListBox(pParent, this);
	m_pListBox->Show(false);

	m_iDropHeight = DEFAULT_DROP_HEIGHT;

	if(szName!=NULL && szName[0]!=0) Add(szName);

	m_bHighlight = false; // 콤보박스에서는 하이라이트(&)를 지원할 필요가 없다.

	m_pComboBoxListener = pListener;

	m_AlignmentMode = CCD_HCENTER|CCD_VCENTER;

	m_iComboType = 0;
	m_iNextComboBoxTypeSize = 0;

	m_bAutoDrop = true;
	m_bDropUnder = true;
}

CCComboBox::~CCComboBox(void)
{
	delete m_pListBox;
}

void CCComboBox::SetDropSize(int nHeight)
{
	m_iDropHeight = nHeight;
}

void CCComboBox::Add(const char* szItem)
{
	m_pListBox->Add(szItem);
}

void CCComboBox::Add(CCListItem* pItem)
{
	m_pListBox->Add(pItem);
}

const char* CCComboBox::GetString(int i)
{
	return m_pListBox->GetString(i);
}

CCListItem* CCComboBox::Get(int i)
{
	return m_pListBox->Get(i);
}

void CCComboBox::Remove(int i)
{
	m_pListBox->Remove(i);
}

void CCComboBox::RemoveAll(void)
{
	m_pListBox->RemoveAll();
}

int CCComboBox::GetCount(void)
{
	return m_pListBox->GetCount();
}

int CCComboBox::GetSelIndex(void)
{
	return m_pListBox->GetSelIndex();
}

bool CCComboBox::SetSelIndex(int i)
{
	bool bReturn = m_pListBox->SetSelIndex(i);
	if(bReturn==true) SetText(GetSelItemString());
	return bReturn;
}

const char* CCComboBox::GetSelItemString(void)
{
	return m_pListBox->GetSelItemString();
}

CCListItem* CCComboBox::GetSelItem(void)
{
	return m_pListBox->GetSelItem();
}

void CCComboBox::SetNextSel()
{
	int cnt = GetCount();
	int sel = GetSelIndex();

	if(cnt > 1) {
		if(sel+1==cnt) {
			SetSelIndex(0);
		}
		else {
			SetSelIndex(sel+1);
		}

		if(m_pComboBoxListener!=NULL){
			m_pComboBoxListener->OnCommand(this, CCCMBBOX_CHANGED);
		}
	}
}

void CCComboBox::SetPrevSel()
{
	int cnt = GetCount();
	int sel = GetSelIndex();

	if(cnt > 1) {
		if(sel==0) {
			SetSelIndex(cnt-1);
		} else {
			SetSelIndex( sel-1 );
		}

		if(m_pComboBoxListener!=NULL){
			m_pComboBoxListener->OnCommand(this, CCCMBBOX_CHANGED);
		}
	}
}

bool CCComboBox::OnCommand(CCWidget* pWindow, const char* szMessage)
{
	sRect r = GetRect();

 	if(pWindow==this && strcmp(szMessage, CCBTN_CLK_MSG)==0) {

		if( GetComboDropped() ) 
		{
			m_pListBox->Show(false);
			SetComboDropped(false);
			ReleaseFocus();
			return true;
		}
		
		bool bSCheck = false;

 		if(m_iComboType == 0) { // 2부분으로 나누어처리

			if( GetNextComboBoxTypeSize() > m_ClickPos.x ) {
				bSCheck = true;
			}
		}

		// DropHeight가 넉넉하면 자동으로 크기를 줄인다.

		if(bSCheck) {

			int cnt = GetCount();
			int sel = GetSelIndex();

			if(cnt > 1) {
				if(sel+1==cnt) {
					SetSelIndex(0);
				}
				else {
					SetSelIndex(sel+1);
				}

				if(m_pComboBoxListener!=NULL){
					m_pComboBoxListener->OnCommand(this, CCCMBBOX_CHANGED);
				}

			}
		}
		else {
		
			int nFrameHeight = m_pListBox->GetRect().h - m_pListBox->GetClientRect().h;
 			int nDropHeight	 = min(m_iDropHeight,m_pListBox->GetItemHeight()*m_pListBox->GetCount()+nFrameHeight);

			bool bDropUnder = true;

			sPoint sp = CCClientToScreen(this,sPoint(r.x,r.y+r.h));

			if( ( GetRect().y + GetRect().h + nDropHeight + 10) > CCGetWorkspaceHeight())
				bDropUnder = false;

			if(m_bAutoDrop)
			{
				if(bDropUnder==true)	m_pListBox->SetBounds(sRect(r.x, r.y+r.h, r.w, nDropHeight));
				else					m_pListBox->SetBounds(sRect(r.x, r.y-nDropHeight, r.w, nDropHeight));
			}
			else
			{
				if(m_bDropUnder)		m_pListBox->SetBounds(sRect(r.x, r.y+r.h, r.w, nDropHeight));
				else					m_pListBox->SetBounds(sRect(r.x, r.y-nDropHeight, r.w, nDropHeight));
			}

			m_pListBox->Show(true, true);
			SetComboDropped( true );
		}

		return true;
	}
	else if(pWindow==m_pListBox) {

		if(IsMsg(szMessage, CCLB_ITEM_SEL)==true || IsMsg(szMessage, CCLB_ITEM_SEL2)==true || IsMsg(szMessage, CCLB_ITEM_DBLCLK)==true){
			m_pListBox->Show(false);
			SetComboDropped( false );
			SetText(m_pListBox->GetSelItemString());
			if(m_pComboBoxListener!=NULL){
				m_pComboBoxListener->OnCommand(this, CCCMBBOX_CHANGED);
			}
			return true;
		}
	}
	return false;
}

void CCComboBox::Sort(void)
{
	m_pListBox->Sort();
}

void CCComboBox::SetListener(CCListener* pListener)
{
	m_pComboBoxListener = pListener;
}
CCListener* CCComboBox::GetListener(void)
{
	return CCWidget::GetListener();
}

void CCComboBox::OnReleaseFocus()
{
	if( !m_pListBox->IsFocus() && GetComboDropped() )
	{
		SetComboDropped(false);
		m_pListBox->Show(false);
	}
}

void CCComboBox::CloseComboBoxList()
{
	m_pListBox->Show(false);
	SetComboDropped( false );
}

void CCComboBox::MultiplySize( float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight )
{
	CCButton::MultiplySize(byIDLWidth, byIDLHeight, byCurrWidth, byCurrHeight);

	m_iDropHeight = int(m_iDropHeight * byCurrHeight);
}