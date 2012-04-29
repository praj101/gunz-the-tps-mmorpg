#include "stdafx.h"
#include "CCLabel.h"
#include "CCColorTable.h"

#define CCLABEL_DEFAULT_ALIGNMENT_MODE	CCD_LEFT

IMPLEMENT_LOOK(CCLabel, CCLabelLook)

CCLabel::CCLabel(const char* szName, CCWidget* pParent, CCListener* pListener) : CCWidget(szName, pParent, pListener){
	LOOK_IN_CONSTRUCTOR()

	m_TextColor		= DEFCOLOR_CCLABEL_TEXT;
	m_AlignmentMode	= CCLABEL_DEFAULT_ALIGNMENT_MODE;

	CCFont* pFont = GetFont();
	int w = 100;
	if(szName!=NULL) w = pFont->GetWidth(szName);
	int h = pFont->GetHeight();
	SetSize(w, h);
}

void CCLabel::SetTextColor(sColor color){
	m_TextColor = color;
}

sColor CCLabel::GetTextColor(void){
	return m_TextColor;
}

CCAlignmentMode CCLabel::GetAlignment(void){
	return m_AlignmentMode;
}

CCAlignmentMode CCLabel::SetAlignment(CCAlignmentMode am){
	CCAlignmentMode temp = m_AlignmentMode;
	m_AlignmentMode = am;
	return temp;
}

void CCLabelLook::OnDraw(CCLabel* pLabel, CCDrawContext* pDC){
	pDC->SetColor(pLabel->GetTextColor());
	sColor PrevHCol = pDC->SetHighlightColor(sColor(DEFCOLOR_PEN_HIGHLIGHT));
	sRect r = pLabel->GetClientRect();
	pDC->TextWithHighlight(r, pLabel->m_szName, pLabel->GetAlignment());
	pDC->SetHighlightColor(PrevHCol);
}

sRect CCLabelLook::GetClientRect(CCLabel* pLabel, sRect& r){
	sRect t = r;
	t.Enlarge(-1);
	return t;
}

