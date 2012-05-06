#include "stdafx.h"
#include "CCToolTip.h"
#include "CCColorTable.h"
#include "Core.h"

#define TOOLTIP_WIDTH_GAP	20
#define TOOLTIP_HEIGHT_GAP	5

void CCToolTip::OnDraw(CCDrawContext* pDC){
	sRect r = GetClientRect();
	pDC->SetColor(sColor(DEFCOLOR_CCTOOLTIP_PLANE));
	pDC->FillRectangle(r);
	pDC->SetColor(sColor(DEFCOLOR_CCTOOLTIP_OUTLINE));
	pDC->Rectangle(r);
	pDC->SetColor(sColor(DEFCOLOR_CCTOOLTIP_TEXT));
	char* szName;
	if(m_bUseParentName==true) szName = GetParent()->m_szName;
	else szName = m_szName;
	pDC->TextWithHighlight(r, szName, (CCD_HCENTER|CCD_VCENTER));
}

CCToolTip::CCToolTip(const char* szName, CCWidget* pParent)
: CCWidget(szName, pParent, NULL){
	_ASSERT(pParent!=NULL);
	SetText(szName);
	m_bClipByParent = false;
	SetFocusEnable(false);
	Show(false);
}

CCToolTip::~CCToolTip(){
}

void CCToolTip::SetBounds(){
	CCFont* pFont = GetFont();

	char szName[CCWIDGET_NAME_LENGTH];
	RemoveAnd(szName, m_bUseParentName==true?GetParent()->m_szName:m_szName);
	int nWidth = pFont->GetWidthWithoutAmpersand(szName);
	int nHeight = pFont->GetHeight();
	int x, y;
	sRect pr = GetParent()->GetClientRect();
	sRect spr = CCClientToScreen(GetParent(), pr);
	if(spr.x+(nWidth+TOOLTIP_WIDTH_GAP/2)<=CCGetWorkspaceWidth())
		x = pr.x+TOOLTIP_WIDTH_GAP/2+1;
	else{
		sPoint p = CCScreenToClient(GetParent(), sPoint(CCGetWorkspaceWidth()-(nWidth+TOOLTIP_WIDTH_GAP/2), 0));
		x = p.x;
	}
	y = pr.y-(nHeight+TOOLTIP_HEIGHT_GAP);
	sPoint p = CCClientToScreen(GetParent(), sPoint(0, y));
	if(p.y<0){
		y = p.y+pr.h+(nHeight+TOOLTIP_HEIGHT_GAP);
		if(y>CCGetWorkspaceHeight()) y = 0;
		p = CCScreenToClient(GetParent(), sPoint(0, y));
		y = p.y;
	}
	CCWidget::SetBounds(sRect(x-TOOLTIP_WIDTH_GAP/2, y, nWidth+TOOLTIP_WIDTH_GAP, nHeight+TOOLTIP_HEIGHT_GAP));
}

void CCToolTip::SetText(const char* szText){
	if(szText==NULL) m_bUseParentName = true;
	else m_bUseParentName = false;
	if(szText!=NULL) CCWidget::SetText(szText);
	SetBounds();
}

bool CCToolTip::IsUseParentName(){
	return m_bUseParentName;
}
