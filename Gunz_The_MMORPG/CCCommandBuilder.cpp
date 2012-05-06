#include "stdafx.h"
#include <winsock2.h>
#include "CCCommandBuilder.h"
#include "CCMatchUtil.h"
//#include "CCPacketHShieldCrypter.h"


CCCommandBuilder::CCCommandBuilder(CCUID uidSender, CCUID uidReceiver, CCCommandManager*	pCmdMgr) 
{	
	m_pPacketCrypter = NULL;
	m_uidSender = uidSender;
	m_uidReceiver = uidReceiver;
	m_pCommandManager = pCmdMgr;

	m_nBufferNext = 0;
	m_bCheckCommandSN = true;

	m_CommandCountPerSec = 0;
	m_dwLastCommandMakeTime = 0;
}

CCCommandBuilder::~CCCommandBuilder() 
{
	Clear();
}

bool CCCommandBuilder::CheckBufferEmpty() 
{
	if (m_nBufferNext == 0) return true;
	else return false;
}

bool CCCommandBuilder::EstimateBufferToCmd() 
{
	if (m_nBufferNext < sizeof(CCPacketHeader))
		return false;
	CCPacketHeader* pPacket = (CCPacketHeader*)m_Buffer;
	if (m_nBufferNext < _CalcPacketSize(pPacket))
		return false;
	return true;
}

void CCCommandBuilder::AddBuffer(char* pBuffer, int nLen) 
{
	if (nLen <= 0) return;
	if ((m_nBufferNext + nLen) >= COMMAND_BUFFER_LEN) {
		_ASSERT(FALSE);
		return;
	}
	CopyMemory(m_Buffer+m_nBufferNext, pBuffer, nLen);
	m_nBufferNext += nLen;
}

bool CCCommandBuilder::MoveBufferToFront(int nStart, int nLen) 
{
	if (nStart+nLen > m_nBufferNext) 
		return false;
	CopyMemory(m_Buffer, m_Buffer+nStart, nLen);
	m_nBufferNext = nLen;
	return true;
}

int CCCommandBuilder::_CalcPacketSize(CCPacketHeader* pPacket)
{
	return pPacket->CalcPacketSize(m_pPacketCrypter);
}

int CCCommandBuilder::CalcCommandCount(char* pBuffer, int nBufferLen)
{
	unsigned int nOffset = 0;
	int nLen = nBufferLen;
	CCPacketHeader* pPacket = (CCPacketHeader*)(pBuffer+nOffset);
	int nCmdCount = 0;
	int nPacketSize = 0;

	while (nLen >= sizeof(CCPacketHeader))
	{
		pPacket = (CCPacketHeader*)(pBuffer+nOffset);

		nPacketSize = _CalcPacketSize(pPacket);
		if ((nPacketSize > nLen) || (nPacketSize <= 0)) break;

		nOffset += nPacketSize;
		nLen -= nPacketSize;			
		nCmdCount++;		
	}

	return nCmdCount;
}

int CCCommandBuilder::MakeCommand(char* pBuffer, int nBufferLen) 
{
	unsigned int nOffset = 0;
	int nLen = nBufferLen;
	CCPacketHeader* pPacket = (CCPacketHeader*)(pBuffer+nOffset);
	int nCmdCount = 0;
	int nPacketSize = 0;

	while (nLen >= sizeof(CCPacketHeader))
	{
		int nPacketSize = _CalcPacketSize(pPacket);
		if ((nPacketSize > nLen) || (nPacketSize <= 0)) break;

		if (pPacket->nMsg == MSGID_RAWCOMMAND)
		{
			unsigned short nCheckSum = MBuildCheckSum(pPacket, nPacketSize);
			if (pPacket->nCheckSum != nCheckSum) {
				return -1;
			} else if (nPacketSize > MAX_PACKET_SIZE)
			{
				return -1;
			}
			else 
			{
				CCCommand* pCmd = new CCCommand();
				int nCmdSize = nPacketSize - sizeof(CCPacketHeader);
				if (pCmd->SetData(((CCCommandMsg*)pPacket)->Buffer, m_pCommandManager, (unsigned short)nCmdSize))
				{
					// 시리얼 체크
					if (m_bCheckCommandSN)
					{
						if (!m_CommandSNChecker.CheckValidate(pCmd->m_nSerialNumber))
						{
							delete pCmd; pCmd = NULL;
							return -1;
						}
					}

					pCmd->m_Sender = m_uidSender;
					pCmd->m_Receiver = m_uidReceiver;
					m_CommandList.push_back(pCmd);
				}
				else
				{
					delete pCmd; pCmd = NULL;
					return -1;
				}
			}
		}
		else if (pPacket->nMsg == MSGID_COMMAND) 
		{
			unsigned short nCheckSum = MBuildCheckSum(pPacket, nPacketSize);
			if (pPacket->nCheckSum != nCheckSum) {
				return -1;
			} 
			else if (nPacketSize > MAX_PACKET_SIZE)
			{
				return -1;
			}
			else 
			{
				// 암호화 디코딩
				CCCommand* pCmd = new CCCommand();

				int nCmdSize = nPacketSize - sizeof(CCPacketHeader);
				if (m_pPacketCrypter)
				{
					if (!m_pPacketCrypter->Decrypt((char*)((CCCommandMsg*)pPacket)->Buffer, nCmdSize))
					{
						delete pCmd; pCmd = NULL;
						return -1;
					}
				}

				if (pCmd->SetData((char*)((CCCommandMsg*)pPacket)->Buffer, m_pCommandManager, (unsigned short)nCmdSize))
				{
					// 시리얼 체크
					if (m_bCheckCommandSN)
					{
						if (!m_CommandSNChecker.CheckValidate(pCmd->m_nSerialNumber))
						{
							delete pCmd; pCmd = NULL;
							return -1;
						}
					}

					pCmd->m_Sender = m_uidSender;
					pCmd->m_Receiver = m_uidReceiver;
					m_CommandList.push_back(pCmd);
				}
				else
				{
					delete pCmd; pCmd = NULL;
					return -1;
				}
			}
		} 
		else if (pPacket->nMsg == MSGID_REPLYCONNECT) {
			// 아직 UID 설정하기 전일수 있음
			if (nPacketSize == sizeof(MReplyConnectMsg))
			{
				CCPacketHeader* pNewPacket = (CCPacketHeader*)malloc(nPacketSize);
				CopyMemory(pNewPacket, pPacket, nPacketSize);
				m_NetCmdList.push_back(pNewPacket);
			}
			else
			{
				return -1;
			}
		}
		else {
			return -1;
		}

		nOffset += nPacketSize;
		nLen -= nPacketSize;			
		nCmdCount++;

//			if (nLen < sizeof(CCPacketHeader)) break;
		pPacket = (CCPacketHeader*)(pBuffer+nOffset);
	}


	return nLen;
}

void CCCommandBuilder::Clear()
{
	if (!m_CommandList.empty())
	{
		for (CCCommandList::iterator itorCmd = m_CommandList.begin(); itorCmd != m_CommandList.end(); ++itorCmd)
		{
			CCCommand* pCmd = (*itorCmd);
			delete pCmd;
		}
		m_CommandList.clear();
	}

	if (!m_NetCmdList.empty())
	{
		for (list<CCPacketHeader*>::iterator itorNetCmd = m_NetCmdList.begin(); itorNetCmd != m_NetCmdList.end(); ++itorNetCmd)
		{
			CCPacketHeader* pNetCmd = (*itorNetCmd);
			free(pNetCmd);
		}
		m_NetCmdList.clear();
	}
}

bool CCCommandBuilder::Read(char* pBuffer, int nBufferLen, bool bFloodCheck, bool *bFloodResult) 
{
	CCPacketHeader* pPacket = (CCPacketHeader*)pBuffer;

	if (CheckBufferEmpty() == true)  {
		if ( (nBufferLen < sizeof(CCPacketHeader)) || (nBufferLen < _CalcPacketSize(pPacket)) ) {	
			AddBuffer(pBuffer, nBufferLen);	// 내부 Buffer에 저장
		} else {	
			// Flood Check
			int nCommandCount = CalcCommandCount(pBuffer, nBufferLen);
			if( CheckFlooding(nCommandCount) == true && bFloodCheck )
			{
				if( bFloodResult != NULL )
					(*bFloodResult) = true;

				return false;
			}
			
			// Build Command
			int nSpareData = MakeCommand(pBuffer, nBufferLen);			// Arg로 넘어온 외부버퍼에서 바로 Cmd생성
			if (nSpareData > 0) {
				AddBuffer(pBuffer+(nBufferLen-nSpareData), nSpareData);	// 남은부분 내부 Buffer에 저장(내부버퍼처음사용)
			}
			else if (nSpareData < 0) return false;
		}
	} else {
		AddBuffer(pBuffer, nBufferLen);
		if (EstimateBufferToCmd() == true) {
			// Flood Check
			int nCommandCount = CalcCommandCount(pBuffer, nBufferLen);
			if( CheckFlooding(nCommandCount) == true && bFloodCheck )
			{
				if( bFloodResult != NULL )
					(*bFloodResult) = true;

				return false;
			}

			// Build Command
			int nSpareData = MakeCommand(m_Buffer, m_nBufferNext);	// m_nBufferNext == nBufferSize
			if (nSpareData >= 0)
				MoveBufferToFront(m_nBufferNext-nSpareData, nSpareData);	// Cmd만들고 남은부분 버퍼 앞쪽으로이동
			else return false;						
		}
	}

	return true;
}

CCCommand* CCCommandBuilder::GetCommand() 
{
	CCCommandList::iterator itorCmd = m_CommandList.begin();
	if (itorCmd != m_CommandList.end()) {
		CCCommand* pCmd = (*itorCmd);
		m_CommandList.pop_front();
		return (pCmd);
	} else {
		return NULL;
	}
}


CCPacketHeader* CCCommandBuilder::GetNetCommand() 
{
	list<CCPacketHeader*>::iterator itorCmd = m_NetCmdList.begin();
	if (itorCmd != m_NetCmdList.end()) {
		CCPacketHeader* pTestCmd = (*itorCmd);
		m_NetCmdList.pop_front();
		return (pTestCmd);
	} else {
		return NULL;
	}
}


void CCCommandBuilder::SetUID(CCUID uidReceiver, CCUID uidSender)
{
	m_uidReceiver = uidReceiver;
	m_uidSender = uidSender;
}

void CCCommandBuilder::InitCrypt(CCPacketCrypter* pPacketCrypter, bool bCheckCommandSerialNumber)
{
	m_pPacketCrypter = pPacketCrypter;
	m_bCheckCommandSN = bCheckCommandSerialNumber;
}

bool CCCommandBuilder::CheckFlooding(int nCommandCount)
{
	DWORD dwCurTime = GetTickCount();

	if( dwCurTime - m_dwLastCommandMakeTime < 1000 )
	{
		m_CommandCountPerSec += nCommandCount;

		if( m_CommandCountPerSec > MAX_COMMAND_COUNT_FLOODING )
			return true;
	}
	else
	{
		m_CommandCountPerSec = nCommandCount;
		m_dwLastCommandMakeTime = dwCurTime;

		if( m_CommandCountPerSec > MAX_COMMAND_COUNT_FLOODING )
			return true;
	}

	return false;
}