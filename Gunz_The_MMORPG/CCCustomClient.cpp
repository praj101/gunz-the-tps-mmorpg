#include "stdafx.h"
#include "CCCustomClient.h"


CCCustomClient::CCCustomClient()
{
	InitializeCriticalSection(&m_csRecvLock);

	m_ClientSocket.SetCallbackContext(this);
	m_ClientSocket.SetConnectCallback(SocketConnectEvent);
	m_ClientSocket.SetDisconnectCallback(SocketDisconnectEvent);
	m_ClientSocket.SetRecvCallback(SocketRecvEvent);
	m_ClientSocket.SetSocketErrorCallback(SocketErrorEvent);
}

CCCustomClient::~CCCustomClient()
{
	DeleteCriticalSection(&m_csRecvLock);
}

bool CCCustomClient::OnSockConnect(SOCKET sock)
{
	return true;
}

bool CCCustomClient::OnSockDisconnect(SOCKET sock)
{
	return true;
}

bool CCCustomClient::OnSockRecv(SOCKET sock, char* pPacket, DWORD dwSize)
{
	return false;
}

void CCCustomClient::OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode)
{

}

int CCCustomClient::Connect(char* szIP, int nPort)
{
	if (m_ClientSocket.Connect(NULL, szIP, nPort))
		return MOK;
	else
		return MERR_UNKNOWN;
}

void CCCustomClient::Send(char* pBuf, DWORD nSize)
{
	m_ClientSocket.Send(pBuf, nSize);
}

bool CCCustomClient::SocketRecvEvent(void* pCallbackContext, SOCKET sock, char* pPacket, DWORD dwSize)
{
	CCCustomClient* pClient = (CCCustomClient*)pCallbackContext;

	return pClient->OnSockRecv(sock, pPacket, dwSize);
}

bool CCCustomClient::SocketConnectEvent(void* pCallbackContext, SOCKET sock)
{
	CCCustomClient* pClient = (CCCustomClient*)pCallbackContext;

	return pClient->OnSockConnect(sock);
}

bool CCCustomClient::SocketDisconnectEvent(void* pCallbackContext, SOCKET sock)
{
	CCCustomClient* pClient = (CCCustomClient*)pCallbackContext;
	return pClient->OnSockDisconnect(sock);
}

void CCCustomClient::SocketErrorEvent(void* pCallbackContext, SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode)
{
	CCCustomClient* pClient = (CCCustomClient*)pCallbackContext;
	pClient->OnSockError(sock, ErrorEvent, ErrorCode);
}

