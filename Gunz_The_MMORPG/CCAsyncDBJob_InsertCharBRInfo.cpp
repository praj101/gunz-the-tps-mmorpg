#include "stdafx.h"
#include "CCAsyncDBJob_InsertCharBRInfo.h"

bool CCAsyncDBJob_GetCharBRInfo::Input(int nCID, int nBRID, int nBRTID)
{
	m_nCID = nCID;

	m_nBRID = nBRID;
	m_nBRTID = nBRTID;

	return true;
}

void CCAsyncDBJob_GetCharBRInfo::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if( !pDBMgr->GetCharBRInfo(m_nCID, m_nBRID, &m_CharBRInfo) )	
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	if( m_CharBRInfo.GetBRID() == -1 )
	{
		if( !pDBMgr->InsertCharBRInfo(m_nCID, m_nBRID, m_nBRTID) )
		{
			SetResult(CCASYNC_RESULT_FAILED);
			return;
		}

		m_CharBRInfo.SetBRInfo(m_nBRID, m_nBRTID, 0, 0, 0);
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}
