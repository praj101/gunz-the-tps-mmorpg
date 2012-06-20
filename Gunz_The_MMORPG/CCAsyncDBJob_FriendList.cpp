#include "stdafx.h"
#include "CCAsyncDBJob_FriendList.h"

void CCAsyncDBJob_FriendList::Run(void* pContext)
{
	_ASSERT(m_pFriendInfo);

	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	// 해당캐릭터의 친구목록 가져오기
	if (!pDBMgr->FriendGetList(m_nCID, m_pFriendInfo)) 
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}
