#pragma once
#include "CCMatchDBMgr.h"
#include "winsock2.h"
#include "CCXml.h"
#include "CCServer.h"
#include "CCMatchObject.h"
#include "CCAgentObject.h"
#include "CCMatchChannel.h"
#include "CCMatchStage.h"
#include "CCMatchClan.h"
#include "CCSafeUDP.h"
#include "CCMatchTransDataType.h"
#include "CCMatchAdmin.h"
#include "CCAsyncProxy.h"
#include "CCMatchGlobal.h"
#include "CCMatchShutdown.h"
#include "CCMatchChatRoom.h"
#include "CCLadderMgr.h"
#include "CCMatchQuest.h"
#include "CCTypes.h"
#include "CCMatchDebug.h"
#include "CCStringRes.h"
#include "CCMatchStringResManager.h"
#include "CCMatchEventManager.h"
#include "CCMatchGambleMachine.h"
#include "CCHackingChatList.h"
#include "CCMatchObjectCommandHistory.h"
#include "CCConnectHistory.h"


#include "CCMatchBRMachine.h"

#include <vector>
using namespace std;

class CCMatchAuthBuilder;
class CCMatchScheduleMgr;
class MNJ_DBAgentClient;
class MBMatchMonitor;
class CCMatchGambleMachine;
class CCMatchDuelTournamentMgr;

#define MATCHSERVER_UID		CCUID(0, 2)	///< MatchServer의 고유 UID(불변)

// 디버그 코드들 삽입
#define CHECKMEMORYNUMBER	888888

//#define _DUELTOURNAMENT_LOG_ENABLE_

enum CUSTOM_IP_STATUS
{
	CIS_INVALID = 0,
	CIS_NONBLOCK,
	CIS_BLOCK,
	CIS_NON,
};

enum COUNT_CODE_STATUS
{
	CCS_INVALID = 0,
	CCS_NONBLOCK,
	CCS_BLOCK,
	CCS_NON,
};


class CCMatchServer : public MServer{
private:
	static CCMatchServer*	m_pInstance;		///< 전역 인스턴스
	unsigned long int		m_nTickTime;		///< 틱 타이머
	inline void SetTickTime(unsigned long int nTickTime);		///< 틱 타이머 설정

	unsigned long		m_HSCheckCounter;

protected:
	unsigned long		m_nItemFileChecksum;	// ZItem.xml 의 변조방지 검사

	CCUID				m_NextUseUID;
	CCCriticalSection	m_csUIDGenerateLock;
	CCCriticalSection	m_csTickTimeLock;

	DWORD				m_checkMemory1;
	CCMatchObjectList	m_Objects;
	DWORD				m_checkMemory2;

	CCMatchChannelMap	m_ChannelMap;
	DWORD				m_checkMemory3;

	char				m_szDefaultChannelName[CHANNELNAME_LEN];
	char				m_szDefaultChannelRuleName[CHANNELRULE_LEN];

	DWORD				m_checkMemory4;
	CCMatchStageMap		m_StageMap;
	DWORD				m_checkMemory5;
	CCMatchClanMap		m_ClanMap;
	DWORD				m_checkMemory6;
	CCAgentObjectMap		m_AgentMap;
	DWORD				m_checkMemory7;


	DWORD				m_checkMemory8;
	CCSafeUDP			m_SafeUDP;
	DWORD				m_checkMemory9;
	CCMatchDBMgr			m_MatchDBMgr;
	DWORD				m_checkMemory10;
	CCAsyncProxy			m_AsyncProxy;
	DWORD				m_checkMemory11;
	CCMatchAdmin			m_Admin;
	DWORD				m_checkMemory12;
	CCMatchShutdown		m_MatchShutdown;
	DWORD				m_checkMemory13;
	CCMatchChatRoomMgr	m_ChatRoomMgr;
	DWORD				m_checkMemory14;
	MLadderMgr			m_LadderMgr;
	CCMatchDuelTournamentMgr*  m_pDTMgr;

	DWORD				m_checkMemory15;

	bool				m_bCreated;

	DWORD					m_checkMemory16;

	DWORD					m_checkMemory17;

	DWORD					m_checkMemory18;
	CCMatchScheduleMgr*		m_pScheduler;
	DWORD					m_checkMemory19;
	CCMatchAuthBuilder*		m_pAuthBuilder;
	DWORD					m_checkMemory20;
	CCMatchQuest				m_Quest;	// 임시로 위치이동
	DWORD					m_checkMemory21;

	CCCountryFilter			m_CountryFilter;
	IPtoCountryList			m_TmpIPtoCountryList;
	BlockCountryCodeList	m_TmpBlockCountryCodeList;
	CustomIPList			m_TmpCustomIPList;
	DWORD					m_dwBlockCount;
	DWORD					m_dwNonBlockCount;

	CCMatchEventManager		m_CustomEventManager;
	unsigned char			m_szMD5Value[16];
	CCMatchGambleMachine		m_GambleMachine;

	//map< int, DWORD >		m_ItemResourceCRC32;
	MHackingChatList		m_HackingChatList;
	CCMatchObjectCommandHistory	m_objectCommandHistory;
	MConnectHistory			m_connectionHistory;

	void InitItemCRC32Cache();
	void InitBuffCRC32Cache();

	const DWORD GetItemCRC32Cache( const int nItemID );
	const DWORD GetBuffCRC32Cache( const int nBuffID );

public:
	CCMatchServer();
	virtual ~CCMatchServer();

	/// 전역 인스턴스 얻기
	static CCMatchServer* GetInstance();

	/// 초기화
	bool Create(int nPort);
	/// 해제
	void Destroy();
	virtual void Shutdown();
	/// 새로운 UID 얻어내기
	virtual CCUID UseUID();

	CCMatchAuthBuilder* GetAuthBuilder()					{ return m_pAuthBuilder; }
#ifndef NEW_AUTH_MODULE
	void SetAuthBuilder(CCMatchAuthBuilder* pBuilder)	{ m_pAuthBuilder = pBuilder; }
#endif

	CCMatchChatRoomMgr* GetChatRoomMgr()					{ return &m_ChatRoomMgr; }


	virtual bool SendMonitorUDP(const DWORD dwIP, const USHORT nPort, const string& strMonitorCommand)
	{
		return true;
	}



protected:
	/// Create()호출시에 불리는 함수
	virtual bool OnCreate();
	/// Destroy()호출시에 불리는 함수
	virtual void OnDestroy();
	/// 사용자 커맨드 등록
	virtual void OnRegisterCommand(CCCommandManager* pCommandManager);
	/// 사용자 커맨드 처리
	virtual bool OnCommand(CCCommand* pCommand);
	/// 사용자 루프
	virtual void OnRun();
	/// 루프 전
	virtual void OnPrepareRun();

	virtual int Connect(CCCommObject* pCommObj);	// 연결실패시 반드시 Disconnect() 호출해야함;
	virtual int OnConnected(CCUID* pTargetUID, CCUID* pAllocUID, unsigned int nTimeStamp, CCCommObject* pCommObj);

	// virtual void OnNetClear(const CCUID& ComCCUID);
	virtual void OnNetPong(const CCUID& ComCCUID, unsigned int nTimeStamp);
	virtual void OnHShieldPong(const CCUID& ComCCUID, unsigned int nTimeStamp);
	bool CheckOnLoginPre(const CCUID& ComCCUID, int nCmdVersion, bool& outbFreeIP, string& strCountryCode3);
	void OnMatchLogin(CCUID ComCCUID, const char* szUserID, const char* szPassword, int nCommandVersion, unsigned long nChecksumPack, char *szEncryptMd5Value);
	void OnMatchLoginFromNetmarbleJP(const CCUID& ComCCUID, const char* szLoginID, const char* szLoginPW, int nCmdVersion, unsigned long nChecksumPack);
	void OnMatchLoginFromDBAgent(const CCUID& ComCCUID, const char* szLoginID, const char* szName, int nSex, bool bFreeLoginIP, unsigned long nChecksumPack);
	void OnMatchLoginFailedFromDBAgent(const CCUID& ComCCUID, int nResult);
	void OnBridgePeer(const CCUID& uidChar, DWORD dwIP, DWORD nPort);

	bool AddObjectOnMatchLogin(const CCUID& uidComm, 
		const CCMatchAccountInfo* pSrcAccountInfo,
		const CCMatchAccountPenaltyInfo* pSrcAccountPenaltyInfo,
		bool bFreeLoginIP, string strCountryCode3, unsigned long nChecksumPack);

	void LockUIDGenerate()		{ m_csUIDGenerateLock.Lock(); }
	void UnlockUIDGenerate()	{ m_csUIDGenerateLock.Unlock(); }

	/// 오브젝트 생성
	int ObjectAdd(const CCUID& uidComm);
	/// 오브젝트 제거
	int ObjectRemove(const CCUID& uid, CCMatchObjectList::iterator* pNextItor);

	/// 대화 메시지
	int MessageSay(CCUID& uid, char* pszSay);

	/// UDP
	CCSafeUDP* GetSafeUDP() { return &m_SafeUDP; }
	void SendCommandByUDP(CCCommand* pCommand, char* szIP, int nPort);
	void ParsePacket(char* pData, CCPacketHeader* pPacketHeader, DWORD dwIP, WORD wRawPort);
	static bool UDPSocketRecvEvent(DWORD dwIP, WORD wRawPort, char* pPacket, DWORD dwSize);
	void ParseUDPPacket(char* pData, CCPacketHeader* pPacketHeader, DWORD dwIP, WORD wRawPort);

	/// Async DB
	void ProcessAsyncJob();
	void OnAsyncGetAccountCharList(CCAsyncJob* pJobResult);
	void OnAsyncGetAccountCharInfo(CCAsyncJob* pJobResult);
	void OnAsyncGetCharInfo(CCAsyncJob* pJobResult);
	void OnAsyncCreateChar(CCAsyncJob* pJobResult);
	void OnAsyncDeleteChar(CCAsyncJob* pJobResult);
	void OnAsyncGetFriendList(CCAsyncJob* pJobInput);
	void OnAsyncGetLoginInfo(CCAsyncJob* pJobInput);
	void OnAsyncWinTheClanGame(CCAsyncJob* pJobInput);
	void OnAsyncUpdateCharInfoData(CCAsyncJob* pJobInput);
	void OnAsyncCharFinalize(CCAsyncJob* pJobInput);
	void OnAsyncInsertConnLog(CCAsyncJob* pJobResult);
	void OnAsyncInsertGameLog(CCAsyncJob* pJobResult);
	void OnAsyncCreateClan(CCAsyncJob* pJobResult);
	void OnAsyncExpelClanMember(CCAsyncJob* pJobResult);
	void OnAsyncInsertEvent( CCAsyncJob* pJobResult );
	void OnAsyncUpdateIPtoCoutryList( CCAsyncJob* pJobResult );
	void OnAsyncUpdateBlockCountryCodeList( CCAsyncJob* pJobResult );
	void OnAsyncUpdateCustomIPList( CCAsyncJob* pJobResult );
	void OnAsyncGetAccountItemList( CCAsyncJob* pJobResult );
	void OnAsyncBuyQuestItem( CCAsyncJob* pJobResult );


	virtual void OnProcessAsyncJob( CCAsyncJob* pJob ) {}

	/// 스케쥴러 초기화
	bool InitScheduler();
	/// 로케일 초기화
	bool InitLocale();
	/// Event초기화.
	bool InitEvent();

	bool InitGambleMachine();

	bool IsEquipmentTypeItem( const CCMatchItemDesc* pItemDesc );
public:
	/// Async DB
	void PostAsyncJob(CCAsyncJob* pJob );

private :
	// 추가적인 스케쥬을 추가하려면 이 함수를 재정의 하면 됨.
	virtual bool InitSubTaskSchedule() { return true; }

protected:
	/// Object의 접속을 끊는다.
	void DisconnectObject(const CCUID& uidObject);
	void DebugTest();
protected:
	// 채널 관련
	const char* GetDefaultChannelName()					{ return m_szDefaultChannelName; }
	void SetDefaultChannelName(const char* pszName)		{ strcpy(m_szDefaultChannelName, pszName); }
	const char* GetDefaultChannelRuleName()				{ return m_szDefaultChannelRuleName; }
	void SetDefaultChannelRuleName(const char* pszName)	{ strcpy(m_szDefaultChannelRuleName, pszName); }
	const CCUID FindFreeChannel( const CCUID& uidPlayer );

	bool ChannelAdd(const char* pszChannelName, const char* pszRuleName, CCUID* pAllocUID, CCCHANNEL_TYPE nType=CCCHANNEL_TYPE_PRESET, int nMaxPlayers=DEFAULT_CHANNEL_MAXPLAYERS, int nLevelMin=-1, int nLevelMax=-1,
		const bool bIsTicketChannel = false, const DWORD dwTicketItemID = 0, const bool bIsUseTicket = false, const char* pszChannelNameStrResId = NULL);
	bool ChannelJoin(const CCUID& uidPlayer, const CCUID& uidChannel);
	bool ChannelJoin(const CCUID& uidPlayer, const CCCHANNEL_TYPE nChannelType, const char* pszChannelName);
	bool ChannelLeave(const CCUID& uidPlayer, const CCUID& uidChannel);
	bool ChannelChat(const CCUID& uidPlayer, const CCUID& uidChannel, char* pszChat);

	void ResponseChannelRule(const CCUID& uidPlayer, const CCUID& uidChannel);

	void OnRequestRecommendedChannel(const CCUID& uidComm);
	void OnRequestChannelJoin(const CCUID& uidPlayer, const CCUID& uidChannel);
	void OnRequestChannelJoin(const CCUID& uidPlayer, const CCCHANNEL_TYPE nChannelType, const char* pszChannelName);
	void OnChannelChat(const CCUID& uidPlayer, const CCUID& uidChannel, char* pszChat);
	void OnStartChannelList(const CCUID& uidPlayer, const int nChannelType);
	void OnStopChannelList(const CCUID& uidPlayer);

	void OnChannelRequestPlayerList(const CCUID& uidPlayer, const CCUID& uidChannel, int nPage);
	void OnChannelRequestAllPlayerList(const CCUID& uidPlayer, const CCUID& uidChannel, unsigned long int nPlaceFilter, unsigned long int nOptions);

public:
	void ChannelResponsePlayerList(const CCUID& uidPlayer, const CCUID& uidChannel, int nPage);
	void ChannelResponseAllPlayerList(const CCUID& uidPlayer, const CCUID& uidChannel, unsigned long int nPlaceFilter, unsigned long int nOptions);

public:
	CCMatchStage* FindStage(const CCUID& uidStage);
protected:
	friend CCMatchStage;
	friend MNJ_DBAgentClient;
	bool StageAdd(CCMatchChannel* pChannel, const char* pszStageName, bool bPrivate, const char* pszStagePassword, CCUID* pAllocUID, bool bIsAllowNullChannel = false);
	bool StageRemove(const CCUID& uidStage, CCMatchStageMap::iterator* pNextItor);
	bool StageJoin(const CCUID& uidPlayer, const CCUID& uidStage);
	bool StageLeave(const CCUID& uidPlayer);//, const CCUID& uidStage);
	bool StageEnterBattle(const CCUID& uidPlayer, const CCUID& uidStage);
	bool StageLeaveBattle(const CCUID& uidPlayer, bool bGameFinishLeaveBattle, bool bForcedLeave);//, const CCUID& uidStage);
	bool StageChat(const CCUID& uidPlayer, const CCUID& uidStage, char* pszChat);
	bool StageTeam(const CCUID& uidPlayer, const CCUID& uidStage, CCMatchTeam nTeam);
	bool StagePlayerState(const CCUID& uidPlayer, const CCUID& uidStage, CCMatchObjectStageState nStageState);
	bool StageMaster(const CCUID& uidStage);

protected:
	CCCommand* CreateCmdResponseStageSetting(const CCUID& uidStage);
	CCCommand* CreateCmdMatchResponseLoginOK(const CCUID& uidComm, 
		CCUID& uidPlayer, 
		const char* szUserID, 
		CCMatchUserGradeID nUGradeID, 
		CCMatchPremiumGradeID nPGradeID,
		//											const unsigned char* szRandomValue,
		const unsigned char* pbyGuidReqMsg);
	CCCommand* CreateCmdMatchResponseLoginFailed(const CCUID& uidComm, const int nResult);


	float GetDuelVictoryMultiflier(int nVictorty);		// 듀얼 연승일때의 가중치~
	float GetDuelPlayersMultiflier(int nPlayerCount);	// 듀얼 사람수에 따른 가중치~
	void CalcExpOnGameKill(CCMatchStage* pStage, CCMatchObject* pAttacker, CCMatchObject* pVictim, 
		int* poutAttackerExp, int* poutVictimExp);
	const int CalcBPonGameKill( CCMatchStage* pStage, CCMatchObject* pAttacker, const int nAttackerLevel, const int nVictimLevel );
	void PostGameDeadOnGameKill(CCUID& uidStage, CCMatchObject* pAttacker, CCMatchObject* pVictim,
		int nAddedAttackerExp, int nSubedVictimExp);	// ProcessOnGameKill함수에서 사용


	void OnStageCreate(const CCUID& uidChar, char* pszStageName, bool bPrivate, char* pszStagePassword);
	void OnPrivateStageJoin(const CCUID& uidPlayer, const CCUID& uidStage, char* pszPassword);
	void OnStageFollow(const CCUID& uidPlayer, const char* pszTargetName);
	void OnStageLeave(const CCUID& uidPlayer);//, const CCUID& uidStage);
	void OnStageRequestPlayerList(const CCUID& uidPlayer, const CCUID& uidStage);
	void OnStageEnterBattle(const CCUID& uidPlayer, const CCUID& uidStage);
	void OnStageLeaveBattle(const CCUID& uidPlayer, bool bGameFinishLeaveBattle);//, const CCUID& uidStage);
	void OnStageChat(const CCUID& uidPlayer, const CCUID& uidStage, char* pszChat);
	void OnRequestQuickJoin(const CCUID& uidPlayer, void* pQuickJoinBlob);
	void ResponseQuickJoin(const CCUID& uidPlayer, CCTD_QuickJoinParam* pQuickJoinParam);
	void OnStageGo(const CCUID& uidPlayer, unsigned int nRoomNo);
	void OnStageTeam(const CCUID& uidPlayer, const CCUID& uidStage, CCMatchTeam nTeam);
	void OnStagePlayerState(const CCUID& uidPlayer, const CCUID& uidStage, CCMatchObjectStageState nStageState);
	void OnStageStart(const CCUID& uidPlayer, const CCUID& uidStage, int nCountdown);
	void OnStageRelayStart(const CCUID& uidStage);
	void OnStartStageList(const CCUID& uidComm);
	void OnStopStageList(const CCUID& uidComm);
	void OnStageRequestStageList(const CCUID& uidPlayer, const CCUID& uidChannel, const int nStageCursor);
	void OnStageMap(const CCUID& uidStage, char* pszMapName);

	void OnStageRelayMapElementUpdate(const CCUID& uidStage, int nType, int nTurnCount);
	void OnStageRelayMapListUpdate(const CCUID& uidStage, int nRelayMapType, int nRelayMapRepeatCount, void* pRelayMapListBlob);
	void OnStageRelayMapListInfo(const CCUID& uidStage, const CCUID& uidChar);
	void OnStageSetting(const CCUID& uidPlayer, const CCUID& uidStage, void* pStageBlob, int nStageCount);
	void OnRequestStageSetting(const CCUID& uidComm, const CCUID& uidStage);
	void OnRequestPeerList(const CCUID& uidChar, const CCUID& uidStage);
	void OnRequestGameInfo(const CCUID& uidChar, const CCUID& uidStage);
	void OnMatchLoadingComplete(const CCUID& uidPlayer, int nPercent);
	void OnRequestRelayPeer(const CCUID& uidChar, const CCUID& uidPeer);
	void OnPeerReady(const CCUID& uidChar, const CCUID& uidPeer);
	void OnGameRoundState(const CCUID& uidStage, int nState, int nRound);
	// 코드 정리로 MBMatchServer로 내려감. 하지만 CCMatchStage에서 사용하고 있어서 인터페이스만 남김. - by SungE 2007-07-05

	void OnRequestSpawn(const CCUID& uidChar, const CCVector& pos, const CCVector& dir);
	void OnGameRequestTimeSync(const CCUID& uidComm, unsigned long nLocalTimeStamp);
	void OnGameReportTimeSync(const CCUID& uidComm, unsigned long nLocalTimeStamp, unsigned int nDataChecksum);
	void OnUpdateFinishedRound(const CCUID& uidStage, const CCUID& uidChar, 
		void* pPeerInfo, void* pKillInfo);
	void OnRequestForcedEntry(const CCUID& uidStage, const CCUID& uidChar);
	void OnRequestSuicide(const CCUID& uidPlayer);
	void OnRequestObtainWorldItem(const CCUID& uidPlayer, const int nIteCCUID);
	void OnRequestSpawnWorldItem(const CCUID& uidPlayer, const int nItemID, const float x, const float y, const float z, float fDropDelayTime);
	void OnNotifyThrowTrapItem(const CCUID& uidPlayer, const int nItemID);
	void OnNotifyActivatedTrapItem(const CCUID& uidPlayer, const int nItemID, const CCVector3& pos);

	void OnUserWhisper(const CCUID& uidComm, char* pszSenderName, char* pszTargetName, char* pszMessage);
	void OnUserWhere(const CCUID& uidComm, char* pszTargetName);
	void OnUserOption(const CCUID& uidComm, unsigned long nOptionFlags);
	void OnChatRoomCreate(const CCUID& uidPlayer, const char* pszChatRoomName);
	void OnChatRoomJoin(const CCUID& uidComm, char* pszPlayerName, char* pszChatRoomName);
	void OnChatRoomLeave(const CCUID& uidComm, char* pszPlayerName, char* pszChatRoomName);
	void OnChatRoomSelectWrite(const CCUID& uidComm, const char* szChatRoomName);
	void OnChatRoomInvite(const CCUID& uidComm, const char* pszTargetName);
	void OnChatRoomChat(const CCUID& uidComm, const char* pszMessage);

	void SaveGameLog(const CCUID& uidStage);
	void SaveGamePlayerLog(CCMatchObject* pObj, unsigned int nStageID);
protected:	// 래더
	friend MLadderMgr;
	bool LadderJoin(const CCUID& uidPlayer, const CCUID& uidStage, CCMatchTeam nTeam);
	void LadderGameLaunch(MLadderGroup* pGroupA, MLadderGroup* pGroupB);

protected:	// 래더
	void OnLadderRequestInvite(const CCUID& uidPlayer, void* pGroupBlob);
	void OnLadderInviteAgree(const CCUID& uidPlayer);
	void OnLadderInviteCancel(const CCUID& uidPlayer);
	bool IsLadderRequestUserInRequestClanMember( const CCUID& uidRequestMember
		, const CCTD_LadderTeamMemberNode* pRequestMemberNode );
	void OnLadderRequestChallenge(const CCUID& uidRequestMember, void* pGroupBlob, unsigned long int nOptions);
	void OnLadderRequestCancelChallenge(const CCUID& uidPlayer);

	void OnRequestProposal(const CCUID& uidProposer, const int nProposalMode, const int nRequestID, 
		const int nReplierCount, void* pReplierNamesBlob);
	void OnReplyAgreement(CCUID& uidProposer, CCUID& uidReplier, const char* szReplierName, 
		const int nProposalMode, const int nRequestID, const bool bAgreement);
protected:
	void OnRequestCopyToTestServer(const CCUID& uidPlayer);						// 현재 사용하지 않는다.
	void ResponseCopyToTestServer(const CCUID& uidPlayer, const int nResult);	// 현재 사용하지 않는다.

	void OnRequestMySimpleCharInfo(const CCUID& uidPlayer);
	void ResponseMySimpleCharInfo(const CCUID& uidPlayer);
	void OnRequestCharInfoDetail(const CCUID& uidChar, const char* szCharName);
	void ResponseCharInfoDetail(const CCUID& uidChar, const char* szCharName);
	void OnRequestAccountCharInfo(const CCUID& uidPlayer, int nCharNum);
	void OnRequestSelectChar(const CCUID& uidPlayer, const int nCharIndex);
	void OnRequestDeleteChar(const CCUID& uidPlayer, const int nCharIndex, const char* szCharName);
	bool ResponseDeleteChar(const CCUID& uidPlayer, const int nCharIndex, const char* szCharName);
	void OnRequestCreateChar(const CCUID& uidPlayer, const int nCharIndex, const char* szCharName,
		const unsigned int nSex, const unsigned int nHair, const unsigned int nFace, 
		const unsigned int nCostume);
	bool ResponseCreateChar(const CCUID& uidPlayer, const int nCharIndex, const char* szCharName,
		CCMatchSex nSex, const unsigned int nHair, const unsigned int nFace, 
		const unsigned int nCostume);
	void OnCharClear(const CCUID& uidPlayer);
	bool CharInitialize(const CCUID& uidPlayer);
	bool CharFinalize(const CCUID& uidPlayer);
	bool CorrectEquipmentByLevel(CCMatchObject* pPlayer, CCMatchCharItemParts nPart, int nLegalItemLevelDiff=0);	// 수정되면 true
	bool RemoveExpiredCharItem(CCMatchObject* pObject, CCUID& uidItem);
protected: // 친구
	void OnFriendAdd(const CCUID& uidPlayer, const char* pszName);
	void OnFriendRemove(const CCUID& uidPlayer, const char* pszName);
	void OnFriendList(const CCUID& uidPlayer);
	void OnFriendMsg(const CCUID& uidPlayer, const char* szMsg);
	void FriendList(const CCUID& uidPlayer);

protected:	// 클랜
	int ValidateCreateClan(const char* szClanName, CCMatchObject* pMasterObject, CCMatchObject** ppSponsorObject);
	void UpdateCharClanInfo(CCMatchObject* pObject, const int nCLID, const char* szClanName, const CCMatchClanGrade nGrade);

	void OnClanRequestCreateClan(const CCUID& uidPlayer, const int nRequestID, const char* szClanName, char** szSponsorNames);
	void OnClanAnswerSponsorAgreement(const int nRequestID, const CCUID& uidClanMaster, char* szSponsorCharName, const bool bAnswer);
	void OnClanRequestAgreedCreateClan(const CCUID& uidPlayer, const char* szClanName, char** szSponsorNames);
	void OnClanRequestCloseClan(const CCUID& uidClanMaster, const char* szClanName);
	void ResponseCloseClan(const CCUID& uidClanMaster, const char* szClanName);
	void OnClanRequestJoinClan(const CCUID& uidClanAdmin, const char* szClanName, const char* szJoiner);
	void ResponseJoinClan(const CCUID& uidClanAdmin, const char* szClanName, const char* szJoiner);
	void OnClanAnswerJoinAgreement(const CCUID& uidClanAdmin, const char* szJoiner, const bool bAnswer);
	void OnClanRequestAgreedJoinClan(const CCUID& uidClanAdmin, const char* szClanName, const char* szJoiner);
	void ResponseAgreedJoinClan(const CCUID& uidClanAdmin, const char* szClanName, const char* szJoiner);

	void OnClanRequestLeaveClan(const CCUID& uidPlayer);
	void ResponseLeaveClan(const CCUID& uidPlayer);
	void OnClanRequestChangeClanGrade(const CCUID& uidClanMaster, const char* szMember, int nClanGrade);
	void ResponseChangeClanGrade(const CCUID& uidClanMaster, const char* szMember, int nClanGrade);
	void OnClanRequestExpelMember(const CCUID& uidClanAdmin, const char* szMember);
	void ResponseExpelMember(const CCUID& uidClanAdmin, const char* szMember);
	void OnClanRequestMsg(const CCUID& uidSender, const char* szMsg);
	void OnClanRequestMemberList(const CCUID& uidChar);
	void OnClanRequestClanInfo(const CCUID& uidChar, const char* szClanName);

	void OnClanRequestEmblemURL(const CCUID& uidChar, void* pEmblemURLListBlob);
public:
	CCMatchClan* FindClan(const int nCLID);
	void ResponseClanMemberList(const CCUID& uidChar);
public:
	int GetLadderTeamIDFromDB(const int nTeamTableIndex, const int* pnMemberCIDArray, const int nMemberCount);
	void SaveLadderTeamPointToDB(const int nTeamTableIndex, const int nWinnerTeamID, const int nLoserTeamID, const bool bIsDrawGame);
	void SaveClanPoint(CCMatchClan* pWinnerClan, CCMatchClan* pLoserClan, const bool bIsDrawGame,
		const int nRoundWins, const int nRoundLosses, const int nMapID, const int nGameType,
		const int nOneTeamMemberCount, list<CCUID>& WinnerObjUIDs,
		const char* szWinnerMemberNames, const char* szLoserMemberNames, float fPointRatio);
	void BroadCastClanRenewVictories(const char* szWinnerClanName, const char* szLoserClanName, const int nVictories);
	void BroadCastClanInterruptVictories(const char* szWinnerClanName, const char* szLoserClanName, const int nVictories);
	void BroadCastDuelRenewVictories(const CCUID& chanID, const char* szChampionName, const char* szChannelName, int nRoomNumber, const int nVictories);
	void BroadCastDuelInterruptVictories(const CCUID& chanID, const char* szChampionName, const char* szInterrupterName, const int nVictories);
public:
	friend CCVoteDiscuss;
	// 투쵸관련.
	void OnVoteCallVote(const CCUID& uidPlayer, const char* pszDiscuss, const char* pszArg);
	void OnVoteYes(const CCUID& uidPlayer);
	void OnVoteNo(const CCUID& uidPlayer);
	void VoteAbort( const CCUID& uidPlayer );

	void OnAdminServerHalt();

protected:
	// 관리자 기능
	void OnAdminTerminal(const CCUID& uidAdmin, const char* szText);
	void OnAdminAnnounce(const CCUID& uidAdmin, const char* szChat, unsigned long int nType);
	void OnAdminRequestServerInfo(const CCUID& uidAdmin);
	void OnAdminServerHalt(const CCUID& uidAdmin);

	void OnAdminRequestKickPlayer(const CCUID& uidAdmin, const char* szPlayer);
	void OnAdminRequestMutePlayer(const CCUID& uidAdmin, const char* szPlayer, const int nPenaltyHour);
	void OnAdminRequestBlockPlayer(const CCUID& uidAdmin, const char* szPlayer, const int nPenaltyHour);


	void OnAdminRequestUpdateAccountUGrade(const CCUID& uidAdmin, const char* szPlayer);
	void OnAdminPingToAll(const CCUID& uidAdmin);
	void OnAdminRequestSwitchLadderGame(const CCUID& uidAdmin, const bool bEnabled);
	void OnAdminHide(const CCUID& uidAdmin);
	void OnAdminResetAllHackingBlock( const CCUID& uidAdmin );

	// 이벤트 마스터 기능
	void OnEventChangeMaster(const CCUID& uidAdmin);
	void OnEventChangePassword(const CCUID& uidAdmin, const char* szPassword);
	void OnEventRequestJjang(const CCUID& uidAdmin, const char* pszTargetName);
	void OnEventRemoveJjang(const CCUID& uidAdmin, const char* pszTargetName);

public:
	void AdminTerminalOutput(const CCUID& uidAdmin, const char* szText);
	bool OnAdminExecute(MAdminArgvInfo* pAI, char* szOut);
	void ApplyObjectTeamBonus(CCMatchObject* pObject, int nAddedExp);
	void ProcessPlayerXPBP(CCMatchStage* pStage, CCMatchObject* pPlayer, int nAddedXP, int nAddedBP);
	void ProcessCharPlayInfo(CCMatchObject* pPlayer);
	bool DistributeZItem(const CCUID& uidPlayer, const unsigned long int nItemID, bool bRentItem, int nRentPeriodHour, int nItemCount);
	

protected:
	void OnRequestAccountItemList(const CCUID& uidPlayer);
	void ResponseAccountItemList(const CCUID& uidPlayer);

public:
	void OnDuelSetObserver(const CCUID& uidChar);
	void OnDuelQueueInfo(const CCUID& uidStage, const CCTD_DuelQueueInfo& QueueInfo);	// 쳇-.-
	void OnQuestSendPing(const CCUID& uidStage, unsigned long int t);

protected :
	// Keeper관련.
	void OnResponseServerStatus( const CCUID& uidSender );
	void OnRequestServerHearbeat( const CCUID& uidSender );
	void OnResponseServerHeartbeat( const CCUID& uidSender );
	void OnRequestConnectMatchServer( const CCUID& uidSender );
	void OnResponseConnectMatchServer( const CCUID& uidSender );
	void OnRequestKeeperAnnounce( const CCUID& uidSender, const char* pszAnnounce );
	void OnRequestStopServerWithAnnounce( const CCUID& uidSender );
	void OnResponseStopServerWithAnnounce( const CCUID& uidSender );
	void OnRequestSchedule( const CCUID& uidSender, 
		const int nType, 
		const int nYear, 
		const int nMonth, 
		const int nDay, 
		const int nHour, 
		const int nMin,
		const int nCount,
		const int nCommand,
		const char* pszAnnounce );
	void OnResponseSchedule( const CCUID& uidSender, 
		const int nType, 
		const int nYear, 
		const int nMonth, 
		const int nDay, 
		const int nHour, 
		const int nMin,
		const int nCount,
		const int nCommand,
		const char* pszAnnounce );

protected:
	// 퀘스트 관련
	void OnRequestNPCDead(const CCUID& uidSender, const CCUID& uidKiller, CCUID& uidNPC, CCVector& pos);
	void OnQuestRequestDead(const CCUID& uidVictim);
	void OnQuestTestRequestNPCSpawn(const CCUID& uidPlayer, int nNPCType, int nNPCCount);
	void OnQuestTestRequestClearNPC(const CCUID& uidPlayer);
	void OnQuestTestRequestSectorClear(const CCUID& uidPlayer);
	void OnQuestTestRequestQuestFinish(const CCUID& uidPlayer);
	void OnQuestRequestMovetoPortal(const CCUID& uidPlayer);
	void OnQuestReadyToNewSector(const CCUID& uidPlayer);
	void OnQuestStageMapset(const CCUID& uidStage, int nMapsetID);

	// 퀘스트 아이템 관련.
	void OnResponseCharQuestItemList( const CCUID& uidSender );
	void OnRequestBuyQuestItem( const CCUID& uidSender, const unsigned long int nItemID, const int nItemCount );
	void OnResponseBuyQuestItem( const CCUID& uidSender, const unsigned long int nItemID, const int nItemCount );
	void OnRequestSellQuestItem( const CCUID& uidSender, const unsigned long int nItemID, const int nCount );
	void OnResponseSellQuestItem( const CCUID& uidSender, const unsigned long int nItemID, const int nCount );
	void OnRequestDropSacrificeItemOnSlot( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID );
	void OnRequestCallbackSacrificeItem( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID );
	void OnRequestQL( const CCUID& uidSender );
	void OnRequestSacrificeSlotInfo( const CCUID& uidSender );
	void OnRequestMonsterBibleInfo( const CCUID& uidSender );
	void OnResponseMonsterBibleInfo( const CCUID& uidSender );

	void OnQuestPong( const CCUID& uidSender );
public :
	void OnRequestCharQuestItemList( const CCUID& uidSender );
	///


	int AgentAdd(const CCUID& uidComm);													/// Agent 생성	
	int AgentRemove(const CCUID& uidAgent, CCAgentObjectMap::iterator* pNextItor);		/// Agent 제거	
	void AgentClear();																	/// 종료시 청소

	/// Agent
	CCAgentObject* GetAgent(const CCUID& uidAgent);
	CCAgentObject* GetAgentByComCCUID(const CCUID& uidComm);	
	CCAgentObject* FindFreeAgent();
	void ReserveAgent(CCMatchStage* pStage);

protected:
	bool CheckBridgeFault();
	void LocateAgentToClient(const CCUID& uidPlayer, const CCUID& uidAgent);
	void OnRegisterAgent(const CCUID& uidComm, char* szIP, int nTCPPort, int nUDPPort);
	void OnUnRegisterAgent(const CCUID& uidComm);
	void OnAgentStageReady(const CCUID& uidCommAgent, const CCUID& uidStage);
	void OnRequestLiveCheck(const CCUID& uidComm, unsigned long nTimeStamp, 
		unsigned long nStageCount, unsigned long nUserCount);
public:
	/// UID로 오브젝트 얻어내기
	CCMatchObject* GetObject(const CCUID& uid);
	/// UID로 캐릭터 오브젝트 얻어내기
	CCMatchObject* GetPlayerByComCCUID(const CCUID& uid);
	/// Name으로 오브젝트 얻어내기
	CCMatchObject* GetPlayerByName(const char* pszName);
	/// AID로 오브젝트 얻어내기
	CCMatchObject* GetPlayerByAID(unsigned long int nAID);

	/// UID로 채널 얻어내기
	CCMatchChannel* FindChannel(const CCUID& uidChannel);
	/// Name으로 채널 얻어내기
	CCMatchChannel* FindChannel(const CCCHANNEL_TYPE nChannelType, const char* pszChannelName);

	/// 서버의 공지 메시지 전송
	void Announce(const CCUID& ComCCUID, char* pszMsg);
	void Announce(CCObject* pObj, char* pszMsg);
	/// 서버의 에러 메시지 전송
	void AnnounceErrorMsg(const CCUID& ComCCUID, const int nErrorCode);
	void AnnounceErrorMsg(CCObject* pObj, const int nErrorCode);
	/// Command를 Object의 Listener에게 전송
	void RouteToListener(CCObject* pObject, CCCommand* pCommand);
	/// Command를 전체 커낵션으로 전송
	void RouteToAllConnection(CCCommand* pCommand);
	/// Command를 전체 클라이언트로 전송
	void RouteToAllClient(CCCommand* pCommand);
	/// Command를 지정 Channel 참가자에게 전송
	void RouteToChannel(const CCUID& uidChannel, CCCommand* pCommand);
	/// Command를 지정 Channel 로비에 있는 참가자에게 전송
	void RouteToChannelLobby(const CCUID& uidChannel, CCCommand* pCommand);
	/// Command를 지정 Stage 참가자에게 전송
	void RouteToStage(const CCUID& uidStage, CCCommand* pCommand);
	/// Command를 지정 Stage 내의 지정 참가자에게 전송
	void RouteToObjInStage(const CCUID& uidStage, const CCUID& uidTargetObj, CCCommand* pCommand);
	/// Command를 지정 Stage 대기방 참가자에게 전송
	void RouteToStageWaitRoom(const CCUID& uidStage, CCCommand* pCommand);
	/// Command를 지정 Stage 배틀 참가자에게 전송
	void RouteToBattle(const CCUID& uidStage, CCCommand* pCommand);
	/// Command를 지정 Clan 에게 전송
	void RouteToClan(const int nCLID, CCCommand* pCommand);
	// int 결과값있는 Command를 지정 Object Listener에게 전송
	void RouteResponseToListener(CCObject* pObject, const int nCmdID, int nResult);

	void ResponseBridgePeer(const CCUID& uidChar, int nCode);
	void ResponseRoundState(const CCUID& uidStage);
	void ResponseRoundState(CCMatchObject* pObj, const CCUID& uidStage);
	void ResponsePeerList(const CCUID& uidChar, const CCUID& uidStage);
	void ResponseGameInfo(const CCUID& uidChar, const CCUID& uidStage);


	virtual void ResponseTakeoffItem(const CCUID& uidPlayer, const CCMatchCharItemParts parts) = 0;
	virtual bool CheckUserCanDistributeRewardItem( CCMatchObject* pObj) = 0;
	virtual bool ResponseCharacterItemList(const CCUID& uidPlayer) = 0;

	// x-trap : check client new hash value.

	void NotifyMessage(const CCUID& uidChar, int nMsgID);

	unsigned long GetChannelListChecksum()	{ return m_ChannelMap.GetChannelListChecksum(); }
	void ChannelList(const CCUID& uidPlayer, CCCHANNEL_TYPE nChannelType);

	unsigned long int GetStageListChecksum(CCUID& uidChannel, int nStageCursor, int nStageCount);
	void StageList(const CCUID& uidPlayer, int nStageStartIndex, bool bCacheUpdate);
	void StageLaunch(const CCUID& uidStage);
	void StageRelayLaunch(const CCUID& uidStage);
	void StageRelayMapBattleStart(const CCUID& uidPlayer, const CCUID& uidStage);
	void StageFinishGame(const CCUID& uidStage);	

	void StandbyClanList(const CCUID& uidPlayer, int nClanListStartIndex, bool bCacheUpdate);


	/// 현재 클럭 얻어내기
	unsigned long int GetGlobalClockCount() const;
	/// 클라이언트에게 클럭을 맞추게 한다.
	void SetClientClockSynchronize(const CCUID& ComCCUID);
	/// Local Clock을 Global Clock으로 변환
	static unsigned long int ConvertLocalClockToGlobalClock(unsigned long int nLocalClock, unsigned long int nLocalClockDistance);
	/// Global Clock을 Local Clock으로 변환
	static unsigned long int ConvertGlobalClockToLocalClock(unsigned long int nGlobalClock, unsigned long int nLocalClockDistance);

	bool IsCreated() { return m_bCreated; }
	inline unsigned long int GetTickTime();			///< 쓰레드에 안전한 틱 타이머 반환
protected:
	// 사용 하지 않음.
	// void InsertChatDBLog(const CCUID& uidPlayer, const char* szMsg);
	int ValidateMakingName(const char* szCharName, int nMinLength, int nMaxLength);		// 등록가능한 이름인지 확인한다.

	virtual int ValidateStageJoin(const CCUID& uidPlayer, const CCUID& uidStage) { return 0; }
	int ValidateChannelJoin(const CCUID& uidPlayer, const CCUID& uidChannel);
	int ValidateEquipItem(CCMatchObject* pObj, CCMatchItem* pItem, const CCMatchCharItemParts parts);
	int ValidateChallengeLadderGame(CCMatchObject** ppMemberObject, int nMemberCount);
	void CheckExpiredItems(CCMatchObject* pObj);
	void ResponseExpiredItemIDList(CCMatchObject* pObj, vector<unsigned long int>& vecExpiredItemIDList);

	bool LoadInitFile();
	bool LoadChannelPreset();
	bool InitDB();
	void UpdateServerLog();
	void UpdateServerStatusDB();

	void UpdateCharDBCachingData(CCMatchObject* pObject);
	void UpdateCharItemDBCachingData(CCMatchObject* pObject);

protected:
	unsigned long GetItemFileChecksum()					{ return m_nItemFileChecksum; }
	void SetItemFileChecksum(unsigned long nChecksum)	{ m_nItemFileChecksum = nChecksum; }

	bool CheckItemXML();
	bool CheckItemXMLFromDatabase();
	bool CompareMatchItem(CCMatchItemDescForDatabase *pItem1, CCMatchItemDesc *pItem2);

protected :
	friend bool StageKick(CCMatchServer* pServer, const CCUID& uidPlayer, const CCUID& uidStage, char* pszChat);
	// fitler
	CCCountryFilter& GetCountryFilter()					{ return m_CountryFilter; }
	bool InitCountryFilterDB();
	const CUSTOM_IP_STATUS	CheckIsValidCustomIP( const CCUID& ComCCUID, const string& strIP, string& strCountryCode3, const bool bUseFilter );
	const COUNT_CODE_STATUS CheckIsNonBlockCountry( const CCUID& ComCCUID, const string& strIP, string& strCountryCode3, const bool bUseFilter );

public :
	bool CheckUpdateItemXML();
	IPtoCountryList& GetTmpIPtoCountryList()			{ return m_TmpIPtoCountryList; }
	BlockCountryCodeList& GetTmpBlockCountryCodeList()	{ return m_TmpBlockCountryCodeList; }
	CustomIPList& GetTmpCustomIPList()					{ return m_TmpCustomIPList; }
	void SetUseCountryFilter();
	void SetAccetpInvalidIP();
	void UpdateIPtoCountryList();
	void UpdateBlockCountryCodeLsit();
	void UpdateCustomIPList();
	void ResetBlockCount()								{ m_dwBlockCount = 0; }
	void ResetNonBlockCount()							{ m_dwNonBlockCount = 0; }
	void IncreaseBlockCount()							{ ++m_dwBlockCount = 0; }
	void IncreaseNonBlockCount()						{ ++m_dwNonBlockCount = 0; }
	DWORD GetBlockCount()								{ return m_dwBlockCount; }
	DWORD GetNonBlockCount()							{ return m_dwNonBlockCount; }
	bool CheckIsValidIP( const CCUID& ComCCUID, const string& strIP, string& strCountryCode3, const bool bUseFilter );
	// filter

public :
	void CustomCheckEventObj( const DWORD dwEventID, CCMatchObject* pObj, void* pContext );

	friend bool StageKick(CCMatchServer* pServer, const CCUID& uidPlayer, const CCUID& uidStage, char* pszChat);

public:
	MLadderMgr*	GetLadderMgr()				{ return &m_LadderMgr; }	
	CCMatchObjectList*	GetObjects()		{ return &m_Objects; }
	CCMatchStageMap*		GetStageMap()		{ return &m_StageMap; }
	CCMatchChannelMap*	GetChannelMap()		{ return &m_ChannelMap; }
	CCMatchClanMap*		GetClanMap()		{ return &m_ClanMap; }
	CCMatchDBMgr*		GetDBMgr()			{ return &m_MatchDBMgr; }
	CCMatchQuest*		GetQuest()			{ return &m_Quest; }
	CCMatchDuelTournamentMgr*	GetDTMgr()	{ return m_pDTMgr;	}
	int GetClientCount()	{ return (int)m_Objects.size(); }
	int GetAgentCount()		{ return (int)m_AgentMap.size(); }

	// XTrap
	virtual void	XTrap_OnAdminReloadFileHash(const CCUID& uidAdmin) {}
	virtual bool	PreCheckAddObj(const CCUID& uidObj) { return true; }

	virtual ULONG	HShield_MakeGuidReqMsg(unsigned char *pbyGuidReqMsg, unsigned char *pbyGuidReqInfo) { return 0L; }
	virtual ULONG	HShield_AnalyzeGuidAckMsg(unsigned char *pbyGuidAckMsg, unsigned char *pbyGuidReqInfo, unsigned long **ppCrcInfo) { return 0L; }
	virtual ULONG   HShield_MakeReqMsg(unsigned long *pCrcInfo, unsigned char *pbyReqMsg, unsigned char *pbyReqInfo, unsigned long ulOption) { return 0L; }
	virtual ULONG   HShield_AnalyzeAckMsg(unsigned long *pCrcInfo, unsigned char *pbyAckMsg, unsigned char *pbyReqInfo) { return 0L; }

	void SendHShieldReqMsg();

	//------------------- gameguard -------------------------------
public : 
	void RequestGameguardAuth( const CCUID& uidUser, const DWORD dwIndex, const DWORD dwValue1, const DWORD dwValue2, const DWORD dwValue3 );
	void RequestFirstGameguardAuth( const CCUID& uidUser, const DWORD dwIndex, const DWORD dwValue1, const DWORD dwValue2, const DWORD dwValue3 );

	virtual bool IsSkeepByGameguardFirstAuth( CCCommand* pCommand ) { return true; }
	//------------------- gameguard -------------------------------

	// ---------------- Monitor --------------------------
	// ---------------------------------------------------

	const CCMatchGambleMachine& GetGambleMachine() const { return m_GambleMachine; }
	CCMatchGambleMachine& GetGambleMachine() { return m_GambleMachine; }



	/// 디버그용 
	void CheckMemoryTest(int nState=0, int nValue=0);

	void LogObjectCommandHistory(CCUID uid);

	void CheckMemoryCorruption();

	//////////////////////////////////////////////////////////////////////////////////////////////
	// 2009. 6. 3 - Added By Hong KiJu
	bool OnRequestSurvivalModeGroupRanking();
	bool OnRequestSurvivalModePrivateRanking( const CCUID& uidStage, const CCUID& uidPlayer, DWORD dwScenarioID, DWORD dwCID );
	bool OnPostSurvivalModeGameLog();

	void OnAsyncSurvivalModeGroupRanking(CCAsyncJob *pJobResult);
	void OnAsyncSurvivalModePrivateRanking(CCAsyncJob *pJobResult);
	void OnAsyncSurvivalModeGameLog(CCAsyncJob *pJobResult);

	virtual void Log(unsigned int nLogLevel, const char* szLog);

	//////////////////////////////////////////////////////////////////////////////////////////////
	// 2009. 9. 21 - Added By Hong KiJu
	// Duel Tournament 관련한 부분
protected :
	bool DuelTournamentJoin(const CCUID& uidPlayer, const CCUID& uidStage);

	void ResponseDuelTournamentCharSideRanking(CCUID &uidPlayer);
	void ResponseChannelJoin(CCUID uidPlayer, CCUID uidChannel, int nChannelType, const char* szChannelStrResId, bool bEnableInterface);
	void ResponseDuelTournamentCharStatusInfo(CCUID &uidPlayer, CCCommand *pCommand);
	void ResponseDuelTournamentJoinChallenge(CCUID &uidPlayer, CCDUELTOURNAMENTTYPE nType);
	void ResponseDuelTournamentCancelChallenge(CCUID &uidPlayer, CCDUELTOURNAMENTTYPE nType);	

	bool OnSyncRequest_InsertDuelTournamentGameLog(CCDUELTOURNAMENTTYPE nDTType, int nMatchFactor, CCDuelTournamentPickedGroup *pPickedGroup, int *nOutNumber, char *szOutTimeStamp);
	
	void OnAsyncResponse_GetDuelTournamentTimeStamp(CCAsyncJob *pJobResult);
	void OnAsyncResponse_GetDuelTournamentCharacterInfo(CCAsyncJob *pJobResult);
	void OnAsyncResponse_GetDuelTournamentPreviousCharacterInfo(CCAsyncJob *pJobResult);
	void OnAsyncResponse_GetDuelTournamentSideRanking(CCAsyncJob *pJobResult);	
	void OnAsyncResponse_GetDuelTournamentGroupRanking(CCAsyncJob *pJobResult);

	bool OnAsyncRequest_GetDuelTournamentCharacterInfo(CCUID uidPlayer, DWORD dwCID);
	bool OnAsyncRequest_GetDuelTournamentPreviousCharacterInfo(CCUID uidPlayer, DWORD dwCID);	
	bool OnAsyncRequest_GetDuelTournamentSideRankingInfo(CCUID uidPlayer, DWORD dwCID);
	bool OnAsyncRequest_GetDuelTournamentGroupRankingInfo();

	void PostCmdDuelTournamentChallenge(CCUID uidPlayer, int nResult);
	void PostCmdDuelTournamentCharInfo(CCUID uidPlayer, CCMatchObjectDuelTournamentCharInfo *pDTCharInfo);
	void PostCmdDuelTournamentCharInfoPrevious(CCUID uidPlayer, int nPrevTP, int nPrevWins, int nPrevLoses, int nPrevRanking, int nPrevFinalWins);
	void PostCmdDuelTournamentCharSideRankingInfo(CCUID uidPlayer, list<DTRankingInfo*>* pSideRankingList);	
	void PostCmdDuelTournamentCancelMatch(CCUID uidPlayer, int nErrorCode);

	void RouteCmdDuelTournamentPrepareMatch(CCDUELTOURNAMENTTYPE nType, CCUID uidStage, CCDuelTournamentPickedGroup *pPickedGroup);	
	void RouteCmdDuelTournamentLaunchMatch(CCUID uidStage);
	void RouteCmdDuelTournamentCancelMatch(CCDuelTournamentPickedGroup *pPickedGroup, int nErrorCode);

public:	
	void LaunchDuelTournamentMatch(CCDUELTOURNAMENTTYPE nType, CCDuelTournamentPickedGroup* pPickedGroup, CCDUELTOURNAMENTMATCHMAKINGFACTOR matchFactor);

	void SendDuelTournamentPreviousCharInfoToPlayer(CCUID uidPlayer);
	void SendDuelTournamentCharInfoToPlayer(CCUID uidPlayer);
	void SendDuelTournamentServiceTimeClose(const CCUID& uidPlayer);

	bool OnAsyncRequest_GetDuelTournamentTimeStamp();
	bool OnAsyncRequest_UpdateDuelTournamentCharacterInfo(CCUID uidPlayer, char *szTimeStamp);
	bool OnAsyncRequest_UpdateDuelTournamentGameLog(char* szTimeStamp, int nGameNumber, CCUID uidChampion);
	bool OnAsyncRequest_InsertDuelTournamentGameLogDetail(int nLogID, char* szTimeStamp, CCDUELTOURNAMENTROUNDSTATE nDTRoundState, int nWinnerCID, int nLoserCID, int nGainTP, int nLoseTp, int nPlayTime);

	void RouteCmdDuelTournamentStageSetting(CCUID uidStage);
	void RouteCmdDuelTournamentMTDGameInfo(const CCUID& uidStage, CCTD_DuelTournamentGameInfo& GameInfo);
	void RouteCmdDuelTournamentMTDNextGamePlayerInfo(const CCUID& uidStage, CCTD_DuelTournamentNextMatchPlayerInfo& PlayerInfo);
	void RouteCmdDuelTournamentMTDRoundResultInfo(const CCUID& uidStage, CCTD_DuelTournamentRoundResultInfo *RoundResultInfo);
	void RouteCmdDuelTournamentMTDMatchResultInfo(const CCUID& uidStage, CCTD_DuelTournamentMatchResultInfo *MatchResultInfo);

public:
	//버프정보임시주석 void PostCmdCharacterBuffInfo(const CCUID& uidPlayer);

protected:
	void UseSpendableItem(const CCUID& uidPlayer, const CCUID& uidItem);
	void OnRequestUseSpendableNormalItem(const CCUID& uidPlayer, const CCUID& uidItem);
	/*사용하지 않아서 일단 주석 void OnRequestUseSpendableBuffItem(const CCUID& uidPlayer, const CCUID& uidItem);*/

protected:
	CCMatchBRMachine m_BattletimeRewardMachine;

	bool InitBattletimeRewardMachine();
	void MakeBattleTimeRewardDescriptionMap(vector<CCMatchBRDescription*>& vBattletimeRewardDescription, 
											vector<CCMatchBRItem*>& vBattletimeRewardItem, 
											CCMatchBRDescriptionMap& BattletimeRewardDescriptionMap);
	
	void OnAsyncResponse_GetBR_Description(CCAsyncJob *pJobResult);	
	void OnAsyncResponse_GetCharBRInfo(CCAsyncJob *pJobResult);
	void OnAsyncResponse_UpdateCharBRInfo(CCAsyncJob *pJobResult);
	void OnAsyncResponse_RewardCharBR(CCAsyncJob *pJobResult);

	
public:
	CCMatchBRMachine& GetBattleTimeRewardMachine() { return m_BattletimeRewardMachine; }

	void RouteCmdBattleTimeReward(const CCUID& uidPlayer, CCUID& uidStage, const char* pszName, const char* pszResetDesc, int nItemID, int nItemCnt, int nRentHourPeriod, int nRemainRewardCnt);

	void OnAsyncRequest_RewardCharBP(const CCUID& uidPlayer, int nBRID, int nBRTID, int nRewardCount, int nBattleTime, int nKillCount, int nItemID, int nItemCnt, int nRentHourPeriod);
	void OnAsyncRequest_UpdateCharBRInfo(const CCUID& uidPlayer, int nBRID, int nBRTID, int nRewardCount, int nBattleTime, int nKillCount);

};

void CopyCharInfoForTrans(CCTD_CharInfo* pDest, CCMatchCharInfo* pSrc, CCMatchObject* pSrcObject);
void CopyCharInfoDetailForTrans(CCTD_CharInfo_Detail* pDest, CCMatchCharInfo* pSrcCharInfo, CCMatchObject* pSrcObject);
//버프정보임시주석 void CopyCharBuffInfoForTrans(CCTD_CharBuffInfo* pDest, CCMatchCharInfo* pSrc, CCMatchObject* pSrcObject);


// line functions ///////////////////////////////////////////////////////////////////
inline CCMatchServer* MGetMatchServer()
{
	return CCMatchServer::GetInstance();
}

inline unsigned long int CCMatchServer::GetTickTime()
{ 
	m_csTickTimeLock.Lock();		
	unsigned long int ret = m_nTickTime; 
	m_csTickTimeLock.Unlock();
	return ret;
}

inline void CCMatchServer::SetTickTime(unsigned long int nTickTime)
{
	m_csTickTimeLock.Lock();		
	m_nTickTime = nTickTime;
	m_csTickTimeLock.Unlock();
}

inline const char* MErrStr(const int nID)
{
	return MGetStringResManager()->GetErrorStr(nID);
}

inline void CCMatchServer::LogObjectCommandHistory(CCUID uid)
{
	m_objectCommandHistory.Dump(uid);
}

bool IsExpiredBlockEndTime( const SYSTEMTIME& st );

void _CheckValidPointer(void* pPointer1, void* pPointer2, void* pPointer3, int nState, int nValue);
#define CheckValidPointer(A, B)		_CheckValidPointer(m_pMessengerManager, m_pScheduler, m_pAuthBuilder, A, B);CheckMemoryTest(A, B);
#define CheckValidPointer2(A, B)	CCMatchServer::GetInstance()->CheckMemoryTest(A, B);
