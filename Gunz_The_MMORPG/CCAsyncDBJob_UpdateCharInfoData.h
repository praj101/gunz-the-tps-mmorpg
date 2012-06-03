#pragma once

#include "CCAsyncDBJob.h"

class CCAsyncDBJob_UpdateCharInfoData : public CCAsyncJob {
protected:	// Input Argument
	int			m_nCID;
	int			m_nAddedXP; 
	int			m_nAddedBP;
    int			m_nAddedKillCount;
	int			m_nAddedDeathCount;
	int			m_nAddedPlayTime;
protected:	// Output Result

public:
	CCAsyncDBJob_UpdateCharInfoData(const CCUID& uidOwner)
		: CCAsyncJob(CCASYNCJOB_UPDATECHARINFODATA, uidOwner)
	{

	}
	virtual ~CCAsyncDBJob_UpdateCharInfoData()	{}
	bool Input(const int nCID, 
			   const int nAddedXP, 
			   const int nAddedBP, 
               const int nAddedKillCount, 
			   const int nAddedDeathCount,
			   const int nAddedPlayTime);

	virtual void Run(void* pContext);
};