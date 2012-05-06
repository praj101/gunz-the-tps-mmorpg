#pragma once
#include <windowsx.h>
#include "CCCommandManager.h"
#include "Msg.h"
#include "CCDebug.h"
#include "CCCRC32.h"
#include "CCPacketCrypter.h"

const int MAX_COMMAND_COUNT_FLOODING = 50;			///< 보관 개수 50개

#define COMMAND_BUFFER_LEN	16384
/// CCCommandBuilder : 완결되지않은 Stream을 버퍼링한뒤 Command로 복원한다. <Builder Pattern>
class CCCommandBuilder {	
protected:
	DWORD			m_dwLastCommandMakeTime;
	int				m_CommandCountPerSec;

	CCUID					m_uidSender;	// client
	CCUID					m_uidReceiver;	// server
	CCCommandManager*		m_pCommandManager;


	char					m_Buffer[COMMAND_BUFFER_LEN];
	int						m_nBufferNext;

	CCCommandList			m_CommandList;
	list<CCPacketHeader*>	m_NetCmdList;

	CCPacketCrypter*			m_pPacketCrypter;			// 암호화
	CCCommandSNChecker		m_CommandSNChecker;
	bool					m_bCheckCommandSN;
protected:
	bool CheckBufferEmpty();
	bool EstimateBufferToCmd();
	void AddBuffer(char* pBuffer, int nLen);
	bool MoveBufferToFront(int nStart, int nLen);
	
	int CalcCommandCount(char* pBuffer, int nBufferLen);
	bool CheckFlooding(int nCommandCount);

	int MakeCommand(char* pBuffer, int nBufferLen);

	void Clear();
	int _CalcPacketSize(CCPacketHeader* pPacket);
public:
	CCCommandBuilder(CCUID uidSender, CCUID uidReceiver, CCCommandManager*	pCmdMgr);
	virtual ~CCCommandBuilder();
	void SetUID(CCUID uidReceiver, CCUID uidSender);
	void InitCrypt(CCPacketCrypter* pPacketCrypter, bool bCheckCommandSerialNumber);
	bool Read(char* pBuffer, int nBufferLen, bool bFloodCheck=false, bool *bFloodResult=NULL);
	void SetCheckCommandSN(bool bCheck) { m_bCheckCommandSN = bCheck; }

	CCCommand* GetCommand();
	CCPacketHeader* GetNetCommand();
};


#ifdef _DEBUG
// Network Buffer Emulation for Debug
class CCCmdBufTestor {
public:
	char	m_szBuffer[16000];
	int		m_nBufferNext;

public:
	CCCmdBufTestor() { m_nBufferNext = 0; }
	int PostCmd(CCCommand* pCommand) {
		const int BUF_SIZE = 1024;
		char SendBuf[BUF_SIZE];
		int nSize = pCommand->GetData(SendBuf, BUF_SIZE);
		
		int nBlockSize = nSize+sizeof(CCCommandMsg);
		CCCommandMsg* pMsg = (CCCommandMsg*)(m_szBuffer+m_nBufferNext);
		pMsg->nMsg = MSGID_COMMAND;
		pMsg->nSize = nBlockSize;
		CopyMemory(pMsg->Buffer, SendBuf, nSize);

		m_nBufferNext += nBlockSize;
		return nBlockSize;
	}
};
#endif
// DEBUG SAMPLE //////////////////////////////////////////////////
//	MCOMMUNICATORINFO Info;
//	Info.uid = CCUID(0,100);
//	CCCommandBuilder CmdBuilder(&Info, this);
//
//	CCCommand* pCmd1 = CreateCommand(MC_AGENT_CONNECT, CCUID(0,0));
//	pCmd1->AddParameter(new CCCmdParamStr("192.168.0.30"));
//	pCmd1->AddParameter(new CCCmdParamInt(5555));
//
//	CCCommand* pCmd2 = CreateCommand(MC_AGENT_CONNECT, CCUID(0,0));
//	pCmd2->AddParameter(new CCCmdParamStr("192.168.0.31"));
//	pCmd2->AddParameter(new CCCmdParamInt(6666));
//
//	CCCmdBufTestor CmdTestor;
//	int nSize1 = CmdTestor.PostCmd(pCmd1);
//	int nSize2 = CmdTestor.PostCmd(pCmd2);
//	int nSizeTotal = nSize1+nSize2;
//
//	//CmdBuilder.Read(CmdTestor.m_szBuffer, 4);
//	//CmdBuilder.Read(CmdTestor.m_szBuffer+4, 45);
//	//CmdBuilder.Read(CmdTestor.m_szBuffer+4+45, nSizeTotal-4-45);
//////////////////////////////////////////////////////////////////

