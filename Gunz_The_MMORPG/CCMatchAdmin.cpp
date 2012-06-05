#include "stdafx.h"
#include "CCMatchAdmin.h"
#include "CCMatchServer.h"
#include "CCMatchStage.h"
#include "CCMatchItem.h"
#include "CCMatchRule.h"
#include "CCMatchObject.h"
#include "CCMatchObjCache.h"
#include "CCSharedCommandTable.h"

CCMatchAdmin::CCMatchAdmin()
{
	m_pMatchServer= NULL;
}

CCMatchAdmin::~CCMatchAdmin()
{

}


bool CCMatchAdmin::Create(CCMatchServer* pServer)
{
	m_pMatchServer = pServer;

	return true;
}

void CCMatchAdmin::Destroy()
{

}

bool CCMatchAdmin::Execute(const CCUID& uidAdmin, const char* szStr)
{
	if (!m_pMatchServer) return false;

	MAdminArgvInfo ai;
	memset(&ai, 0, sizeof(ai));

	char szBuf[1024];
	strcpy(szBuf, szStr);

	if (MakeArgv(szBuf, &ai))
	{
		if (ai.cargc > 0)
		{
			char szOut[65535];
			if (m_pMatchServer->OnAdminExecute(&ai, szOut))
			{
				m_pMatchServer->AdminTerminalOutput(uidAdmin, szOut);
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool CCMatchAdmin::MakeArgv(char* szStr, MAdminArgvInfo* pAi)
{
	int c;
	char* scp;
	char* dcp;
	char* dlim;
	char* arg;

	int iArgcMax = (sizeof(pAi->cargv) / sizeof(char *));

	scp = szStr;
	dcp = pAi->argbuf;
	dlim = dcp + sizeof(pAi->argbuf) - 1;

	for (pAi->cargc = 0; pAi->cargc < iArgcMax; )
	{
		for ( ; ; scp++)
		{
			c = *scp;
			if (!isascii(c)) continue;	// 한글처리
			if (isspace(c)) continue;

			if ( (c == '\0') || (c == ';') || (c == '\n') )
			{
				pAi->cargv[pAi->cargc] = NULL;
				return true;
			}
			break;
		}

		arg = dcp;
		pAi->cargv[pAi->cargc] = arg;
		(pAi->cargc)++;

		for ( ; ; )
		{
			c = *scp;
			if ( (c == '\0') || (!isascii(c)) || (isspace(c)) || (c == ';') || (c == '\n') ) break;

			scp++;

			if (dcp >= dlim) return false;

			*dcp++ = c;
		}
		*dcp++ = '\0';
	}

	return false;

}



////////////////////////////////////////////////////////////////////////////////////////////
bool CCMatchServer::OnAdminExecute(MAdminArgvInfo* pAI, char* szOut)
{
	szOut[0] = 0;

	if (pAI->cargc <= 0) return false;

	// wall
	if (!stricmp(pAI->cargv[0], "wall"))
	{
		if (pAI->cargc < 3)
		{
			sprintf(szOut, "인자가 부족합니다.");
			return true;
		}

		char szMsg[256];
		int nMsgType = 0;

		strcpy(szMsg, pAI->cargv[1]);
		nMsgType = atoi(pAI->cargv[2]);


		CCCommand* pCmd = CreateCommand(MC_ADMIN_ANNOUNCE, CCUID(0,0));
		pCmd->AddParameter(new CCCmdParaCCUID(CCUID(0,0)));
		pCmd->AddParameter(new CCCmdParamStr(szMsg));
		pCmd->AddParameter(new CCCmdParamUInt(nMsgType));

		RouteToAllClient(pCmd);
	}
	else
	{
		sprintf(szOut, "%s: no such command", pAI->cargv[0]);
	}
	
	return true;
}


void CCMatchServer::AdminTerminalOutput(const CCUID& uidAdmin, const char* szText)
{
	CCMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (pObj->GetAccountInfo()->m_nUGrade != CCMUG_ADMIN)
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	char szMsg[65535];
	strcpy(szMsg, szText);

	CCCommand* pCmd = CreateCommand(MC_ADMIN_TERMINAL, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParaCCUID(uidAdmin));
	pCmd->AddParameter(new CCCmdParamStr(szMsg));

	RouteToListener(pObj, pCmd);
}
