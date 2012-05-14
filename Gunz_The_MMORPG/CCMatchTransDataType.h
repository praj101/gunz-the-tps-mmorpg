#ifndef _CCMATCHTRANSDATATYPE_H
#define _CCMATCHTRANSDATATYPE_H

#include "CCMatchObject.h"
#include "CCMatchRule.h"		// CCMATCH_GAMETYPE, CCMATCH_ROUNDSTATE, CCMATCH_ROUNDRESULT 참조 -> 정리요망
#include "CCMatchStageSetting.h"
#include "CCMatchGameType.h"
#include "CCMatchGlobal.h"

#pragma pack(push, old)
#pragma pack(1)

// 내 캐릭터 리스트 정보 - 캐릭터 선택시 사용
struct CCTD_AccountCharInfo
{
	char				szName[MATCHOBJECT_NAME_LENGTH];
	char				nCharNum;
	unsigned char		nLevel;
};

struct CCTD_CharInfo
{
	// ＃이 구조체의 내용을 변경하려면 기존 리플레이의 로딩을 위해서 수정 전의 구조체를 ZReplay.cpp에 보존하고
	// ＃버전별 로딩 코드를 작성해줘야 합니다. 변수의 추가는 가급적 마지막에 덧붙이는 편이 그나마 수월합니다.

	// 캐릭터 정보
	char				szName[MATCHOBJECT_NAME_LENGTH];
	char				szClanName[CLAN_NAME_LENGTH];
	CCMatchClanGrade		nClanGrade;
	unsigned short		nClanContPoint;
	char				nCharNum;
	unsigned short		nLevel;
	char				nSex;
	char				nHair;
	char				nFace;
	unsigned long int	nXP;
	int					nBP;
	float				fBonusRate;
	unsigned short		nPrize;
	unsigned short		nHP;
	unsigned short		nAP;
	unsigned short		nMaxWeight;
	unsigned short		nSafeFalls;
	unsigned short		nFR;
	unsigned short		nCR;
	unsigned short		nER;
	unsigned short		nWR;

	// 아이템 정보
	unsigned long int	nEquipedItemDesc[MMCIP_END];

	// account 의 정보
	CCMatchUserGradeID	nUGradeID;

	// ClanCLID
	unsigned int		nClanCLID;

	// 지난주 듀얼토너먼트 등급
	int					nDTLastWeekGrade;	

	// 아이템 정보 추가
	CCUID				uidEquipedItem[MMCIP_END];
	unsigned long int	nEquipedItemCount[MMCIP_END];
};

//버프정보임시주석 
/*
struct CCTD_CharBuffInfo
{
	CCShortBuffInfo	ShortBuffInfo[MAX_CHARACTER_SHORT_BUFF_COUNT];	
};*/

struct CCTD_BuffInfo
{
	unsigned long int	nItemId;		// 버프를 일으킨 아이템ID
	unsigned short		nRemainedTime;	// 버프 몇초 남았나 (초단위로 절사), 아이템에 따라서는 횟수가 담길 수도 있다
};

// 내 캐릭터 정보의 추가 정보
struct CCTD_MyExtraCharInfo
{
	char	nLevelPercent;		///< 현재 경험치 습득 퍼센트
};

// 현재 사용하지 않는다.
struct CCTD_SimpleCharInfo
{
	char				szName[32];
	char				nLevel;
	char				nSex;
	char				nHair;
	char				nFace;
	unsigned long int	nEquipedItemDesc[MMCIP_END];
};


struct CCTD_MySimpleCharInfo
{
	unsigned char		nLevel;
	unsigned long int	nXP;
	int					nBP;
};

struct CCTD_CharLevelInfo
{
	unsigned char		nLevel;
	unsigned long int	nCurrLevelExp;
	unsigned long int	nNextLevelExp;
};

struct CCTD_RoundPeerInfo
{
	CCUID			uidChar;
	unsigned char	nHP;
	unsigned char	nAP;
};

struct CCTD_RoundKillInfo
{
	CCUID	uidAttacker;
	CCUID	uidVictim;
};

struct CCTD_ItemNode
{
	CCUID				uidItem;
	unsigned long int	nItemID;
	int					nRentMinutePeriodRemainder;		// 기간제 아이템 사용가능시간(초단위), RENT_MINUTE_PERIOD_UNLIMITED이면 무제한
	int					iMaxUseHour;					// 최대 사용 시간(시간 단위)
	int					nCount;
};

struct CCTD_RelayMap
{
	int				nMapID;
};

struct CCTD_AccountItemNode
{
	int					nAIID;
	unsigned long int	nItemID;
	int					nRentMinutePeriodRemainder;		// 기간제 아이템 사용가능시간(초단위), RENT_MINUTE_PERIOD_UNLIMITED이면 무제한
	int					nCount;
};

// 게임안 상태 정보
struct CCTD_GameInfoPlayerItem
{
	CCUID	uidPlayer;
	bool	bAlive;
	int		nKillCount;
	int		nDeathCount;
};

struct CCTD_GameInfo
{
	char	nRedTeamScore;		// 팀전에서만 사용하는 레드팀정보
	char	nBlueTeamScore;		// 팀전에서만 사용하는 블루팀정보

	short	nRedTeamKills;		// 무한팀데스매치에서만 사용하는 레드팀킬수
	short	nBlueTeamKills;		// 무한팀데스매치에서만 사용하는 블루팀킬수
};

struct CCTD_RuleInfo	
{
	unsigned char	nRuleType;
};

struct CCTD_RuleInfo_Assassinate : public CCTD_RuleInfo
{
	CCUID	uidRedCommander;
	CCUID	uidBlueCommander;
};

struct CCTD_RuleInfo_Berserker : public CCTD_RuleInfo
{
	CCUID	uidBerserker;
};


enum CCTD_PlayerFlags {
	CCTD_PlayerFlags_AdminHide	= 1,
	CCTD_PlayerFlags_BridgePeer	= 1<<1,
	CCTD_PlayerFlags_Premium		= 1<<2			// 넷마블 피시방 보너스
};

struct CCTD_ChannelPlayerListNode 
{
	CCUID			uidPlayer;
	char			szName[MATCHOBJECT_NAME_LENGTH];
	char			szClanName[CLAN_NAME_LENGTH];
	char			nLevel;
	char			nDTLastWeekGrade;
	CCMatchPlace		nPlace;
	unsigned char	nGrade;			// 로비에서는 uid 로 캐릭터의 등급을 알수가 없어서..
	unsigned char	nPlayerFlags;	// 플레이어 속성(운영자숨김등) - CCTD_PlayerFlags 사용
	unsigned int	nCLID;			// ClanID
	unsigned int	nEmblemChecksum;// Emblem Checksum
};


struct CCTD_ClanMemberListNode 
{
	CCUID				uidPlayer;
	char				szName[MATCHOBJECT_NAME_LENGTH];
	char				nLevel;
	CCMatchClanGrade		nClanGrade;
	CCMatchPlace			nPlace;
};

enum CCTD_WorldItemSubType
{
	CCTD_Dynamic = 0,
	CCTD_Static  = 1,
};

// 아이템 스폰 정보
struct CCTD_WorldItem
{
	unsigned short	nUID;
	unsigned short	nItemID;
	unsigned short  nItemSubType;
	short			x;
	short			y;
	short			z;
/*
	float			x;
	float			y;
	float			z;
*/
};


// 발동된 트랩 정보
struct CCTD_ActivatedTrap
{
	CCUID				uidOwner;
	unsigned short		nItemID;
	unsigned long int	nTimeElapsed;
	short	x;
	short	y;
	short	z;
};


// 바로게임하기 필터링 정보
struct CCTD_QuickJoinParam
{
	unsigned long int	nMapEnum;		// 원하는 맵의 비트어레이
	unsigned long int	nModeEnum;		// 윈하는 게임모드의 비트어레이
};


// 캐릭터의 클랜 업데이트 정보
struct CCTD_CharClanInfo
{
	char				szClanName[CLAN_NAME_LENGTH];		// 클랜 이름
	CCMatchClanGrade		nGrade;
};


// 유저 정보보기
struct CCTD_CharInfo_Detail
{
	char				szName[32];						// 이름
	char				szClanName[CLAN_NAME_LENGTH];	// 클랜이름
	CCMatchClanGrade		nClanGrade;						// 클랜직책
	int					nClanContPoint;					// 클랜 기여도
	unsigned short		nLevel;							// 레벨
	char				nSex;							// 성별
	char				nHair;							// 머리 코스츔
	char				nFace;							// 얼굴 코스츔
	unsigned long int	nXP;							// xp
	int					nBP;							// bp

	int					nKillCount;
	int					nDeathCount;

	// 접속상황

	unsigned long int	nTotalPlayTimeSec;				// 총 플레이 시간
	unsigned long int	nConnPlayTimeSec;				// 현재 접속 시간


	unsigned long int	nEquipedItemDesc[MMCIP_END];	// 아이템 정보

	CCMatchUserGradeID	nUGradeID;						// account UGrade

	// ClanCLID
	unsigned int		nClanCLID;
};


/// 방 리스트 달라고 요청할때 보내는 구조체
struct CCTD_StageListNode
{
	CCUID			uidStage;							///< 방 UID
	unsigned char	nNo;								///< 방번호
	char			szStageName[STAGENAME_LENGTH];		///< 방이름
	char			nPlayers;							///< 현재인원
	char			nMaxPlayers;						///< 최대인원
	STAGE_STATE		nState;								///< 현재상태
	CCMATCH_GAMETYPE nGameType;							///< 게임 타입
	char			nMapIndex;							///< 맵
	int				nSettingFlag;						///< 방 세팅 플래그(난입, 비밀방, 레벨제한)
	char			nMasterLevel;						///< 방장 레벨
	char			nLimitLevel;						///< 제한레벨
};

/// 클라이언트가 알아야할 기타정보 : AdminHide 상태를 명시적으로 교환 & 녹화파일에 저장
struct CCTD_ExtendInfo
{
	char			nTeam;
	unsigned char	nPlayerFlags;	// 플레이어 속성(운영자숨김등) - CCTD_PlayerFlags 사용
	unsigned char	nReserved1;		// 여분
	unsigned char	nReserved2;
};

struct CCTD_PeerListNode
{
	CCUID				uidChar;
	DWORD				dwIP;
	unsigned int		nPort;
	CCTD_CharInfo		CharInfo;
	//버프정보임시주석 CCTD_CharBuffInfo	CharBuffInfo;
	CCTD_ExtendInfo		ExtendInfo;
};


// 동의 답변자
struct CCTD_ReplierNode
{
	char szName[MATCHOBJECT_NAME_LENGTH];
};


// 래더 게임 신청 팀 그룹
struct CCTD_LadderTeamMemberNode
{
	char szName[MATCHOBJECT_NAME_LENGTH];

};

// 클랜 정보
struct CCTD_ClanInfo
{
	char				szClanName[CLAN_NAME_LENGTH];		// 클랜 이름
	short				nLevel;								// 레벨
	int					nPoint;								// 포인트
	int					nTotalPoint;						// 토탈포인트
	int					nRanking;							// 랭킹
	char				szMaster[MATCHOBJECT_NAME_LENGTH];	// 클랜 마스터
	unsigned short		nWins;								// 전적 - 승수
	unsigned short		nLosses;							// 전적 - 패수
	unsigned short		nTotalMemberCount;					// 전체 클랜원수
	unsigned short		nConnedMember;						// 현재 접속된 클랜원수
	unsigned int		nCLID;								// ClanID
	unsigned int		nEmblemChecksum;					// Emblem Checksum
};

// 클랜전 대기중인 클랜 리스트
struct CCTD_StandbyClanList
{
	char				szClanName[CLAN_NAME_LENGTH];		// 클랜 이름
	short				nPlayers;							// 대기중인 인원수
	short				nLevel;								// 레벨
	int					nRanking;							// 랭킹 - 0이면 unranked
	unsigned int		nCLID;								// ClanID
	unsigned int		nEmblemChecksum;					// Emblem Checksum
};


// 퀘스트, 서바이벌의 게임 정보
struct CCTD_QuestGameInfo
{
	unsigned short		nQL;												// 퀘스트 레벨
	float				fNPC_TC;											// NPC 난이도 조절 계수
	unsigned short		nNPCCount;											// 섹터당 등장할 NPC개수

	unsigned char		nNPCInfoCount;										// 등장할 NPC 종류 개수
	unsigned char		nNPCInfo[MAX_QUEST_NPC_INFO_COUNT];					// 등장할 NPC 정보
	unsigned short		nMapSectorCount;									// 맵 노드 개수
	unsigned short		nMapSectorID[MAX_QUEST_MAP_SECTOR_COUNT];			// 맵 노드 ID
	char				nMapSectorLinkIndex[MAX_QUEST_MAP_SECTOR_COUNT];	// 맵 노드의 Link Index
	unsigned char		nRepeat;											// 반복 횟수 (서바이벌용)
	CCMATCH_GAMETYPE		eGameType;											// 게임타입(퀘스트냐, 서바이벌이냐)
};

// 퀘스트, 보상 내용
struct CCTD_QuestReward
{
	CCUID				uidPlayer;	// 해당 플레이어 UID
	int					nXP;		// 해당 플레이어가 얻은 XP
	int					nBP;		// 해당 플레이어가 얻은 BP
};

// 퀘스트 아이템 보상 내용
struct CCTD_QuestItemNode
{
	int		m_nItemID;
	int		m_nCount;
};

// 퀘스트 일반 아이템 보상 내용
struct CCTD_QuestZItemNode
{
	unsigned int		m_nItemID;
	int					m_nRentPeriodHour;
	int					m_nItemCnt;
};


// 퀘스트에 사용할 NPC의 정보.
struct CCTD_NPCINFO
{
	BYTE	m_nNPCTID;
	WORD	m_nMaxHP;
	WORD	m_nMaxAP;
	BYTE	m_nInt;
	BYTE	m_nAgility;
	float	m_fAngle;
	float	m_fDyingTime;

	float	m_fCollisonRadius;
	float	m_fCollisonHight;

	BYTE	m_nAttackType;
	float	m_fAttackRange;
	DWORD	m_nWeaponItemID;
	float	m_fDefaultSpeed;
};

// 서바이벌 랭킹 정보
struct CCTD_SurvivalRanking
{
	char	m_szCharName[MATCHOBJECT_NAME_LENGTH];
	DWORD	m_dwPoint;
	DWORD	m_dwRank;		// 공동순위가 있을 수 있으므로 실제 랭크 순위값은 이것으로 사용

	CCTD_SurvivalRanking() : m_dwPoint(0), m_dwRank(0) { m_szCharName[0] = 0; }
};

#if defined(LOCALE_NHNUSA)
struct CCTD_ServerStatusInfo
{
	DWORD			m_dwIP;
	DWORD			m_dwAgentIP;
	int				m_nPort;
	unsigned char	m_nServerID;
	short			m_nMaxPlayer;
	short			m_nCurPlayer;
	char			m_nType;
	bool			m_bIsLive;
	char			m_szServerName[ 64 ];
};
#else
struct CCTD_ServerStatusInfo
{
	DWORD			m_dwIP;
	int				m_nPort;
	unsigned char	m_nServerID;
	short			m_nMaxPlayer;
	short			m_nCurPlayer;
	char			m_nType;
	bool			m_bIsLive;
	char			m_szServerName[ 64 ];
};
#endif

struct CCTD_ResetTeamMembersData
{
	CCUID			m_uidPlayer;		// 해당 플레이어
	char			nTeam;				// 팀
};


// 듀얼 큐 정보

struct CCTD_DuelQueueInfo
{
	CCUID			m_uidChampion;
	CCUID			m_uidChallenger;
	CCUID			m_WaitQueue[14];				// 팀
	char			m_nQueueLength;
	char			m_nVictory;						// 연승수
	bool			m_bIsRoundEnd;					// 라운드 끝날때인가
};

struct CCTD_DuelTournamentGameInfo
{
	CCUID			uidPlayer1;					// 진행할 게임의 참가할 Player1
	CCUID			uidPlayer2;					// 진행할 게임의 참가할 Player1
	int				nMatchType;					// 진행할 게임의 MatchType(
	int				nMatchNumber;				// 진행할 게임의 MatchNumber
	int				nRoundCount;				// 진행할 게임의 라운드수
	bool			bIsRoundEnd;				// 라운드가 종료되었는가에 대한 Flag(플레이어 이탈자 때문에..)
	char			nWaitPlayerListLength;		// 대기자 리스트의 Length
	byte			dummy[2];					// 4바이트씩 맞추기 위한 더미
	CCUID			WaitPlayerList[8];			// 대기자들의 CCUID
};

struct CCTD_DuelTournamentNextMatchPlayerInfo
{
	CCUID			uidPlayer1;					// 진행할 게임의 참가할 Player1
	CCUID			uidPlayer2;					// 진행할 게임의 참가할 Player1
};


struct CCTD_DuelTournamentRoundResultInfo
{
	CCUID			uidWinnerPlayer;			// 현재 라운드의 승리자
	CCUID			uidLoserPlayer;				// 현재 라운드의 패배자(180cm 이하...)
	bool			bIsTimeOut;					// 현재 라운드가 타임 아웃이었나?
	bool			bDraw;						// 현재 라운드가 비겼을 경우 true
	bool			bIsMatchFinish;				// 현재 라운드가 종료되면서, Match가 종료되었을 경우 true
	byte			dummy[2];					// 역시 더미

	///////////////////////////////////////////////////////////
	// Notice 
	// bDraw는 비겼을 때, True이다.
	// bIsMatchFinish는 Match가 종료되었을 때, True이다.
	// 다음과 같은 경우의 수가 존재할 수 있다.
	// bDraw = true,  bIsMatchFinish = true  => 플레이어 두명이 모두 이탈했을 경우
	// bDraw = true,  bIsMatchFinish = false => 동반 자살로 인하여 비겼을 경우
	// bDraw = false, bIsMatchFinish = true  => 그냥 한명이 이겼고, 매치가 끝났을 경우
	// bDraw = false, bIsMatchFinish = false => 그냥 한명이 이겼는데, 매치가 계속 진행되야될 경우(4강, 결승)
	///////////////////////////////////////////////////////////
};

struct CCTD_DuelTournamentMatchResultInfo
{
	int				nMatchNumber;
	int				nMatchType;
	CCUID			uidWinnerPlayer;			// 현재 라운드의 승리자
	CCUID			uidLoserPlayer;				// 현재 라운드의 패배자(180cm 이하...)
	int				nGainTP;
	int				nLoseTP;
};

#pragma pack(pop, old)


// admin 전용
enum ZAdminAnnounceType
{
	ZAAT_CHAT = 0,
	ZAAT_MSGBOX
};

// 겜블 아이템 데이터
struct CCTD_GambleItemNode
{
	CCUID			uidItem;
	unsigned int	nItemID;							// 아이템 ID
	unsigned int	nItemCnt;
};


// 겜블 아이템 데이터
struct CCTD_DBGambleItmeNode
{
	unsigned int	nItemID;							// 아이템 ID
	char			szName[ MAX_GAMBLEITEMNAME_LEN ];	// 아이템 이름
	char			szDesc[ MAX_GAMBLEITEMDESC_LEN ];	// 아이템 설명.
	int				nBuyPrice;							// 아이템 구입가격
	bool			bIsCash;							// Cash아이템 정보.
};

// Shop 아이템 데이터 - Added by 2010-03-18 홍기주
struct CCTD_ShopItemInfo
{
	unsigned int	nItemID;
	int				nItemCount;
};
/////////////////////////////////////////////////////////
void Make_MTDItemNode(CCTD_ItemNode* pout, CCUID& uidItem, unsigned long int nItemID, int nRentMinutePeriodRemainder, int iMaxUseHour, int nCount);
void Make_MTDAccountItemNode(CCTD_AccountItemNode* pout, int nAIID, unsigned long int nItemID, int nRentMinutePeriodRemainder, int nCount);

void Make_MTDQuestItemNode( CCTD_QuestItemNode* pOut, const unsigned long int nItemID, const int nCount );

struct CCMatchWorldItem;
void Make_MTDWorldItem(CCTD_WorldItem* pOut, CCMatchWorldItem* pWorldItem);

class CCMatchActiveTrap;
void Make_MTDActivatedTrap(CCTD_ActivatedTrap *pOut, CCMatchActiveTrap* pTrapItem);

// 경험치, 경험치 비율을 4byte로 조합
// 상위 2바이트는 경험치, 하위 2바이트는 경험치의 퍼센트이다.
inline unsigned long int MakeExpTransData(int nAddedXP, int nPercent)
{
	unsigned long int ret = 0;
	ret |= (nAddedXP & 0x0000FFFF) << 16;
	ret |= nPercent & 0xFFFF;
	return ret;
}
inline int GetExpFromTransData(unsigned long int nValue)
{
	return (int)((nValue & 0xFFFF0000) >> 16);

}
inline int GetExpPercentFromTransData(unsigned long int nValue)
{
	return (int)(nValue & 0x0000FFFF);
}

#endif
