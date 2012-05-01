#pragma once
#include <list>
#include <map>
using namespace std;
#include "CCMatchGlobal.h"
#include "CCCommandCommunicator.h"
#include "CCClient.h"
#include "CCSafeUDP.h"
#include "CCMatchObjCache.h"
#include "CCMatchObject.h"
#include "CCMatchTransDataType.h"
#include "CCMatchStage.h"
#include "CCMatchGlobal.h"
#include "CCPacketCrypter.h"


#define MATCHCLIENT_DEFAULT_UDP_PORT	10000
#define MAX_PING						999

class CCMatchPeerInfo
{
public:
	CCUID	uidChar;
	char	szIP[64];
	DWORD	dwIP;
	int		nPort;
	CCTD_CharInfo		CharInfo;
	//버프정보임시주석 CCTD_CharBuffInfo	CharBuffInfo;
	CCTD_ExtendInfo		ExtendInfo;

protected:
	bool				m_bUDPTestResult;
	bool				m_bUDPTestProcess;
	int					m_nUDPTestCount;

	bool				m_bOpened;
	int					m_nPing;
	int					m_nPingTryCount;
	unsigned int		m_nLastPingTime;
	unsigned int		m_nLastPongTime;
	CCCommandSNChecker	m_CommandSNChecker;
public:
	CCMatchPeerInfo() {
		memset(&CharInfo, 0, sizeof(CCTD_CharInfo));
		//버프정보임시주석 memset(&CharBuffInfo, 0, sizeof(CCTD_CharBuffInfo));

		uidChar = CCUID(0,0);
		szIP[0] = NULL;
		dwIP	= 0;
		nPort	= 0;

		m_bUDPTestResult = false;
		m_bUDPTestProcess = false;
		m_nUDPTestCount = 0;

		m_bOpened = false;
		m_nPing = 0;
		m_nLastPingTime = 0;
		m_nLastPongTime = 0;
		m_nPingTryCount = 0;
	}

	virtual ~CCMatchPeerInfo()			{}

	bool GetUDPTestResult()				{ return m_bUDPTestResult; }
	void SetUDPTestResult(bool bResult)	{ m_bUDPTestResult = bResult; }
	void StartUDPTest()					{ m_bUDPTestProcess = true; m_nUDPTestCount = 10; }
	void StopUDPTest()					{ m_bUDPTestProcess = false; m_nUDPTestCount = 0; }
	bool GetProcess()					{ return m_bUDPTestProcess; }
	int GetTestCount()					{ return m_nUDPTestCount; }
	void UseTestCount()					{ m_nUDPTestCount--; }

	bool IsOpened()						{ return m_bOpened; }
	void SetOpened(bool bVal)			{ m_bOpened = bVal; }
	int GetPing(unsigned int nCurrTime);
	void UpdatePing(unsigned int nTime, int nPing);
	void SetLastPingTime(unsigned int nTime);
	bool CheckCommandValidate(CCCommand* pCmd)
	{
		return m_CommandSNChecker.CheckValidate(pCmd->m_nSerialNumber);
	}
};


class CCMatchPeerInfoList : public map<CCUID, CCMatchPeerInfo*>
{
private:
	map<CCUID, CCMatchPeerInfo*>		m_IPnPortMap;
	CRITICAL_SECTION				m_csLock;
	void Lock()			{ EnterCriticalSection(&m_csLock); }
	void Unlock()		{ LeaveCriticalSection(&m_csLock); }
public:
	CCMatchPeerInfoList();
	virtual ~CCMatchPeerInfoList();
	void Clear();
	void Add(CCMatchPeerInfo* pPeerInfo);
	bool Delete(CCMatchPeerInfo* pPeerInfo);
	CCMatchPeerInfo* Find(const CCUID& uidChar);
	CCUID FindUID(DWORD dwIP, int nPort);
};

/// 게임 클라이언트
class CCMatchClient : public CCClient
{
protected:
// 핵 제작을 방해하기 위해 멤버변수의 위치를 빌드때마다 뒤섞기 위한 주석매크로(runtime/ShuffleCode.bat 실행)
// m_uidPlayer와 기타 uid들은 위조 커맨드를 생성하기 위해 알아내야 하는 값이다.
/* [[SHUFFLE_LINE]] CCMatchClient */	CCUID				m_uidPlayer;
/* [[SHUFFLE_LINE]] CCMatchClient */	CCMatchServerMode	m_nServerMode;				///< 서버모드
/* [[SHUFFLE_LINE]] CCMatchClient */	bool				m_bEnabledSurvivalMode;		///< 서바이벌 모드 활성화 여부 (퀘스트서버에서만 적용)
/* [[SHUFFLE_LINE]] CCMatchClient */	char				m_szAgentIP[32];

/* [[SHUFFLE_LINE]] CCMatchClient */	int					m_nServerPeerPort;
/* [[SHUFFLE_LINE]] CCMatchClient */	bool				m_bAgentPeerFlag;	// Agent와 UDP연결여부
/* [[SHUFFLE_LINE]] CCMatchClient */	CCUID				m_uidChannel;
/* [[SHUFFLE_LINE]] CCMatchClient */	bool				m_bAllowTunneling;
/* [[SHUFFLE_LINE]] CCMatchClient */	bool				m_bEnabledDuelTournament;	///< 듀얼토너먼트 활성화 여부

/* [[SHUFFLE_LINE]] CCMatchClient */	int					m_nAgentPeerCount;	// Agent와 UDP연결시 Retry를 위한 정보
/* [[SHUFFLE_LINE]] CCMatchClient */	int					m_nAgentPeerPort;

/* [[SHUFFLE_LINE]] CCMatchClient */	int					m_nAgentPort;

/* [[SHUFFLE_LINE]] CCMatchClient */	CCUID				m_uidAgentClient;
/* [[SHUFFLE_LINE]] CCMatchClient */	char				m_szServerName[64];

/* [[SHUFFLE_LINE]] CCMatchClient */	CCUID				m_uidServer;
/* [[SHUFFLE_LINE]] CCMatchClient */	int					m_nServerPort;
/* [[SHUFFLE_LINE]] CCMatchClient */	CCUID				m_uidStage;

/* [[SHUFFLE_LINE]] CCMatchClient */	char				m_szServerIP[32];
/* [[SHUFFLE_LINE]] CCMatchClient */	CCClientSocket		m_AgentSocket;

/* [[SHUFFLE_LINE]] CCMatchClient */	CCUID				m_uidAgentServer;

protected:
	CCMatchObjCacheMap	m_ObjCacheMap;
	CCSafeUDP			m_SafeUDP;
	CCMatchPeerInfoList	m_Peers;
	bool				m_bBridgePeerFlag;
	bool				m_bUDPTestProcess;
	CCPacketCrypter		m_AgentPacketCrypter;		///< 에이전트 암호화 키
	CCPacketCrypter		m_PeerPacketCrypter;		///< 클라이언트끼리 통신할때의 암호화 키


public:
	CCCommand* MakeCmdFromTunnelingBlob(const CCUID& uidSender, void* pBlob, int nBlobArrayCount);
	bool MakeTunnelingCommandBlob(CCCommand* pWrappingCmd, CCCommand* pSrcCmd);
protected:
	bool GetAgentPeerFlag()				{ return m_bAgentPeerFlag; }
	void SetAgentPeerFlag(bool bVal)	{ m_bAgentPeerFlag = bVal; }
	int GetAgentPeerCount()				{ return m_nAgentPeerCount; }
	void SetAgentPeerCount(int nCount)	{ m_nAgentPeerCount = nCount; }
	void StartAgentPeerConnect();
	void CastAgentPeerConnect();
	void StartUDPTest(const CCUID& uidChar);
	void InitPeerCrypt(const CCUID& uidStage, unsigned int nChecksum);
protected:
	// tcp socket event
	virtual bool OnSockConnect(SOCKET sock);
	virtual bool OnSockDisconnect(SOCKET sock);
	virtual bool OnSockRecv(SOCKET sock, char* pPacket, DWORD dwSize);
	virtual void OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);

	virtual int OnConnected(SOCKET sock, CCUID* pTargetUID, CCUID* pAllocUID, unsigned int nTimeStamp);
	virtual void OnRegisterCommand(CCCommandManager* pCommandManager);
	virtual bool OnCommand(CCCommand* pCommand);
	virtual int OnResponseMatchLogin(const CCUID& uidServer, 
									 int nResult, 
									 const char* szServerName, 
		                             const CCMatchServerMode nServerMode, 
									 const char* szAccountID, 
                                     const CCMatchUserGradeID nUGradeID, 
                                     const CCMatchPremiumGradeID nPGradeID,
									 const CCUID& uidPlayer,
									 bool bEnabledSurvivalMode,
									 bool bEnabledDuelTournament,
//									 unsigned char* szRandomValue,
									 unsigned char* pbyGuidReqMsg);
	virtual void OnObjectCache(unsigned int nType, void* pBlob, int nCount);
	virtual void OnUDPTest(const CCUID& uidChar);
	virtual void OnUDPTestReply(const CCUID& uidChar);
	virtual void OnResponseAgentLogin();
	virtual void OnLocateAgentToClient(const CCUID& uidAgent, char* szIP, int nPort, int nUDPPort);
	virtual void OnTunnelingTCP(const CCUID& uidSender, void* pBlob, int nCount);
	virtual void OnTunnelingUDP(const CCUID& uidSender, void* pBlob, int nCount);	
	virtual void OnAllowTunnelingTCP();
	virtual void OnAllowTunnelingUDP();	
	virtual void OnAgentConnected(const CCUID& uidAgentServer, const CCUID& uidAlloc);
	virtual void OnAgentError(int nError);

	void OutputLocalInfo(void);

	virtual void SendCommand(CCCommand* pCommand);
	bool SendCommandToAgent(CCCommand* pCommand);
	void SendCommandByTunneling(CCCommand* pCommand);
	void ParseUDPPacket(char* pData,CCPacketHeader* pPacketHeader,DWORD dwIP,unsigned int nPort);
public:
	void SendCommandByUDP(CCCommand* pCommand, char* szIP, int nPort);
/*
public :
	void SendCommandByUDP(CCCommand* pCommand, char* szIP, int nPort);
	*/

public:
	CCMatchClient();
	virtual ~CCMatchClient();

	bool Create(unsigned short nUDPPort);
	
	bool GetBridgePeerFlag()			{ return m_bBridgePeerFlag; }
	void SetBridgePeerFlag(bool bFlag)	{ m_bBridgePeerFlag = bFlag; }
	void AddPeer(CCMatchPeerInfo* pPeerInfo);
	//bool DeletePeer(const char* szIP);
	bool DeletePeer(const CCUID uid);
	CCUID FindPeerUID(const DWORD dwIP, const int nPort);
	CCMatchPeerInfo* FindPeer(const CCUID& uidChar);
	void ClearPeers();
	void CastStageBridgePeer(const CCUID& uidChar, const CCUID& uidStage);	// UDP lost 대비해 Stage입장시 여러번 호출 필요

	bool GetUDPTestProcess()			{ return m_bUDPTestProcess; }
	void SetUDPTestProcess(bool bVal)	{ m_bUDPTestProcess = bVal; }
	void UpdateUDPTestProcess();
	void GetUDPTraffic(int* nSendTraffic, int* nRecvTraffic)	{ return m_SafeUDP.GetTraffic(nSendTraffic, nRecvTraffic); }

	void SetUDPPort(int nPort);
	__forceinline CCUID GetServerUID() { return m_uidServer; }
	__forceinline CCUID GetPlayerUID()	{ return m_uidPlayer; }
	__forceinline CCUID GetChannelUID() { return m_uidChannel; }
	__forceinline CCUID GetStageUID() { return m_uidStage; }
	virtual CCUID GetSenderUIDBySocket(SOCKET socket);


	void SetServerAddr(const char* szIP, int nPort)	{ 
		strcpy(m_szServerIP,szIP), m_nServerPort = nPort; 
	}
	char* GetServerIP() { return m_szServerIP; }
	int GetServerPort() { return m_nServerPort; }
	void SetServerPeerPort(int nPeerPort) { m_nServerPeerPort = nPeerPort; }
	int GetServerPeerPort() { return m_nServerPeerPort; }

	CCMatchPeerInfoList* GetPeers() { return &m_Peers; }	
	CCSafeUDP* GetSafeUDP() { return &m_SafeUDP; }
	string GetObjName(const CCUID& uid);
	CCMatchObjCache* FindObjCache(const CCUID& uid);
	void ReplaceObjCache(CCMatchObjCache* pCache);
	void UpdateObjCache(CCMatchObjCache* pCache);
	void RemoveObjCache(const CCUID& uid);
	void ClearObjCaches();

	static bool UDPSocketRecvEvent(DWORD dwIP, WORD wRawPort, char* pPacket, DWORD dwSize);

public:
	void SetAgentAddr(const char* szIP, int nPort)	{ 
		strcpy(m_szAgentIP,szIP), m_nAgentPort = nPort; 
	}
	char* GetAgentIP() { return m_szAgentIP; }
	int GetAgentPort() { return m_nAgentPort; }
	void SetAgentPeerPort(int nPeerPort) { m_nAgentPeerPort = nPeerPort; }
	int GetAgentPeerPort() { return m_nAgentPeerPort; }
	const CCUID& GetAgentServerUID() { return m_uidAgentServer; }
	const CCUID& GetAgentClientUID() { return m_uidAgentClient; }
	bool GetAllowTunneling() { return m_bAllowTunneling; }
	void SetAllowTunneling(bool bAllow) { m_bAllowTunneling = bAllow; }

	int AgentConnect(SOCKET* pSocket, char* szIP, int nPort);
	void AgentDisconnect();

	CCMatchObjCacheMap* GetObjCacheMap() { return &m_ObjCacheMap; }
	CCMatchServerMode GetServerMode()	{ return m_nServerMode; }
	const char* GetServerName()			{ return m_szServerName; }

	bool IsEnabledSurvivalMode() const	{ return m_bEnabledSurvivalMode; }
	bool IsEnabledDuelTournament() const { return m_bEnabledDuelTournament; }

	virtual bool IsUDPCommandValidationCheck(int nCommandID) = 0;
};
