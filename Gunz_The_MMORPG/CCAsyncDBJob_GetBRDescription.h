#pragma once

#include "CCAsyncDBJob.h"

class CCAsyncDBJob_GetBattleTimeRewardDescription : public CCAsyncJob 
{
protected:	// Output Result
	vector<CCMatchBRDescription*>	m_vBattletimeRewardDescription;
	vector<CCMatchBRItem*>			m_vBattletimeRewardItem;

public:
	CCAsyncDBJob_GetBattleTimeRewardDescription() : CCAsyncJob(CCASYNCJOB_GET_BR_DESCRIPTION, CCUID(0, 0)) {}
	virtual ~CCAsyncDBJob_GetBattleTimeRewardDescription() 
	{
		m_vBattletimeRewardDescription.clear();
		m_vBattletimeRewardItem.clear();
	}

	virtual void Run(void* pContext);

	vector<CCMatchBRDescription*>& GetBattleTimeRewardDescription()	{ return m_vBattletimeRewardDescription; }
	vector<CCMatchBRItem*>& GetBattleTimeRewardItem()					{ return m_vBattletimeRewardItem; }
};

