#ifndef _ZPLAYERLIST_H
#define _ZPLAYERLIST_H

#include "CCUID.h"
#include "MMatchGlobal.h"
#include "MMatchObject.h"

struct ZChannelPlayerListNode
{
	CCUID				uidPlayer;
	MMatchUserGradeID	nGradeID;
	MMatchPlace			nPlace;
	char				szName[MATCHOBJECT_NAME_LENGTH];
	char				szClanName[CLAN_NAME_LENGTH];
	int					nLevel;
};

struct ZClanMemberListNode
{
	CCUID				uidPlayer;
	char				szName[MATCHOBJECT_NAME_LENGTH];
	int					nLevel;
	MMatchClanGrade		nClanGrade;
	MMatchPlace			nPlace;
};

struct ZFriendListNode
{
	unsigned char	nState;
	char			szName[MATCHOBJECT_NAME_LENGTH];
	char			szDescription[MATCH_SIMPLE_DESC_LENGTH];
};



#endif