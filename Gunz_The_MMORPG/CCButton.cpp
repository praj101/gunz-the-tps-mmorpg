#include "stdafx.h"
#include "CCButton.h"
#include "MColorTable.h"
#include "MTooltip.h"
#include "CCMsgBox.h"
#include "Mint.h"

#define MBUTTON_DEFAULT_ALIGNMENT_MODE	(MAM_HCENTER|MAM_VCENTER)

/*
CCButtonLook CCButton::m_DefaultLook;
CCButtonLook* CCButton::m_pLook = &CCButton::m_DefaultLook;
*/

IMPLEMENT_LOOK(CCButton, CCButtonLook)

/*
void CCButton::OnDraw(CCDrawContext* pDC)
{
	m_pLook->OnDraw(this, pDC);
}
*/

void CCButton::OnMouseIn(void)
{
}

void CCButton::OnMouseOut(void)
{
}

void CCButton::OnButtonDown(void)
{
}

void CCButton::OnButtonUp(void)
{
}

void CCButton::OnButtonClick(void)
{
	if(GetListener()) GetListener()->OnCommand(this, MBTN_CLK_MSG);
}

bool CCButton::OnEvent(CCEvent* pEvent, CCListener* pListener)
{
	sRect r = GetInitialClientRect();

	m_ClickPos = pEvent->sPos;

	switch(pEvent->iMessage){
	case CCWM_LBUTTONDOWN:
		if(r.InPoint(pEvent->sPos)==true){
			m_bLButtonDown = true;

			m_LDragStartClickPos = pEvent->sPos;
			m_LDragVariationX = 0;
			m_LDragVariationY = 0;

			OnButtonDown();
			if(pListener!=NULL) pListener->OnCommand(this, CCBTN_DN_MSG);
			SetCapture();
			return true;
		}
		break;

	case CCWM_LBUTTONUP:
		if(m_bLButtonDown==true){
			m_bLButtonDown = false;

			m_LDragVariationX = 0;
			m_LDragVariationY = 0;

			OnButtonUp();
			ReleaseCapture();
			if(r.InPoint(pEvent->sPos)==true){
				if(GetType()==CCBT_PUSH || GetType()==CCBT_PUSH2 ) 
				{
					if(m_pButtonGroup)
						SetCheck(true);
					else
						SetCheck(!m_bChecked);
				}

				if(m_pMsgBox!=NULL) m_pMsgBox->Show(true, true);
				else if(pListener!=NULL) {
					pListener->OnCommand(this, MBTN_UP_MSG);
					OnButtonClick();
				}
				return true;
			}
		}
		break;

	case CCWM_LBUTTONDBLCLK:
		if(r.InPoint(pEvent->sPos)==true){
			if(m_pMsgBox!=NULL) m_pMsgBox->Show(true, true);
			else OnButtonClick();
			return true;
		}
		break;

	case CCWM_RBUTTONDBLCLK:
		if(r.InPoint(pEvent->sPos)==true){
			if(m_pMsgBox!=NULL) m_pMsgBox->Show(true, true);
			else if(pListener!=NULL) pListener->OnCommand(this, CCBTN_RCLK_MSG);
			return true;
		}
		break;

	case CCWM_RBUTTONDOWN:
		if(r.InPoint(pEvent->sPos)==true){
			m_bRButtonDown = true;
			if(pListener!=NULL) pListener->OnCommand(this, CCBTN_RDN_MSG);
			SetCapture();
			return true;
		}
		break;

	case CCWM_RBUTTONUP:
		if(m_bRButtonDown==true){
			m_bRButtonDown = false;
			ReleaseCapture();
			if(r.InPoint(pEvent->sPos)==true && pListener!=NULL){
				if(GetType()==MBT_PUSH && GetType() == CCBT_PUSH2 ) m_bChecked = !m_bChecked;
				pListener->OnCommand(this, CCBTN_RUP_MSG);
				pListener->OnCommand(this, CCBTN_RCLK_MSG);
			}
			return true;
		}
		break;
	case CCWM_MOUSEMOVE:
		if(r.InPoint(pEvent->sPos)==true){
			if(m_bMouseOver==false){
				m_bMouseOver = true;
				OnMouseIn();
				if(pListener!=NULL) pListener->OnCommand(this, CCBTN_IN_MSG);
			}

			if(m_bLButtonDown) {//drag
				m_LDragVariationX = m_LDragStartClickPos.x - pEvent->Pos.x;
				m_LDragVariationY = m_LDragStartClickPos.y - pEvent->Pos.y;
			}
		}
		else{
			if(m_bMouseOver==true){
				m_bMouseOver = false;
				OnMouseOut();
				if(pListener!=NULL) pListener->OnCommand(this, CCBTN_OUT_MSG);
			}
		}
		return false;
	case CCWM_KEYDOWN:
		if(m_bEnableEnter==true && IsFocus()==true){
			if(pEvent->uKey==VK_RETURN){
				if(m_pMsgBox!=NULL) m_pMsgBox->Show(true, true);
				else OnButtonClick();
				return true;
			}
		}
		break;
	}
	return false;
}

bool CCButton::OnShow(void)
{
	m_bMouseOver = false;
	m_bLButtonDown = false;
	if(GetToolTip()!=NULL) GetToolTip()->Show(false);
	return true;
}

void CCButton::OnHide(void)
{
	if(GetToolTip()!=NULL) GetToolTip()->Show(false);
	if(m_pMsgBox) m_pMsgBox->Show(false);
}

CCButton::CCButton(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCWidget(szName, pParent, pListener), m_TextColor(0, 255, 0)//, m_TextOffset(0, 0)
{
	m_bMouseOver = false;
	m_bLButtonDown = false;
	m_bShowText = true;

	m_AlignmentMode = MBUTTON_DEFAULT_ALIGNMENT_MODE;
	m_Type = MBT_NORMAL;
	m_bChecked = false;

	m_bEnableEnter = true;
	m_bHighlight = true;

	m_pIcon = NULL;

	m_nKeyAssigned = MBKA_NONE;

	SetFocusEnable(true);

	m_pMsgBox = NULL;

	m_ClickPos = sPoint(0,0);

	m_LDragStartClickPos = sPoint(0,0);
	m_LDragVariationX = 0;
	m_LDragVariationY = 0;

	m_bComboDropped = false;
	m_bStretch = false;

	m_pButtonGroup = NULL;
	m_nIndexInGroup = 0;

	LOOK_IN_CONSTRUCTOR()
}

CCButton::~CCButton(void)
{
	if(m_pMsgBox!=NULL) delete m_pMsgBox;
}


/*
void CCButton::SetTextOffset(sPoint p)
{
	m_TextOffset = p;
}
*/
void CCButton::SetTextColor(sColor color)
{
	m_TextColor = color;
}

sColor CCButton::GetTextColor(void)
{
	return m_TextColor;
}

void CCButton::ShowText(bool bShow)
{
	m_bShowText = bShow;
}

bool CCButton::DefaultCommand(void)
{
	CCListener* pListener = GetListener();
	if(m_pMsgBox!=NULL) m_pMsgBox->Show(true, true);
	else OnButtonClick(); 
	return true;
}

CCAlignmentMode CCButton::GetAlignment(void)
{
	return m_AlignmentMode;
}

CCAlignmentMode CCButton::SetAlignment(CCAlignmentMode am)
{
	CCAlignmentMode temp = m_AlignmentMode;
	m_AlignmentMode = am;
	return temp;
}

void CCButton::SetType(CCButtonType t)
{
	m_Type = t;
}

CCButtonType CCButton::GetType(void)
{
	return m_Type;
}

void CCButton::SetCheck(bool bCheck)
{
	if(bCheck && m_pButtonGroup)
	{
		m_pButtonGroup->m_pPrevious->SetCheck(false);
		m_pButtonGroup->m_pPrevious=this;
	}

	m_bChecked = bCheck;
}

bool CCButton::GetCheck(void)
{
	return m_bChecked;
}

bool CCButton::IsButtonDown(void)
{
	if(m_bLButtonDown==true && m_bMouseOver==true) return true;
	return false;
}

bool CCButton::IsMouseOver(void)
{
	return m_bMouseOver;
}

void CCButton::SetConfirmMessageBox(const char* szMessage)
{
	if(szMessage==NULL || szMessage[0]==0){
		if(m_pMsgBox!=NULL) delete m_pMsgBox;
		m_pMsgBox = NULL;
		return;
	}

	
	//if(m_pMsgBox==NULL) m_pMsgBox = new CCMsgBox(szMessage, GetParent(), MT_OKCANCEL, this);
	if(m_pMsgBox==NULL) {
		m_pMsgBox = (CCMsgBox*)Core::GetInstance()->NewWidget(CORE_MSGBOX, "", Core::GetInstance()->GetMainFrame(), this );
		m_pMsgBox->SetType(CCT_OKCANCEL);
//		m_pMsgBox->SetTitle(szMessage);		// title 은 look의 default title로 대체
	}
	
	m_pMsgBox->SetText(szMessage);
	m_strIDLConfirmText = szMessage;
}

void CCButton::SetAlterableConfirmMessage(const char* szMessage)
{
	if (m_pMsgBox)
		m_pMsgBox->SetText(szMessage);
}
void CCButton::RestoreIDLConfirmMessage()
{
	if (m_pMsgBox)
		m_pMsgBox->SetText(m_strIDLConfirmText.c_str());
}

bool CCButton::OnCommand(CCWidget* pWidget, const char* szMessage)
{
	if(pWidget==m_pMsgBox){
		m_pMsgBox->Show(false);
		CCListener* pListener = GetListener();
		if(strcmp(szMessage, MMSGBOX_OK)==0) if(pListener!=NULL) pListener->OnCommand(this, MBTN_CLK_MSG);
		return true;
	}
	return false;
}

void CCButton::SetButtonGroup(CCButtonGroup *pGroup)
{
	m_pButtonGroup = pGroup;
	m_nIndexInGroup = pGroup->m_nCount;
	pGroup->m_nCount++;
	
	if(GetCheck()==true)
		pGroup->m_pPrevious = this;
}

void CCButton::MultiplySize( float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight )
{
	CCWidget::MultiplySize(byIDLWidth, byIDLHeight, byCurrWidth, byCurrHeight);

	if (m_pMsgBox)
		m_pMsgBox->MultiplySize(byIDLWidth, byIDLHeight, byCurrWidth, byCurrHeight);
}
void CCButtonLook::OnDrawText(CCButton* pButton, sRect& r, CCDrawContext* pDC)
{
	if( !pButton->IsVisible() ) return;
	if(pButton->m_pIcon!=NULL){
		pDC->SetBitmap(pButton->m_pIcon);
		pDC->Draw(r.x, r.y+(r.h-pButton->m_pIcon->GetHeight())/2);
		r.x+=pButton->m_pIcon->GetWidth();
		r.w-=pButton->m_pIcon->GetWidth();
	}

	pDC->SetColor(sColor(DEFCOLOR_MBUTTON_TEXT));
	if(pButton->m_bHighlight==true){
		sColor PrevHCol = pDC->SetHighlightColor(sColor(DEFCOLOR_PEN_HIGHLIGHT));
		pDC->TextWithHighlight(r, pButton->m_szName, pButton->GetAlignment());
		pDC->SetHighlightColor(PrevHCol);
	}
	else{
		pDC->Text(r, pButton->m_szName, pButton->GetAlignment());
	}
}

void CCButtonLook::OnDownDraw(CCButton* pButton, CCDrawContext* pDC)
{
	sRect r = pButton->GetInitialClientRect();
	pDC->SetColor(sColor(DEFCOLOR_MBUTTON_DARKPLANE));
	pDC->FillRectangle(r);

	OnDrawText(pButton, pButton->GetClientRect(), pDC);
}

void CCButtonLook::OnUpDraw(CCButton* pButton, CCDrawContext* pDC)
{
	sRect r = pButton->GetInitialClientRect();
	pDC->SetColor(sColor(DEFCOLOR_MBUTTON_PLANE));
	pDC->FillRectangle(r);

	OnDrawText(pButton, pButton->GetClientRect(), pDC);
}

void CCButtonLook::OnOverDraw(CCButton* pButton, CCDrawContext* pDC)
{
	if(pButton->GetType()==MBT_PUSH){
		//if(pButton->GetCheck()==true) OnDownDraw(pButton, pDC);
		//else OnUpDraw(pButton, pDC);
		OnCheckBoxDraw(pButton, pDC, pButton->GetCheck());
	}
	else{
		sRect r = pButton->GetInitialClientRect();
		pDC->SetColor(sColor(DEFCOLOR_MBUTTON_PLANE));
		pDC->FillRectangle(r);

		OnDrawText(pButton, pButton->GetClientRect(), pDC);
	}
}

void CCButtonLook::OnDisableDraw(CCButton* pButton, CCDrawContext* pDC)
{
	sRect r = pButton->GetInitialClientRect();
	pDC->SetColor(sColor(DEFCOLOR_MBUTTON_PLANE));
	pDC->FillRectangle(r);

	OnDrawText(pButton, pButton->GetClientRect(), pDC);
}

void CCButtonLook::OnDraw(CCButton* pButton, CCDrawContext* pDC)
{
 	if(pButton->GetType()==MBT_PUSH) OnCheckBoxDraw(pButton, pDC, pButton->GetCheck());
	else if(pButton->IsEnable()==false) OnDisableDraw(pButton, pDC);
	else if(pButton->IsButtonDown()==true) OnDownDraw(pButton, pDC);
	else if(pButton->IsMouseOver()==true) OnOverDraw(pButton, pDC);
	else{
		//if(pButton->GetType()==MBT_PUSH){
		//	//if(pButton->GetCheck()) OnDownDraw(pButton, pDC);
		//	//else OnUpDraw(pButton, pDC);
		//	OnCheckBoxDraw(pButton, pDC, pButton->GetCheck());
 	//	}
		//else
			OnUpDraw(pButton, pDC);
	}
}

sRect CCButtonLook::GetClientRect(CCButton* pButton, sRect& r)
{
	return r;
}

////////////////////////////////////
// CCButtonGroup

CCButtonGroup::CCButtonGroup()
{
	m_nCount = 0;
	m_pPrevious = NULL;
}

CCButtonGroup::~CCButtonGroup()
{
}
