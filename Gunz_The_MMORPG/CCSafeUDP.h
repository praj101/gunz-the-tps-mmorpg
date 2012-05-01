#pragma once
/////////////////////////////////////////////////////////////
//	SafeUDP.h	- SafeUDP 1.9.2
//								 Programmed by Kim Young-Ho 
//								    LastUpdate : 2000/07/25
/////////////////////////////////////////////////////////////

#pragma warning(disable:4786)
#include <map>
#include <list>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <winsock2.h>
#include "CCSync.h"
#include "CCThread.h"
#include "CCBasePacket.h"
#include "CCTrafficLog.h"


class CCSafeUDP;


// INNER CLASS //////////////////////////////////////////////////////////////////////////
struct CCSendQueueItem {
	DWORD			dwIP;
	WORD			wRawPort;
	CCBasePacket*	pPacket;
	DWORD			dwPacketSize;
};

struct CCACKQueueItem {
	DWORD			dwIP;
	WORD			wRawPort;
	BYTE			nSafeIndex;
};

struct CCACKWaitItem {
	CCSafePacket*	pPacket;
	DWORD			dwPacketSize;
	timeval			tvFirstSent;
	timeval			tvLastSent;
	BYTE			nSendCount;
	CCACKWaitItem() {}
	~CCACKWaitItem() { if (pPacket) delete pPacket; }
};

// OUTER CLASS //////////////////////////////////////////////////////////////////////////
class CCNetLink {
public:
	enum LINKSTATE {
		LINKSTATE_CLOSED,
		LINKSTATE_ESTABLISHED,
		LINKSTATE_SYN_SENT,
		LINKSTATE_SYN_RCVD,
		LINKSTATE_FIN_SENT,
		LINKSTATE_FIN_RCVD
	};

	typedef list<CCACKWaitItem*>		ACKWaitList;
	typedef ACKWaitList::iterator	ACKWaitListItor;

private:
	CCSafeUDP*		m_pSafeUDP;
	bool			m_bConnected;
	LINKSTATE		m_nLinkState;
	sockaddr_in		m_Address;		// IP and Port
	BYTE			m_nNextReadIndex;
	BYTE			m_nNextWriteIndex;

	DWORD			m_dwAuthKey;
	void*			m_pUserData;

public:
	timeval			m_tvConnectedTime;
	timeval			m_tvLastPacketRecvTime;

public:
	ACKWaitList		m_ACKWaitQueue;	// Safe Sent queue, Wait for ACK

private:
	void Setconnected(bool bConnected)	{ m_bConnected = bConnected; }
	void CreateAuthKey() { 	
		srand((unsigned)time(NULL));
		m_dwAuthKey = rand()*rand();
	}
	BYTE GetNextReadIndex()				{ return m_nNextReadIndex++; }
	BYTE GetNextWriteIndex()			{ return m_nNextWriteIndex++; }

public:
	CCNetLink();
	~CCNetLink();

	bool SendControl(CCControlPacket::CONTROL nControl);
	bool OnRecvControl(CCControlPacket* pPacket);

	bool SetACKWait(CCSafePacket* pPacket, DWORD dwPacketSize);
	bool ClearACKWait(BYTE nSafeIndex);

	void SetSafeUDP(CCSafeUDP* pSafeUDP)	{ m_pSafeUDP = pSafeUDP; }
	CCSafeUDP* GetSafeUDP()				{ return m_pSafeUDP; }
	bool IsConnected()					{ return m_bConnected; }
	timeval GetConnectedTime()			{ return m_tvConnectedTime; }
	void SetLinkState(CCNetLink::LINKSTATE nState);
	CCNetLink::LINKSTATE GetLinkState()	{ return m_nLinkState; }
	static bool MakeSockAddr(char* pszIP, int nPort, sockaddr_in* pSockAddr);
	bool SetAddress(char* pszIP, int nPort);
	char* GetIPString()					{ return inet_ntoa(m_Address.sin_addr); }
	DWORD GetIP()						{ return m_Address.sin_addr.S_un.S_addr; }
	WORD GetRawPort()					{ return m_Address.sin_port; }
	int GetPort()						{ return ntohs(m_Address.sin_port); }
	sockaddr_in* GetSockAddr()			{ return &m_Address; }
	__int64 GetMapKey();
	static __int64 GetMapKey(sockaddr_in* pSockAddr);
	timeval GetLastPacketRecvTime()		{ return m_tvLastPacketRecvTime; }

	DWORD GetAuthKey()					{ return m_dwAuthKey; }
	void SetUserData(void* pUserData)	{ m_pUserData = pUserData; }
	void* GetUserData()					{ return m_pUserData; }
};

typedef map<__int64, CCNetLink*>	NetLinkMap;
typedef NetLinkMap::value_type	NetLinkType;
typedef NetLinkMap::iterator	NetLinkItor;


// INNER CLASS //////////////////////////////////////////////////////////////////////////
typedef void(MNETLINKSTATECALLBACK)(CCNetLink* pNetLink, CCNetLink::LINKSTATE nState);
typedef bool(MCUSTOMRECVCALLBACK)(DWORD dwIP, WORD wRawPort, char* pPacket, DWORD dwSize);	// Real UDP Packet
typedef void(MLIGHTRECVCALLBACK)(DWORD dwIP, WORD wRawPort, CCLightPacket* pPacket, DWORD dwSize);
typedef void(MGENERICRECVCALLBACK)(CCNetLink* pNetLink, CCBasePacket* pPacket, DWORD dwSize);

class CCSafeUDP;
class CCSocketThread : public CCThread {
public:
	typedef list<CCACKQueueItem*>	ACKSendList;
	typedef ACKSendList::iterator	ACKSendListItor;
	typedef list<CCSendQueueItem*>	SendList;
	typedef SendList::iterator		SendListItor;

private:
	CCSafeUDP*				m_pSafeUDP;
	CCSignalEvent			m_ACKEvent;
	CCSignalEvent			m_SendEvent;
	CCSignalEvent			m_KillEvent;

	ACKSendList				m_ACKSendList;		// Sending priority High
	ACKSendList				m_TempACKSendList;	// Temporary ACK List for Sync
	CRITICAL_SECTION		m_csACKLock;

	SendList				m_SendList;			// Sending priority Low	(Safe|Normal) Packet
	SendList				m_TempSendList;		// Temporary Send List for Sync
	CRITICAL_SECTION		m_csSendLock;

	DWORD					m_nTotalSend;
	DWORD					m_nTotalRecv;
	CCTrafficLog				m_SendTrafficLog;
	CCTrafficLog				m_RecvTrafficLog;

	CRITICAL_SECTION		m_csCrashDump;
public:
	MCUSTOMRECVCALLBACK*	m_fnCustomRecvCallback;
	MLIGHTRECVCALLBACK*		m_fnLightRecvCallback;
	MGENERICRECVCALLBACK*	m_fnGenericRecvCallback;

private:
	void LockACK() { EnterCriticalSection(&m_csACKLock); }
	void UnlockACK() { LeaveCriticalSection(&m_csACKLock); }
	void LockSend() { EnterCriticalSection(&m_csSendLock); }
	void UnlockSend() { LeaveCriticalSection(&m_csSendLock); }

	bool PushACK(CCNetLink* pNetLink, CCSafePacket* pPacket);
	bool FlushACK();
	bool FlushSend();

	bool SafeSendManage();

	bool Recv();
	bool OnCustomRecv(DWORD dwIP, WORD wRawPort, char* pPacket, DWORD dwSize);
	bool OnControlRecv(DWORD dwIP, WORD wRawPort, CCBasePacket* pPacket, DWORD dwSize);
	bool OnLightRecv(DWORD dwIP, WORD wRawPort, CCLightPacket* pPacket, DWORD dwSize);
	bool OnACKRecv(DWORD dwIP, WORD wRawPort, CCACKPacket* pPacket);
	bool OnGenericRecv(DWORD dwIP, WORD wRawPort, CCBasePacket* pPacket, DWORD dwSize);

	DWORD CrashDump(PEXCEPTION_POINTERS ExceptionInfo);
public:
	CCSocketThread()						{ 
		m_pSafeUDP=NULL; 
		m_fnCustomRecvCallback = NULL; m_fnLightRecvCallback=NULL; m_fnGenericRecvCallback=NULL; 
		m_nTotalSend = 0;	m_nTotalRecv = 0;
	}
	~CCSocketThread()					{ m_pSafeUDP=NULL; }
	void Create();						
	void Destroy();
	CCSafeUDP* GetSafeUDP()				{ return m_pSafeUDP; }
	void SetSafeUDP(CCSafeUDP* pSafeUDP)	{ m_pSafeUDP = pSafeUDP; }

	bool PushSend(CCNetLink* pNetLink, CCBasePacket* pPacket, DWORD dwpPacketSize, bool bRetransmit);	
	bool PushSend(char* pszIP, int nPort, char* pPacket, DWORD dwPacketSize);
	bool PushSend( DWORD dwIP, int nPort, char* pPacket, DWORD dwPacketSize );

	int GetSendTraffic()	{ return m_SendTrafficLog.GetTrafficSpeed(); }
	int GetRecvTraffic()	{ return m_RecvTrafficLog.GetTrafficSpeed(); }

	// Dbg code. don't use for release mode. - by SungE.
	int GetSendListSize() { return (int)m_SendList.size(); }
	int GetTempSendList() { return (int)m_TempSendList.size(); }
	// 

	virtual void Run();
	virtual void Debug();
};

// OUTER CLASS //////////////////////////////////////////////////////////////////////////
class CCSafeUDP {
	bool						m_bBindWinsockDLL;		// Socket DLL Load
	SOCKET						m_Socket;			// My Socket
	sockaddr_in					m_LocalAddress;		// My IP and Port

	CCSocketThread				m_SocketThread;

	CRITICAL_SECTION			m_csNetLink;

public:
	NetLinkMap					m_NetLinkMap;
	MNETLINKSTATECALLBACK*		m_fnNetLinkStateCallback;
	void LockNetLink()			{ EnterCriticalSection(&m_csNetLink); }
	void UnlockNetLink()		{ LeaveCriticalSection(&m_csNetLink); }

public:
	SOCKET GetLocalSocket()		{ return m_Socket; }
	char* GetLocalIPString()	{ return inet_ntoa(m_LocalAddress.sin_addr); }
	DWORD GetLocalIP()			{ return m_LocalAddress.sin_addr.S_un.S_addr; }
	WORD GetLocalPort()			{ return m_LocalAddress.sin_port; }

	CCNetLink* FindNetLink(DWORD dwIP, WORD wRawPort);
	CCNetLink* FindNetLink(__int64 nMapKey);

	void GetTraffic(int* nSendTraffic, int* nRecvTraffic) {
		*nSendTraffic = m_SocketThread.GetSendTraffic();
		*nRecvTraffic = m_SocketThread.GetRecvTraffic();
	}

private:
	bool OpenSocket(int nPort, bool bReuse=true);
	void CloseSocket();
	void OnConnect(CCNetLink* pNetLink);
	void OnDisconnect(CCNetLink* pNetLink);

public:
	bool Create(bool bBindWinsockDLL, int nPort, bool bReuse=true);
	void Destroy();

	void SetNetLinkStateCallback(MNETLINKSTATECALLBACK pCallback) { m_fnNetLinkStateCallback = pCallback; }
	void SetLightRecvCallback(MLIGHTRECVCALLBACK pCallback) { m_SocketThread.m_fnLightRecvCallback = pCallback; }
	void SetGenericRecvCallback(MGENERICRECVCALLBACK pCallback) { m_SocketThread.m_fnGenericRecvCallback = pCallback; }
	void SetCustomRecvCallback(MCUSTOMRECVCALLBACK pCallback) { m_SocketThread.m_fnCustomRecvCallback = pCallback; }

	CCNetLink* OpenNetLink(char* szIP, int nPort);
	bool CloseNetLink(CCNetLink* pNetLink);

	CCNetLink* Connect(char* szIP, int nPort);
	void Reconnect(CCNetLink* pNetLink);
	bool Disconnect(CCNetLink* pNetLink);
	int DisconnectAll();

	bool Send(CCNetLink* pNetLink, CCBasePacket* pPacket, DWORD dwSize);
	bool Send(char* pszIP, int nPort, char* pPacket, DWORD dwSize);
	bool Send(DWORD dwIP, int nPort, char* pPacket, DWORD dwSize );

	void Debug() { m_SocketThread.Debug(); }

	// Dbg code. don't use for release mode. - by SungE.
	CCSocketThread& GetSocketThread() { return m_SocketThread; }
	//
};


#pragma comment(lib, "ws2_32.lib")
