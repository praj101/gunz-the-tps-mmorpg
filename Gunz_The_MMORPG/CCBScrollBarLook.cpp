#include "stdafx.h"
#include "CCBScrollBarLook.h"
#include "CCBitmapDrawer.h"

void CCBArrowLook::OnDrawUpArrow(CCDrawContext* pDC, sRect& r, bool bPressed)
{
	if(bPressed==false) pDC->SetBitmap(m_pArrowBitmaps[4]);
	else pDC->SetBitmap(m_pArrowBitmaps[6]);
	pDC->Draw(r.x, r.y, r.w, r.h);
}  

void CCBArrowLook::OnDrawDownArrow(CCDrawContext* pDC, sRect& r, bool bPressed)
{
	if(bPressed==false) pDC->SetBitmap(m_pArrowBitmaps[5]);
	else pDC->SetBitmap(m_pArrowBitmaps[7]);
	pDC->Draw(r.x, r.y, r.w, r.h);
}

void CCBArrowLook::OnDrawLeftArrow(CCDrawContext* pDC, sRect& r, bool bPressed)
{
	if(bPressed==false) pDC->SetBitmap(m_pArrowBitmaps[0]);
	else pDC->SetBitmap(m_pArrowBitmaps[2]);
	pDC->Draw(r.x, r.y, r.w, r.h);
}

void CCBArrowLook::OnDrawRightArrow(CCDrawContext* pDC, sRect& r, bool bPressed)
{
	if(bPressed==false) pDC->SetBitmap(m_pArrowBitmaps[1]);
	else pDC->SetBitmap(m_pArrowBitmaps[3]);
	pDC->Draw(r.x, r.y, r.w, r.h);
}

CCBArrowLook::CCBArrowLook(void)
{
	for(int i=0; i<8; i++){
		m_pArrowBitmaps[i] = NULL;
	}
}

sSize CCBArrowLook::GetDefaultSize(CCArrow* pThumb)
{
	if(pThumb->m_iDirection==0){
		if(m_pArrowBitmaps[4]==NULL) return CCArrowLook::GetDefaultSize(pThumb);
		return sSize(m_pArrowBitmaps[4]->GetWidth(), m_pArrowBitmaps[4]->GetHeight());
	}
	else{
		if(m_pArrowBitmaps[0]==NULL) return CCArrowLook::GetDefaultSize(pThumb);
		return sSize(m_pArrowBitmaps[0]->GetWidth(), m_pArrowBitmaps[0]->GetHeight());
	}
}


#define IMAGEVGAP		3
void CCBThumbLook::OnDraw(CCThumb* pThumb, CCDrawContext* pDC)
{
	sRect r = pThumb->GetInitialClientRect();

//	if(pThumb->m_nDirection==0){
//		DrawBitmapFrameH3(pDC, r, m_pHBitmaps);
//	}
//	else{
//		DrawBitmapFrameV3(pDC, r, m_pVBitmaps);
//	}

	CCBitmap *pBmp = m_pHBitmaps[0];
	pDC->SetBitmap( pBmp);

	pDC->Draw( r.x, r.y, r.w, r.h/2,
		       0, IMAGEVGAP, pBmp->GetWidth(), IMAGEVGAP);
	pDC->Draw( r.x, r.y+r.h/2, r.w, r.h/2,
		       0, pBmp->GetWidth() - IMAGEVGAP*2, pBmp->GetWidth(), IMAGEVGAP);
	pDC->Draw( r.x, r.y, r.w, IMAGEVGAP,
		       0, 0, pBmp->GetWidth(), IMAGEVGAP);
	pDC->Draw( r.x, r.y+r.h-IMAGEVGAP, r.w, IMAGEVGAP,
		       0, pBmp->GetHeight() - IMAGEVGAP, pBmp->GetWidth(), IMAGEVGAP);
	pDC->Draw( r.x, r.y + r.h/2 - (pBmp->GetHeight()-IMAGEVGAP*4)/2, r.w, pBmp->GetHeight() - IMAGEVGAP*4,
		       0, IMAGEVGAP*2, pBmp->GetWidth(), pBmp->GetHeight() - IMAGEVGAP*4);
}

CCBThumbLook::CCBThumbLook(void)
{
	for(int i=0; i<3; i++){
		m_pHBitmaps[i] = NULL;
		m_pHPressedBitmaps[i] = NULL;
		m_pVBitmaps[i] = NULL;
		m_pVPressedBitmaps[i] = NULL;
	}
}



CCBScrollBarLook::CCBScrollBarLook(void)
{
	for(int i=0; i<3; i++){
		m_pVFrameBitmaps[i] = NULL;
		m_pHFrameBitmaps[i] = NULL;
	}
}

void CCBScrollBarLook::OnDraw(CCScrollBar* pScrollBar, CCDrawContext* pDC)
{
	sRect r = pScrollBar->GetInitialClientRect();
	//if(pScrollBar->GetType()==MSBT_VERTICAL)
	//	DrawBitmapFrameV3(pDC, r, m_pVFrameBitmaps);
	//else 
	//	DrawBitmapFrameH3(pDC, r, m_pHFrameBitmaps);
	sRect rtemp  = pDC->GetClipRect();
	pDC->SetClipRect(rtemp.x, rtemp.y, rtemp.w, rtemp.h );
	pDC->SetColor(sColor(0xFF000000));
	pDC->FillRectangle( r.x, r.y, r.w, r.h );
	pDC->SetClipRect(rtemp);
}

sRect CCBScrollBarLook::GetClientRect(CCScrollBar* pScrollBar, sRect& r)
{
	return r;
	/*
	if(IsNull(m_pVFrameBitmaps, 3)==true) return r;
	if(IsNull(m_pHFrameBitmaps, 3)==true) return r;

	if(pScrollBar->GetType()==MSBT_VERTICAL){
		int lw = m_pVFrameBitmaps[0]->GetWidth();
		int rw = m_pVFrameBitmaps[2]->GetWidth();
		return sRect(r.x, r.y+lw, r.w, r.h-(lw+rw));
	}
	else{
		int lw = m_pHFrameBitmaps[0]->GetWidth();
		int rw = m_pHFrameBitmaps[2]->GetWidth();
		return sRect(r.x+lw, r.y, r.w-(lw+rw), r.h);
	}
	*/
}
