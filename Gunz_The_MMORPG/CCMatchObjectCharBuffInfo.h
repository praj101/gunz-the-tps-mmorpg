#pragma once
#include "CCMatchBuff.h"
#include "CCMatchBuffSummary.h"

class CCMatchObject;
class CCMatchObjectCharBuff
{
protected:
	static CCUID				m_uidGenerate;
	static CCCriticalSection	m_csUIDGenerateLock;

public:
	static CCUID UseUID() 
	{
		m_csUIDGenerateLock.Lock();
		{
			m_uidGenerate.Increase();	
		}		
		m_csUIDGenerateLock.Unlock();
		return m_uidGenerate;
	}

protected:
	CCMatchObject* m_pObj;
	unsigned int m_nLastCheckBuffInfoTime;

public:
	void SetObject(CCMatchObject* pObj)					{ m_pObj = pObj; }
	CCMatchObject* GetObject()							{ return m_pObj; }

	unsigned int GetLastCheckBuffInfoTime()				{ return m_nLastCheckBuffInfoTime; }
	void SetLastCheckBuffInfoTime(unsigned int nVal)	{ m_nLastCheckBuffInfoTime = nVal; }

protected:
	CCMatchShortBuffMap m_ShortBuffInfoMap;

	CCMatchBuffSummary m_BuffSummary;

public:
	bool ApplyShortBuffInfo(int nBuffID, int nBuffSecondPeriod);
	void DeleteShortBuffInfo(CCUID& uidBuff);	

public:
	CCMatchObjectCharBuff();
	~CCMatchObjectCharBuff();	
	
	void FreeCharBuffInfo();

	bool Tick(int nGlobalTick);	

	void MakeBuffSummary();
	CCMatchBuffSummary*	GetBuffSummary()			{ return &m_BuffSummary;	 }
	CCMatchShortBuffMap* GetShortBuffInfoMap()		{ return &m_ShortBuffInfoMap;}
};
