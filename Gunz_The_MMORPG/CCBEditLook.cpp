#include "stdafx.h"
#include "CCBEditLook.h"
#include "CCBitmapDrawer.h"

#include "CCDebug.h"

void CCBEditLook::OnFrameDraw(CCEdit* pEdit, CCDrawContext* pDC)
{
	sRect r = pEdit->GetInitialClientRect();
 	if(GetCustomLook())
	{
		pDC->SetColor(sColor(200,200,200,255));
		pDC->Rectangle(r);
		HLineBitmap( pDC, r.x+1, r.y+1, r.w-2, m_pFrameBitmaps[4], false );
		return;
	}
	DrawBitmapFrame9(pDC, r, m_pFrameBitmaps);
}

CCBEditLook::CCBEditLook(void)
{
	for(int i=0; i<9; i++){
		m_pFrameBitmaps[i] = NULL;
	}
	m_pFont=NULL;
}

sRect CCBEditLook::GetClientRect(CCEdit* pEdit, sRect& r)
{
	if( m_bCustomLook )
	{
		return sRect(r.x+1, r.y+1, r.w-2, r.h-2);
	}
	int al = GETWIDTH(m_pFrameBitmaps[3]);
	int au = GETHEIGHT(m_pFrameBitmaps[7]);
	int ar = GETWIDTH(m_pFrameBitmaps[5]);
	int ab = GETHEIGHT(m_pFrameBitmaps[1]);
	return sRect(r.x+al, r.y+au, r.w-(al+ar), r.h-(au+ab));
}

void CCBEditLook::OnDraw(CCEdit* pEdit, CCDrawContext* pDC) 
{
	if(m_pFont!=NULL) pDC->SetFont(m_pFont);

    CCEditLook::OnDraw(pEdit,pDC);
}
