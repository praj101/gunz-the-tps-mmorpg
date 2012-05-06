#include "stdafx.h"
#include "CCMatchClient.h"
#include "CCErrorTable.h"
#include "CCSharedCommandTable.h"
#include "Msg.h"
#include "CCDebug.h"
#include "CCBlobArray.h"
#include "CCMatchStage.h"



CCMatchClient* g_pMatchClient = NULL;
CCMatchClient* GetMainMatchClient() { return g_pMatchClient; }

/////////////////////////////////////////////////////////////////////////////////////////

void MakeTCPCommandSerialNumber(CCCommand* pCmd)
{
	static unsigned char nSerial = 0;
	nSerial++;
	pCmd->m_nSerialNumber = nSerial;
}


void MakeUDPCommandSerialNumber(CCCommand* pCmd)
{
	static unsigned char nSerial = 0;
	nSerial++;
	pCmd->m_nSerialNumber = nSerial;
}

#define MAX_PING_TRY_COUNT		3

/////////////////////////////////////////////////////////////////////////////////////////
int CCMatchPeerInfo::GetPing(unsigned int nCurrTime) 
{ 
	if ((int)m_nLastPongTime - (int)m_nLastPingTime < 0) 
	{
		int nDelay = nCurrTime - m_nLastPingTime;
		if ((nDelay >= MAX_PING) && (m_nPingTryCount >= MAX_PING_TRY_COUNT))
		{
			return MAX_PING;
		}
	}

	return m_nPing;
}

void CCMatchPeerInfo::UpdatePing(unsigned int nTime, int nPing) 
{ 
	m_nLastPongTime = nTime;
	m_nPingTryCount = 0;
	m_nPing = nPing; 
}

void CCMatchPeerInfo::SetLastPingTime(unsigned int nTime) 
{ 
	if ((int)m_nLastPongTime - (int)m_nLastPingTime >= 0)
		m_nLastPingTime = nTime; 

	m_nPingTryCount++;
}

/////////////////////////////////////////////////////////////////////////////////////////
CCMatchPeerInfoList::CCMatchPeerInfoList()
{
	InitializeCriticalSection(&m_csLock);
}

CCMatchPeerInfoList::~CCMatchPeerInfoList()
{
	Clear();
	DeleteCriticalSection(&m_csLock);
	cclog("PeerInfoList Released\n");
}

bool CCMatchPeerInfoList::Delete(CCMatchPeerInfo* pPeerInfo)
{
	bool ret = false;
	Lock();
	iterator itor = find(pPeerInfo->uidChar);
	if (itor != end())
	{
		erase(itor);

		map<CCUID, CCMatchPeerInfo*>::iterator itorIPPortNode = 
			m_IPnPortMap.find(CCUID(pPeerInfo->dwIP, (unsigned long)pPeerInfo->nPort));

		if (itorIPPortNode != m_IPnPortMap.end())
		{
			m_IPnPortMap.erase(itorIPPortNode);
		}

		delete pPeerInfo; pPeerInfo = NULL;

		ret = true;
	}

	Unlock();
	return ret;
}

void CCMatchPeerInfoList::Clear() 
{ 
	Lock();

	while(empty()==false) 
	{ 
		delete (*begin()).second;
		erase(begin()); 
	} 

	m_IPnPortMap.clear();

	Unlock();
}

void CCMatchPeerInfoList::Add(CCMatchPeerInfo* pPeerInfo)
{
	Lock();
	insert(value_type(pPeerInfo->uidChar, pPeerInfo));

	CCUID uidIPPort = CCUID(pPeerInfo->dwIP, (unsigned long)pPeerInfo->nPort);
	m_IPnPortMap.insert(map<CCUID, CCMatchPeerInfo*>::value_type(uidIPPort, pPeerInfo));
	Unlock();
}

CCMatchPeerInfo* CCMatchPeerInfoList::Find(const CCUID& uidChar)
{
	CCMatchPeerInfo* pPeer = NULL;
	Lock();

	iterator itor = find(uidChar);
	if (itor != end())
	{
		pPeer = (*itor).second;
	}

	Unlock();
	return pPeer;
}

CCUID CCMatchPeerInfoList::FindUID(DWORD dwIP, int nPort)
{
	CCUID uidRet = CCUID(0,0);

	Lock();
	map<CCUID, CCMatchPeerInfo*>::iterator itor = m_IPnPortMap.find(CCUID(dwIP, (unsigned long)nPort));
	if (itor != m_IPnPortMap.end())
	{
		CCMatchPeerInfo* pPeerInfo = (*itor).second;
		uidRet = pPeerInfo->uidChar;
	}
	Unlock();

	return uidRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
CCMatchClient::CCMatchClient()
{
	g_pMatchClient = this;
	SetServerAddr("", 6000);
	SetServerPeerPort(7777);

	m_uidAgentServer = CCUID(0,0);
	m_uidAgentClient = CCUID(0,0);

	SetAgentAddr("", 6000);
	SetAgentPeerPort(7776);

	m_szServerName[0] = 0;
	m_nServerMode = CSM_NORMAL;
	m_bEnabledSurvivalMode = false;
	m_bEnabledDuelTournament = false;

//	m_SafeUDP.Create(true, MATCHCLIENT_DEFAULT_UDP_PORT);
}

CCMatchClient::~CCMatchClient()
{
	m_SafeUDP.SetCustomRecvCallback(NULL);
	m_SafeUDP.Destroy();
	ClearObjCaches();
}

bool CCMatchClient::Create(unsigned short nUDPPort)
{
	if (CCCommandCommunicator::Create() == false) return false;

	if (m_SafeUDP.Create(true, nUDPPort, false) == false) return false;	// REUSEADDR OFF

	m_SafeUDP.SetCustomRecvCallback(UDPSocketRecvEvent);
	SetUDPTestProcess(false);

	// Agent 소켓 이벤트 연결
	m_AgentSocket.SetCallbackContext(this);
	m_AgentSocket.SetConnectCallback(SocketConnectEvent);
	m_AgentSocket.SetDisconnectCallback(SocketDisconnectEvent);
	m_AgentSocket.SetRecvCallback(SocketRecvEvent);
	m_AgentSocket.SetSocketErrorCallback(SocketErrorEvent);

	return true;
}

CCUID CCMatchClient::GetSenderUIDBySocket(SOCKET socket)
{ 
	if (m_ClientSocket.GetSocket() == socket)
		return m_Server;
	else if (m_AgentSocket.GetSocket() == socket)
		return GetAgentServerUID();
	else
		return CCUID(0,0);
}

bool CCMatchClient::OnSockConnect(SOCKET sock)
{
	CCClient::OnSockConnect(sock);

	SetServerAddr(m_ClientSocket.GetHost(), m_ClientSocket.GetPort());

	return true;
}
bool CCMatchClient::OnSockDisconnect(SOCKET sock)
{
	CCClient::OnSockDisconnect(sock);
	OutputMessage("TCP Socket disconnected.", CCZMOM_LOCALREPLY);

	return true;
}
bool CCMatchClient::OnSockRecv(SOCKET sock, char* pPacket, DWORD dwSize)
{
	CCClient::OnSockRecv(sock, pPacket, dwSize);

	return true;
}
void CCMatchClient::OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode)
{
	CCClient::OnSockError(sock, ErrorEvent, ErrorCode);

	if (ErrorCode == 10053)
		OutputMessage(CCZMOM_LOCALREPLY, "Disconnected", ErrorCode);
	else
		OutputMessage(CCZMOM_LOCALREPLY, "TCP Socket Error(Code =  %d)", ErrorCode);	
}

bool CCMatchClient::OnCommand(CCCommand* pCommand)
{
	bool ret = CCClient::OnCommand(pCommand);

	if ( (pCommand->m_pCommandDesc->IsFlag(MCDT_PEER2PEER)==true) )
	{
		// Peer Network 안타고 OnCommand 불린경우 ComCCUID를 PlayerUID로 치환
		if (pCommand->GetSenderUID() == GetUID())
		{
			pCommand->SetSenderUID(GetPlayerUID());
		}
		else
		{
			// Peer의 패킷 시리얼은 여기서 체크한다.
			CCMatchPeerInfo* pPeer = FindPeer(pCommand->GetSenderUID());
			if (pPeer)
			{
				if (!pPeer->CheckCommandValidate(pCommand))
				{
					// 암호화안한 데이타는 무시
					if (pCommand->m_pCommandDesc->IsFlag(MCCT_NON_ENCRYPTED) == false)
					{
						return false;
					}
				}
			}
		}
	}


	switch(pCommand->GetID())
	{
		case MC_MATCH_RESPONSE_LOGIN:
			{
				int nResult;
				char nServerMode;
				unsigned char nUGradeID, nPGradeID;
				CCUID uidPlayer;
				char szServerName[256];
				char szAccountID[MAX_USERID_STRING_LEN];
				bool bEnabledSurvivalMode;
				bool bEnabledDuelTournament;

				pCommand->GetParameter(&nResult,		0, MPT_INT);
				pCommand->GetParameter(szServerName,	1, MPT_STR, sizeof(szServerName) );
				pCommand->GetParameter(&nServerMode,	2, MPT_CHAR);
				pCommand->GetParameter(szAccountID,		3, MPT_STR, MAX_USERID_STRING_LEN );
				pCommand->GetParameter(&nUGradeID,		4, MPT_UCHAR);
				pCommand->GetParameter(&nPGradeID,		5, MPT_UCHAR);
				pCommand->GetParameter(&uidPlayer,		6, MPT_UID);
				pCommand->GetParameter(&bEnabledSurvivalMode,	7, MPT_BOOL);
				pCommand->GetParameter(&bEnabledDuelTournament,	8, MPT_BOOL);
//				pCommand->GetParameter(szRandomValue,	7, MPT_STR, sizeof(szRandomValue) );

//				CCCommandParameter* pParam1 = pCommand->GetParameter(7);
//				if (pParam1->GetType() != MPT_BLOB)
//				{
//					break;
//				}
//				void* pBlob1 = pParam1->GetPointer();
//				unsigned char *szRandomValue = (unsigned char*)CCGetBlobArrayElement(pBlob1, 0);

				CCCommandParameter* pParam = pCommand->GetParameter(9);
				if (pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();
				if( NULL == pBlob )
					break;

				int nCount = CCGetBlobArrayCount(pBlob);
				unsigned char* pbyGuidReqMsg = (unsigned char*)CCGetBlobArrayElement(pBlob, 0);

				OnResponseMatchLogin(pCommand->GetSenderUID(), nResult, szServerName, CCMatchServerMode(nServerMode), 
					szAccountID, CCMatchUserGradeID(nUGradeID), CCMatchPremiumGradeID(nPGradeID), uidPlayer, bEnabledSurvivalMode, bEnabledDuelTournament, pbyGuidReqMsg);
			}
			break;
		case MC_MATCH_OBJECT_CACHE:
			{
				unsigned char nType;
				pCommand->GetParameter(&nType, 0, MPT_UCHAR);
				CCCommandParameter* pParam = pCommand->GetParameter(1);
				if(pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();
				if( NULL == pBlob )
					break;

				int nCount = CCGetBlobArrayCount(pBlob);
				OnObjectCache((unsigned int)nType, pBlob, nCount);
			}
			break;
		case MC_AGENT_RESPONSE_LOGIN:
			{
				OnResponseAgentLogin();
			}
			break;
		case MC_AGENT_LOCATETO_CLIENT:
			{
				CCUID uidAgent;
				char szIP[64];
				int nPort, nUDPPort;

				if (pCommand->GetParameter(&uidAgent, 0, MPT_UID) == false) break;
				if (pCommand->GetParameter(szIP, 1, MPT_STR, sizeof(szIP) ) == false) break;
				if (pCommand->GetParameter(&nPort, 2, MPT_INT) == false) break;
				if (pCommand->GetParameter(&nUDPPort, 3, MPT_INT) == false) break;

				OnLocateAgentToClient(uidAgent, szIP, nPort, nUDPPort);
			}
			break;
		case MC_AGENT_TUNNELING_TCP:
			{
				CCUID uidSender, uidReceiver;
				if (pCommand->GetParameter(&uidSender, 0, MPT_UID)==false) break;
				if (pCommand->GetParameter(&uidReceiver, 1, MPT_UID)==false) break;
				
				CCCommandParameter* pParam = pCommand->GetParameter(2);
				if (pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();
				if( NULL == pBlob )
					break;
				int nCount = CCGetBlobArrayCount(pBlob);

				OnTunnelingTCP(uidSender, pBlob, nCount);
			}
			break;
		case MC_AGENT_TUNNELING_UDP:
			{
				CCUID uidSender, uidReceiver;
				if (pCommand->GetParameter(&uidSender, 0, MPT_UID)==false) break;
				if (pCommand->GetParameter(&uidReceiver, 1, MPT_UID)==false) break;
				
				CCCommandParameter* pParam = pCommand->GetParameter(2);
				if (pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();
				if( NULL == pBlob )
					break;

				int nCount = CCGetBlobArrayCount(pBlob);

				OnTunnelingUDP(uidSender, pBlob, nCount);
			}
			break;			
		case MC_AGENT_ALLOW_TUNNELING_TCP:
			{
				OnAllowTunnelingTCP();
			}
			break;
		case MC_AGENT_ALLOW_TUNNELING_UDP:
			{
				OnAllowTunnelingUDP();
			}
			break;			
		case MC_AGENT_ERROR:
			{
				int nError;
				if (pCommand->GetParameter(&nError, 0, MPT_INT) == false) break;

				OnAgentError(nError);
			}
			break;

		case MC_VERSION:
			OutputMessage("MAIET MatchClient Version", CCZMOM_LOCALREPLY);
			break;
		case MC_NET_ENUM:
			break;
		case MC_NET_RESPONSE_INFO:
			break;
		case MC_PEER_UDPTEST:
			{
				OnUDPTest(pCommand->GetSenderUID());
			}
			break;
		case MC_PEER_UDPTEST_REPLY:
			{
				OnUDPTestReply(pCommand->GetSenderUID());
			}
			break;
		case MC_AGENT_DEBUGTEST:
			{

			}
			break;
		default:
			if (!ret)
			{
				return false;
			}
	}
	return true;
}

void CCMatchClient::OnRegisterCommand(CCCommandManager* pCommandManager)
{
	CCClient::OnRegisterCommand(pCommandManager);
	MAddSharedCommandTable(pCommandManager, MSCT_CLIENT);
}

void CCMatchClient::OutputLocalInfo()
{
	OutputMessage("MAIET Match Client", CCZMOM_LOCALREPLY);
	OutputMessage(CCZMOM_LOCALREPLY, "UID : %u:%u", m_This.High, m_This.Low);
	OutputMessage(CCZMOM_LOCALREPLY, "Connected Communicator : %u:%u", m_Server.High, m_Server.Low);
}

int CCMatchClient::OnConnected(SOCKET sock, CCUID* pTargetUID, CCUID* pAllocUID, unsigned int nTimeStamp)
{
#ifdef _DEBUG
	// UDP Socket Log 
	SOCKADDR_IN SockAddr;	int nErrorCode=0;
	int nAddrLen = sizeof(SOCKADDR_IN);
	if (getsockname(m_SafeUDP.GetLocalSocket(), (SOCKADDR*)&SockAddr, &nAddrLen) == SOCKET_ERROR)
		nErrorCode = WSAGetLastError();
	char* pszIP = inet_ntoa(SockAddr.sin_addr);
	unsigned int nPort = ntohs(SockAddr.sin_port);
	cclog("UDP Address = %s:%d \n", pszIP, nPort);
#endif

	if (sock == m_ClientSocket.GetSocket()) {
		int ret = CCClient::OnConnected(sock, pTargetUID, pAllocUID, nTimeStamp);

		return ret;
	} else if (sock == m_AgentSocket.GetSocket()) {
		OnAgentConnected(*pTargetUID, *pAllocUID);
		return MOK;
	} else {
		return MERR_UNKNOWN;
	}
}

void CCMatchClient::OnAgentConnected(const CCUID& uidAgentServer, const CCUID& uidAlloc)
{
	m_uidAgentServer = uidAgentServer;
	m_uidAgentClient = uidAlloc;

	SetAllowTunneling(false);

	CCPacketCrypterKey key;
	CCMakeSeedKey(&key, uidAgentServer, uidAlloc, 0);
	m_AgentPacketCrypter.InitKey(&key);

//	CCCommand* pCmd = CreateCommand(MC_AGENT_PEER_BIND, GetAgentServerUID());
//	pCmd->AddParameter(new CCCmdParaCCUID(GetPlayerUID()));
//	Post(pCmd);
}

int CCMatchClient::OnResponseMatchLogin(const CCUID& uidServer, int nResult, const char* szServerName,
									   const CCMatchServerMode nServerMode, 
									   const char* szAccountID,
									   const CCMatchUserGradeID nUGradeID, 
									   const CCMatchPremiumGradeID nPGradeID,
									   const CCUID& uidPlayer,
									   bool bEnabledSurvivalMode,
									   bool bEnabledDuelTournament,
//									   unsigned char* szRandomValue,
									   unsigned char* pbyGuidReqMsg)
{
	m_uidServer = uidServer;
	m_uidPlayer = uidPlayer;

	strcpy(m_szServerName, szServerName);
	m_nServerMode = nServerMode;
	m_bEnabledSurvivalMode = bEnabledSurvivalMode;
	m_bEnabledDuelTournament = bEnabledDuelTournament;

	return MOK;
}


void CCMatchClient::OnObjectCache(unsigned int nType, void* pBlob, int nCount)
{
	if (nType == MATCHCACHEMODE_REPLACE) {
		for(int i=0; i<nCount; i++){
			CCMatchObjCache* pCache = (CCMatchObjCache*)CCGetBlobArrayElement(pBlob, i);
			ReplaceObjCache(pCache);
		}
	} else {
		if (nType == MATCHCACHEMODE_UPDATE)
			ClearObjCaches();

		for(int i=0; i<nCount; i++){
			CCMatchObjCache* pCache = (CCMatchObjCache*)CCGetBlobArrayElement(pBlob, i);
			if (nType==MATCHCACHEMODE_ADD || nType==MATCHCACHEMODE_UPDATE)
				UpdateObjCache(pCache);
			else if (nType == MATCHCACHEMODE_REMOVE)
				RemoveObjCache(pCache->GetUID());
		}
	}
}

void CCMatchClient::CastStageBridgePeer(const CCUID& uidChar, const CCUID& uidStage)
{
	CCCommand* pCmd = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_BRIDGEPEER), GetServerUID(), m_This);		
	pCmd->AddParameter(new CCCommandParameterUID(uidChar));
	pCmd->AddParameter(new CCCommandParameterUInt(0));	// 수신측에서 IP로 치환됨
	pCmd->AddParameter(new CCCommandParameterUInt(0));		// 수신측에서 Port로 치환됨
	
	CCSafeUDP* pSafeUDP = GetSafeUDP();
	SendCommandByUDP(pCmd, GetServerIP(), GetServerPeerPort());

	delete pCmd;
}

void CCMatchClient::OnUDPTest(const CCUID& uidChar)
{
	CCMatchPeerInfo* pPeer = FindPeer(uidChar);
	if (pPeer) {

#ifdef _DEBUG
		if ( //(strcmp("발렌타인", pPeer->CharInfo.szName)==0) ||
			(strcmp("버드", pPeer->CharInfo.szName)==0) ||
			(strcmp("dddd", pPeer->CharInfo.szName)==0) ||
			(strcmp("라온하제5", pPeer->CharInfo.szName)==0) ||
			(strcmp("라온하제6", pPeer->CharInfo.szName)==0) )
		{
			return;
		}
#endif

		CCCommand* pCmd = CreateCommand(MC_PEER_UDPTEST_REPLY, uidChar);
		SendCommandByUDP(pCmd, pPeer->szIP, pPeer->nPort);
		delete pCmd;
	}
}

void CCMatchClient::OnUDPTestReply(const CCUID& uidChar)
{
//// UDPTEST LOG ////////////////////////////////
#ifdef _DEBUG
char szLog[64];
sprintf(szLog, "[%d:%d] UDP_TEST_REPLY: from (%d:%d) \n", 
		GetPlayerUID().High, GetPlayerUID().Low, uidChar.High, uidChar.Low);
cclog(szLog);
#endif
/////////////////////////////////////////////////
	CCMatchPeerInfo* pPeer = FindPeer(uidChar);
	if (pPeer) {
		pPeer->SetUDPTestResult(true);
		pPeer->StopUDPTest();
	}
}

void CCMatchClient::UpdateUDPTestProcess()
{
	int nProcessCount = 0;
	for (CCMatchPeerInfoList::iterator i=m_Peers.begin(); i!=m_Peers.end(); i++) {
		CCMatchPeerInfo* pPeer = (*i).second;
		if (pPeer->GetProcess()) {
			pPeer->UseTestCount();
			if (pPeer->GetTestCount() <= 0) {
				pPeer->StopUDPTest();

				CCCommand* pCmd = CreateCommand(MC_MATCH_REQUEST_PEER_RELAY, GetServerUID());
				pCmd->AddParameter(new CCCmdParaCCUID(GetPlayerUID()));
				pCmd->AddParameter(new CCCmdParaCCUID(pPeer->uidChar));
				Post(pCmd);
			} else {
				nProcessCount++;
			}
		}
	}
	if (nProcessCount <= 0)
		SetUDPTestProcess(false);
}

void CCMatchClient::OnResponseAgentLogin()
{
	CCCommand* pCmd = CreateCommand(MC_AGENT_PEER_BINDTCP, GetAgentServerUID());
	pCmd->AddParameter(new CCCmdParaCCUID(GetPlayerUID()));
	Post(pCmd);	

	cclog("Logged in Agent, Bind TCP \n");
}

void CCMatchClient::OnLocateAgentToClient(const CCUID& uidAgent, char* szIP, int nPort, int nUDPPort)
{
	SetAgentAddr(szIP, nPort);
	SetAgentPeerPort(nUDPPort);

	if (GetBridgePeerFlag() == false) {
		AgentConnect(NULL, szIP, nPort);
// #ifdef _DEBUG
		cclog("Connect to Agent by TCP (%s:%d) \n", szIP, nPort);
// #endif
	} else {
		StartAgentPeerConnect();
// #ifdef _DEBUG
		cclog("Connect to Agent by UDP (%s:%d) \n", szIP, nPort);
// #endif
	}
}

CCCommand* CCMatchClient::MakeCmdFromTunnelingBlob(const CCUID& uidSender, void* pBlob, int nBlobArrayCount)
{
	if (nBlobArrayCount != 1) 
	{
		cclog("MakeCmdFromTunnelingBlob: BlobArrayCount is not 1\n");
		return NULL;
	}

	char* pPacket = (char*)CCGetBlobArrayElement(pBlob, 0);

	int nSize = CCGetBlobArraySize(pBlob) - (sizeof(int) * 2);
	if ((nSize <= 0) || (nSize >= MAX_BLOB_SIZE))
	{
		cclog("MakeCmdFromTunnelingBlob: Blob Size Error(size = %d)\n", nSize);
		return NULL;
	}

	char* pData = new char[nSize];

	if (!m_PeerPacketCrypter.Decrypt(pPacket, nSize, pData, nSize))
	{
		delete [] pData;
		return NULL;
	}


	CCCommand* pCmd = new CCCommand();
	if (!pCmd->SetData(pData, &m_CommandManager))
	{
		delete [] pData;
		delete pCmd; 
		return NULL;
	}

	delete [] pData;

	pCmd->m_Sender = uidSender;
	pCmd->m_Receiver = m_This;

	CCMatchPeerInfo* pPeer = FindPeer(uidSender);
	if (pPeer == NULL)
	{
		delete pCmd;
		return NULL;
	}

	return pCmd;
}

void CCMatchClient::OnTunnelingTCP(const CCUID& uidSender, void* pBlob, int nCount)
{
	CCCommand* pCmd = MakeCmdFromTunnelingBlob(uidSender, pBlob, nCount);
	if (pCmd == NULL) return;

	LockRecv();
	m_CommandManager.Post(pCmd);
	UnlockRecv();
}

void CCMatchClient::OnTunnelingUDP(const CCUID& uidSender, void* pBlob, int nCount)
{
	CCCommand* pCmd = MakeCmdFromTunnelingBlob(uidSender, pBlob, nCount);
	if (pCmd == NULL) return;

	LockRecv();
	m_CommandManager.Post(pCmd);
	UnlockRecv();
}

void CCMatchClient::OnAllowTunnelingTCP()
{
	SetAllowTunneling(true);
}

void CCMatchClient::OnAllowTunnelingUDP()
{
	SetAllowTunneling(true);
	SetAgentPeerFlag(true);
	cclog("TUNNELING_UDP_ALLOWED \n");
}

void CCMatchClient::OnAgentError(int nError)
{
}

void CCMatchClient::SendCommand(CCCommand* pCommand)
{
	// P2P일경우에는 UDP를 이용
	if (pCommand->m_pCommandDesc->IsFlag(MCDT_PEER2PEER)==true)
	{
		// 시리얼 입력
		MakeUDPCommandSerialNumber(pCommand);

		if (GetBridgePeerFlag() == false) {
			SendCommandByTunneling(pCommand);
		} else {
			if (pCommand->GetReceiverUID() == CCUID(0,0)) {	// BroadCasting
				int nTunnelingCount = 0;

				// Peer2Peer 메세지는 Sender가 플레이어이다.
				for (CCMatchPeerInfoList::iterator itor = m_Peers.begin(); 
					itor != m_Peers.end(); ++itor)
				{
					CCMatchPeerInfo* pPeerInfo = (*itor).second;
					if ( (pPeerInfo->uidChar==CCUID(0,0)) || 
						 (pPeerInfo->uidChar != GetPlayerUID()) )	
					{
						if ( (pPeerInfo->GetProcess() == false) &&
							 (pPeerInfo->GetUDPTestResult() == false) )
							nTunnelingCount++;
						else
							SendCommandByUDP(pCommand, pPeerInfo->szIP, pPeerInfo->nPort);
					}
				}

				if (nTunnelingCount > 0) {
					SendCommandByTunneling(pCommand);
				}
			} else {
				CCMatchPeerInfo* pPeerInfo = FindPeer(pCommand->GetReceiverUID());
				if (pPeerInfo) {
					if ( (pPeerInfo->GetProcess() == false) &&
						 (pPeerInfo->GetUDPTestResult() == false) )
						SendCommandByTunneling(pCommand);
					else	
						SendCommandByUDP(pCommand, pPeerInfo->szIP, pPeerInfo->nPort);
				}
			}
		}
	}
	else 
	{
		if ( (pCommand->GetReceiverUID() != CCUID(0,0)) && 
			 (pCommand->GetReceiverUID() == GetAgentServerUID()) ) 
		{
			#ifdef _DEBUG
			bool bResult = SendCommandToAgent(pCommand);

			if (pCommand->GetID() == MC_AGENT_PEER_BINDTCP) {
				if (bResult)
					OutputDebugString("SendCommand(AGENT_PEER_BINDTCP) \n");
				else
					OutputDebugString("SendCommand(AGENT_PEER_FAILED) \n");
			}
			#endif
		} else {
			MakeTCPCommandSerialNumber(pCommand);
			CCClient::SendCommand(pCommand);
		}
	}
}

bool CCMatchClient::SendCommandToAgent(CCCommand* pCommand)
{
	int nPacketSize = CalcPacketSize(pCommand);
	char* pSendBuf = new char[nPacketSize];

	int size = MakeCmdPacket(pSendBuf, nPacketSize, &m_AgentPacketCrypter, pCommand);

	if (size > 0)
	{
		return m_AgentSocket.Send(pSendBuf, size);
	}
	else
	{
		delete [] pSendBuf;
		return false;
	}
}

void CCMatchClient::SendCommandByUDP(CCCommand* pCommand, char* szIP, int nPort)
{
	int nPacketSize = CalcPacketSize(pCommand);
	char* pSendBuf = new char[nPacketSize];

	// ##중요## - CCMatchServer, CCMatchAgent와 UDP통신할 때에는 암호화하지 않는 Command만 전송이 가능하다. 
	int nSize = MakeCmdPacket(pSendBuf, nPacketSize, &m_PeerPacketCrypter, pCommand);


	_ASSERT(nPacketSize > 0 && nPacketSize == nSize);

	if (nSize > 0)
	{
		m_SafeUDP.Send(szIP, nPort, pSendBuf, nSize);
	}
	else
	{
		_ASSERT(0);
		delete [] pSendBuf;
	}
}

bool CCMatchClient::MakeTunnelingCommandBlob(CCCommand* pWrappingCmd, CCCommand* pSrcCmd)
{
	// Create Param : Command Blob ////
	int nCmdSize = pSrcCmd->GetSize();
	if (nCmdSize == 0)
	{
		return false;
	}

	char* pCmdData = new char[nCmdSize];
	int nSize = pSrcCmd->GetData(pCmdData, nCmdSize);
	if (nSize != nCmdSize)
	{
		delete [] pCmdData; return false;
	}

	if (!m_PeerPacketCrypter.Encrypt(pCmdData, nSize))
	{
		delete [] pCmdData; return false;
	}

	void* pBlob = CCMakeBlobArray(nSize, 1);
	char* pCmdBlock = (char*)CCGetBlobArrayElement(pBlob, 0);
	CopyMemory(pCmdBlock, pCmdData, nSize);

	pWrappingCmd->AddParameter(new CCCmdParamBlob(pBlob, CCGetBlobArraySize(pBlob)));

	CCEraseBlobArray(pBlob);
	delete [] pCmdData;

	return true;
}

void CCMatchClient::SendCommandByTunneling(CCCommand* pCommand)
{
	if (GetAllowTunneling() == false) {
	} else {
		if (GetBridgePeerFlag() == false) {
			CCCommand* pCmd = CreateCommand(MC_AGENT_TUNNELING_TCP, GetAgentServerUID());
				pCmd->AddParameter(new CCCmdParaCCUID(GetPlayerUID()));
				pCmd->AddParameter(new CCCmdParaCCUID(pCommand->GetReceiverUID()));
				
				// Create Param : Command Blob ////
				if (!MakeTunnelingCommandBlob(pCmd, pCommand))
				{
					delete pCmd; pCmd=NULL; return;
				}
				///////////////////////////////////
			SendCommandToAgent(pCmd);
			delete pCmd;	// PACKETQUEUE 만들때까지 delete 임시로 사용
		} else {
			CCCommand* pCmd = CreateCommand(MC_AGENT_TUNNELING_UDP, GetAgentServerUID());
				pCmd->AddParameter(new CCCmdParaCCUID(GetPlayerUID()));
				pCmd->AddParameter(new CCCmdParaCCUID(pCommand->GetReceiverUID()));
				// Create Param : Command Blob ////
				if (!MakeTunnelingCommandBlob(pCmd, pCommand))
				{
					delete pCmd; pCmd=NULL; return;
				}
				///////////////////////////////////
			SendCommandByUDP(pCmd, GetAgentIP(), GetAgentPeerPort());
			delete pCmd;	// PACKETQUEUE 만들때까지 delete 임시로 사용
		}
	}
}

bool CCMatchClient::UDPSocketRecvEvent(DWORD dwIP, WORD wRawPort, char* pPacket, DWORD dwSize)
{
	if (GetMainMatchClient() == NULL) return false;
	if (dwSize < sizeof(CCPacketHeader)) return false;

	CCPacketHeader*	pPacketHeader;
	pPacketHeader = (CCPacketHeader*)pPacket;
	int nPacketSize = pPacketHeader->CalcPacketSize(&GetMainMatchClient()->m_PeerPacketCrypter);

	if ((dwSize != nPacketSize) || 
		((pPacketHeader->nMsg != MSGID_COMMAND) && (pPacketHeader->nMsg != MSGID_RAWCOMMAND)) ) return false;

	unsigned int nPort = ntohs(wRawPort);
	GetMainMatchClient()->ParseUDPPacket(&pPacket[sizeof(CCPacketHeader)], pPacketHeader, dwIP, nPort);
	return true;
}


bool CCMatchClient::DeletePeer(const CCUID uid)
{
	CCMatchPeerInfo* pPeer = FindPeer(uid);
	if (pPeer != NULL)
	{
		return m_Peers.Delete(pPeer);
	}
	return false;
}

void CCMatchClient::AddPeer(CCMatchPeerInfo* pPeerInfo)
{
	m_Peers.Add(pPeerInfo);
}

CCUID CCMatchClient::FindPeerUID(const DWORD dwIP, const int nPort)
{
	return m_Peers.FindUID(dwIP, nPort);
}

CCMatchPeerInfo* CCMatchClient::FindPeer(const CCUID& uidChar)
{
	return m_Peers.Find(uidChar);
}

void CCMatchClient::ClearPeers()
{
	m_Peers.Clear();
}

void CCMatchClient::SetUDPPort(int nPort)
{
	m_SafeUDP.Destroy();
	m_SafeUDP.Create(true, nPort);
}

string CCMatchClient::GetObjName(const CCUID& uid)
{
	CCMatchObjCache* pCache = FindObjCache(uid);
	char szName[32];
	if (pCache && strlen(pCache->GetName()) > 2)
		sprintf(szName, "%s", pCache->GetName());
	else
		sprintf(szName, "%d%d", uid.High, uid.Low);
	string name = szName;
	return name;
}

CCMatchObjCache* CCMatchClient::FindObjCache(const CCUID& uid)
{
	CCMatchObjCacheMap::iterator i = m_ObjCacheMap.find(uid);
	if (i == m_ObjCacheMap.end()) return NULL;
	return (*i).second;
}

void CCMatchClient::ReplaceObjCache(CCMatchObjCache* pCache)
{
	CCMatchObjCache* pScanCache = FindObjCache(pCache->GetUID());
	if (pScanCache == NULL) return;

	pScanCache->SetInfo(pCache->GetUID(), pCache->GetName(), pCache->GetClanName(), pCache->GetLevel(), 
						pCache->GetUGrade(), pCache->GetPGrade()
						, pCache->GetRank(), pCache->GetKillCount(), pCache->GetDeathCount(), pCache->GetDTGrade() );
	pScanCache->SetCLID(pCache->GetCLID());
	pScanCache->SetEmblemChecksum(pCache->GetEmblemChecksum());
	pScanCache->AssignCostume(pCache->GetCostume());
	pScanCache->SetFlags(pCache->GetFlags());
}

void CCMatchClient::UpdateObjCache(CCMatchObjCache* pCache)
{
	CCMatchObjCache* pScanCache = FindObjCache(pCache->GetUID());
	if (pScanCache != NULL) return;

	CCMatchObjCache* pNewCache = new CCMatchObjCache;

	pNewCache->SetInfo(pCache->GetUID(), pCache->GetName(), pCache->GetClanName(), pCache->GetLevel(), 
					   pCache->GetUGrade(), pCache->GetPGrade()
					   , pCache->GetRank(), pCache->GetKillCount(), pCache->GetDeathCount(), pCache->GetDTGrade());
	pNewCache->SetCLID(pCache->GetCLID());
	pNewCache->SetEmblemChecksum(pCache->GetEmblemChecksum());
	pNewCache->AssignCostume(pCache->GetCostume());
	pNewCache->SetFlags(pCache->GetFlags());

	_ASSERT(m_ObjCacheMap.find(pNewCache->GetUID())==m_ObjCacheMap.end());
	m_ObjCacheMap.Insert(pNewCache->GetUID(), pNewCache);
}

void CCMatchClient::RemoveObjCache(const CCUID& uid)
{
	CCMatchObjCacheMap::iterator i = m_ObjCacheMap.find(uid);
	if (i==m_ObjCacheMap.end()) return;

	CCMatchObjCache* pCache = i->second;
	delete pCache;
	m_ObjCacheMap.erase(i); // Channel Cache
}

void CCMatchClient::ClearObjCaches()
{
	while( m_ObjCacheMap.begin() != m_ObjCacheMap.end()) {
		CCMatchObjCache* pCache = (*m_ObjCacheMap.begin()).second;
		delete pCache;
		m_ObjCacheMap.erase(m_ObjCacheMap.begin());
	}
}

void CCMatchClient::ParseUDPPacket(char* pData, CCPacketHeader* pPacketHeader, DWORD dwIP, unsigned int nPort)
{
	switch (pPacketHeader->nMsg)
	{
	case MSGID_RAWCOMMAND:
		{
			unsigned short nCheckSum = MBuildCheckSum(pPacketHeader, pPacketHeader->nSize);
			if (pPacketHeader->nCheckSum != nCheckSum) {
				static int nLogCount = 0;
				if (nLogCount++ < 100) {	// Log Flooding 방지
					cclog("CCMatchClient::ParseUDPPacket() -> CHECKSUM ERROR(R=%u/C=%u)\n", 
						pPacketHeader->nCheckSum, nCheckSum);
				}
				return;
			} else {
				CCCommand* pCmd = new CCCommand();
				if (!pCmd->SetData(pData, &m_CommandManager))
				{
					cclog("CCMatchClient::ParseUDPPacket() -> SetData Error\n");

					delete pCmd;
					return;
				}

				CCUID uidPeer = FindPeerUID(dwIP, nPort);
				if (uidPeer != CCUID(0,0))
				{
					pCmd->m_Sender = uidPeer;
				} else {
					// TODO: 여기 수정해야함.
					sockaddr_in Addr;
					Addr.sin_addr.S_un.S_addr = dwIP;
					Addr.sin_port = nPort;
					char* pszIP = inet_ntoa(Addr.sin_addr);

					if (strcmp(pszIP, GetAgentIP()) == 0) 
					{
						pCmd->m_Sender = GetAgentServerUID();
					}
					else if( (MC_RESPONSE_SERVER_LIST_INFO == pCmd->GetID()) ||
						(MC_RESPONSE_BLOCK_COUNTRY_CODE_IP == pCmd->GetID()) )
					{
						// 특별히 하는건 없음.
						// Lcator는 Peer설정이 되지 않기때문에 여기서 따로 처리함.
					}
					else if (pCmd->GetID() == MC_UDP_PONG) 
					{
						// 특별히 하는건 없음. Command를 넘겨주기 위해서...(밑에 커맨드를 딜리트하기때문에)
					}	
					else 
					{
						delete pCmd; pCmd = NULL;
						return;
					}
				}

				pCmd->m_Receiver = m_This;

				if( IsUDPCommandValidationCheck(pCmd->GetID()) ) {
					LockRecv();				
					m_CommandManager.Post(pCmd);
					UnlockRecv();
				} else {
#ifdef _DEBUG
					cclog("%s(ID:%d) is Denied Command!\n"
						, pCmd->m_pCommandDesc->GetName(), pCmd->GetID());
#endif
				}
			}
		}
		break;
	case MSGID_COMMAND:
		{
			int nPacketSize = pPacketHeader->CalcPacketSize(&m_PeerPacketCrypter);
			unsigned short nCheckSum = MBuildCheckSum(pPacketHeader, nPacketSize);

			if (pPacketHeader->nCheckSum != nCheckSum) {
				static int nLogCount = 0;
				if (nLogCount++ < 100) {	// Log Flooding 방지
					cclog("CCMatchClient::ParseUDPPacket() -> CHECKSUM ERROR(R=%u/C=%u)\n", 
						pPacketHeader->nCheckSum, nCheckSum);
				}
				return;
			} else {
				CCCommand* pCmd = new CCCommand();

				int nCmdSize = nPacketSize - sizeof(CCPacketHeader);

				if (!m_PeerPacketCrypter.Decrypt(pData, nCmdSize))
				{
					cclog("CCMatchClient::ParseUDPPacket() -> Decrypt Error\n");

					delete pCmd; pCmd = NULL;
					return;
				}

				if (!pCmd->SetData(pData, &m_CommandManager))
				{
					// TODO: 여기 수정해야함.
					sockaddr_in Addr;
					Addr.sin_addr.S_un.S_addr = dwIP;
					Addr.sin_port = nPort;
					char* pszIP = inet_ntoa(Addr.sin_addr);

					cclog("CCMatchClient::ParseUDPPacket() -> MSGID_COMMAND SetData Error(%s:%d), size=%d\n", 
						pszIP, nPort, nCmdSize);

					delete pCmd; pCmd = NULL;
					return;
				}

				CCUID uidPeer = FindPeerUID(dwIP, nPort);
				if (uidPeer != CCUID(0,0)) {
					pCmd->m_Sender = uidPeer;
				} else {
					// Agent와는 암호화된 커맨드는 사용하지 않는다.
					delete pCmd;
					return;
/*
					// TODO: 여기 수정해야함.
					sockaddr_in Addr;
					Addr.sin_addr.S_un.S_addr = dwIP;
					Addr.sin_port = nPort;
					char* pszIP = inet_ntoa(Addr.sin_addr);

					if (strcmp(pszIP, GetAgentIP()) == 0) {
						pCmd->m_Sender = GetAgentServerUID();
					}else {
						delete pCmd; pCmd = NULL;
						return;
					}
*/
				}

				pCmd->m_Receiver = m_This;

				if( IsUDPCommandValidationCheck(pCmd->GetID()) ) {
					LockRecv();				
					m_CommandManager.Post(pCmd);
					UnlockRecv();
				} else {
#ifdef _DEBUG
					cclog("%s(ID:%d) is Denied Command!\n"
						, pCmd->m_pCommandDesc->GetName(), pCmd->GetID());
#endif
				}
			}
		}
		break;
	default:
		{
			Log("MatchClient: Parse Packet Error");
		}
		break;
	}
}

int CCMatchClient::AgentConnect(SOCKET* pSocket, char* szIP, int nPort)
{
	if (m_AgentSocket.Connect(pSocket, szIP, nPort))
		return MOK;
	else 
		return MERR_UNKNOWN;
}

void CCMatchClient::AgentDisconnect()
{
	m_AgentSocket.Disconnect();
}

void CCMatchClient::StartAgentPeerConnect()
{
	SetAgentPeerFlag(false);
	SetAgentPeerCount(10);
}

void CCMatchClient::CastAgentPeerConnect()
{
	CCCommand* pCmd = new CCCommand(m_CommandManager.GetCommandDescByID(MC_AGENT_PEER_BINDUDP), GetAgentServerUID(), m_This);		
	pCmd->AddParameter(new CCCommandParameterUID(GetPlayerUID()));
	pCmd->AddParameter(new CCCommandParameterString("localhost"));
	pCmd->AddParameter(new CCCommandParameterUInt(0));
	pCmd->AddParameter(new CCCommandParameterString(""));	// 수신측에서 IP로 치환됨
	pCmd->AddParameter(new CCCommandParameterUInt(0));		// 수신측에서 Port로 치환됨
	
	// CCSafeUDP* pSafeUDP = GetSafeUDP();
	SendCommandByUDP(pCmd, GetAgentIP(), GetAgentPeerPort());

	delete pCmd;
}


void CCMatchClient::StartUDPTest(const CCUID& uidChar)
{
	SetUDPTestProcess(true);
	if (uidChar == GetPlayerUID()) {
		for (CCMatchPeerInfoList::iterator i=m_Peers.begin(); i!=m_Peers.end(); i++) {
			CCMatchPeerInfo* pPeer = (*i).second;
			if (pPeer->GetUDPTestResult() == false)
				pPeer->StartUDPTest();
		}
	} else {
		CCMatchPeerInfo* pPeer = FindPeer(uidChar);
		if ( (pPeer) && (pPeer->GetUDPTestResult() == false) )
			pPeer->StartUDPTest();
	}
}

void CCMatchClient::InitPeerCrypt(const CCUID& uidStage, unsigned int nChecksum)
{
	//cclog("Init Peer Crypt (%u,%u,%u)\n", uidStage.High, uidStage.Low, nChecksum);

	CCPacketCrypterKey key;
	CCMakeSeedKey(&key, CCUID(3465, nChecksum), uidStage, 9578234);

	m_PeerPacketCrypter.InitKey(&key);
}