#include "stdafx.h"

#include "ZInterface.h"
#include "CCListBox.h"
#include "CCFormulaTable.h"
#include "CCEdit.h"
//#include "ZPost.h"

ZInterface::ZInterface(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCWidget(szName,pParent,pListener)
{
}

ZInterface::~ZInterface()
{
}

bool ZInterface::IsDone()
{
	return false;
}

bool ZInterface::OnCreate()
{
	return false;
}

void ZInterface::OnDraw(CCDrawContext* pDC)
{
}

void ZInterface::OnDestroy()
{
}
