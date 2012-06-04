#include "stdafx.h"
#include "CCCommandCommunicator.h"
#include "CCErrorTable.h"
#include <stdarg.h>
#include "CCCommandBuilder.h"
#include "CCMatchCheckLoopTime.h"

CCCommObject::CCCommObject(CCCommandCommunicator* pCommunicator)
{
	m_uid = CCUID(0,0);

	m_pDirectConnection = NULL;
	m_dwUserContext = 0;

	m_szIP[0] = NULL;
	m_nPort = 0;

	m_bAllowed = true;
	m_bPassiveSocket = false;

	m_pCommandBuilder = new CCCommandBuilder(CCUID(0,0), pCommunicator->GetUID(), 
											pCommunicator->GetCommandManager());

}

CCCommObject::~CCCommObject()
{
	if (m_pCommandBuilder) {
		delete m_pCommandBuilder;
		m_pCommandBuilder = NULL;
	}
}

void CCCommandCommunicator::ReceiveCommand(CCCommand* pCommand)
{
	pCommand->m_Receiver = m_This;
	m_CommandManager.Post(pCommand);
}

void CCCommandCommunicator::OnRegisterCommand(CCCommandManager* pCommandManager)
{
}

bool CCCommandCommunicator::OnCommand(CCCommand* pCommand)
{
	return false;
}

void CCCommandCommunicator::OnPrepareRun()
{
}

void CCCommandCommunicator::OnPrepareCommand(CCCommand* pCommand)
{
}

void CCCommandCommunicator::OnRun()
{
}

void CCCommandCommunicator::SetDefaultReceiver(CCUID Receiver)
{
	m_DefaultReceiver = Receiver;
}

CCCommandCommunicator::CCCommandCommunicator()
{
	m_This.SetZero();
	m_DefaultReceiver.SetZero();
}

CCCommandCommunicator::~CCCommandCommunicator()
{
	Destroy();
#ifdef _CMD_PROFILE
	m_CommandProfiler.Analysis();
#endif
}

bool CCCommandCommunicator::Create()
{
#ifdef _CMD_PROFILE
	m_CommandProfiler.Init(&m_CommandManager);
#endif

	OnRegisterCommand(&m_CommandManager);	// Virtual Function은 Constructor에서 호출되지 않는다.
	return true;
}

void CCCommandCommunicator::Destroy()
{
	while(CCCommand* pCmd = GetCommandSafe()) {
		delete pCmd;
	}
}

int CCCommandCommunicator::OnConnected(CCUID* pTargetUID, CCUID* pAllocUID, unsigned int nTimeStamp, CCCommObject* pCommObj)
{
	m_This = *pAllocUID;
	SetDefaultReceiver(*pTargetUID);

	if (pCommObj)
	{
		CCCommandBuilder* pCmdBuilder = pCommObj->GetCommandBuilder();
		pCmdBuilder->SetUID(*pAllocUID, *pTargetUID);
	}
	return MOK;
}

bool CCCommandCommunicator::Post(CCCommand* pCommand)
{
	return m_CommandManager.Post(pCommand);
}

bool CCCommandCommunicator::Post(char* szErrMsg, int nErrMsgCount, const char* szCommand)
{
	CCCommand* pCmd = new CCCommand;
	if(m_CommandManager.ParseMessage(pCmd, szErrMsg, nErrMsgCount, szCommand)==false){
		delete pCmd;
		return false;
	}

	pCmd->m_Sender = m_This;
	pCmd->m_Receiver = m_DefaultReceiver;

	if(Post(pCmd)==false){
		delete pCmd;
		return false;
	}

	return true;
}

CCCommand* CCCommandCommunicator::CreateCommand(int nCmdID, const CCUID& TargetUID)
{
	return new CCCommand(m_CommandManager.GetCommandDescByID(nCmdID), TargetUID, m_This);
}

CCCommand* CCCommandCommunicator::GetCommandSafe()
{
	return m_CommandManager.GetCommand();
}

void CCCommandCommunicator::Run()
{
	CCGetCheckLoopTimeInstance()->SetPrepareRunTick();
	OnPrepareRun();

	CCGetCheckLoopTimeInstance()->SetCommandTick();
	int nVecIndex;
	while(1)
	{
		CCCommand* pCommand = GetCommandSafe();

		if(pCommand==NULL) break;
		
		unsigned int CommandID = pCommand->GetID(); // 덤프시확인위해

		nVecIndex = CCGetCheckLoopTimeInstance()->AddCommandTimeGap(pCommand->GetID());
		OnPrepareCommand(pCommand);

		if ((pCommand->m_pCommandDesc->IsFlag(MCDT_PEER2PEER)==true))
		{
			if (pCommand->m_Sender != m_This)
			{
				#ifdef _CMD_PROFILE
					m_CommandProfiler.OnRecv(pCommand);
					m_CommandProfiler.OnCommandBegin(pCommand, timeGetTime());
				#endif
				OnCommand(pCommand);

				#ifdef _CMD_PROFILE
					m_CommandProfiler.OnCommandEnd(pCommand, timeGetTime());
				#endif
			}
			else
			{
				#ifdef _CMD_PROFILE
					m_CommandProfiler.OnSend(pCommand);
				#endif

				SendCommand(pCommand);

				#ifdef _CMD_PROFILE
					m_CommandProfiler.OnCommandBegin(pCommand, timeGetTime());
				#endif

				OnCommand(pCommand);

				#ifdef _CMD_PROFILE
					m_CommandProfiler.OnCommandEnd(pCommand, timeGetTime());
				#endif
			}
		}
		else if (pCommand->m_pCommandDesc->IsFlag(MCDT_LOCAL)==true || 
			    (m_This.IsValid() && pCommand->m_Receiver==m_This))
		{
			// Local Command 이거나 도착한 커맨드는 처리
			#ifdef _CMD_PROFILE
				m_CommandProfiler.OnRecv(pCommand);
				m_CommandProfiler.OnCommandBegin(pCommand, timeGetTime());
			#endif

			OnCommand(pCommand);

			#ifdef _CMD_PROFILE
				m_CommandProfiler.OnCommandEnd(pCommand, timeGetTime());
			#endif
		}
		else 
		{
			// 그외에는 설정된 Receiver로 전송
			#ifdef _CMD_PROFILE
				m_CommandProfiler.OnSend(pCommand);
			#endif

			SendCommand(pCommand);
		}

		delete pCommand;
		pCommand = NULL;
		CCGetCheckLoopTimeInstance()->SetCommandEndTick(nVecIndex);
	}

	OnRun();
}

void CCCommandCommunicator::LOG(unsigned int nLogLevel, const char *pFormat,...)
{
	if (nLogLevel != LOG_DEBUG)
	{
		va_list args;
		static char temp[1024];

		va_start(args, pFormat);
		vsprintf(temp, pFormat, args);
		Log(nLogLevel, temp);
		va_end(args);
	}
	else
	{
#if defined(_DEBUG) && (!defined(_DEBUG_PUBLISH))
		va_list args;
		static char temp[1024];

		va_start(args, pFormat);
		vsprintf(temp, pFormat, args);
		Log(nLogLevel, temp);
		va_end(args);
#endif
	}
}


int CalcPacketSize(CCCommand* pCmd)
{
	return (sizeof(CCPacketHeader) + pCmd->GetSize());
}
