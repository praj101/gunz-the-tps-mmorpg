#include "stdafx.h"
#include "CCBListBoxLook.h"
#include "CCBitmapDrawer.h"

void CCBListBoxLook::OnFrameDraw(CCListBox* pListBox, CCDrawContext* pDC)
{
	sRect r = pListBox->GetInitialClientRect();
	//DrawBitmapFrame9(pDC, r, m_pFrameBitmaps);
	pDC->SetColor(19,19,19,255);
	pDC->FillRectangle(r);
	pDC->SetColor(128,128,128,255);
	pDC->Rectangle(r);
}

CCBListBoxLook::CCBListBoxLook(void)
{
	for(int i=0; i<9; i++){
		m_pFrameBitmaps[i] = NULL;
	}
	m_pFont=NULL;
}

sRect CCBListBoxLook::GetClientRect(CCListBox* pListBox, sRect& r)
{
	//int lw = GETWIDTH(m_pFrameBitmaps[3]);
	//int rw = GETWIDTH(m_pFrameBitmaps[5]);
	//int uh = GETHEIGHT(m_pFrameBitmaps[7]);
	//int bh = GETHEIGHT(m_pFrameBitmaps[1]);
	//return sRect(r.x+lw, r.y+uh, r.w-(lw+rw)-((pListBox->GetScrollBar()->IsVisible()==true)?(pListBox->GetScrollBar()->GetClientRect().w):0), r.h-(uh+bh));
	return sRect(r.x+3, r.y+3, r.w-(6)-((pListBox->GetScrollBar()->IsVisible()==true)?(pListBox->GetScrollBar()->GetClientRect().w):0), r.h-(6));
}

void CCBListBoxLook::OnDraw(CCListBox* pListBox, CCDrawContext* pDC) 
{
	if(m_pFont!=NULL) pDC->SetFont(m_pFont);

	CCListBoxLook::OnDraw(pListBox,pDC);
}
