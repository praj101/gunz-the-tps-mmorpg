#pragma once

#include "CCPopupMenu.h"
#include "CCUID.h"


enum ZCMD_ITEMMENU {
	ZCMD_ITEMMENU_BRINGBACK_ACCOUNTITEM,
	ZCMD_ITEMMENU_END
};


class ZItemMenuItem : public CCMenuItem {
protected:
	ZCMD_ITEMMENU		m_nCmdID;
public:
	ZItemMenuItem(ZCMD_ITEMMENU nCmdID, const char* szName=NULL);
	ZCMD_ITEMMENU GetCmdID() { return m_nCmdID; }
};


class ZItemMenu : public CCPopupMenu {
protected:
	char	m_szItemName[128];
	CCUID	m_IteCCUID;

public:
	ZItemMenu(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL, CCPopupMenuTypes t=CCPMT_VERTICAL);
	void AddMenuItem(ZItemMenuItem* pMenuItem);

	const char* GetTargetName()	{ return m_szItemName; }
	void SetTargetName(const char* pszItemName) { strcpy(m_szItemName, pszItemName); }
	const CCUID& GetTargetUID()	{ return m_IteCCUID; }
	void SetTargetUID(const CCUID& uidTarget)	{ m_IteCCUID = uidTarget; }

	void SetupMenu();
	virtual void Show(int x, int y, bool bVisible=true);
};


class ZItemMenuListener :	public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage);
};
