#pragma once

#include "CCCommandCommunicator.h"
#include "CCClient.h"

/// 게임 클라이언트
class CCGameClient : public CCClient
{
private:
protected:
	// socket event
	virtual bool OnSockConnect(SOCKET sock);
	virtual bool OnSockDisconnect(SOCKET sock);
	virtual bool OnSockRecv(SOCKET sock, char* pPacket, DWORD dwSize);
	virtual void OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);

	virtual int OnConnected(CCUID* pTargetUID, CCUID* pAllocUID, CCCommObject* pCommObj);
	virtual void OnRegisterCommand(CCCommandManager* pCommandManager);
	virtual bool OnCommand(CCCommand* pCommand);

	void OutputLocalInfo(void);
	
public:
	CCGameClient();
	virtual ~CCGameClient();
	
};