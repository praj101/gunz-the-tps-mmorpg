#pragma once

#include "CCAsyncDBJob.h"

class CCAsyncDBJob_InsertGamePlayerLog : public CCAsyncJob 
{
protected: // Input Argument
	int m_nGameLogID;
	int m_nCID;
	int m_nPlayTime;
	int m_nKills;
	int m_nDeaths;
	int m_nXP;
	int m_nBP;

protected:	// Output Result

public:
	CCAsyncDBJob_InsertGamePlayerLog() : CCAsyncJob(CCASYNCJOB_INSERTGAMEPLAYERLOG, CCUID(0, 0)) {}
	virtual ~CCAsyncDBJob_InsertGamePlayerLog() {}

	bool Input(int nGameLogID, int nCID, int nPlayTime, int nKills, int nDeaths, int nXP, int nBP);

	virtual void Run(void* pContext);
};
