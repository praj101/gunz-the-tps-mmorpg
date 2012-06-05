#include "stdafx.h"
#include "CCBSliderLook.h"
#include "CCBitmapDrawer.h"

void CCBSliderThumbLook::OnDraw(CCSliderThumb* pThumb, CCDrawContext* pDC)
{
	sRect r = pThumb->GetInitialClientRect();
	if (m_pBitmap == NULL) {
		pDC->SetColor(sColor(m_ThumbColor));
		pDC->FillRectangle(r);
	}  else {
		pDC->SetBitmap(m_pBitmap);
		pDC->Draw(r.x, r.y, r.w, r.h);
	}
}

CCBSliderThumbLook::CCBSliderThumbLook(void)
{
	m_pBitmap = NULL;
	m_pPressedBitmap = NULL;
}

sSize CCBSliderThumbLook::GetDefaultSize(CCSliderThumb* pThumb)
{
	if(m_pBitmap==NULL) return CCSliderThumbLook::GetDefaultSize(pThumb);
	return sSize(m_pBitmap->GetWidth(), m_pBitmap->GetWidth());
}
