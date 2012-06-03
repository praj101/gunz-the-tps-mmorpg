#pragma once

#include "CCAsyncDBJob.h"

#include "CCQuestItem.h"

class CCAsyncDBJob_CharFinalize : public CCAsyncJob {
protected:	// Input Argument
	DWORD				m_nAID;
	int					m_nCID;
	unsigned long int	m_nPlayTime;
	int					m_nConnKillCount;
	int					m_nConnDeathCount;
	int					m_nConnXP;
	int					m_nXP;
	CCQuestItemMap		m_QuestItemMap;
	CCQuestMonsterBible	m_QuestMonster;
	bool				m_bIsRequestQItemUpdate;
	
protected:	// Output Result

public:
	CCAsyncDBJob_CharFinalize(const CCUID& uidOwner)
		: CCAsyncJob(CCASYNCJOB_CHARFINALIZE, uidOwner), m_bIsRequestQItemUpdate( false )
	{
		m_nAID = 0;
	}
	virtual ~CCAsyncDBJob_CharFinalize()
	{
		m_QuestItemMap.Clear();
	}

	bool Input( DWORD nAID,
				int	nCID, 
				unsigned long int nPlayTime, 
				int nConnKillCount, 
				int nConnDeathCount, 
				int nConnXP, 
				int nXP,
				CCQuestItemMap& rfQuestItemMap,
				CCQuestMonsterBible& rfQuestMonster,
				const bool bIsRequestQItemUpdate );


	virtual void Run(void* pContext);
};