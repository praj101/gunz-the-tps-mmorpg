#include "stdafx.h"

#include "ZApplication.h"
#include "ZInterfaceListener.h"
#include "CCWidget.h"
#include "CCEdit.h"
#include "CCComboBox.h"
#include "ZConfiguration.h"
#include "CCSlider.h"
#include "CCDebug.h"
#include "ZMsgBox.h"
#include "ZActionKey.h"
#include "CCTabCtrl.h"
#include "ZApplication.h"
#include "ZStringResManager.h"
#include "CCBmButton.h"

#ifdef LOCALE_NHNUSA
#include "ZNHN_USA_Report.h"
#endif

class CCLoginListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, CCBTN_CLK_MSG)==true)
		{
			return false;
		}
	}
};
CCLoginListener	g_LoginListener;


class MExitListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, CCBTN_CLK_MSG)==true){

			cclog("MExitListener !\n");
			ZApplication::Exit();

			return true;
		}
		return false;
	}
};
MExitListener	g_ExitListener;

class MParentCloseListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, CCBTN_CLK_MSG)==true){
			if(pWidget->GetParent()!=NULL) pWidget->GetParent()->Show(false);
			return true;
		}
		return false;
	}
};
MParentCloseListener	g_ParentCloseListener;




CCListener* ZGetLoginListener()
{
	return &g_LoginListener;
}

CCListener* ZGetExitListener()
{
	return &g_ExitListener;
}


CCListener* ZGetParentCloseListener()
{
	return &g_ParentCloseListener;
}

