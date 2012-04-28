#include "stdafx.h"
#include "CCPanel.h"
#include "CCColorTable.h"

IMPLEMENT_LOOK(CCPanel, CCPanelLook)

CCPanel::CCPanel(const char* szName, CCWidget* pParent, CCListener* pListener) : CCWidget(szName, pParent, pListener){
	LOOK_IN_CONSTRUCTOR()

	m_BorderColor = sColor(DEFCOLOR_CCPANEL_BORDER);
	m_BackgroundColor = sColor(DEFCOLOR_CCPANEL_PLANE);
	m_nBorderStyle = CCBS_SINGLE;
}


void CCPanel::SetBorderColor(sColor color){	
	m_BorderColor = color;
}


sColor CCPanel::GetBorderColor(void){
	return m_BorderColor;
}

void CCPanel::SetBorderStyle(CCBorderStyle style){
	m_nBorderStyle = style;
}

CCBorderStyle CCPanel::GetBorderStyle(){
	return m_nBorderStyle;
}

void CCPanel::SetBackgroundColor(sColor color){
	m_BackgroundColor = color;
}

sColor CCPanel::GetBackgroundColor(void){
	return m_BackgroundColor;
}

void CCPanelLook::OnDraw(CCPanel* pPanel, CCDrawContext* pDC){
	if (pPanel->GetBorderStyle() == CCBS_SINGLE) OnFrameDraw(pPanel, pDC);
}

sRect CCPanelLook::GetClientRect(CCPanel* pLabel, sRect& r){
	return r;
}

void CCPanelLook::OnFrameDraw(CCPanel* pPanel, CCDrawContext* pDC){
	sRect r = pPanel->GetInitialClientRect();
	if (pPanel->GetBackgroundColor().a != 0)
	{
		pDC->SetColor(pPanel->GetBackgroundColor());
		pDC->FillRectangle(r);
	}
	pDC->SetColor(pPanel->GetBorderColor());
	pDC->Rectangle(r);
}

