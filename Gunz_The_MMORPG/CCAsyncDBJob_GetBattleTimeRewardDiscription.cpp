#include "stdafx.h"
#include "CCAsyncDBJob_GetBRDescription.h"

void CCAsyncDBJob_GetBattleTimeRewardDescription::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if ( !pDBMgr->GetBattletimeRewardList(m_vBattletimeRewardDescription) )
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	if ( !pDBMgr->GetBattletimeRewardItemList(m_vBattletimeRewardItem) )
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}


	SetResult(CCASYNC_RESULT_SUCCEED);
}
