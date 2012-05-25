#pragma once

#include "CCAsyncDBJob.h"

class CCAsyncDBJob_UpdateCharBRInfo : public CCAsyncJob 
{
protected:	// Input Value
	int m_nCID;

	int m_nBRID;
	int m_nBRTID;

	int m_nRewardCount;

	int m_nBattleTime;
	int m_nKillCount;

public:
	CCAsyncDBJob_UpdateCharBRInfo(const CCUID& uidOwner) : 
	  CCAsyncJob(CCASYNCJOB_UPDATE_CHAR_BR_INFO, uidOwner) {}

	virtual ~CCAsyncDBJob_UpdateCharBRInfo() {}

	bool Input(int nCID, int nBRID, int nBRTID, int nRewardCount, int nBattleTime, int nKillCount);

	virtual void Run(void* pContext);

	int GetCID()			{ return m_nCID; }

	int GetBRID()			{ return m_nBRID; }
	int GetBRTID()			{ return m_nBRTID; }

	int GetRewardCount()	{ return m_nRewardCount; }

	int GetBattleTime()		{ return m_nBattleTime; }
	int GetKillCount()		{ return m_nKillCount; }
};
