#ifndef _MMATCHTRANSDATATYPE_H
#define _MMATCHTRANSDATATYPE_H

#include "CCMatchObject.h"
#include "MMatchRule.h"		// MMATCH_GAMETYPE, MMATCH_ROUNDSTATE, MMATCH_ROUNDRESULT ���� -> �������
#include "CCMatchStageSetting.h"
#include "CCMatchGameType.h"
#include "CCMatchGlobal.h"

#pragma pack(push, old)
#pragma pack(1)

// �� ĳ���� ����Ʈ ���� - ĳ���� ���ý� ���
struct MTD_AccountCharInfo
{
	char				szName[MATCHOBJECT_NAME_LENGTH];
	char				nCharNum;
	unsigned char		nLevel;
};

struct MTD_CharInfo
{
	// ���� ����ü�� ������ �����Ϸ��� ���� ���÷����� �ε��� ���ؼ� ���� ���� ����ü�� ZReplay.cpp�� �����ϰ�
	// �������� �ε� �ڵ带 �ۼ������ �մϴ�. ������ �߰��� ������ �������� �����̴� ���� �׳��� �����մϴ�.

	// ĳ���� ����
	char				szName[MATCHOBJECT_NAME_LENGTH];
	char				szClanName[CLAN_NAME_LENGTH];
	MMatchClanGrade		nClanGrade;
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

	// ������ ����
	unsigned long int	nEquipedItemDesc[MMCIP_END];

	// account �� ����
	MMatchUserGradeID	nUGradeID;

	// ClanCLID
	unsigned int		nClanCLID;

	// ������ �����ʸ�Ʈ ���
	int					nDTLastWeekGrade;	

	// ������ ���� �߰�
	MUID				uidEquipedItem[MMCIP_END];
	unsigned long int	nEquipedItemCount[MMCIP_END];
};

//���������ӽ��ּ� 
/*
struct MTD_CharBuffInfo
{
	MShortBuffInfo	ShortBuffInfo[MAX_CHARACTER_SHORT_BUFF_COUNT];	
};*/

struct MTD_BuffInfo
{
	unsigned long int	nItemId;		// ������ ����Ų ������ID
	unsigned short		nRemainedTime;	// ���� ���� ���ҳ� (�ʴ����� ����), �����ۿ� ���󼭴� Ƚ���� ��� ���� �ִ�
};

// �� ĳ���� ������ �߰� ����
struct MTD_MyExtraCharInfo
{
	char	nLevelPercent;		///< ���� ����ġ ���� �ۼ�Ʈ
};

// ���� ������� �ʴ´�.
struct MTD_SimpleCharInfo
{
	char				szName[32];
	char				nLevel;
	char				nSex;
	char				nHair;
	char				nFace;
	unsigned long int	nEquipedItemDesc[MMCIP_END];
};


struct MTD_MySimpleCharInfo
{
	unsigned char		nLevel;
	unsigned long int	nXP;
	int					nBP;
};

struct MTD_CharLevelInfo
{
	unsigned char		nLevel;
	unsigned long int	nCurrLevelExp;
	unsigned long int	nNextLevelExp;
};

struct MTD_RoundPeerInfo
{
	MUID			uidChar;
	unsigned char	nHP;
	unsigned char	nAP;
};

struct MTD_RoundKillInfo
{
	MUID	uidAttacker;
	MUID	uidVictim;
};

struct MTD_ItemNode
{
	MUID				uidItem;
	unsigned long int	nItemID;
	int					nRentMinutePeriodRemainder;		// �Ⱓ�� ������ ��밡�ɽð�(�ʴ���), RENT_MINUTE_PERIOD_UNLIMITED�̸� ������
	int					iMaxUseHour;					// �ִ� ��� �ð�(�ð� ����)
	int					nCount;
};

struct MTD_RelayMap
{
	int				nMapID;
};

struct MTD_AccountItemNode
{
	int					nAIID;
	unsigned long int	nItemID;
	int					nRentMinutePeriodRemainder;		// �Ⱓ�� ������ ��밡�ɽð�(�ʴ���), RENT_MINUTE_PERIOD_UNLIMITED�̸� ������
	int					nCount;
};

// ���Ӿ� ���� ����
struct MTD_GameInfoPlayerItem
{
	MUID	uidPlayer;
	bool	bAlive;
	int		nKillCount;
	int		nDeathCount;
};

struct MTD_GameInfo
{
	char	nRedTeamScore;		// ���������� ����ϴ� ����������
	char	nBlueTeamScore;		// ���������� ����ϴ� ����������

	short	nRedTeamKills;		// ������������ġ������ ����ϴ� ������ų��
	short	nBlueTeamKills;		// ������������ġ������ ����ϴ� ������ų��
};

struct MTD_RuleInfo	
{
	unsigned char	nRuleType;
};

struct MTD_RuleInfo_Assassinate : public MTD_RuleInfo
{
	MUID	uidRedCommander;
	MUID	uidBlueCommander;
};

struct MTD_RuleInfo_Berserker : public MTD_RuleInfo
{
	MUID	uidBerserker;
};


enum MTD_PlayerFlags {
	MTD_PlayerFlags_AdminHide	= 1,
	MTD_PlayerFlags_BridgePeer	= 1<<1,
	MTD_PlayerFlags_Premium		= 1<<2			// �ݸ��� �ǽù� ���ʽ�
};

struct MTD_ChannelPlayerListNode 
{
	MUID			uidPlayer;
	char			szName[MATCHOBJECT_NAME_LENGTH];
	char			szClanName[CLAN_NAME_LENGTH];
	char			nLevel;
	char			nDTLastWeekGrade;
	MMatchPlace		nPlace;
	unsigned char	nGrade;			// �κ񿡼��� uid �� ĳ������ ����� �˼��� ���..
	unsigned char	nPlayerFlags;	// �÷��̾� �Ӽ�(��ڼ����) - MTD_PlayerFlags ���
	unsigned int	nCLID;			// ClanID
	unsigned int	nEmblemChecksum;// Emblem Checksum
};


struct MTD_ClanMemberListNode 
{
	MUID				uidPlayer;
	char				szName[MATCHOBJECT_NAME_LENGTH];
	char				nLevel;
	MMatchClanGrade		nClanGrade;
	MMatchPlace			nPlace;
};

enum MTD_WorldItemSubType
{
	MTD_Dynamic = 0,
	MTD_Static  = 1,
};

// ������ ���� ����
struct MTD_WorldItem
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


// �ߵ��� Ʈ�� ����
struct MTD_ActivatedTrap
{
	MUID				uidOwner;
	unsigned short		nItemID;
	unsigned long int	nTimeElapsed;
	short	x;
	short	y;
	short	z;
};


// �ٷΰ����ϱ� ���͸� ����
struct MTD_QuickJoinParam
{
	unsigned long int	nMapEnum;		// ���ϴ� ���� ��Ʈ���
	unsigned long int	nModeEnum;		// ���ϴ� ���Ӹ���� ��Ʈ���
};


// ĳ������ Ŭ�� ������Ʈ ����
struct MTD_CharClanInfo
{
	char				szClanName[CLAN_NAME_LENGTH];		// Ŭ�� �̸�
	MMatchClanGrade		nGrade;
};


// ���� ��������
struct MTD_CharInfo_Detail
{
	char				szName[32];						// �̸�
	char				szClanName[CLAN_NAME_LENGTH];	// Ŭ���̸�
	MMatchClanGrade		nClanGrade;						// Ŭ����å
	int					nClanContPoint;					// Ŭ�� �⿩��
	unsigned short		nLevel;							// ����
	char				nSex;							// ����
	char				nHair;							// �Ӹ� �ڽ���
	char				nFace;							// �� �ڽ���
	unsigned long int	nXP;							// xp
	int					nBP;							// bp

	int					nKillCount;
	int					nDeathCount;

	// ���ӻ�Ȳ

	unsigned long int	nTotalPlayTimeSec;				// �� �÷��� �ð�
	unsigned long int	nConnPlayTimeSec;				// ���� ���� �ð�


	unsigned long int	nEquipedItemDesc[MMCIP_END];	// ������ ����

	MMatchUserGradeID	nUGradeID;						// account UGrade

	// ClanCLID
	unsigned int		nClanCLID;
};


/// �� ����Ʈ �޶�� ��û�Ҷ� ������ ����ü
struct MTD_StageListNode
{
	MUID			uidStage;							///< �� UID
	unsigned char	nNo;								///< ���ȣ
	char			szStageName[STAGENAME_LENGTH];		///< ���̸�
	char			nPlayers;							///< �����ο�
	char			nMaxPlayers;						///< �ִ��ο�
	STAGE_STATE		nState;								///< �������
	MMATCH_GAMETYPE nGameType;							///< ���� Ÿ��
	char			nMapIndex;							///< ��
	int				nSettingFlag;						///< �� ���� �÷���(����, ��й�, ��������)
	char			nMasterLevel;						///< ���� ����
	char			nLimitLevel;						///< ���ѷ���
};

/// Ŭ���̾�Ʈ�� �˾ƾ��� ��Ÿ���� : AdminHide ���¸� ���������� ��ȯ & ��ȭ���Ͽ� ����
struct MTD_ExtendInfo
{
	char			nTeam;
	unsigned char	nPlayerFlags;	// �÷��̾� �Ӽ�(��ڼ����) - MTD_PlayerFlags ���
	unsigned char	nReserved1;		// ����
	unsigned char	nReserved2;
};

struct MTD_PeerListNode
{
	MUID				uidChar;
	DWORD				dwIP;
	unsigned int		nPort;
	MTD_CharInfo		CharInfo;
	//���������ӽ��ּ� MTD_CharBuffInfo	CharBuffInfo;
	MTD_ExtendInfo		ExtendInfo;
};


// ���� �亯��
struct MTD_ReplierNode
{
	char szName[MATCHOBJECT_NAME_LENGTH];
};


// ���� ���� ��û �� �׷�
struct MTD_LadderTeamMemberNode
{
	char szName[MATCHOBJECT_NAME_LENGTH];

};

// Ŭ�� ����
struct MTD_ClanInfo
{
	char				szClanName[CLAN_NAME_LENGTH];		// Ŭ�� �̸�
	short				nLevel;								// ����
	int					nPoint;								// ����Ʈ
	int					nTotalPoint;						// ��Ż����Ʈ
	int					nRanking;							// ��ŷ
	char				szMaster[MATCHOBJECT_NAME_LENGTH];	// Ŭ�� ������
	unsigned short		nWins;								// ���� - �¼�
	unsigned short		nLosses;							// ���� - �м�
	unsigned short		nTotalMemberCount;					// ��ü Ŭ������
	unsigned short		nConnedMember;						// ���� ���ӵ� Ŭ������
	unsigned int		nCLID;								// ClanID
	unsigned int		nEmblemChecksum;					// Emblem Checksum
};

// Ŭ���� ������� Ŭ�� ����Ʈ
struct MTD_StandbyClanList
{
	char				szClanName[CLAN_NAME_LENGTH];		// Ŭ�� �̸�
	short				nPlayers;							// ������� �ο���
	short				nLevel;								// ����
	int					nRanking;							// ��ŷ - 0�̸� unranked
	unsigned int		nCLID;								// ClanID
	unsigned int		nEmblemChecksum;					// Emblem Checksum
};


// ����Ʈ, �����̹��� ���� ����
struct MTD_QuestGameInfo
{
	unsigned short		nQL;												// ����Ʈ ����
	float				fNPC_TC;											// NPC ���̵� ���� ���
	unsigned short		nNPCCount;											// ���ʹ� ������ NPC����

	unsigned char		nNPCInfoCount;										// ������ NPC ���� ����
	unsigned char		nNPCInfo[MAX_QUEST_NPC_INFO_COUNT];					// ������ NPC ����
	unsigned short		nMapSectorCount;									// �� ��� ����
	unsigned short		nMapSectorID[MAX_QUEST_MAP_SECTOR_COUNT];			// �� ��� ID
	char				nMapSectorLinkIndex[MAX_QUEST_MAP_SECTOR_COUNT];	// �� ����� Link Index
	unsigned char		nRepeat;											// �ݺ� Ƚ�� (�����̹���)
	MMATCH_GAMETYPE		eGameType;											// ����Ÿ��(����Ʈ��, �����̹��̳�)
};

// ����Ʈ, ���� ����
struct MTD_QuestReward
{
	MUID				uidPlayer;	// �ش� �÷��̾� UID
	int					nXP;		// �ش� �÷��̾ ���� XP
	int					nBP;		// �ش� �÷��̾ ���� BP
};

// ����Ʈ ������ ���� ����
struct MTD_QuestItemNode
{
	int		m_nItemID;
	int		m_nCount;
};

// ����Ʈ �Ϲ� ������ ���� ����
struct MTD_QuestZItemNode
{
	unsigned int		m_nItemID;
	int					m_nRentPeriodHour;
	int					m_nItemCnt;
};


// ����Ʈ�� ����� NPC�� ����.
struct MTD_NPCINFO
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

// �����̹� ��ŷ ����
struct MTD_SurvivalRanking
{
	char	m_szCharName[MATCHOBJECT_NAME_LENGTH];
	DWORD	m_dwPoint;
	DWORD	m_dwRank;		// ���������� ���� �� �����Ƿ� ���� ��ũ �������� �̰����� ���

	MTD_SurvivalRanking() : m_dwPoint(0), m_dwRank(0) { m_szCharName[0] = 0; }
};

#if defined(LOCALE_NHNUSA)
struct MTD_ServerStatusInfo
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
struct MTD_ServerStatusInfo
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

struct MTD_ResetTeamMembersData
{
	MUID			m_uidPlayer;		// �ش� �÷��̾�
	char			nTeam;				// ��
};


// ��� ť ����

struct MTD_DuelQueueInfo
{
	MUID			m_uidChampion;
	MUID			m_uidChallenger;
	MUID			m_WaitQueue[14];				// ��
	char			m_nQueueLength;
	char			m_nVictory;						// ���¼�
	bool			m_bIsRoundEnd;					// ���� �������ΰ�
};

struct MTD_DuelTournamentGameInfo
{
	MUID			uidPlayer1;					// ������ ������ ������ Player1
	MUID			uidPlayer2;					// ������ ������ ������ Player1
	int				nMatchType;					// ������ ������ MatchType(
	int				nMatchNumber;				// ������ ������ MatchNumber
	int				nRoundCount;				// ������ ������ �����
	bool			bIsRoundEnd;				// ���尡 ����Ǿ��°��� ���� Flag(�÷��̾� ��Ż�� ������..)
	char			nWaitPlayerListLength;		// ����� ����Ʈ�� Length
	byte			dummy[2];					// 4����Ʈ�� ���߱� ���� ����
	MUID			WaitPlayerList[8];			// ����ڵ��� MUID
};

struct MTD_DuelTournamentNextMatchPlayerInfo
{
	MUID			uidPlayer1;					// ������ ������ ������ Player1
	MUID			uidPlayer2;					// ������ ������ ������ Player1
};


struct MTD_DuelTournamentRoundResultInfo
{
	MUID			uidWinnerPlayer;			// ���� ������ �¸���
	MUID			uidLoserPlayer;				// ���� ������ �й���(180cm ����...)
	bool			bIsTimeOut;					// ���� ���尡 Ÿ�� �ƿ��̾���?
	bool			bDraw;						// ���� ���尡 ����� ��� true
	bool			bIsMatchFinish;				// ���� ���尡 ����Ǹ鼭, Match�� ����Ǿ��� ��� true
	byte			dummy[2];					// ���� ����

	///////////////////////////////////////////////////////////
	// Notice 
	// bDraw�� ����� ��, True�̴�.
	// bIsMatchFinish�� Match�� ����Ǿ��� ��, True�̴�.
	// ������ ���� ����� ���� ������ �� �ִ�.
	// bDraw = true,  bIsMatchFinish = true  => �÷��̾� �θ��� ��� ��Ż���� ���
	// bDraw = true,  bIsMatchFinish = false => ���� �ڻ�� ���Ͽ� ����� ���
	// bDraw = false, bIsMatchFinish = true  => �׳� �Ѹ��� �̰��, ��ġ�� ������ ���
	// bDraw = false, bIsMatchFinish = false => �׳� �Ѹ��� �̰�µ�, ��ġ�� ��� ����Ǿߵ� ���(4��, ���)
	///////////////////////////////////////////////////////////
};

struct MTD_DuelTournamentMatchResultInfo
{
	int				nMatchNumber;
	int				nMatchType;
	MUID			uidWinnerPlayer;			// ���� ������ �¸���
	MUID			uidLoserPlayer;				// ���� ������ �й���(180cm ����...)
	int				nGainTP;
	int				nLoseTP;
};

#pragma pack(pop, old)


// admin ����
enum ZAdminAnnounceType
{
	ZAAT_CHAT = 0,
	ZAAT_MSGBOX
};

// �׺� ������ ������
struct MTD_GambleItemNode
{
	MUID			uidItem;
	unsigned int	nItemID;							// ������ ID
	unsigned int	nItemCnt;
};


// �׺� ������ ������
struct MTD_DBGambleItmeNode
{
	unsigned int	nItemID;							// ������ ID
	char			szName[ MAX_GAMBLEITEMNAME_LEN ];	// ������ �̸�
	char			szDesc[ MAX_GAMBLEITEMDESC_LEN ];	// ������ ����.
	int				nBuyPrice;							// ������ ���԰���
	bool			bIsCash;							// Cash������ ����.
};

// Shop ������ ������ - Added by 2010-03-18 ȫ����
struct MTD_ShopItemInfo
{
	unsigned int	nItemID;
	int				nItemCount;
};
/////////////////////////////////////////////////////////
void Make_MTDItemNode(MTD_ItemNode* pout, MUID& uidItem, unsigned long int nItemID, int nRentMinutePeriodRemainder, int iMaxUseHour, int nCount);
void Make_MTDAccountItemNode(MTD_AccountItemNode* pout, int nAIID, unsigned long int nItemID, int nRentMinutePeriodRemainder, int nCount);

void Make_MTDQuestItemNode( MTD_QuestItemNode* pOut, const unsigned long int nItemID, const int nCount );

struct MMatchWorldItem;
void Make_MTDWorldItem(MTD_WorldItem* pOut, MMatchWorldItem* pWorldItem);

class MMatchActiveTrap;
void Make_MTDActivatedTrap(MTD_ActivatedTrap *pOut, MMatchActiveTrap* pTrapItem);

// ����ġ, ����ġ ������ 4byte�� ����
// ���� 2����Ʈ�� ����ġ, ���� 2����Ʈ�� ����ġ�� �ۼ�Ʈ�̴�.
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