#pragma once

#include "CCAsyncDBJob.h"

class CCAsyncDBJob_InsertConnLog : public CCAsyncJob {
protected:
	
protected:	// Input Argument
	unsigned long int		m_nAID;
	char 					m_szIP[64];
	string					m_strCountryCode3;
protected:	// Output Result

public:
	CCAsyncDBJob_InsertConnLog(const CCUID& uidOwner)
		: CCAsyncJob(CCASYNCJOB_INSERTCONNLOG, uidOwner)
	{

	}
	virtual ~CCAsyncDBJob_InsertConnLog()	{}

	bool Input(unsigned long int nAID, char* szIP, const string& strCountryCode3 );
	virtual void Run(void* pContext);
};