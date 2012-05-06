#pragma once

#include "CCAsyncProxy.h"
#include "CCUID.h"
#include "CCErrorTable.h"
#include "CCQuestItem.h"
#include "CCMatchGlobal.h"

class CCCommand;
class CCMatchCharInfo;

enum CCASYNCJOB {
	CCASYNCJOB_TEST=0,
	CCASYNCJOB_GETACCOUNTCHARLIST,
	CCASYNCJOB_GETACCOUNTCHARINFO,
	CCASYNCJOB_GETCHARINFO,
	CCASYNCJOB_UPDATCHARCLANCONTPOINT,
	CCASYNCJOB_FRIENDLIST,
	CCASYNCJOB_GETLOGININFO,	
	CCASYNCJOB_CREATECHAR,
	CCASYNCJOB_DELETECHAR,	
	CCASYNCJOB_WINTHECLANGAME,
	CCASYNCJOB_UPDATECHARINFODATA,
	CCASYNCJOB_CHARFINALIZE,	
	CCASYNCJOB_INSERTCONNLOG,
	CCASYNCJOB_INSERTGAMELOG,
	CCASYNCJOB_INSERTGAMEPLAYERLOG,
	CCASYNCJOB_CREATECLAN,
	CCASYNCJOB_EXPELCLANMEMBER,
	CCASYNCJOB_INSERTQUESTGAMELOG,
	CCASYNCJOB_UPDATEQUESTITEMINFO,
	CCASYNCJOB_UPDATEIPTOCOUNTRYLIST,
	CCASYNCJOB_UPDATEBLOCKCOUNTRYCODELIST,
	CCASYNCJOB_UPDATECUSTOMIPLIST,
	CCASYNCJOB_PROBABILITYEVENTPERTIME,
	CCASYNCJOB_SETBLOCKHACKING,
	CCASYNCJOB_RESETACCOUNTHACKINGBLOCK,
	CCASYNCJOB_NETMARBLE_KOR_LOGIN,
	CCASYNCJOB_NHNLOGIN,
	CCASYNCJOB_GAMEONLOGIN,
	CCASYNCJOB_GETSYSTEMINFO,
	CCASYNCJOB_BUYBOUNTRYITEM,
	CCASYNCJOB_SELL_ITEM_TO_BOUNTY,
	CCASYNCJOB_SELL_SPENDABLEITEM_TO_BOUNTY,
	CCASYNCJOB_GAMBLE_ITEM,
	CCASYNCJOB_GAMBLE_QUESTITEM,
	CCASYNCJOB_GETACCOUNTITEMLIST,	
	CCASYNCJOB_BRING_ACCOUNTITEM,
	CCASYNCJOB_BRING_ACCOUNTITEM_STACKABLE,
	CCASYNCJOB_BRINGBACK_ACCOUNTITEM,
	CCASYNCJOB_BRINGBACK_ACCOUNTITEM_STACKABLE,
	CCASYNCJOB_UPDATECHARLEVEL,
	CCASYNCJOB_UPDATECHARPLAYINFO,
	CCASYNCJOB_UPDATEEQUIPITEM,
	CCASYNCJOB_TAKEOFFITEM,
	CCASYNCJOB_BUYQUESTITEM,
	CCASYNCJOB_UPDATEACCOUNTLASTLOGINTIME,
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Added By Hong KiJu
	CCASYNCJOB_INSERT_SURVIVALMODE_GAME_LOG,
	CCASYNCJOB_GET_SURVIVALMODE_GROUP_RANKING,
	CCASYNCJOB_GET_SURVIVALMODE_PRIVATE_RANKING,

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Added By Hong KiJu
	CCASYNCJOB_GET_DUELTOURNAMENT_TIMESTAMP,
	CCASYNCJOB_GET_DUELTOURNAMENT_CHARINFO,
	CCASYNCJOB_GET_DUELTOURNAMENT_PREVIOUS_CHARINFO,
	CCASYNCJOB_UPDATE_DUELTOURNAMENT_CHARINFO,
	CCASYNCJOB_UPDATE_DUELTOURNAMENT_GAMELOG,
	CCASYNCJOB_INSERT_DUELTOURNAMENT_GAMELOGDETAIL,

	CCASYNCJOB_GET_DUELTOURNAMENT_SIDERANKING,
	CCASYNCJOB_GET_DUELTOURNAMENT_GROUPRANKING,

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Added By Hong KiJu
	CCASYNCJOB_UPDATE_CHARITEM_COUNT,
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Added By Hong KiJu
	CCASYNCJOB_INSERT_CHAR_BUFF_INFO,
	CCASYNCJOB_UPDATE_CHAR_BUFF_INFO,

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Added By Hong KiJu
	CCASYNCJOB_GET_BR_DESCRIPTION,
	CCASYNCJOB_GET_CHAR_BR_INFO,
	CCASYNCJOB_UPDATE_CHAR_BR_INFO,
	CCASYNCJOB_REWARD_CHAR_BR,

	CCASYNCJOB_MAX,	
};

class CCAsyncDBJob_Test : public CCAsyncJob {
public:
	CCAsyncDBJob_Test( const CCUID& uidOwner ) : CCAsyncJob(CCASYNCJOB_TEST, uidOwner)	{}
	virtual ~CCAsyncDBJob_Test()	{}
	virtual void Run(void* pContext);
};

class CCAsyncDBJob_GetAccountCharList : public CCAsyncJob {
protected:
	CCUID			m_uid;

protected:	// Input Argument
	int				m_nAID;

protected:	// Output Result
	int					m_nCharCount;
	int					m_nCharMaxLevel;		// newbie인지 체크하기 위함
	CCTD_AccountCharInfo m_CharList[MAX_CHAR_COUNT];
public:
	CCAsyncDBJob_GetAccountCharList(const CCUID& uid, int nAID) 
		: CCAsyncJob(CCASYNCJOB_GETACCOUNTCHARLIST, uid)
	{
		m_uid = uid;
		m_nAID = nAID;
		m_nCharMaxLevel = 0;
		m_nCharCount = 0;
	}
	virtual ~CCAsyncDBJob_GetAccountCharList()	{}

    const DWORD GetAID()			{ return m_nAID; }
	const CCUID& GetUID()			{ return m_uid; }
	const int	GetCharCount()		{ return m_nCharCount; }
	int GetCharMaxLevel()			{ return m_nCharMaxLevel; }
	const CCTD_AccountCharInfo* GetCharList() { return m_CharList; }

	virtual void Run(void* pContext);
};

class CCAsyncDBJob_GetCharInfo : public CCAsyncJob {
protected:
	CCUID				m_uid;

protected:	// Input Argument
	int					m_nAID;
	int					m_nCharIndex;

protected:	// Output Result
	CCMatchCharInfo*			m_pCharInfo;	
	CCMatchClanDeleteState	m_DeleteState;

private :
	void SetDeleteState( const CCMatchClanDeleteState DeleteState ) { m_DeleteState = DeleteState; }

public:
	CCAsyncDBJob_GetCharInfo(const CCUID& uid, int nAID, int nCharIndex) 
		: CCAsyncJob(CCASYNCJOB_GETCHARINFO, uid)
	{
		m_uid			= uid;
		m_nAID			= nAID;
		m_nCharIndex	= nCharIndex;
		m_DeleteState	= MMCDS_NORMAL;
	}
	virtual ~CCAsyncDBJob_GetCharInfo()			
	{
	}

	const CCUID&					GetUID()				{ return m_uid; }
	CCMatchCharInfo*				GetCharInfo()			{ return m_pCharInfo; }
	CCMatchClanDeleteState		GetDeleteState()		{ return m_DeleteState; }

	void SetCharInfo(CCMatchCharInfo* pCharInfo)	{ m_pCharInfo = pCharInfo; }

	virtual void Run(void* pContext);
};

class CCAsyncDBJob_UpdateCharClanContPoint : public MAsyncJob {
protected:
	int					m_nCID;
	int					m_nCLID;
	int					m_nAddedContPoint;
public:
	CCAsyncDBJob_UpdateCharClanContPoint(int nCID, int nCLID, int nAddedContPoint)
		: CCAsyncJob(CCASYNCJOB_UPDATCHARCLANCONTPOINT, CCUID(0, 0))
	{
		m_nCID = nCID;
		m_nCLID = nCLID;
		m_nAddedContPoint = nAddedContPoint;
	}
	virtual ~CCAsyncDBJob_UpdateCharClanContPoint()	{}

	virtual void Run(void* pContext);

};


class CCAsyncDBJob_GetAccountCharInfo : public CCAsyncJob {
protected:
	CCUID			m_uid;

protected:	// Input Argument
	int				m_nAID;
	int				m_nCharNum;

protected:	// Output Result
	CCTD_CharInfo	m_CharInfo;
	
public:
	CCAsyncDBJob_GetAccountCharInfo(const CCUID& uid, int nAID, int nCharNum)
		: CCAsyncJob(CCASYNCJOB_GETACCOUNTCHARINFO, uid)
	{
		m_uid = uid;
		m_nAID = nAID;
		m_nCharNum = nCharNum;

		memset(&m_CharInfo, 0, sizeof(CCTD_CharInfo));
	}
	virtual ~MAsyncDBJob_GetAccountCharInfo()	{}

	const MUID&		GetUID()		{ return m_uid; }
	MTD_CharInfo*	GetCharInfo()	{ return &m_CharInfo; }
	const int		GetCharNum()	{ return m_nCharNum; }
	
	virtual void Run(void* pContext);
};

class MAsyncDBJob_CreateChar : public MAsyncJob {
protected:
	MUID		m_uid;

protected: // Input Argument
	int			m_nAID;
	char		m_szCharName[32];
	int			m_nCharNum;
	int			m_nSex;
	int			m_nHair;
	int			m_nFace;
	int			m_nCostume;

protected:	// Output Result
	int			m_nResult;
	MCommand*	m_pResultCommand;

public:
	MAsyncDBJob_CreateChar(const MUID& uid, int nAID, const char* szCharName, int nCharNum, int nSex, int nHair, int nFace, int nCostume)
	: MAsyncJob(CCASYNCJOB_CREATECHAR, uid)
	{
		m_uid = uid;
		m_nAID = nAID;
		strcpy(m_szCharName, szCharName);
		m_nCharNum = nCharNum;
		m_nSex = nSex;
		m_nHair = nHair;
		m_nFace = nFace;
		m_nCostume = nCostume;
		m_pResultCommand = NULL;
		m_nResult = MERR_UNKNOWN;
	}
	virtual ~MAsyncDBJob_CreateChar()	{}

	const MUID& GetUID()		{ return m_uid; }


	const int GetDBResult() { return m_nResult; }
	const char* GetCharName() { return m_szCharName; }

	virtual void Run(void* pContext);
};

class MAsyncDBJob_DeleteChar : public MAsyncJob {
protected:
	MUID		m_uid;

protected: // Input Argument
	int			m_nAID;
	int			m_nCharNum;
	char		m_szCharName[32];

protected:	// Output Result
	int			m_nDeleteResult;

public:
	MAsyncDBJob_DeleteChar(const MUID& uid, int nAID, int nCharNum, const char* szCharName)
	: MAsyncJob(CCASYNCJOB_DELETECHAR, uid)
	{
		m_uid = uid;
		m_nAID = nAID;
		m_nCharNum = nCharNum;
		strcpy(m_szCharName, szCharName);
		m_nDeleteResult = MERR_UNKNOWN;
	}
	virtual ~MAsyncDBJob_DeleteChar()	{}

	const MUID& GetUID()		{ return m_uid; }
	int GetDeleteResult()		{ return m_nDeleteResult; }

	virtual void Run(void* pContext);

};


////////////////////////////////////////////////////////////////////////////////////////////////////
class MAsyncDBJob_InsertGameLog : public MAsyncJob {
protected: // Input Argument
	unsigned int	m_nMasterCID;

	char			m_szMap[256];
	char			m_szGameType[256];
	
	/*
	char			m_szGameName[256];
	char			m_szMap[256];
	char			m_szGameType[256];
	int				m_nRound;
	unsigned int	m_nMasterCID;
	int				m_nPlayerCount;
	char			m_szPlayers[512];
	*/
protected:	// Output Result
	unsigned int m_nID;

public:
	MAsyncDBJob_InsertGameLog(const MUID& uidStage)	: MAsyncJob(CCASYNCJOB_INSERTGAMELOG, uidStage) {}
	virtual ~MAsyncDBJob_InsertGameLog()	{}
/*
	bool Input(const char* szGameName, 
			   const char* szMap, 
			   const char* szGameType,
               const int nRound, 
			   const unsigned int nMasterCID,
               const int nPlayerCount, 
			   const char* szPlayers);
*/
	bool Input(const unsigned int nMasterCID, const char* szMap, const char* szGameType);

	virtual void Run(void* pContext);

	unsigned int GetID()		{ return m_nID; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
class MAsyncDBJob_CreateClan : public MAsyncJob {
protected:
	MUID		m_uidMaster;
	MUID		m_uidMember1;
	MUID		m_uidMember2;
	MUID		m_uidMember3;
	MUID		m_uidMember4;
protected: // Input Argument
	char		m_szClanName[256];
	int			m_nMasterCID;
	int			m_nMember1CID;
	int			m_nMember2CID;
	int			m_nMember3CID;
	int			m_nMember4CID;
protected:	// Output Result
	bool		m_bDBResult;
	int			m_nNewCLID;
public:
	MAsyncDBJob_CreateClan(const MUID& uidOwne)	: MAsyncJob(CCASYNCJOB_CREATECLAN, uidOwne) ,
											  m_bDBResult(false), 
											  m_nNewCLID(0)
	{
	
	}
	virtual ~MAsyncDBJob_CreateClan()	{}
	bool Input(const TCHAR* szClanName, 
			   const int nMasterCID, 
			   const int nMember1CID, 
			   const int nMember2CID,
               const int nMember3CID, 
			   const int nMember4CID,
			   const MUID& uidMaster,
			   const MUID& uidMember1,
			   const MUID& uidMember2,
			   const MUID& uidMember3,
			   const MUID& uidMember4);
	virtual void Run(void* pContext);
	bool GetDBResult() { return m_bDBResult; }
	int GetNewCLID() { return m_nNewCLID; }
	const MUID& GetMasterUID() { return m_uidMaster; }
	const MUID& GetMember1UID() { return m_uidMember1; }
	const MUID& GetMember2UID() { return m_uidMember2; }
	const MUID& GetMember3UID() { return m_uidMember3; }
	const MUID& GetMember4UID() { return m_uidMember4; }
	const char* GetClanName() { return m_szClanName; }
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class MAsyncDBJob_ExpelClanMember : public MAsyncJob {
protected:
	MUID		m_uidAdmin;
protected: // Input Argument
	int			m_nCLID;
	int			m_nClanGrade;
	char		m_szTarMember[256]; 

protected:	// Output Result
	int			m_nDBResult;
public:
	MAsyncDBJob_ExpelClanMember(const MUID& uidOwner)	: MAsyncJob(CCASYNCJOB_EXPELCLANMEMBER, uidOwner), m_nDBResult(0)
	{
	
	}
	virtual ~MAsyncDBJob_ExpelClanMember()	{}
	bool Input(const MUID& uidAdmin, int nCLID, int nClanGrade, const char* szTarMember);
	virtual void Run(void* pContext);

	int GetDBResult() { return m_nDBResult; }
	const MUID& GetAdminUID() { return m_uidAdmin; }
	const char* GetTarMember() { return m_szTarMember; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MMatchQuestGameLogInfoManager;
class MQuestPlayerLogInfo;

#define MAX_QUEST_LOG_PLAYER_COUNT 3 // 방장을 제외하고 최대 인원.

class MAsyncDBJob_InsertQuestGameLog : public MAsyncJob 
{
public :
	MAsyncDBJob_InsertQuestGameLog() : MAsyncJob(CCASYNCJOB_INSERTQUESTGAMELOG, MUID(0, 0)), m_nMasterCID( 0 ), m_nScenarioID( 0 )
	{
	}

	~MAsyncDBJob_InsertQuestGameLog();
	

	bool Input( const char* pszStageName, 
				const int nScenarioID,
				const int nMasterCID, 
				MMatchQuestGameLogInfoManager* pQGameLogInfoMgr,
				const int nTotalRewardQItemCount,
				const int nElapsedPlayerTime );

	virtual void Run( void* pContext );

private :
	// by SungE 2007-04-02
	// char							m_szStageName[ 64 ];
	char							m_szStageName[ STAGENAME_LENGTH ];
	int								m_nMasterCID;

	// by SungE 2007-04-02
	// char							m_szMapName[ 32 ];
	char							m_szMapName[ MAPNAME_LENGTH ];	

	int								m_nElapsedPlayTime;
	int								m_nScenarioID;
	vector< MQuestPlayerLogInfo* >	m_Player;
	int								m_PlayersCID[ MAX_QUEST_LOG_PLAYER_COUNT ];
	int								m_nTotalRewardQItemCount;
};

class MAsyncDBJob_UpdateQuestItemInfo : public MAsyncJob
{
public :
	MAsyncDBJob_UpdateQuestItemInfo (const MUID& uidOwner) : MAsyncJob( CCASYNCJOB_UPDATEQUESTITEMINFO, uidOwner )
	{
	}

	virtual ~MAsyncDBJob_UpdateQuestItemInfo();
	
	bool Input( const int nCID, 
				MQuestItemMap& QuestItemList, 
				MQuestMonsterBible& QuestMonster );

	virtual void Run( void* pContext );

private :
	int					m_nCID;
	MQuestItemMap		m_QuestItemList;
	MQuestMonsterBible	m_QuestMonster;
};



class MAsyncDBJob_SetBlockHacking : public MAsyncJob
{
public :
	MAsyncDBJob_SetBlockHacking(const MUID& uidOwner) : MAsyncJob( CCASYNCJOB_SETBLOCKHACKING, uidOwner )
	{
	}

	virtual ~MAsyncDBJob_SetBlockHacking()
	{
	}

	bool Input( const DWORD dwAID
		, const DWORD dwCID
		, const BYTE btBlockType
		, const BYTE btBlockLevel
		, const string& strComment
		, const string& strIP
		, const string& strEndDate
		, const BYTE nServerID
		, const string& strChannelName );

	virtual void Run( void* pContext );

private :
	DWORD	m_dwAID;
	DWORD	m_dwCID;
	BYTE	m_btBlockType;
	BYTE	m_btBlockLevel;
	string	m_strComment;
	string	m_strIP;
	string	m_strEndDate;
	BYTE	m_nServerID;
	string	m_strChannelName;
};


class MAsyncDBJob_ResetAccountHackingBlock : public MAsyncJob
{
public :
	MAsyncDBJob_ResetAccountHackingBlock(const MUID& uidOwner) : MAsyncJob( CCASYNCJOB_RESETACCOUNTHACKINGBLOCK, uidOwner )
	{
	}

	~MAsyncDBJob_ResetAccountHackingBlock() 
	{
	}

	bool Input( const DWORD dwAID, const BYTE btBlockType = 0 );

	virtual void Run( void* pContext );

private :
	DWORD	m_dwAID;
	BYTE	m_btBlockType;
};
	