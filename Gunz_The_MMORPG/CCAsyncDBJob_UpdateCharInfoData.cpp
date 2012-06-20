#include "stdafx.h"
#include "CCAsyncDBJob_UpdateCharInfoData.h"

void CCAsyncDBJob_UpdateCharInfoData::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->UpdateCharInfoData(m_nCID, 
								   m_nAddedXP, 
								   m_nAddedBP, 
								   m_nAddedKillCount, 
								   m_nAddedDeathCount,
								   m_nAddedPlayTime))
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}

bool CCAsyncDBJob_UpdateCharInfoData::Input(const int nCID, 
											const int nAddedXP, 
											const int nAddedBP, 
											const int nAddedKillCount, 
											const int nAddedDeathCount,
											const int nAddedPlayTime)
{
	m_nCID = nCID;
	m_nAddedXP = nAddedXP;
	m_nAddedBP = nAddedBP;
	m_nAddedKillCount = nAddedKillCount;
	m_nAddedDeathCount = nAddedDeathCount;
	m_nAddedPlayTime = nAddedPlayTime;

	return true;
}
