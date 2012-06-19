#include "stdafx.h"
#include "CCAsyncDBJob_InsertGamePlayerLog.h"

bool CCAsyncDBJob_InsertGamePlayerLog::Input(int nGameLogID, int nCID, int nPlayTime, int nKills, int nDeaths, int nXP, int nBP)
{
	m_nGameLogID = nGameLogID;

	m_nCID = nCID;
	m_nPlayTime = nPlayTime;
	m_nKills = nKills;
	m_nDeaths = nDeaths;
	m_nXP = nXP;
	m_nBP = nBP;

	return true;
}

void CCAsyncDBJob_InsertGamePlayerLog::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->InsertGamePlayerLog(m_nGameLogID, m_nCID, m_nPlayTime, m_nKills, m_nDeaths, m_nXP, m_nBP)) {
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}
