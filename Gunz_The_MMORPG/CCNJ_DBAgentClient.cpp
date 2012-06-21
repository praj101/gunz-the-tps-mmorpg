#include "stdafx.h"
#include "CCNJ_DBAgentClient.h"

CCNJ_DBAgentClient::CCNJ_DBAgentClient(int nGameCode, int nServerCode) 
		: CCCustomClient(), m_bConnected(false), m_nGameCode(nGameCode), m_nServerCode(nServerCode), m_nQueueTop(0)
{
	InitializeCriticalSection(&m_csPoolLock);

	memset(m_cPacketBuf, 0, sizeof(m_cPacketBuf));
}

CCNJ_DBAgentClient::~CCNJ_DBAgentClient()
{
	DeleteCriticalSection(&m_csPoolLock);
}

bool CCNJ_DBAgentClient::OnSockConnect(SOCKET sock)
{
	m_bConnected = true;

	NJ_PACKET* pNewPacket = new NJ_PACKET;
	memset(pNewPacket, 0, sizeof(NJ_PACKET));

	pNewPacket->nCMD = NJ_CMD_INIT_INFO;
	pNewPacket->nDataSize = 8;

	int nCode[2];
	nCode[0] = m_nGameCode;
	nCode[1] = m_nServerCode;

	memcpy(pNewPacket->cDataBody, nCode, sizeof(int)*2);
	CCCustomClient::Send((char*)pNewPacket, sizeof(NJ_PACKET));

	return true;
}

bool CCNJ_DBAgentClient::OnSockDisconnect(SOCKET sock)
{
	m_bConnected = false;
	return true;
}

void CCNJ_DBAgentClient::Send(const CCUID& uidComm, const char* szCN, const char* szPW, bool bFreeLoginIP, unsigned long nChecksumPack, int nTotalUserCount)
{
	// 풀에 넣어놓는다.
	LockPool();  //-------------------------------------------------------|
	m_Pool.Insert(szCN, uidComm, nChecksumPack, bFreeLoginIP);
	UnlockPool(); //------------------------------------------------------|

	NJ_PACKET* pNewPacket = new NJ_PACKET;
	memset(pNewPacket, 0, sizeof(NJ_PACKET));

	NJ_USERINFO* pUserInfo = (NJ_USERINFO*)(pNewPacket->cDataBody);

	pNewPacket->nCMD = NJ_CMD_LOGIN_C;
	pNewPacket->nDataSize = sizeof(NJ_USERINFO);
	strcpy(pUserInfo->szCN, szCN);
	strcpy(pUserInfo->szPW, szPW);
	pUserInfo->m_nTotalUserCount = nTotalUserCount;

	CCCustomClient::Send((char*)pNewPacket, sizeof(NJ_PACKET));
}


bool CCNJ_DBAgentClient::OnSockRecv(SOCKET sock, char* pPacket, DWORD dwSize)
{
	if (((m_nQueueTop + dwSize) >= NJ_QUE_SIZE) || (dwSize <= 0))
	{
		_ASSERT(0);
		return false;
	}

	memcpy(m_cPacketBuf + m_nQueueTop, pPacket, dwSize);
	m_nQueueTop += dwSize;

	while (m_nQueueTop >= sizeof(NJ_PACKET))
	{
		NJ_PACKET* pNJPacket = (NJ_PACKET*)(m_cPacketBuf);
		OnRecvPacket(pNJPacket);

		if (m_nQueueTop-sizeof(NJ_PACKET) > 0)
		{
			memcpy(m_cPacketBuf, m_cPacketBuf+sizeof(NJ_PACKET), m_nQueueTop-sizeof(NJ_PACKET));
		}
		m_nQueueTop -= sizeof(NJ_PACKET);
	}

	return true;
}


void CCNJ_DBAgentClient::OnRecvPacket(NJ_PACKET *pPacket)
{
	NJ_USERINFO* pU = (NJ_USERINFO*)pPacket->cDataBody;
	
	bool bExist = false;
	CCUID uidComm;
	bool bFreeLoginIP;
	unsigned long nChecksumPack;

	LockPool(); //------------------------------------------------------|

	CCDBAgentPoolNode* pPoolNode = m_Pool.GetNode(pU->szCN);
	if (pPoolNode)
	{
		bExist = true;
		uidComm = pPoolNode->uidComm;
		bFreeLoginIP = pPoolNode->bFreeLoginIP;
		nChecksumPack = pPoolNode->nChecksumPack;
	}

	m_Pool.Remove(pU->szCN);

	UnlockPool(); //----------------------------------------------------|

	if (bExist == false) return;

	switch( pPacket->nCMD )
	{
	case NJ_CMD_LOGIN_OK:
		{
			//=====================================================================================

			CCCommand* pNew = CCMatchServer::GetInstance()->CreateCommand(MC_MATCH_LOGIN_FROM_DBAGENT, CCUID(0,0));
			pNew->AddParameter(new CCCommandParameterUID(uidComm));
			pNew->AddParameter(new CCCommandParameterString(pU->szCN));
			pNew->AddParameter(new CCCommandParameterString(pU->szNN));
			pNew->AddParameter(new CCCommandParameterInt((int)pU->sSex));
			pNew->AddParameter(new CCCommandParameterBool(bFreeLoginIP));
			pNew->AddParameter(new CCCommandParameterUInt(nChecksumPack));
			CCMatchServer::GetInstance()->PostSafeQueue(pNew);

			//=====================================================================================

		} break;

	case NJ_CMD_LOGIN_DENY_SERVERBUSY:
	case NJ_CMD_LOGIN_DENY_PASSERR:
	case NJ_CMD_LOGIN_DENY_USINGID:
	case NJ_CMD_LOGIN_DENY_CANT:
		{
			CCCommand* pNew = CCMatchServer::GetInstance()->CreateCommand(MC_MATCH_LOGIN_FROM_DBAGENT_FAILED, CCUID(0,0));
			pNew->AddParameter(new CCCommandParameterUID(uidComm));
			pNew->AddParameter(new CCCommandParameterInt(MERR_FAILED_AUTHENTICATION));
			CCMatchServer::GetInstance()->PostSafeQueue(pNew);
		} break;
	}


	

}