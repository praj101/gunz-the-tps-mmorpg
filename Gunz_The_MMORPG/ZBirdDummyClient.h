#ifndef _ZBIRDDUMMYCLIENT_H
#define _ZBIRDDUMMYCLIENT_H

// 여기서부터 테스트를 위한 코드 - Bird ////////////////////////////////////////////////
//#ifdef _BIRDTEST

#include "MCommandCommunicator.h"
#include "MClient.h"
#include "ZGameClient.h"

class MClientCommandProcessor;
class MCommand;
class ZBirdDummyClient;

typedef void (*ZBT_DummyONCommand)(ZBirdDummyClient*, MCommand* pCmd);

class ZBirdDummyClient : public MCommandCommunicator
{
protected:
	int					m_nDummyID;

	MClientSocket		m_ClientSocket;
//	char				m_PacketBuffer[MAX_PACKETBUFFER_SIZE];
	int					m_nPBufferTop;
	CRITICAL_SECTION	m_csRecvLock;	///< CommandQueue critical section
	MCommandBuilder*	m_pCommandBuilder;
	CCUID				m_Server;		///< 연결된 커뮤니케이터

	ZBT_DummyONCommand	m_fnOnCommandCallBack;
protected:
	CCUID				m_uidServer;
	CCUID				m_uidPlayer;
	CCUID				m_uidChannel;
	CCUID				m_uidStage;
	char				m_szChannel[256];
	char				m_szStageName[256];
	char				m_szPlayerName[256];
protected:
	void LockRecv() { EnterCriticalSection(&m_csRecvLock); }
	void UnlockRecv() { LeaveCriticalSection(&m_csRecvLock); }

	virtual void OnRegisterCommand(MCommandManager* pCommandManager);
	virtual bool OnCommand(MCommand* pCommand);

	virtual void OutputLocalInfo(void);
	virtual void OutputMessage(const char* szMessage, MZMOMType nType=MZMDM_GENERAL);

	virtual void SendCommand(MCommand* pCommand);
	virtual MCommand* GetCommandSafe();
	virtual int Connect(MCommObject* pCommObj) { return 0; }
	int OnConnected(SOCKET sock, CCUID* pTargetUID, CCUID* pAllocUID);

	int MakeCmdPacket(char* pOutPacket, int iMaxPacketSize, MCommand* pCommand);
	CCUID GetSenderUIDBySocket(SOCKET socket);

	// Socket Event
	bool OnSockConnect(SOCKET sock);
	bool OnSockDisconnect(SOCKET sock);
	bool OnSockRecv(SOCKET sock, char* pPacket, DWORD dwSize);
	void OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);
protected:
	int OnResponseMatchLogin(const CCUID& uidServer, int nResult, const CCUID& uidPlayer);
	void OnResponseRecommandedChannel(const CCUID& uidChannel, char* szChannel);
	void OnStageJoin(const CCUID& uidChar, const CCUID& uidStage, char* szStageName);
	void OnStageLeave(const CCUID& uidChar, const CCUID& uidStage);
public:
	ZBirdDummyClient();
	virtual ~ZBirdDummyClient();
	void Create(int nID, ZBT_DummyONCommand pCallBack);
public:
	bool Post(MCommand* pCommand);
	int Connect(SOCKET* pSocket, char* szIP, int nPort);
	void Disconnect(CCUID uid);

	static bool SocketRecvEvent(void* pCallbackContext, SOCKET sock, char* pPacket, DWORD dwSize);
	static bool SocketConnectEvent(void* pCallbackContext, SOCKET sock);
	static bool SocketDisconnectEvent(void* pCallbackContext, SOCKET sock);
	static void SocketErrorEvent(void* pCallbackContext, SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);

public:
	CCUID GetServerUID() { return m_uidServer; }
	CCUID GetPlayerUID()	{ return m_uidPlayer; }
	CCUID GetChannelUID() { return m_uidChannel; }
	CCUID GetStageUID() { return m_uidStage; }
	const char* GetChannelName() { return m_szChannel; }
	const char* GetStageName() { return m_szStageName; }
	void SetPlayerName(const char* szPlayerName) { strcpy(m_szPlayerName, szPlayerName); }
	const char* GetPlayerName() { return m_szPlayerName; }

	int GetDummyID() { return m_nDummyID; }
};

bool ZBirdPostCommand(ZBirdDummyClient* pDummyClient, MCommand* pCmd);
MCommand* ZNewBirdCmd(int nID);

#define ZBIRDPOSTCMD0(_CLIENT, _ID)									{ MCommand* pC=ZNewCmd(_ID); ZBirdPostCommand(_CLIENT, pC); }
#define ZBIRDPOSTCMD1(_CLIENT, _ID, _P0)								{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); ZBirdPostCommand(_CLIENT, pC); }
#define ZBIRDPOSTCMD2(_CLIENT, _ID, _P0, _P1)						{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); ZBirdPostCommand(_CLIENT, pC); }
#define ZBIRDPOSTCMD3(_CLIENT, _ID, _P0, _P1, _P2)					{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); ZBirdPostCommand(_CLIENT, pC); }
#define ZBIRDPOSTCMD4(_CLIENT, _ID, _P0, _P1, _P2, _P3)				{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); ZBirdPostCommand(_CLIENT, pC); }
#define ZBIRDPOSTCMD5(_CLIENT, _ID, _P0, _P1, _P2, _P3, _P4)			{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); ZBirdPostCommand(_CLIENT, pC); }
#define ZBIRDPOSTCMD6(_CLIENT, _ID, _P0, _P1, _P2, _P3, _P4, _P5)	{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); ZBirdPostCommand(_CLIENT, pC); }
#define ZBIRDPOSTCMD7(_CLIENT, _ID, _P0, _P1, _P2, _P3, _P4, _P5, _P6)	{ MCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); pC->AP(_P6); ZBirdPostCommand(_CLIENT, pC); }

void AddToLogFrame(int nDummyID, const char* szStr);

#ifdef _BIRDTEST
bool OnCommonLogin(ZBirdDummyClient* pClient, MCommand* pCmd);
#endif
//#endif


#endif