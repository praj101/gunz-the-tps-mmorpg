#include "stdafx.h"
#include "CCAsyncDBJob_UpdateCharBRInfo.h"

bool CCAsyncDBJob_UpdateCharBRInfo::Input(int nCID, int nBRID, int nBRTID, int nRewardCount, int nBattleTime, int nKillCount)
{
	m_nCID = nCID;

	m_nBRID = nBRID;
	m_nBRTID = nBRTID;

	m_nRewardCount = nRewardCount;

	m_nBattleTime = nBattleTime;

	m_nKillCount = nKillCount;

	return true;
}

void CCAsyncDBJob_UpdateCharBRInfo::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if( !pDBMgr->UpdateCharBRInfo(m_nCID, m_nBRID, m_nBRTID, m_nRewardCount, m_nBattleTime, m_nKillCount) )
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	SetResult(MASYNC_RESULT_SUCCEED);
}
