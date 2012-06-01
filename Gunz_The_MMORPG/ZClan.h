#ifndef _ZCLAN_H
#define _ZCLAN_H

#include "ZPrerequisites.h"
#include "CCMatchClan.h"
#include "ZMessages.h"

#include "ZApplication.h"
#include "ZLanguageConf.h"

// Added R347a
#include "ZStringResManager.h"

inline const char* ZGetClanGradeStr(const CCMatchClanGrade nGrade)
{
	switch (nGrade)
	{
	case CCG_NONE:		
		return ZMsg(MSG_WORD_CLAN_NONE); break;

	case CCG_MASTER:	
		return ZMsg(MSG_WORD_CLAN_MASTER); break;

	case CCG_ADMIN:		
		return ZMsg(MSG_WORD_CLAN_ADMIN); break;

	case CCG_MEMBER:	
		return ZMsg(MSG_WORD_CLAN_MEMBER); break;

	default:
		return "";
	}

}


class ZClan
{
private:
public:
	ZClan();
	virtual ~ZClan();
public:
};


/*
	case ZCMD_PLAYERMENU_CLAN_INVITE:
		ZPostRequestJoinClan(ZGetMyUID(),ZGetMyInfo()->GetClanName(),pMenu->GetTargetName());
		return true;

	case ZCMD_PLAYERMENU_CLAN_KICK:
		ZPostRequestExpelClanMember(ZGetMyUID(),pMenu->GetTargetName());
		return true;

	case ZCMD_PLAYERMENU_CLAN_GRADE_MASTER:
		ZPostRequestChangeClanGrade(ZGetMyUID(),pMenu->GetTargetName(),CCG_MASTER);
		return true;

	case ZCMD_PLAYERMENU_CLAN_GRADE_ADMIN:
		ZPostRequestChangeClanGrade(ZGetMyUID(),pMenu->GetTargetName(),CCG_ADMIN);
		return true;

	case ZCMD_PLAYERMENU_CLAN_GRADE_MEMBER:
		ZPostRequestChangeClanGrade(ZGetMyUID(),pMenu->GetTargetName(),CCG_MEMBER);
		return true;

	case ZCMD_PLAYERMENU_CLAN_LEAVE:
		ZPostRequestLeaveClan(ZGetMyUID());
		return true;
*/

#endif