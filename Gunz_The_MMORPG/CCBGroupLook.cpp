#include "stdafx.h"
#include "CCBGroupLook.h"
#include "CCBitmapDrawer.h"

void CCBGroupLook::OnDraw(CCGroup* pGroup, CCDrawContext* pDC)
{
	sRect r = pGroup->GetInitialClientRect();
	
	//DrawBitmapFrame9(pDC, r, m_pFrameBitmaps, false, m_bStretch);

	DrawBitmapFrameCustom1( pDC, r, m_pFrameBitmaps, m_bStretch );

	if(m_pFont!=NULL) pDC->SetFont(m_pFont);
	pDC->SetColor(m_FontColor);
	pDC->Text(r.x+m_TitlePosition.x, r.y+m_TitlePosition.y, pGroup->m_szName);
}

CCBGroupLook::CCBGroupLook(void)
{
	for(int i=0; i<FRAME_BITMAP_COUNT; i++){
		m_pFrameBitmaps[i] = NULL;
	}
	m_pFont=NULL;
	m_bStretch = true;
	m_TitlePosition = sPoint(8, 4);
}

sRect CCBGroupLook::GetClientRect(CCGroup* pGroup, sRect& r)
{
/*
	int al = GETWIDTH(m_pFrameBitmaps[6]);
	int au = GETHEIGHT(m_pFrameBitmaps[6]);
	int ar = GETWIDTH(m_pFrameBitmaps[8]);
	int ab = GETHEIGHT(m_pFrameBitmaps[0]);
	return sRect(r.x+al, r.y+au, r.w-(al+ar), r.h-(au+ab));
*/
	int al = GETWIDTH(m_pFrameBitmaps[3]);
	int au = GETHEIGHT(m_pFrameBitmaps[6]);
	int ar = GETWIDTH(m_pFrameBitmaps[5]);
	int ab = GETHEIGHT(m_pFrameBitmaps[0]);
	return sRect(r.x+al, r.y+au, r.w-(al+ar), r.h-(au+ab));
}
