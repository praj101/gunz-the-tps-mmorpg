#pragma once

#include "CCAsyncDBJob.h"

class CCAsyncDBJob_GetCharBRInfo : public CCAsyncJob 
{
protected:	// Input Value
	int m_nCID;

	int m_nBRID;
	int m_nBRTID;

	CCMatchCharBRInfo m_CharBRInfo;

public:
	CCAsyncDBJob_GetCharBRInfo(CCUID& uidOwner) : 
	  CCAsyncJob(CCASYNCJOB_GET_CHAR_BR_INFO, uidOwner) {}

	virtual ~CCAsyncDBJob_GetCharBRInfo() {}

	bool Input(int nCID, int nBRID, int nBRTID);

	virtual void Run(void* pContext);	

	int GetCID()	{ return m_nCID; }
	int GetBRID()	{ return m_nBRID;}

	CCMatchCharBRInfo& GetCharBRInfo() { return m_CharBRInfo; }
};
