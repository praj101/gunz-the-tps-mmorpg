#pragma once

#include "CCAsyncDBJob.h"

class MAsyncDBJob_InsertConnLog : public MAsyncJob {
protected:
	
protected:	// Input Argument
	unsigned long int		m_nAID;
	char 					m_szIP[64];
	string					m_strCountryCode3;
protected:	// Output Result

public:
	MAsyncDBJob_InsertConnLog(const MUID& uidOwner)
		: MAsyncJob(MASYNCJOB_INSERTCONNLOG, uidOwner)
	{

	}
	virtual ~MAsyncDBJob_InsertConnLog()	{}

	bool Input(unsigned long int nAID, char* szIP, const string& strCountryCode3 );
	virtual void Run(void* pContext);
};