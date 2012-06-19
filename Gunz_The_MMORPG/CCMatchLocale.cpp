#include "stdafx.h"
#include "CCMatchLocale.h"
#include "CCMatchServer.h"
#include "CCMatchConfig.h"

CCMatchLocale* CCMatchLocale::GetInstance()
{
	static CCMatchLocale m_stLocale;
	return &m_stLocale;
}

CCMatchLocale::CCMatchLocale() : CCBaseLocale(), m_pDBAgentClient(NULL), m_bCheckAntiHackCrack(false)
{

}

CCMatchLocale::~CCMatchLocale()
{
	if (m_pDBAgentClient) 
	{
		delete m_pDBAgentClient; m_pDBAgentClient = NULL;
	}
}

bool CCMatchLocale::OnInit()
{
	m_bCheckAntiHackCrack = false;

	switch (m_iCountry)
	{
		case CCC_KOREA:
			break;

		case CCC_US:
			break;

		case CCC_JAPAN:
			{
//				int nGameCode = MGetServerConfig()->GetNJDBAgentGameCode();
//				int nServerCode = MGetServerConfig()->GetServerID();
//				m_pDBAgentClient = new MNJ_DBAgentClient(nGameCode, nServerCode);
//				ConnectToDBAgent();		// 일본 넷마블판만 DBAgent에 접속한다.
			}
			break;

		case CCC_BRAZIL:
			break;

		case CCC_INDIA:
			break;
	}

#ifdef _XTRAP
	// XTrap Crack을 체크한다.
	if( MGetServerConfig()->IsUseXTrap() )
		m_bCheckAntiHackCrack = true;		
#endif

	return true;
}

bool CCMatchLocale::ConnectToDBAgent()
{
//	if (m_nCountry != CCC_JAPAN) return false;

	if ((m_pDBAgentClient) && (!m_pDBAgentClient->IsConnected()) && (!m_pDBAgentClient->GetSock()->IsActive()))
	{
		char szIP[128];
		strcpy(szIP, CCGetServerConfig()->GetNJDBAgentIP());
		m_pDBAgentClient->Connect(szIP, CCGetServerConfig()->GetNJDBAgentPort());

		CCMatchServer::GetInstance()->LOG(CCMatchServer::LOG_PROG, "Connect to DBAgent\n");
		return true;
	}

	return false;
}


bool CCMatchLocale::PostLoginInfoToDBAgent(const CCUID& uidComm, const char* szCN, const char* szPW, bool bFreeLoginIP, unsigned long nChecksumPack, int nTotalUserCount)
{
//	if (m_nCountry != CCC_JAPAN)
	{
		cclog( "country code is not JPN.\n" );
		return false;
	}

	if (m_pDBAgentClient)
	{
		if (!m_pDBAgentClient->GetSock()->IsActive())
		{
			if (!m_pDBAgentClient->IsConnected())
			{
				cclog( "Retry connect to DBAgent.\n" );
				ConnectToDBAgent();
				return false;
			}
		}

		if (m_pDBAgentClient->IsConnected())
		{
			m_pDBAgentClient->Send(uidComm, szCN, szPW, bFreeLoginIP, nChecksumPack, nTotalUserCount);
			return true;
		}

		cclog( "DBAgent not connect.\n" );
	}
	
	cclog( "DBAgentClient NULL.\n" );
	
	return false;
}

//bool CCMatchLocale::SkipCheckAntiHackCrack()
//{
//#ifdef _DEBUG
//	return true;
//#endif

//	return !m_bCheckAntiHackCrack;
//}