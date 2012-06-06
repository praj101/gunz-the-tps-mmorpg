#include "stdafx.h"
#include "CCGroup.h"
#include "CCColorTable.h"

void CCGroupLook::OnDraw(CCGroup* pGroup, CCDrawContext* pDC)
{
	sRect r = pGroup->GetInitialClientRect();
	pDC->SetColor(sColor(DEFCOLOR_FRAME_PLANE));
	pDC->FillRectangle(r);
	pDC->SetColor(sColor(DEFCOLOR_FRAME_OUTLINE));
	pDC->Rectangle(r);

	pDC->SetColor(sColor(DEFCOLOR_FRAME_TEXT));
	pDC->Text(r.x+2, r.y+2, pGroup->m_szName);
}

sRect CCGroupLook::GetClientRect(CCGroup* pGroup, sRect& r)
{
	return r;
}


IMPLEMENT_LOOK(CCGroup, CCGroupLook)

CCGroup::CCGroup(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCWidget(szName, pParent, pListener)
{
	LOOK_IN_CONSTRUCTOR()
}

CCGroup::~CCGroup(void)
{
}
