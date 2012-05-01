#pragma once
#include "CCCommandCommunicator.h"
#include "CCPacketCrypter.h"

class CCClientCommandProcessor;
class CCCommand;

#define RINGBUFSIZE 16384

// 나중에 패킷 버퍼 대체로 사용할 목적으로 우선 만들어둔다.
class CCRingBuffer {
private:
	int			m_iBufSize;
	char*		m_Buf;
	char*		m_cpBegin, m_cpEnd;
protected:
public:
	CCRingBuffer();
	CCRingBuffer(int iBufSize);
	virtual ~CCRingBuffer();
	void Reserve(int iBufSize); 
	bool Enqueue(char* cp, int iDataSize);
	bool Dequeue(char* cpOut, int iDataSize);
	int GetSize()		{ return m_iBufSize; }
};

/// Message Type
enum CCZMOMType{
	CCZMDM_GENERAL,			///< 일반 메세지
	CCZMOM_USERCOMMAND,		///< 사용자 입력 커맨드
	CCZMOM_ERROR,			///< 에러
	CCZMOM_LOCALREPLY,		///< 로컬 응답 메세지
	CCZMOM_SERVERREPLY,		///< 서버 응답 메세지
};


/// 클라이언트
class CCClient : public CCCommandCommunicator{
private:
	static CCClient*	m_pInstance;				///< 전역 인스턴스
protected:
	CCUID				m_Server;				///< 연결된 커뮤니케이터

	CCClientSocket		m_ClientSocket;			///< 클라이언트 소켓용 클래스
	CRITICAL_SECTION	m_csRecvLock;			///< CommandQueue critical section

	CCCommandBuilder*	m_pCommandBuilder;
	CCPacketCrypter		m_ServerPacketCrypter;	///< MatchServer와의 암호화 클래스

protected:
	void LockRecv() { EnterCriticalSection(&m_csRecvLock); }
	void UnlockRecv() { LeaveCriticalSection(&m_csRecvLock); }

	virtual void OnRegisterCommand(CCCommandManager* pCommandManager);
	virtual bool OnCommand(CCCommand* pCommand);

	/// 로컬 정보를 출력한다.
	virtual void OutputLocalInfo(void) = 0;
	/// 기본 출력
	virtual void OutputMessage(const char* szMessage, CCZMOMType nType=CCZMDM_GENERAL) = 0;

	CCCommandBuilder* GetCommandBuilder()	{ return m_pCommandBuilder; }
	virtual void SendCommand(CCCommand* pCommand);
	virtual CCCommand* GetCommandSafe();

	virtual int OnConnected(CCUID* pTargetUID, CCUID* pAllocUID, unsigned int nTimeStamp, CCCommObject* pCommObj);
	virtual int OnConnected(SOCKET sock, CCUID* pTargetUID, CCUID* pAllocUID, unsigned int nTimeStamp);
	int MakeCmdPacket(char* pOutPacket, int iMaxPacketSize, CCPacketCrypter* pPacketCrypter, CCCommand* pCommand);
	

	// Socket Event
	virtual bool OnSockConnect(SOCKET sock);
	virtual bool OnSockDisconnect(SOCKET sock);
	virtual bool OnSockRecv(SOCKET sock, char* pPacket, DWORD dwSize);
	virtual void OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);

public:
	CCClient();
	virtual ~CCClient();

	/// 전역 인스턴스 얻기
	static CCClient* GetInstance(void);

	CCUID GetServerUID(void){ return m_Server; }
	virtual CCUID GetSenderUIDBySocket(SOCKET socket);
	CCClientSocket* GetClientSocket()						{ return &m_ClientSocket; }
	void GetTraffic(int* nSendTraffic, int* nRecvTraffic)	{ return m_ClientSocket.GetTraffic(nSendTraffic, nRecvTraffic); }

	virtual bool Post(CCCommand* pCommand);
	virtual bool Post(char* szErrMsg, int nErrMsgCount, const char* szCommand);

	virtual int Connect(CCCommObject* pCommObj);
	virtual int Connect(SOCKET* pSocket, char* szIP, int nPort);
	virtual void Disconnect( const CCUID&  uid );
	virtual void Log(const char* szLog){}

	/// 파라미터 출력
	void OutputMessage(CCZMOMType nType, const char *pFormat,...);

	static bool SocketRecvEvent(void* pCallbackContext, SOCKET sock, char* pPacket, DWORD dwSize);
	static bool SocketConnectEvent(void* pCallbackContext, SOCKET sock);
	static bool SocketDisconnectEvent(void* pCallbackContext, SOCKET sock);
	static void SocketErrorEvent(void* pCallbackContext, SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);

	bool IsConnected() { return m_ClientSocket.IsActive(); }
};

void SplitIAddress(char* szIP, int* pPort, const char* szAddress);

