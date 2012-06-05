#include "stdafx.h"
#include "CCBLabelLook.h"
#include "CCBitmapDrawer.h"

void CCBLabelLook::OnDraw(CCLabel* pLabel, CCDrawContext* pDC)
{
	sRect r = pLabel->GetInitialClientRect();
	if(pLabel->GetFont() != NULL ) pDC->SetFont( pLabel->GetFont() );
	else if(m_pFont!=NULL) pDC->SetFont(m_pFont);
	//pDC->SetColor(m_FontColor);
	pDC->SetColor( pLabel->GetTextColor() );
	pDC->Text(r, pLabel->m_szName, pLabel->GetAlignment());
}

CCBLabelLook::CCBLabelLook(void)
{
	m_FontColor = sColor(255, 255, 255);
}

sRect CCBLabelLook::GetClientRect(CCLabel* pLabel, sRect& r)
{
	return r;
}
