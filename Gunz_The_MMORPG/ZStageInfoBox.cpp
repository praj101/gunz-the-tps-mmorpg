#include "stdafx.h"

#include "ZStageInfoBox.h"
#include "CCBitmapDrawer.h"

//IMPLEMENT_LOOK(ZStageInfoBox, ZStageInfoBoxLook)

ZStageInfoBox::ZStageInfoBox(const char* szName, MWidget* pParent, MListener* pListener)
: MListBox(szName, pParent, pListener)
{
//	LOOK_IN_CONSTRUCTOR()

	SetItemHeight(GetItemHeight()*3);
	SetAlwaysVisibleScrollbar(true);

	m_pLook=NULL;
}

ZStageInfoBox::~ZStageInfoBox()
{
}

void ZStageInfoBoxLook::OnDraw(ZStageInfoBox* pBox, CCDrawContext* pDC)
{
	MListBoxLook::OnDraw(pBox,pDC);
}

void ZStageInfoBox::OnDraw(CCDrawContext* pDC)
{
	if(m_pLook)
	{
		sRect r = GetInitialClientRect();

		int nShowCount=0;
		for(int i=GetStartItem(); i<GetCount(); i++){

			nShowCount++;

			if(nShowCount>=GetShowItemCount()) break;

			sRect itemrect=sRect(r.x,r.y+GetItemHeight()*(nShowCount-1),r.w,GetItemHeight());
			DrawBitmapFrame9(pDC, itemrect, m_pLook->m_pFrameBitmaps);
		}
	}
	
	MListBox::OnDraw(pDC);
}
