#include "stdafx.h"
#include "CCAsyncDBJob_BuyQuestItem.h"



void CCAsyncDBJob_BuyQuestItem::Run( void* pContext )
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if( !pDBMgr->UpdateCharBP(m_dwCID, -m_nPrice) )
	{
		SetResult( CCASYNC_RESULT_FAILED );
		return;
	}

	SetResult( CCASYNC_RESULT_SUCCEED );
}