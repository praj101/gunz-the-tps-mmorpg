#pragma once

#include "MPopupMenu.h"
#include "CCUID.h"


enum ZCMD_ITEMMENU {
	ZCMD_ITEMMENU_BRINGBACK_ACCOUNTITEM,
	ZCMD_ITEMMENU_END
};


class ZItemMenuItem : public MMenuItem {
protected:
	ZCMD_ITEMMENU		m_nCmdID;
public:
	ZItemMenuItem(ZCMD_ITEMMENU nCmdID, const char* szName=NULL);
	ZCMD_ITEMMENU GetCmdID() { return m_nCmdID; }
};


class ZItemMenu : public MPopupMenu {
protected:
	char	m_szItemName[128];
	CCUID	m_ItemUID;

public:
	ZItemMenu(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL, MPopupMenuTypes t=MPMT_VERTICAL);
	void AddMenuItem(ZItemMenuItem* pMenuItem);

	const char* GetTargetName()	{ return m_szItemName; }
	void SetTargetName(const char* pszItemName) { strcpy(m_szItemName, pszItemName); }
	const CCUID& GetTargetUID()	{ return m_ItemUID; }
	void SetTargetUID(const CCUID& uidTarget)	{ m_ItemUID = uidTarget; }

	void SetupMenu();
	virtual void Show(int x, int y, bool bVisible=true);
};


class ZItemMenuListener :	public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage);
};
