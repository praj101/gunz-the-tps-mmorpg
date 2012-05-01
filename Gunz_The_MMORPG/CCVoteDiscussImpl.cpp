#include "stdafx.h"
#include "CCVoteDiscussImpl.h"
#include "CCMatchServer.h"
#include "CCSharedCommandTable.h"
#include "CCLex.h"


///////////////////////////////////////////////////////
// CCVoteDiscussBuilder
CCVoteDiscuss* CCVoteDiscussBuilder::Build(const CCUID& uidDrafter, const CCUID& uidStage, const char* pszDiscuss, const char* pszArg)
{
	if ( (stricmp(pszDiscuss, "joke") == 0) || (stricmp(pszDiscuss, "³ó´ã") == 0) ) {
		CCVoteDiscussJoke* pDiscuss = new CCVoteDiscussJoke(uidStage);
		pDiscuss->m_strJoke = pszArg;
		return pDiscuss;
	}
	else if ( (stricmp(pszDiscuss, "kick") == 0) || (stricmp(pszDiscuss, "Ãß¹æ") == 0) ) {
		CCVoteDiscussKick* pDiscuss = new CCVoteDiscussKick(uidStage);
		pDiscuss->m_strTarget = pszArg;
		return pDiscuss;
	}
	return NULL;
}

///////////////////////////////////////////////////////
// CCVoteDiscussJoke
CCVoteDiscussJoke::CCVoteDiscussJoke(const CCUID& uidStage) : CCVoteDiscuss(uidStage)
{
}

bool CCVoteDiscussJoke::OnJudge(bool bJudge)
{
	CCMatchServer* pServer = CCMatchServer::GetInstance();

	char szMsg[ VOTE_ARG_STRING_LEN ] = {0,};

	sprintf(szMsg, "%s", m_strJoke.c_str());

	MCommand* pCmd = pServer->CreateCommand(MC_MATCH_ANNOUNCE, CCUID(0,0));
	pCmd->AddParameter(new MCmdParamUInt(0));
	pCmd->AddParameter(new MCmdParamStr(szMsg));
	pServer->RouteToStage(GetStageUID(), pCmd);

	return true;
}

///////////////////////////////////////////////////////
// CCVoteDiscussKick
CCVoteDiscussKick::CCVoteDiscussKick(const CCUID& uidStage) : CCVoteDiscuss(uidStage)
{
}

bool CCVoteDiscussKick::OnJudge(bool bJudge)
{
	CCMatchServer* pServer = CCMatchServer::GetInstance();
	CCMatchStage* pStage = pServer->FindStage(GetStageUID());
	if (pStage == NULL) return false;

	if (bJudge == true) {
		if ( pStage->KickBanPlayer(m_strTarget.c_str()) ) {
		}
	}

	return true;
}
