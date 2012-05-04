#include "stdafx.h"
#include "CCClient.h"
#include "CCSharedCommandTable.h"
#include <stdarg.h>
#include "CCErrorTable.h"
#include "Msg.h"
#include "MDebug.h"
#include "CCCommandBuilder.h"
#include "MMatchUtil.h"
//#include "CCPacketHShieldCrypter.h"
#include "MMatchConfig.h"


CCClient* CCClient::m_pInstance = NULL;

void SplitIAddress(char* szIP, int* pPort, const char* szAddress)
{
	*szIP = NULL;
	*pPort = 0;
	//*szPort = NULL;

	char* szCopyAddress = new char[strlen(szAddress)+2];
	strcpy(szCopyAddress, szAddress);

	char seps[] =":\n";
	char* token = strtok(szCopyAddress, seps);
	if(token!=NULL) strcpy(szIP, token);
	token = strtok(NULL, seps);
	if(token!=NULL) *pPort = atoi(token);

	delete[] szCopyAddress;
}


CCClient::CCClient()
{
	// _ASSERT(m_pInstance==NULL); 여러개의 클라이언트를 생성할수 있을경우 - by 추교성. KeeperManager.
	m_pInstance = this;

	m_pCommandBuilder = new CCCommandBuilder(CCUID(0,0), CCUID(0,0), GetCommandManager());

//	m_iPBufferTop = 0;
	m_Server.SetInvalid();

	InitializeCriticalSection(&m_csRecvLock);

	// 소켓 이벤트 연결
	m_ClientSocket.SetCallbackContext(this);
	m_ClientSocket.SetConnectCallback(SocketConnectEvent);
	m_ClientSocket.SetDisconnectCallback(SocketDisconnectEvent);
	m_ClientSocket.SetRecvCallback(SocketRecvEvent);
	m_ClientSocket.SetSocketErrorCallback(SocketErrorEvent);
}

CCClient::~CCClient()
{
	delete m_pCommandBuilder;
	m_pCommandBuilder = NULL;

	DeleteCriticalSection(&m_csRecvLock);
}

CCClient* CCClient::GetInstance(void)
{
	return m_pInstance;
}

CCUID CCClient::GetSenderUIDBySocket(SOCKET socket)
{ 
	if (m_ClientSocket.GetSocket() == socket)
		return m_Server;
	else
		return CCUID(0,0);
}

int CCClient::OnConnected(CCUID* pTargetUID, CCUID* pAllocUID, unsigned int nTimeStamp, CCCommObject* pCommObj)
{
	_ASSERT(FALSE);	// Don't use this OnConnected method
	return MERR_UNKNOWN;
}

int CCClient::OnConnected(SOCKET sock, CCUID* pTargetUID, CCUID* pAllocUID, unsigned int nTimeStamp)
{
	if (sock == m_ClientSocket.GetSocket()) {
		int ret = CCCommandCommunicator::OnConnected(pTargetUID, pAllocUID, nTimeStamp, NULL);
		m_Server = *pTargetUID;

		CCCommandBuilder* pCmdBuilder = GetCommandBuilder();

		// 암호키 설정
		CCPacketCrypterKey key;
		MMakeSeedKey(&key, *pTargetUID, *pAllocUID, nTimeStamp);
		m_ServerPacketCrypter.InitKey(&key);
		pCmdBuilder->InitCrypt(&m_ServerPacketCrypter, false);

		return MOK;
	} else {
		return MERR_UNKNOWN;
	}
}

void CCClient::OnRegisterCommand(CCCommandManager* pCommandManager)
{
}

bool CCClient::OnCommand(CCCommand* pCommand)
{
	char szMessage[256];
	switch(pCommand->GetID()){
		case MC_LOCAL_INFO:
			OutputLocalInfo();
			break;
		case MC_LOCAL_ECHO:
			if(pCommand->GetParameter(szMessage, 0, MPT_STR, sizeof(szMessage))==false) break;
			OutputMessage(szMessage, CCZMOM_LOCALREPLY);
			break;
		case MC_HELP:
			OutputMessage("MAIET Default Help :", CCZMOM_LOCALREPLY);
			for(int i=0; i<m_CommandManager.GetCommandDescCount(); i++){
				CCCommandDesc* pCmdDesc = m_CommandManager.GetCommandDesc(i);
				static char szMessage[1024];
				sprintf(szMessage, "%s : %s", pCmdDesc->GetName(), pCmdDesc->GetDescription());
				OutputMessage(szMessage, CCZMOM_LOCALREPLY);
				char szSyntax[256];
				m_CommandManager.GetSyntax(szSyntax, pCmdDesc);
				sprintf(szMessage, "[Syntax] %s", szSyntax);
				OutputMessage(szMessage, CCZMOM_LOCALREPLY);
			}
			break;
		case MC_NET_CONNECT:
			{
				char szAddress[256];
				if( !pCommand->GetParameter(szAddress, 0, MPT_STR, sizeof(szAddress)) )
					break;

				char szIP[256];
				int nPort;
				SplitIAddress(szIP, &nPort, szAddress);

				SOCKET socket;
				int nReturn = Connect(&socket, szIP, nPort);
				if(nReturn!=MOK){
					OutputMessage("Can't connect to communicator", CCZMOM_ERROR);
					break;
				}
			}
			break;
		case MC_NET_CONNECTTOZONESERVER:
			{
				SOCKET socket;
				int nReturn = Connect(&socket, "127.0.0.1", 6000);
				if(nReturn!=MOK){
					OutputMessage("Can't connect to communicator", CCZMOM_ERROR);
					break;
				}
			}
			break;
		case MC_NET_DISCONNECT:
			Disconnect(m_Server);
			break;

		case MC_NET_ECHO:
			if(pCommand->GetParameter(szMessage, 0, MPT_STR, sizeof(szMessage) )==false) break;
			OutputMessage(szMessage, CCZMOM_LOCALREPLY);
			break;
		default:
			return false;
	}

	return true;
}

int CCClient::Connect(SOCKET* pSocket, char* szIP, int nPort)
{
	if (m_ClientSocket.Connect(pSocket, szIP, nPort))
		return MOK;
	else 
		return MERR_UNKNOWN;
}

int CCClient::Connect(CCCommObject* pCommObj)
{
	if (m_ClientSocket.Connect(NULL, pCommObj->GetIPString(), pCommObj->GetPort()))
		return MOK;
	else
		return MERR_UNKNOWN;
}

void CCClient::SendCommand(CCCommand* pCommand)
{
	int nPacketSize = CalcPacketSize(pCommand);
	char* pSendBuf = new char[nPacketSize];

	int size = MakeCmdPacket(pSendBuf, nPacketSize, &m_ServerPacketCrypter, pCommand);

	if (size > 0)
	{
		if(!m_ClientSocket.Send(pSendBuf, size))
		{
			delete [] pSendBuf;	// 2006.12.4 dubble added. 실제 커넥션이 이루어지지 않았을경우, 메모리 릭
		}
	}
	else
	{
		delete [] pSendBuf;
	}
}

bool CCClient::Post(CCCommand* pCommand)
{
	LockRecv();
	bool bRet = CCCommandCommunicator::Post(pCommand);
	UnlockRecv();
	return bRet;
}

bool CCClient::Post(char* szErrMsg, int nErrMsgCount, const char* szCommand)
{
	return CCCommandCommunicator::Post(szErrMsg, nErrMsgCount, szCommand);
}

CCCommand* CCClient::GetCommandSafe()
{
	LockRecv();
	CCCommand* pCmd = CCCommandCommunicator::GetCommandSafe();
	UnlockRecv();

	return pCmd;
}

void CCClient::Disconnect( const CCUID&  uid )
{
	m_ClientSocket.Disconnect();
}

int CCClient::MakeCmdPacket(char* pOutPacket, int iMaxPacketSize, CCPacketCrypter* pPacketCrypter, CCCommand* pCommand)
{
	CCCommandMsg* pMsg = (CCCommandMsg*)pOutPacket;

	int nCmdSize = iMaxPacketSize-sizeof(CCPacketHeader);

	pMsg->Buffer[0] = 0;
	pMsg->nCheckSum = 0;
	int nPacketSize = 0;

	if(pCommand->m_pCommandDesc->IsFlag(MCCT_NON_ENCRYPTED))
	{
		// 암호화하지 않는 커맨드
		pMsg->nMsg = MSGID_RAWCOMMAND;

		nCmdSize = pCommand->GetData(pMsg->Buffer, nCmdSize);
		pMsg->nSize = (unsigned short)(sizeof(CCPacketHeader) + nCmdSize);

		nPacketSize = pMsg->nSize;
	}
	else
	{
		if (pPacketCrypter == NULL) 
		{
			_ASSERT(0);
			return 0;
		}

		// 암호화되는 커맨드
		pMsg->nMsg = MSGID_COMMAND;

		nCmdSize = pCommand->GetData(pMsg->Buffer, nCmdSize);
		nPacketSize = sizeof(CCPacketHeader) + nCmdSize;
		pMsg->nSize = (unsigned short)(nPacketSize);

//#ifdef _HSHIELD
//		// 핵실드 암호화
//		DWORD dwRet = CCPacketHShieldCrypter::Encrypt((PBYTE)&pMsg->nSize, sizeof(unsigned short));
//		if(dwRet != ERROR_SUCCESS)
//		{
//			mlog("CCClient::MakeCmdPacket -> HShield Encrypt error. (Error code : %d)\n", dwRet);
//			return 0;
//		}
//#else
		// size 암호화
		if (!pPacketCrypter->Encrypt((char*)&pMsg->nSize, sizeof(unsigned short)))
		{
			mlog("CCClient::MakeCmdPacket -> Size Encrypt error\n");
			return 0;
		}
//#endif
		// 커맨드 암호화
		if (!pPacketCrypter->Encrypt(pMsg->Buffer, nCmdSize))
		{
			mlog("CCClient::MakeCmdPacket -> Cmd Encrypt error\n");
			return 0;
		}

	}

	pMsg->nCheckSum = MBuildCheckSum(pMsg, nPacketSize);


	return nPacketSize;
}


bool CCClient::OnSockConnect(SOCKET sock)
{
	return true;
}
bool CCClient::OnSockDisconnect(SOCKET sock)
{
	return true;
}
bool CCClient::OnSockRecv(SOCKET sock, char* pPacket, DWORD dwSize)
{
	// New Cmd Buffer ////////////////
	CCCommandBuilder* pCmdBuilder = GetCommandBuilder();
	pCmdBuilder->SetUID(m_This, GetSenderUIDBySocket(sock));
	pCmdBuilder->Read((char*)pPacket, dwSize);

	LockRecv();
	while(CCCommand* pCmd = pCmdBuilder->GetCommand()) {
		//// LOG ////////
		#ifdef _DEBUG
		// char szBuf[64];
		// sprintf(szBuf, "Recv Command(%d) , %d Bytes \n", pCmd->GetID(), dwSize);
		// OutputDebugString(szBuf);
		#endif
		/////////////////
		Post(pCmd);
	}
	UnlockRecv();

	while(CCPacketHeader* pNetCmd = pCmdBuilder->GetNetCommand()) {
		if (pNetCmd->nMsg == MSGID_REPLYCONNECT) {
			MReplyConnectMsg* pMsg = (MReplyConnectMsg*)pNetCmd;
			CCUID HostUID, AllocUID;
			unsigned int nTimeStamp;

			HostUID.High = pMsg->nHostHigh;
			HostUID.Low = pMsg->nHostLow;
			AllocUID.High = pMsg->nAllocHigh;
			AllocUID.Low = pMsg->nAllocLow;
			nTimeStamp = pMsg->nTimeStamp;
			
			free(pNetCmd);

			LockRecv();
			OnConnected(sock, &HostUID, &AllocUID, nTimeStamp);
			UnlockRecv();
		}
	}

	return true;
}

void CCClient::OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode)
{

}

bool CCClient::SocketRecvEvent(void* pCallbackContext, SOCKET sock, char* pPacket, DWORD dwSize)
{
	CCClient* pClient = (CCClient*)pCallbackContext;

	return pClient->OnSockRecv(sock, pPacket, dwSize);
}

bool CCClient::SocketConnectEvent(void* pCallbackContext, SOCKET sock)
{
	CCClient* pClient = (CCClient*)pCallbackContext;

	return pClient->OnSockConnect(sock);
}
bool CCClient::SocketDisconnectEvent(void* pCallbackContext, SOCKET sock)
{
	CCClient* pClient = (CCClient*)pCallbackContext;
	return pClient->OnSockDisconnect(sock);
}

void CCClient::SocketErrorEvent(void* pCallbackContext, SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode)
{
	CCClient* pClient = (CCClient*)pCallbackContext;

	pClient->OnSockError(sock, ErrorEvent, ErrorCode);
}

void CCClient::OutputMessage(CCZMOMType nType, const char *pFormat,...)
{
	va_list args;
	static char temp[1024];

	va_start(args, pFormat);
	vsprintf(temp, pFormat, args);
	OutputMessage(temp, nType);
	va_end(args);
}


/////////////////////////////////////////////////////////////////////
CCRingBuffer::CCRingBuffer()
{
	m_iBufSize = 0;
	m_Buf = NULL;
	m_cpBegin = NULL;
	m_cpEnd = NULL;
}
CCRingBuffer::CCRingBuffer(int iBufSize)
{
	m_iBufSize = 0;
	m_Buf = NULL;
	m_cpBegin = NULL;
	m_cpEnd = NULL;

	Reserve(iBufSize);
}

CCRingBuffer::~CCRingBuffer()
{
	if (m_Buf) delete [] m_Buf;
}

void CCRingBuffer::Reserve(int iBufSize)
{
	if (m_Buf) delete [] m_Buf;

	m_Buf = new char[iBufSize];
	memset(m_Buf, 0, iBufSize);

	m_iBufSize = iBufSize;
}

bool CCRingBuffer::Enqueue(char* cp, int iDataSize)
{
		

	return true;
}
bool CCRingBuffer::Dequeue(char* cpOut, int iDataSize)
{

	return true;
}
