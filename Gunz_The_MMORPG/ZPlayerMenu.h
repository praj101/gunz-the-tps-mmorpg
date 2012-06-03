#pragma once

#include "CCPopupMenu.h"
#include "CCUID.h"


enum ZPLAYERMENU_SET {
	ZPLAYERMENU_SET_LOBBY,
	ZPLAYERMENU_SET_STAGE,
	ZPLAYERMENU_SET_FRIEND,
	ZPLAYERMENU_SET_CLAN,
	ZPLAYERMENU_SET_CLAN_ME,	// 클랜에서 나를 찍었을때
};

enum ZCMD_PLAYERMENU {

	ZCMD_PLAYERMENU_WHERE,
	ZCMD_PLAYERMENU_WHISPER,
	ZCMD_PLAYERMENU_FOLLOW,
	ZCMD_PLAYERMENU_KICK,

	ZCMD_PLAYERMENU_FRIEND_ADD,
	ZCMD_PLAYERMENU_FRIEND_REMOVE,
	ZCMD_PLAYERMENU_FRIEND_PROMOTE,
	ZCMD_PLAYERMENU_FRIEND_DEMOTE,
	ZCMD_PLAYERMENU_FRIEND_FOLLOW,

	ZCMD_PLAYERMENU_CLAN_INVITE,
	ZCMD_PLAYERMENU_CLAN_KICK,
	ZCMD_PLAYERMENU_CLAN_GRADE_MASTER,
	ZCMD_PLAYERMENU_CLAN_GRADE_ADMIN,
	ZCMD_PLAYERMENU_CLAN_GRADE_MEMBER,
	ZCMD_PLAYERMENU_CLAN_LEAVE,
	ZCMD_PLAYERMENU_CLAN_FOLLOW,

	ZCMD_PLAYERMENU_END
};


class ZPlayerMenuItem : public CCMenuItem {
protected:
	ZCMD_PLAYERMENU		m_nCmdID;
public:
	ZPlayerMenuItem(ZCMD_PLAYERMENU nCmdID, const char* szName=NULL);
	ZCMD_PLAYERMENU GetCmdID() { return m_nCmdID; }
};


class ZPlayerMenu : public CCPopupMenu {
protected:
	char	m_szPlayerName[128];
	CCUID	m_PlayerUID;

public:
	ZPlayerMenu(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL, CCPopupMenuTypes t=CCPMT_VERTICAL);
	void AddMenuItem(ZPlayerMenuItem* pMenuItem);

	const char* GetTargetName()	{ return m_szPlayerName; }
	void SetTargetName(const char* pszPlayerName) { strcpy(m_szPlayerName, pszPlayerName); }
	const CCUID& GetTargetUID()	{ return m_PlayerUID; }
	void SetTargetUID(const CCUID& uidTarget)	{ m_PlayerUID = uidTarget; }

	void SetupMenu(ZPLAYERMENU_SET nMenuSet);
	virtual void Show(int x, int y, bool bVisible=true);
};


class ZPlayerMenuListener :	public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage);
};
