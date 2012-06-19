#include "stdafx.h"
#include "CCSafeUDP.h"
#include "CCBasePacket.h"
#include <time.h>
#include "CCCrashDump.h"
#ifndef _PUBLISH
#include "CCProcessController.h"
#endif

#ifdef _DEBUG
	#include "assert.h"
#endif


// 에이스사가 컴파일 할 경우에는 이 _OLD_SAFEUDP를 활성화해주세요 - 남기룡(2005/07/26)
//#define _OLD_SAFEUDP


#define MAX_RECVBUF_LEN		65535

#define SAFEUDP_MAX_SENDQUEUE_LENGTH		5120
#define SAFEUDP_MAX_ACKQUEUE_LENGTH			5120
#define SAFEUDP_MAX_ACKWAITQUEUE_LENGTH		64

#define SAFEUDP_SAFE_MANAGE_TIME			100		// WSA_INFINITE for debug
#define SAFEUDP_SAFE_RETRANS_TIME			500
#define SAFEUDP_MAX_SAFE_RETRANS_TIME		5000


#define LINKSTATE_LOG
void MTRACE(char* pszLog)
{
#ifdef _DEBUG
	char szBuf[_MAX_DIR];
	sprintf(szBuf, "%s", pszLog);
	OutputDebugString(szBuf);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////
// CCNetLink ////////////////////////////////////////////////////////////////////////////////
CCNetLink::CCNetLink()
{
	m_pSafeUDP = NULL;
	m_bConnected = false;
	m_nLinkState = LINKSTATE_CLOSED;
	memset((char*)&m_Address, 0, sizeof(sockaddr_in));
	m_nNextReadIndex = 0;
	m_nNextWriteIndex = 0;
	m_dwAuthKey = 0;
	m_pUserData = NULL;

	CCTime::GetTime(&m_tvConnectedTime);
	CCTime::GetTime(&m_tvLastPacketRecvTime);
}

CCNetLink::~CCNetLink()
{
	for (ACKWaitListItor itor = m_ACKWaitQueue.begin(); itor != m_ACKWaitQueue.end(); ) {
		delete (*itor);
		itor = m_ACKWaitQueue.erase(itor);
	}
}

void CCNetLink::SetLinkState(CCNetLink::LINKSTATE nState) 
{ 
	if (m_nLinkState == nState)
		return;

	m_nLinkState = nState; 

	#ifdef LINKSTATE_LOG
	switch(m_nLinkState) {
	case LINKSTATE_CLOSED:
		Setconnected(false);
		MTRACE("<LINK_STATE> LINKSTATE_CLOSED \n");
		if (m_pSafeUDP && m_pSafeUDP->m_fnNetLinkStateCallback)
			m_pSafeUDP->m_fnNetLinkStateCallback(this, LINKSTATE_CLOSED);
		break;
	case LINKSTATE_ESTABLISHED:
		Setconnected(true);
		MTRACE("<LINK_STATE> LINKSTATE_ESTABLISHED \n");
		if (m_pSafeUDP && m_pSafeUDP->m_fnNetLinkStateCallback)
			m_pSafeUDP->m_fnNetLinkStateCallback(this, LINKSTATE_ESTABLISHED);
		break;
	case LINKSTATE_SYN_SENT:
		MTRACE("<LINK_STATE> LINKSTATE_SYN_SENT \n");
		break;
	case LINKSTATE_SYN_RCVD:
		MTRACE("<LINK_STATE> LINKSTATE_SYN_RCVD \n");
		break;
	case LINKSTATE_FIN_SENT:
		MTRACE("<LINK_STATE> LINKSTATE_FIN_SENT \n");
		break;
	case LINKSTATE_FIN_RCVD:
		MTRACE("<LINK_STATE> LINKSTATE_FIN_RCVD \n");
		break;
	};
	#endif
}

bool CCNetLink::MakeSockAddr(char* pszIP, int nPort, sockaddr_in* pSockAddr)
{
	sockaddr_in 	RemoteAddr;
	memset((char*)&RemoteAddr, 0, sizeof(sockaddr_in));

	//	Set Dest IP and Port 
	RemoteAddr.sin_family = AF_INET;
	RemoteAddr.sin_port = htons(nPort);
	DWORD dwAddr = inet_addr(pszIP);
	if (dwAddr != INADDR_NONE) {
		memcpy(&(RemoteAddr.sin_addr), &dwAddr, 4);
	} else {		// 연결할 host name을 입력한 경우
		HOSTENT* pHost = gethostbyname(pszIP);
		if (pHost == NULL) {	// error
#ifdef _DEBUG
			OutputDebugString("<SAFEUDP_ERROR> Can't resolve hostname </SAFEUDP_ERROR>\n");
#endif
			return false;
		}
		memcpy((char FAR *)&(RemoteAddr.sin_addr), pHost->h_addr, pHost->h_length);
	}
	// Socket Error가 가끔 난다?
	//Gunzd.exe의 0x7c94b21a에 첫째 예외가 있습니다. 0xC0000005: 0xcdcdcddd 위치를 기록하는 동안 액세스 위반이 발생했습니다.
	memcpy(pSockAddr, &RemoteAddr, sizeof(sockaddr_in));
	return true;
}

bool CCNetLink::SetAddress(char* pszIP, int nPort)
{
	return MakeSockAddr(pszIP, nPort, &m_Address);
}

__int64 CCNetLink::GetMapKey()
{
	__int64 nKey = GetRawPort();
	nKey = nKey << 32;
	nKey += GetIP();
	return nKey;
}

__int64 CCNetLink::GetMapKey(sockaddr_in* pSockAddr)
{
	__int64 nKey =  pSockAddr->sin_port;
	nKey = nKey << 32;
	nKey += pSockAddr->sin_addr.S_un.S_addr;
	return nKey;
}

bool CCNetLink::SendControl(CCControlPacket::CONTROL nControl)
{
	CCControlPacket* pPacket = new CCControlPacket;

	if (nControl == CCControlPacket::CONTROL_SYN) {	// Connect.1
		pPacket->nControl = nControl;
		SetLinkState(LINKSTATE_SYN_SENT);
	}
	else if (nControl == CCControlPacket::CONTROL_FIN) {	// Disconnect.1
		pPacket->nControl = nControl;
		SetLinkState(LINKSTATE_FIN_SENT);
	}

	return m_pSafeUDP->Send(this, pPacket, sizeof(CCControlPacket));
}

bool CCNetLink::OnRecvControl(CCControlPacket* pPacket)
{
	CCControlPacket::CONTROL nControl = pPacket->nControl;
	CCControlPacket* pReply = new CCControlPacket;
	bool bCheckState = false;

	if (nControl == CCControlPacket::CONTROL_SYN) {
		SetLinkState(LINKSTATE_SYN_RCVD);
		pReply->nControl = CCControlPacket::CONTROL_SYN_RCVD;
		bCheckState = true;
	}
	else if (nControl == CCControlPacket::CONTROL_FIN) {
		SetLinkState(LINKSTATE_FIN_RCVD);
		pReply->nControl = CCControlPacket::CONTROL_FIN_RCVD;
		bCheckState = true;
	} else {
		switch(m_nLinkState) {
		case LINKSTATE_SYN_SENT:
			{
				if (nControl == CCControlPacket::CONTROL_SYN_RCVD) {
					SetLinkState(LINKSTATE_ESTABLISHED);
					pReply->nControl = CCControlPacket::CONTROL_ACK;
					bCheckState = true;
				}
			}
			break;
		case LINKSTATE_SYN_RCVD:
			{
				if (nControl == CCControlPacket::CONTROL_ACK) {
					SetLinkState(LINKSTATE_ESTABLISHED);
					bCheckState = false;
				}
			}
			break;
		case LINKSTATE_FIN_SENT:
			{
				if (nControl == CCControlPacket::CONTROL_FIN_RCVD) {
					SetLinkState(LINKSTATE_CLOSED);
					pReply->nControl = CCControlPacket::CONTROL_ACK;
					bCheckState = true;
				}
			}
			break;
		case LINKSTATE_FIN_RCVD:
			{
				if (nControl == CCControlPacket::CONTROL_ACK) {
					SetLinkState(LINKSTATE_CLOSED);
					bCheckState = false;
				}
			}
			break;
		};
	}
	if (bCheckState == false) {
		delete pReply;
		return false;
	}

	return m_pSafeUDP->Send(this, pReply, sizeof(CCControlPacket));
}

bool CCNetLink::SetACKWait(CCSafePacket* pPacket, DWORD dwPacketSize)
{
	if (m_ACKWaitQueue.size() > SAFEUDP_MAX_ACKWAITQUEUE_LENGTH)
		return false;

	pPacket->nSafeIndex = GetNextWriteIndex();
	CCACKWaitItem* pACKWaitItem = new CCACKWaitItem;
	pACKWaitItem->pPacket = pPacket;
	pACKWaitItem->dwPacketSize = dwPacketSize;
	pACKWaitItem->nSendCount = 1;		// SendQueue 에 넣었지만 보냈다고 가정
	CCTime::GetTime(&pACKWaitItem->tvFirstSent);
	CCTime::GetTime(&pACKWaitItem->tvLastSent);
	m_ACKWaitQueue.push_back(pACKWaitItem);

	return true;
}

bool CCNetLink::ClearACKWait(BYTE nSafeIndex)
{
	for (ACKWaitListItor itor = m_ACKWaitQueue.begin(); itor != m_ACKWaitQueue.end(); ) {
		CCACKWaitItem* pACKWaitItem = *itor;
		if (pACKWaitItem->pPacket->nSafeIndex == nSafeIndex) {
			delete pACKWaitItem;	// pACKWaitItem->pPacket will Delete too
			itor = m_ACKWaitQueue.erase(itor);
			return true;
		} else {
			++itor;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
// CCSocketThread class /////////////////////////////////////////////////////////////////////
void CCSocketThread::Create()
{
	InitializeCriticalSection(&m_csACKLock);
	InitializeCriticalSection(&m_csSendLock);

	InitializeCriticalSection(&m_csCrashDump);
	CCThread::Create(); 
}

void CCSocketThread::Destroy()
{ 
	m_KillEvent.SetEvent(); 
	CCThread::Destroy();		// Wait for Thread Death

	DeleteCriticalSection(&m_csCrashDump);

	DeleteCriticalSection(&m_csSendLock);
	DeleteCriticalSection(&m_csACKLock);
}

void CCSocketThread::Run()
{
/*	__try	*/
	{
			//throw(pThread);
		while(true) {	// Waiting for SafeUDP Settting...
			DWORD dwVal = WaitForSingleObject(m_KillEvent.GetEvent(), 100);
			if (dwVal == WAIT_OBJECT_0) {
				return;
			} else if (dwVal == WAIT_TIMEOUT) {
				if (m_pSafeUDP)
					break;
			}
		}

		WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
		WORD wEventIndex = 0;

		bool bSendable = false;
		WSANETWORKEVENTS NetEvent;
		WSAEVENT hFDEvent = WSACreateEvent();
		EventArray[wEventIndex++] = hFDEvent;
		EventArray[wEventIndex++] = m_ACKEvent.GetEvent();
		EventArray[wEventIndex++] = m_SendEvent.GetEvent();
		EventArray[wEventIndex++] = m_KillEvent.GetEvent();

		WSAEventSelect(m_pSafeUDP->GetLocalSocket(), hFDEvent, FD_READ|FD_WRITE);

		while(TRUE) {
			DWORD dwReturn = WSAWaitForMultipleEvents(wEventIndex, EventArray, FALSE, SAFEUDP_SAFE_MANAGE_TIME, FALSE);
			if (dwReturn == WSA_WAIT_TIMEOUT) {					// Time
				m_pSafeUDP->LockNetLink();
				SafeSendManage();
				m_pSafeUDP->UnlockNetLink();
			} else if (dwReturn == WSA_WAIT_EVENT_0) {			// Socket Event
				WSAEnumNetworkEvents(m_pSafeUDP->GetLocalSocket(), hFDEvent, &NetEvent);
				if ((NetEvent.lNetworkEvents & FD_READ) == FD_READ) {
	//				OutputDebugString("SUDP> FD_READ \n");
					m_pSafeUDP->LockNetLink();
					Recv();
					m_pSafeUDP->UnlockNetLink();
				} 
				if ((NetEvent.lNetworkEvents & FD_WRITE) == FD_WRITE) {
					bSendable = true;
	//				OutputDebugString("SUDP> FD_WRITE \n");
				}
			} else if (dwReturn == WSA_WAIT_EVENT_0 + 1) {		// ACK Send Event
	//			OutputDebugString("SUDP> ACK_EVENT \n");
				FlushACK();
			} else if (dwReturn == WSA_WAIT_EVENT_0 + 2) {		// Packet Send Event
	//			OutputDebugString("SUDP> SEND_EVENT \n");
				if (bSendable == true)
					FlushSend();
			} else if (dwReturn == WSA_WAIT_EVENT_0 + 3) {		// Kill the Thread
				break;	// Stop Thread
			}
		}

		WSACloseEvent(hFDEvent);

		// Clear Queues
		LockSend();
		{
			for (SendListItor itor = m_SendList.begin(); itor != m_SendList.end(); ) {
				delete (*itor);
				itor = m_SendList.erase(itor);
			}
		}
		{
			for (SendListItor itor = m_TempSendList.begin(); itor != m_TempSendList.end(); ) {
				delete (*itor);
				itor = m_TempSendList.erase(itor);
			}
		}
		UnlockSend();

		LockACK();
		{
			for (ACKSendListItor itor = m_ACKSendList.begin(); itor != m_ACKSendList.end(); ) {
				delete (*itor);
				itor = m_ACKSendList.erase(itor);
			}
		}
		{
			for (ACKSendListItor itor = m_TempACKSendList.begin(); itor != m_TempACKSendList.end(); ) {
				delete (*itor);
				itor = m_TempACKSendList.erase(itor);
			}
		}
		UnlockACK();
	}/*
	__except(this->CrashDump(GetExceptionInformation())) 
	{
#ifndef _PUBLISH
		char szFileName[_MAX_DIR];
		GetModuleFileName(NULL, szFileName, _MAX_DIR);
		HANDLE hProcess = CCProcessController::OpenProcessHandleByFilePath(szFileName);
		TerminateProcess(hProcess, 0);
#endif
	}*/

}
DWORD CCSocketThread::CrashDump(PEXCEPTION_POINTERS ExceptionInfo)
{
	cclog("CrashDump Entered 1\n");
	EnterCriticalSection(&m_csCrashDump);
	cclog("CrashDump Entered 2\n");

	if (PathIsDirectory("Log") == FALSE)
		CreateDirectory("Log", NULL);

	time_t		tClock;
	struct tm*	ptmTime;

	time(&tClock);
	ptmTime = localtime(&tClock);

	char szFileName[_MAX_DIR];

	int nFooter = 1;
	while(TRUE) {
		sprintf(szFileName, "Log/CCSocketThread_%02d-%02d-%02d-%d.dmp", 
			ptmTime->tm_year+1900, ptmTime->tm_mon+1, ptmTime->tm_mday, nFooter);

		if (PathFileExists(szFileName) == FALSE)
			break;

		nFooter++;
		if (nFooter > 100) 
		{
			LeaveCriticalSection(&m_csCrashDump);
			return false;
		}
	}

	DWORD ret = CrashExceptionDump(ExceptionInfo, szFileName, true);

	cclog("CrashDump Leaving\n");
	LeaveCriticalSection(&m_csCrashDump);
	cclog("CrashDump Leaved\n");

	return ret;
}

void CCSocketThread::Debug()
{

}

bool CCSocketThread::PushACK(CCNetLink* pNetLink, CCSafePacket* pPacket)
{
	if (m_ACKSendList.size() > SAFEUDP_MAX_ACKQUEUE_LENGTH)
		return false;

	CCACKQueueItem* pACKItem = new CCACKQueueItem;
	pACKItem->dwIP = pNetLink->GetIP();
	pACKItem->wRawPort = pNetLink->GetRawPort();
	pACKItem->nSafeIndex = pPacket->nSafeIndex;

	LockACK();
	m_TempACKSendList.push_back(pACKItem);
	UnlockACK();

	m_ACKEvent.SetEvent();

	return true;
}

bool CCSocketThread::PushSend(CCNetLink* pNetLink, CCBasePacket* pPacket, DWORD dwPacketSize, bool bRetransmit)
{
	if ( !pNetLink || (m_SendList.size() > SAFEUDP_MAX_SENDQUEUE_LENGTH) )
		return false;

	CCSendQueueItem* pSendItem = new CCSendQueueItem;
	pSendItem->dwIP = pNetLink->GetIP();
	pSendItem->wRawPort = pNetLink->GetRawPort();
	pSendItem->pPacket = pPacket;
	pSendItem->dwPacketSize = dwPacketSize;

	if ((pPacket->GetFlag(SAFEUDP_FLAG_SAFE_PACKET) != FALSE) && (bRetransmit == false)) {
		pNetLink->SetACKWait((CCSafePacket*)pPacket, dwPacketSize);
	}

	LockSend();
	m_TempSendList.push_back(pSendItem);
	UnlockSend();

	m_SendEvent.SetEvent();

	return true;
}

bool CCSocketThread::PushSend(char* pszIP, int nPort, char* pPacket, DWORD dwPacketSize)
{
	if (m_SendList.size() > SAFEUDP_MAX_SENDQUEUE_LENGTH )
		return false;

	sockaddr_in Addr;
	if (CCNetLink::MakeSockAddr(pszIP, nPort, &Addr) == false)
		return false;

	CCSendQueueItem* pSendItem = new CCSendQueueItem;
	pSendItem->dwIP = Addr.sin_addr.S_un.S_addr;
	pSendItem->wRawPort = Addr.sin_port;
	pSendItem->pPacket = (CCBasePacket*)pPacket;
	pSendItem->dwPacketSize = dwPacketSize;

	LockSend();
	m_TempSendList.push_back(pSendItem);
	UnlockSend();

	m_SendEvent.SetEvent();

	return true;
}


bool CCSocketThread::PushSend( DWORD dwIP, int nPort, char* pPacket, DWORD dwPacketSize )
{
	if( (SAFEUDP_MAX_SENDQUEUE_LENGTH < m_SendList.size()) ||		
	 	(INADDR_NONE == dwIP) )
	 	return false;

	sockaddr_in Addr;
	memset((char*)&Addr, 0, sizeof(sockaddr_in));

	//	Set Dest IP and Port 
	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(nPort);
	memcpy(&(Addr.sin_addr), &dwIP, 4);

	CCSendQueueItem* pSendItem = new CCSendQueueItem;
	if( 0 != pSendItem )
	{
		pSendItem->dwIP = dwIP;
		pSendItem->wRawPort = Addr.sin_port;
		pSendItem->pPacket = (CCBasePacket*)pPacket;
		pSendItem->dwPacketSize = dwPacketSize;

		LockSend();
		m_TempSendList.push_back( pSendItem );
		UnlockSend();

		m_SendEvent.SetEvent();

		return true;
	}

	return false;
}


bool CCSocketThread::FlushACK()
{
	LockACK();
	while(m_TempACKSendList.size() > 0) {
		ACKSendListItor itor = m_TempACKSendList.begin();
		m_ACKSendList.push_back(*itor);
		m_TempACKSendList.erase(itor);
	}
	UnlockACK();

	while(m_ACKSendList.size() > 0) {
		ACKSendListItor itor = m_ACKSendList.begin();
		CCACKQueueItem* pACKItem = *itor;

		CCACKPacket ACKPacket;
		ACKPacket.nSafeIndex = pACKItem->nSafeIndex;

		sockaddr_in DestAddr;
		DestAddr.sin_family = AF_INET;
		DestAddr.sin_addr.S_un.S_addr = pACKItem->dwIP;
		DestAddr.sin_port = pACKItem->wRawPort;

		int nResult = sendto(m_pSafeUDP->GetLocalSocket(), (char*)&ACKPacket, sizeof(CCACKPacket), 
							 0, (sockaddr*)&DestAddr, sizeof(sockaddr_in));
		if (nResult == SOCKET_ERROR) {
			return false;
		} else {
			m_nTotalSend += nResult;
			m_SendTrafficLog.Record(m_nTotalSend);
		}

		delete pACKItem; //delete *itor;
		m_ACKSendList.erase(itor);
	}

	return true;
}

bool CCSocketThread::FlushSend()
{
	LockSend();
	while(m_TempSendList.size() > 0) {
		SendListItor itor = m_TempSendList.begin();
		m_SendList.push_back(*itor);
		m_TempSendList.erase(itor);
	}
	UnlockSend();

	CCSendQueueItem* pSendItem;
	sockaddr_in		DestAddr;
	int				nResult;
	while(m_SendList.size() > 0) {
		SendListItor itor = m_SendList.begin();
		pSendItem = *itor;

		DestAddr.sin_family = AF_INET;
		DestAddr.sin_addr.S_un.S_addr = pSendItem->dwIP;
		DestAddr.sin_port = pSendItem->wRawPort;

		nResult = sendto(m_pSafeUDP->GetLocalSocket(), (char*)pSendItem->pPacket, pSendItem->dwPacketSize, 
						 0, (sockaddr*)&DestAddr, sizeof(sockaddr_in));
		if (nResult == SOCKET_ERROR) {
			int nErrCode = WSAGetLastError();
			char szBuf[64]; sprintf(szBuf, "<SAFEUDP_ERROR>FlushSend() - sendto() ErrCode=%d \n", nErrCode);
			MTRACE(szBuf);
//			return false;
		} else {
			m_nTotalSend += nResult;
			m_SendTrafficLog.Record(m_nTotalSend);
		}
		
		#ifdef _OLD_SAFEUDP
			if (pSendItem->pPacket->GetFlag(SAFEUDP_FLAG_SAFE_PACKET) != FALSE) {
				// Don't Delete SafePacket (pSendItem->pPacket)
				delete pSendItem;
				m_SendList.erase(itor);
			} else {	// Means Normal Packet
				delete pSendItem->pPacket;
				delete pSendItem; //delete (*itor);
				m_SendList.erase(itor);
			}
		#else
			// 건즈에서는 SAFE_UDP_FLAG_SAFE_PACKET를 사용하지 않는다.
			delete pSendItem->pPacket;
			delete pSendItem;
			m_SendList.erase(itor);
		#endif

	}
	return true;
}

bool CCSocketThread::SafeSendManage()
{
	int nCnt = m_pSafeUDP->m_NetLinkMap.size();
	for (NetLinkItor itorLink = m_pSafeUDP->m_NetLinkMap.begin(); itorLink != m_pSafeUDP->m_NetLinkMap.end(); ) {
		CCNetLink* pNetLink = (*itorLink).second;

		timeval tvNow;
		CCTime::GetTime(&tvNow);

		// Closed Idle time check
		timeval tvIdleDiff;
		tvIdleDiff = CCTime::TimeSub(tvNow, pNetLink->m_tvLastPacketRecvTime);
		if ( (pNetLink->GetLinkState() != CCNetLink::LINKSTATE_ESTABLISHED) &&
			 ((tvIdleDiff.tv_sec*1000 + tvIdleDiff.tv_usec) > SAFEUDP_MAX_SAFE_RETRANS_TIME) ) {
			MTRACE("SUDP> Idle Control Timeout \n");
			pNetLink->SetLinkState(CCNetLink::LINKSTATE_CLOSED);

			delete (*itorLink).second;
			m_pSafeUDP->m_NetLinkMap.erase(itorLink++);
			continue;
		} else {
			++itorLink;
		}

		for (CCNetLink::ACKWaitListItor itorACK = pNetLink->m_ACKWaitQueue.begin(); itorACK != pNetLink->m_ACKWaitQueue.end(); ++itorACK) {
			CCACKWaitItem* pACKWaitItem = *itorACK;

			timeval tvDiff;
			tvDiff = CCTime::TimeSub(tvNow, pACKWaitItem->tvFirstSent);
			if ((tvDiff.tv_sec*1000 + tvDiff.tv_usec) > SAFEUDP_MAX_SAFE_RETRANS_TIME) {
				// Disconnect....
				MTRACE("SUDP> Retransmit Timeout \n");
				pNetLink->SetLinkState(CCNetLink::LINKSTATE_CLOSED);
				break;
			}

			tvDiff = CCTime::TimeSub(tvNow, pACKWaitItem->tvLastSent);
			if ((tvDiff.tv_sec*1000 + tvDiff.tv_usec) > SAFEUDP_SAFE_RETRANS_TIME) {
				PushSend(pNetLink, pACKWaitItem->pPacket, pACKWaitItem->dwPacketSize, true);
				pACKWaitItem->tvLastSent = tvNow;
				pACKWaitItem->nSendCount++;
			}
		}
	}
	return true;
}

bool CCSocketThread::Recv()
{
	sockaddr_in		AddrFrom;
	int				nAddrFromLen = sizeof(sockaddr);

	char			RecvBuf[MAX_RECVBUF_LEN];
	int				nRecv = 0;

	while(TRUE) {
		nRecv = recvfrom(m_pSafeUDP->GetLocalSocket(), RecvBuf, MAX_RECVBUF_LEN, 0, 
						 (struct sockaddr*)&AddrFrom, &nAddrFromLen);
		if (nRecv != SOCKET_ERROR) {
			m_nTotalRecv += nRecv;
			m_RecvTrafficLog.Record(m_nTotalRecv);
		}

		if (nRecv <= 0) break;					// Stop Receive Loop

/*////////////////
char szBuf[256];
wsprintf(szBuf, "[%s:%d] \n", inet_ntoa(AddrFrom.sin_addr), ntohs(AddrFrom.sin_port));
OutputDebugString(szBuf);
////////////////*/

		if (m_fnCustomRecvCallback && OnCustomRecv(AddrFrom.sin_addr.S_un.S_addr, AddrFrom.sin_port, RecvBuf, nRecv) == true) {
			continue;
		} else if (((CCBasePacket*)RecvBuf)->GetFlag(SAFEUDP_FLAG_CONTROL_PACKET) != FALSE) {
			OnControlRecv(AddrFrom.sin_addr.S_un.S_addr, AddrFrom.sin_port, (CCBasePacket*)RecvBuf, nRecv);
			continue;
		} else if (((CCBasePacket*)RecvBuf)->GetFlag(SAFEUDP_FLAG_LIGHT_PACKET) != FALSE) {
			OnLightRecv(AddrFrom.sin_addr.S_un.S_addr, AddrFrom.sin_port, (CCLightPacket*)RecvBuf, nRecv);
			continue;
		} else if (((CCBasePacket*)RecvBuf)->GetFlag(SAFEUDP_FLAG_ACK_PACKET) != FALSE) {
			OnACKRecv(AddrFrom.sin_addr.S_un.S_addr, AddrFrom.sin_port, (CCACKPacket*)RecvBuf);
			continue;
		} else {
			OnGenericRecv(AddrFrom.sin_addr.S_un.S_addr, AddrFrom.sin_port, (CCBasePacket*)RecvBuf, nRecv);
			continue;
		}
	}

	return true;
}

bool CCSocketThread::OnCustomRecv(DWORD dwIP, WORD wRawPort, char* pPacket, DWORD dwSize)
{
	if (m_fnCustomRecvCallback)
		return m_fnCustomRecvCallback(dwIP, wRawPort, pPacket, dwSize);
	return false;
}

bool CCSocketThread::OnControlRecv(DWORD dwIP, WORD wRawPort, CCBasePacket* pPacket, DWORD dwSize)
{
	CCControlPacket* pControlPacket = (CCControlPacket*)pPacket;
	CCNetLink* pNetLink = m_pSafeUDP->FindNetLink(dwIP, wRawPort);

	if (pNetLink == NULL) {
		if (pControlPacket->nControl == CCControlPacket::CONTROL_SYN) {
			IN_ADDR addr;
			addr.S_un.S_addr = dwIP;
			pNetLink = m_pSafeUDP->OpenNetLink(inet_ntoa(addr), ntohs(wRawPort));
			pNetLink->OnRecvControl(pControlPacket);
		} 
	} else {
		pNetLink->OnRecvControl(pControlPacket);
	}

	if ( pNetLink && (pPacket->GetFlag(SAFEUDP_FLAG_SAFE_PACKET) != FALSE) )
		PushACK(pNetLink, (CCSafePacket*)pPacket);

	return true;
}

bool CCSocketThread::OnLightRecv(DWORD dwIP, WORD wRawPort, CCLightPacket* pPacket, DWORD dwSize)
{
	if (m_fnLightRecvCallback)
		m_fnLightRecvCallback(dwIP, wRawPort, pPacket, dwSize);
	return true;
}

bool CCSocketThread::OnACKRecv(DWORD dwIP, WORD wRawPort, CCACKPacket* pPacket)
{
	CCNetLink* pNetLink = m_pSafeUDP->FindNetLink(dwIP, wRawPort);
	if (pNetLink == NULL)
		return false;

	pNetLink->ClearACKWait(pPacket->nSafeIndex);

	return false;
}

bool CCSocketThread::OnGenericRecv(DWORD dwIP, WORD wRawPort, CCBasePacket* pPacket, DWORD dwSize)
{
	CCNetLink* pNetLink = m_pSafeUDP->FindNetLink(dwIP, wRawPort);
	if (pNetLink == NULL)
		return false;

	if (pPacket->GetFlag(SAFEUDP_FLAG_SAFE_PACKET) != FALSE)
		PushACK(pNetLink, (CCSafePacket*)pPacket);

	// recv
	if (m_fnGenericRecvCallback)
		m_fnGenericRecvCallback(pNetLink, pPacket, dwSize);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// CCSafeUDP class //////////////////////////////////////////////////////////////////////////
bool CCSafeUDP::Create(bool bBindWinsockDLL, int nPort, bool bReusePort)
{
	m_bBindWinsockDLL = bBindWinsockDLL;

	WSADATA	wsaData;
	if ((bBindWinsockDLL == true) && (WSAStartup(MAKEWORD(2,2), &wsaData) != 0))
		return false;

	if (OpenSocket(nPort, bReusePort) == false) {
#ifdef _DEBUG
		OutputDebugString("<SAFEUDP_ERROR> OpenSocket() FAILED </SAFEUDP_ERROR>\n");
#endif
		return false;
	}

	InitializeCriticalSection(&m_csNetLink);

	m_SocketThread.SetSafeUDP(this);
	m_SocketThread.Create();
	return true;
}

void CCSafeUDP::Destroy()
{
	if (m_SocketThread.GetSafeUDP() == NULL)
		return;

	DisconnectAll();

	m_SocketThread.Destroy();
	CloseSocket();

	DeleteCriticalSection(&m_csNetLink);

	if (m_bBindWinsockDLL == true) {
		WSACleanup();
		m_bBindWinsockDLL = false;
	}
}

bool CCSafeUDP::OpenSocket(int nPort, bool bReuse)
{
	SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == INVALID_SOCKET)
		return false;

	if (bReuse) {
		int opt = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR)
			return false;

		int rcv_size = 64 * 1024;
		if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char*)&rcv_size, sizeof(rcv_size)) == SOCKET_ERROR)
			return false;
	}

	sockaddr_in LocalAddress;
	LocalAddress.sin_family			= AF_INET;
	LocalAddress.sin_addr.s_addr	= htonl(INADDR_ANY);
	LocalAddress.sin_port			= htons(nPort);

	if (bind(sockfd, (struct sockaddr*)&LocalAddress, sizeof(LocalAddress)) == SOCKET_ERROR) {
		closesocket(sockfd);
		return false;
	}

	m_Socket = sockfd;
	m_LocalAddress = LocalAddress;

	return true;
}

void CCSafeUDP::CloseSocket()
{
	shutdown(m_Socket, SD_SEND);
	closesocket(m_Socket);
	m_Socket = 0;
}

bool CCSafeUDP::Send(CCNetLink* pNetLink, CCBasePacket* pPacket, DWORD dwPacketSize)
{
	return m_SocketThread.PushSend(pNetLink, pPacket, dwPacketSize, false);
}

bool CCSafeUDP::Send(char* pszIP, int nPort, char* pPacket, DWORD dwSize)
{
	return m_SocketThread.PushSend(pszIP, nPort, pPacket, dwSize);
}

bool CCSafeUDP::Send(DWORD dwIP, int nPort, char* pPacket, DWORD dwSize )
{
	return m_SocketThread.PushSend( dwIP, nPort, pPacket, dwSize );
}

CCNetLink* CCSafeUDP::FindNetLink(DWORD dwIP, WORD wRawPort)
{
	__int64 nKey = wRawPort;
	nKey = nKey << 32;
	nKey += dwIP;

	NetLinkItor pos = m_NetLinkMap.find(nKey);
	if (pos != m_NetLinkMap.end())
		return (*pos).second;

	return NULL;
}

CCNetLink* CCSafeUDP::FindNetLink(__int64 nMapKey)
{
	NetLinkItor pos = m_NetLinkMap.find(nMapKey);
	if (pos != m_NetLinkMap.end())
		return (*pos).second;
	return NULL;
}

CCNetLink* CCSafeUDP::OpenNetLink(char* szIP, int nPort)
{
	CCNetLink* pNetLink = new CCNetLink;
	pNetLink->SetSafeUDP(this);
	pNetLink->SetAddress(szIP, nPort);
	
	__int64 nKey = pNetLink->GetMapKey();
	
	NetLinkItor pos = m_NetLinkMap.find(nKey);
	if (pos != m_NetLinkMap.end()) {
		Reconnect((*pos).second);
		delete pNetLink;
		pNetLink = (*pos).second;
	} else {
		m_NetLinkMap.insert(NetLinkType(nKey, pNetLink));
	}

	return pNetLink;
}

bool CCSafeUDP::CloseNetLink(CCNetLink* pNetLink)
{
	__int64 nKey = pNetLink->GetMapKey();

	NetLinkItor pos = m_NetLinkMap.find(nKey);
	if (pos == m_NetLinkMap.end())
		return false;

	delete (*pos).second;
	m_NetLinkMap.erase(pos);

	return true;
}

CCNetLink* CCSafeUDP::Connect(char* szIP, int nPort)
{
	CCNetLink* pNetLink = OpenNetLink(szIP, nPort);
	pNetLink->SendControl(CCControlPacket::CONTROL_SYN);

	return pNetLink;
}

void CCSafeUDP::Reconnect(CCNetLink* pNetLink)
{
	pNetLink->SendControl(CCControlPacket::CONTROL_SYN);
}

bool CCSafeUDP::Disconnect(CCNetLink* pNetLink)
{
	pNetLink->SendControl(CCControlPacket::CONTROL_FIN);

	return true;
}

int CCSafeUDP::DisconnectAll()
{
	LockNetLink();
	int nCount = 0;
	for (NetLinkItor itor = m_NetLinkMap.begin(); itor != m_NetLinkMap.end(); ) {
		delete (*itor).second;
		m_NetLinkMap.erase(itor++);
		++nCount;
	}
	UnlockNetLink();
	return nCount;
}

