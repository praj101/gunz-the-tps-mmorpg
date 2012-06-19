#include "stdafx.h"
#include "CCAsyncDBJob_InsertConnLog.h"

void CCAsyncDBJob_InsertConnLog::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	pDBMgr->InsertConnLog(m_nAID, m_szIP, m_strCountryCode3);

	SetResult(CCASYNC_RESULT_SUCCEED);
}


bool CCAsyncDBJob_InsertConnLog::Input(unsigned long int nAID, char* szIP, const string& strCountryCode3 )
{
	m_nAID = nAID;
	strcpy(m_szIP, szIP);
	m_strCountryCode3 = strCountryCode3;

	return true;
}