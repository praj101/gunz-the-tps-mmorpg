//////////////////////////////////////////////////////////////////////////////////////////////
// 2009. 6. 3 - Added By Hong KiJu

#include "stdafx.h"
#include "CCAsyncDBJob_SurvivalMode.h"

void CCAsyncDBJob_InsertSurvivalModeGameLog::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->InsertSurvivalModeGameLog(m_szGameName, m_dwScenarioID, m_dwTotalRound, 
											m_dwMasterPlayerCID, m_dwMasterPlayerRankPoint, 
											m_dwPlayer2CID, m_dwPlayer2RankPoint, 
											m_dwPlayer3CID,	m_dwPlayer3RankPoint, 
											m_dwPlayer4CID, m_dwPlayer4RankPoint, 
											m_dwGamePlayTime) )
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}


void CCAsyncDBJob_GetSurvivalModeGroupRanking::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->GetSurvivalModeGroupRanking(m_RankingVec) )
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}

void CCAsyncDBJob_GetSurvivalModePrivateRanking::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->GetSurvivalModePrivateRanking(m_dwCID, m_RankingInfo) )
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}