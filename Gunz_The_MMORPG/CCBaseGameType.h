#pragma once
#include <crtdbg.h>
#include <map>
#include <set>
using namespace std;


/// 게임 타입
enum CCMATCH_GAMETYPE {
	CCMATCH_GAMETYPE_DEATHMATCH_SOLO		=0,			///< 개인 데쓰매치
	CCMATCH_GAMETYPE_DEATHMATCH_TEAM		=1,			///< 팀 데쓰매치
	CCMATCH_GAMETYPE_GLADIATOR_SOLO		=2,			///< 개인 글래디에이터
	CCMATCH_GAMETYPE_GLADIATOR_TEAM		=3,			///< 팀 글래디에이터
	CCMATCH_GAMETYPE_ASSASSINATE			=4,			///< 보스전(암살전)
	CCMATCH_GAMETYPE_TRAINING			=5,			///< 연습

	CCMATCH_GAMETYPE_SURVIVAL			=6,			///< 서바이벌
	CCMATCH_GAMETYPE_QUEST				=7,			///< 퀘스트

	CCMATCH_GAMETYPE_BERSERKER			=8,			///< 데쓰매치 버서커
	CCMATCH_GAMETYPE_DEATHMATCH_TEAM2	=9,			///< 팀데쓰매치 익스트림
	CCMATCH_GAMETYPE_DUEL				=10,		///< 듀얼 매치
	CCMATCH_GAMETYPE_DUELTOURNAMENT		=11,		///< 듀얼 토너먼트
/*
#ifndef _CLASSIC
	CCMATCH_GAMETYPE_CLASSIC_SOLO,
	CCMATCH_GAMETYPE_CLASSIC_TEAM,
#endif
*/
	CCMATCH_GAMETYPE_MAX,
};
// 모드가 추가되면 리플레이 저장용 모드 약어도 추가해야 한다 (cpp)
extern const char* CCMatchGameTypeAcronym[CCMATCH_GAMETYPE_MAX];

#define		MAX_RELAYMAP_LIST_COUNT			20
struct RelayMap
{
	int				nMapID;
};

enum RELAY_MAP_TYPE
{
	RELAY_MAP_TURN			= 0,
	RELAY_MAP_RANDOM,

	RELAY_MAP_MAX_TYPE_COUNT
};

enum RELAY_MAP_REPEAT_COUNT
{
	RELAY_MAP_1REPEAT			= 0,
	RELAY_MAP_2REPEAT,
	RELAY_MAP_3REPEAT,
	RELAY_MAP_4REPEAT,
	RELAY_MAP_5REPEAT,

	RELAYMAP_MAX_REPEAT_COUNT,
};

// 디폴트 게임 타입
const CCMATCH_GAMETYPE CCMATCH_GAMETYPE_DEFAULT = CCMATCH_GAMETYPE_DEATHMATCH_SOLO;
// const CCMATCH_GAMETYPE CCMATCH_GAMETYPE_DEFAULT = CCMATCH_GAMETYPE_DEATHMATCH_TEAM2;


struct CCMatchGameTypeInfo
{
	CCMATCH_GAMETYPE		nGameTypeID;			// ID
	char				szGameTypeStr[24];		// 게임 타입 이름
	float				fGameExpRatio;			// 경험치 배분 비율 - 현재 Gladiator만 50%이다
	float				fTeamMyExpRatio;		// 팀전에서 개인에게 적용되는 경험치
	float				fTeamBonusExpRatio;		// 팀전에서 팀에게 적립되는 경험치
	set<int>			MapSet;					// 이 게임타입에서 플레이 가능한 맵
	void Set(const CCMATCH_GAMETYPE a_nGameTypeID, const char* a_szGameTypeStr, const float a_fGameExpRatio,
		     const float a_fTeamMyExpRatio, const float a_fTeamBonusExpRatio);
	void AddMap(int nMapID);
	void AddAllMap();
};


class CCBaseGameTypeCatalogue
{
private:
	CCMatchGameTypeInfo			m_GameTypeInfo[CCMATCH_GAMETYPE_MAX];
public:
	CCBaseGameTypeCatalogue();
	virtual ~CCBaseGameTypeCatalogue();

	inline CCMatchGameTypeInfo* GetInfo(CCMATCH_GAMETYPE nGameType);
	inline const char* GetGameTypeStr(CCMATCH_GAMETYPE nGameType);
	inline void SetGameTypeStr(CCMATCH_GAMETYPE nGameType, const char* szName);
	inline bool IsCorrectGameType(const int nGameTypeID);	///< 올바른 게임 타입 ID인지 여부
	inline bool IsTeamGame(CCMATCH_GAMETYPE nGameType);		///< 해당 게임타입이 팀전인지 여부
	inline bool IsTeamLimitTime(CCMATCH_GAMETYPE nGameType);
	inline bool IsWaitForRoundEnd(CCMATCH_GAMETYPE nGameType);		///< 라운드 끝날때까지 대기모드 해야하는지 여부
	inline bool IsQuestOnly(CCMATCH_GAMETYPE nGameType);	///< 해당 게임타입이 퀘스트인지 여부
	inline bool IsSurvivalOnly(CCMATCH_GAMETYPE nGameType); ///< 해당 게임타입이 서바이벌인지 여부
	inline bool IsQuestDerived(CCMATCH_GAMETYPE nGameType);	///< 해당 게임타입이 퀘스트 관련(NPC가 나오는지)인지 여부
	inline bool IsWorldItemSpawnEnable(CCMATCH_GAMETYPE nGameType);	///< 해당 게임타입에서 월드아이템이 스폰되는지 여부
};


//////////////////////////////////////////////////////////////////////////////////
inline bool CCBaseGameTypeCatalogue::IsTeamGame(CCMATCH_GAMETYPE nGameType)
{
	// 좀더 복잡해지면 Description에 teamgame인지 여부값을 넣도록 하자.

	if ((nGameType == CCMATCH_GAMETYPE_DEATHMATCH_TEAM) ||
		(nGameType == CCMATCH_GAMETYPE_DEATHMATCH_TEAM2) ||
		(nGameType == CCMATCH_GAMETYPE_GLADIATOR_TEAM) ||
		(nGameType == CCMATCH_GAMETYPE_ASSASSINATE) )
	{
		return true;
	}
	return false;
}

inline bool CCBaseGameTypeCatalogue::IsTeamLimitTime(CCMATCH_GAMETYPE nGameType)
{
	// 좀더 복잡해지면 Description에 teamgame인지 여부값을 넣도록 하자.

	if ((nGameType == CCMATCH_GAMETYPE_DEATHMATCH_TEAM) ||
		(nGameType == CCMATCH_GAMETYPE_GLADIATOR_TEAM) ||
		(nGameType == CCMATCH_GAMETYPE_DUEL) ||
		(nGameType == CCMATCH_GAMETYPE_ASSASSINATE) )
	{
		return true;
	}
	return false;
}

inline bool CCBaseGameTypeCatalogue::IsWaitForRoundEnd(CCMATCH_GAMETYPE nGameType)
{
	// 라운드 끝날때까지 대기모드 하는가?

	if ((nGameType == CCMATCH_GAMETYPE_DEATHMATCH_TEAM) ||
		(nGameType == CCMATCH_GAMETYPE_DUEL) ||
		(nGameType == CCMATCH_GAMETYPE_GLADIATOR_TEAM) ||
		(nGameType == CCMATCH_GAMETYPE_ASSASSINATE) ||
		(nGameType == CCMATCH_GAMETYPE_DUELTOURNAMENT))
	{
		return true;
	}
	return false;
}


inline bool CCBaseGameTypeCatalogue::IsQuestDerived(CCMATCH_GAMETYPE nGameType)
{
	if ( (nGameType == CCMATCH_GAMETYPE_SURVIVAL) ||(nGameType == CCMATCH_GAMETYPE_QUEST) )
	{
		return true;
	}

	return false;
}

inline bool CCBaseGameTypeCatalogue::IsQuestOnly(CCMATCH_GAMETYPE nGameType)
{
	return nGameType == CCMATCH_GAMETYPE_QUEST;
}

inline bool CCBaseGameTypeCatalogue::IsSurvivalOnly(CCMATCH_GAMETYPE nGameType)
{
	return nGameType == CCMATCH_GAMETYPE_SURVIVAL;
}

inline const char* CCBaseGameTypeCatalogue::GetGameTypeStr(CCMATCH_GAMETYPE nGameType)
{
	return m_GameTypeInfo[nGameType].szGameTypeStr;
}

inline void CCBaseGameTypeCatalogue::SetGameTypeStr(CCMATCH_GAMETYPE nGameType, const char* szName)
{
	strcpy( m_GameTypeInfo[nGameType].szGameTypeStr, szName) ;
}

bool CCBaseGameTypeCatalogue::IsCorrectGameType(const int nGameTypeID)
{
	if ((nGameTypeID < 0) || (nGameTypeID >= CCMATCH_GAMETYPE_MAX)) return false;
	return true;
}

inline CCMatchGameTypeInfo* CCBaseGameTypeCatalogue::GetInfo(CCMATCH_GAMETYPE nGameType)
{
	_ASSERT((nGameType >= 0) && (nGameType < CCMATCH_GAMETYPE_MAX));
	return &m_GameTypeInfo[nGameType];
}

inline bool CCBaseGameTypeCatalogue::IsWorldItemSpawnEnable(CCMATCH_GAMETYPE nGameType)
{
	if ( (nGameType == CCMATCH_GAMETYPE_SURVIVAL) ||(nGameType == CCMATCH_GAMETYPE_QUEST) )
	{
		return false;
	}

	return true;

}