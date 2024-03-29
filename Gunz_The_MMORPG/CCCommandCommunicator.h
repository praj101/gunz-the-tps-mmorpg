#ifndef MCOMMANDCOMMUNICATOR_H
#define MCOMMANDCOMMUNICATOR_H

#include "CCCommandManager.h"
#include "CCUID.h"
#include "CCTCPSocket.h"
#include "CCPacketCrypter.h"

//#define _CMD_PROFILE

#ifdef _CMD_PROFILE
#include "CCCommandProfiler.h"
#endif


class CCCommandCommunicator;
class CCCommandBuilder;


/// 커뮤니케이터와의 접속을 위한 객체. CCCommandCommunicator::Connect()의 파라미터로 들어간다.
class CCCommObject {
protected:
	CCUID					m_uid;

	CCCommandBuilder*		m_pCommandBuilder;
	CCPacketCrypter			m_PacketCrypter;

	CCCommandCommunicator*	m_pDirectConnection;	// 포인터 커넥션 Direction Connection
	DWORD					m_dwUserContext;

	char					m_szIP[128];
	int						m_nPort;
	DWORD					m_dwIP;
	bool					m_bAllowed;

	bool					m_bPassiveSocket;
public:
	CCCommObject(CCCommandCommunicator* pCommunicator);
	virtual ~CCCommObject();

	CCUID GetUID()			{ return m_uid; }
	void SetUID(CCUID uid)	{ m_uid = uid; }

	CCCommandBuilder*	GetCommandBuilder()				{ return m_pCommandBuilder; }
	CCPacketCrypter*		GetCrypter()					{ return &m_PacketCrypter; }

	CCCommandCommunicator* GetDirectConnection()			{ return m_pDirectConnection; }
	void SetDirectConnection(CCCommandCommunicator* pDC)	{ m_pDirectConnection = pDC; }
	DWORD GetUserContext()								{ return m_dwUserContext; }
	void SetUserContext(DWORD dwContext)				{ m_dwUserContext = dwContext; }

	char* GetIPString()		{ return m_szIP; }
	DWORD GetIP()			{ return m_dwIP; }
	int GetPort()			{ return m_nPort; }
	void SetAddress(const char* pszIP, int nPort) {
		strcpy(m_szIP, pszIP);
		m_dwIP = inet_addr(m_szIP);
		m_nPort = nPort;
	}
	void SetAllowed(bool bAllowed) { m_bAllowed = bAllowed; }
	bool IsAllowed() { return m_bAllowed; }

	// 추후 삭제해도 되지만, 현재 IOCP의 취약점을 위하여 만든 부분
	// Passive란, 사용자에 의해 생성된 CommObject가 아닌, IOCP상에서 자동으로 생성되는 CommObject를 의미.
	// (RCP_IO_ACCEPT에서 생성됨, 수동적으로 생성되는 소켓) 
	//																2010. 04. 06 - 홍기주
	void SetPassiveSocket(bool bActive) { m_bPassiveSocket = bActive; }
	bool IsPassiveSocket() { return m_bPassiveSocket; }

};


class CCPacketInfo {
public:
	CCCommObject*		m_pCommObj;
	CCPacketHeader*		m_pPacket;

	CCPacketInfo(CCCommObject* pCommObj, CCPacketHeader* pPacket) { m_pCommObj = pCommObj, m_pPacket = pPacket; }
};
typedef list<CCPacketInfo*>			CCPacketInfoList;
typedef CCPacketInfoList::iterator	CCPacketInfoListItor;


/// 커맨드 기반의 통신을 목적으로한 커뮤니케이터
class CCCommandCommunicator{
protected:
	CCCommandManager	m_CommandManager;		///< 커맨드 매니저

	CCUID			m_This;					///< 자기 커뮤니케이터 UID
	CCUID			m_DefaultReceiver;		///< 커맨드를 파싱할때 기본이 되는 타겟 커뮤니케이터 UID

protected:
	/// Low-Level Command Transfer Function. 나중에 모아두었다가 블럭 전송등이 가능하게 해줄 수 있다.
	virtual void SendCommand(CCCommand* pCommand)=0;
	/// Low-Level Command Transfer Function. 나중에 모아두었다가 블럭 전송등이 가능하게 해줄 수 있다.
	virtual void ReceiveCommand(CCCommand* pCommand);

	/// 초기 커맨드 등록할때 불리는 함수
	virtual void OnRegisterCommand(CCCommandManager* pCommandManager);
	/// 커뮤니케이터가 커맨드를 처리하기 위해 불리는 함수
	virtual bool OnCommand(CCCommand* pCommand);
	/// 커뮤니케이터 루프 전 준비
	virtual void OnPrepareRun();
	/// 커맨드를 처리하기 전에
	virtual void OnPrepareCommand(CCCommand* pCommand);
	/// 커뮤니케이터 루프
	virtual void OnRun();

	/// Post()되는 기본 리시버 커뮤니케이터 설정
	void SetDefaultReceiver(CCUID Receiver);
public:
	CCCommandCommunicator();
	virtual ~CCCommandCommunicator();

	/// 초기화
	bool Create();
	/// 해제
	void Destroy();

	/// 다른 커뮤티케이터로 연결 설정
	/// @param	pAllocUID	자기 Communicator가 배정받은 UID
	/// @return				에러 코드 (MErrorTable.h 참조)
	virtual int Connect(CCCommObject* pCommObj)=0;
	/// 커넥션이 이루어진 경우
	virtual int OnConnected(CCUID* pTargetUID, CCUID* pAllocUID, unsigned int nTimeStamp, CCCommObject* pCommObj);
	/// 연결 해제
	virtual void Disconnect( const CCUID& uid)=0;

	/// 커맨드 입력
	virtual bool Post(CCCommand* pCommand);
	/// 스트링으로 커맨드 입력
	virtual bool Post(char* szErrMsg, int nErrMsgCount, const char* szCommand);

	virtual CCCommand* GetCommandSafe();

	/// 실행 ( 커맨드 전송및 처리 )
	void Run();

	/// 커맨드 매니져 얻기
	CCCommandManager* GetCommandManager(){
		return &m_CommandManager;
	}
	CCCommand* CreateCommand(int nCmdID, const CCUID& TargetUID);

	// 의미가 없으므로ㅡ LOG_PROG 없애버리기

	enum _LogLevel	{ LOG_DEBUG = 1, LOG_FILE = 3, LOG_PROG = 7, }; //, LOG_PROG = 7,  };
	/// 디폴트 로그 출력 ( 로그 서버로 리다이렉션할 수 있게 해준다. )
	virtual void Log(unsigned int nLogLevel, const char* szLog){}
	void LOG(unsigned int nLogLevel, const char *pFormat,...);

	/// 자신의 UID를 얻어낸다.
	CCUID GetUID(){ return m_This; }

#ifdef _CMD_PROFILE
	CCCommandProfiler		m_CommandProfiler;
#endif
};

int CalcPacketSize(CCCommand* pCmd);

#endif
