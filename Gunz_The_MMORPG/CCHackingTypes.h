#pragma once

#include "MSingleton.h"
#include "MUtil.h"

// 여기에 하나 추가 하시면 아래 응징 테이블도 하나 추가하셔야 합니다
enum CCMatchHackingType
{
	CCMHT_NO = 0,	
	CCMHT_XTRAP_HACKER,
	CCMHT_HSHIELD_HACKER,
	CCMHT_BADFILECRC,
	CCMHT_BADUSER,
	CCMHT_GAMEGUARD_HACKER,
	CCMHT_GIVE_ONESELF_UP_DLLINJECTION,
	CCMHT_INVALIDSTAGESETTING,
	CCMHT_COMMAND_FLOODING,
	CCMHT_COMMAND_BLOCK_BY_ADMIN,
	CCMHT_SLEEP_ACCOUNT = 10,					///< 일단은 NHN 요청에 따른 휴명 계정 처리이다. (절대로 10이어야 한다. 바뀌어선 안된다)
	CCMHT_END,
};

struct PUNISH_TABLE_ITEM {
	DWORD				dwMessageID;
	const char*			szComment;

	u_short				nDay;
	u_short				nHour;
	u_short				nMin;
	
	CCMatchBlockLevel	eLevel;
};

class MPunishTable {
	static const PUNISH_TABLE_ITEM PUNISH_TABLE[CCMHT_END];
public:
	static const PUNISH_TABLE_ITEM& GetPunish( CCMatchHackingType eType );
};