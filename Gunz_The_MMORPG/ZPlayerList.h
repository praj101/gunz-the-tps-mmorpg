#ifndef _ZPLAYERLIST_H
#define _ZPLAYERLIST_H

#include "CCUID.h"
#include "CCMatchGlobal.h"
#include "CCMatchObject.h"

struct ZChannelPlayerListNode
{
	CCUID				uidPlayer;
	CCMatchUserGradeID	nGradeID;
	CCMatchPlace			nPlace;
	char				szName[MATCHOBJECT_NAME_LENGTH];
	char				szClanName[CLAN_NAME_LENGTH];
	int					nLevel;
};

struct ZClanMemberListNode
{
	CCUID				uidPlayer;
	char				szName[MATCHOBJECT_NAME_LENGTH];
	int					nLevel;
	CCMatchClanGrade		nClanGrade;
	CCMatchPlace			nPlace;
};

struct ZFriendListNode
{
	unsigned char	nState;
	char			szName[MATCHOBJECT_NAME_LENGTH];
	char			szDescription[MATCH_SIMPLE_DESC_LENGTH];
};



#endif