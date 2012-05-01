#ifndef _CCMATCHPREMIUMIPCACHE_H
#define _CCMATCHPREMIUMIPCACHE_H


#include <map>
using namespace std;

class CCMatchPremiumIPNode
{
private:
	DWORD	m_dwIP;
	DWORD	m_tmTime;	// Last updated time
public:
	CCMatchPremiumIPNode(DWORD dwIP, DWORD tmTime) { m_dwIP=dwIP; m_tmTime=tmTime; }
	DWORD GetIP()			{ return m_dwIP; }
	void SetIP(DWORD dwIP)	{ m_dwIP = dwIP; }
	DWORD GetTime()			{ return m_tmTime; }
	void SetTime(DWORD tmTime)	{ m_tmTime = tmTime; }
};

class CCMatchPremiumIPMap : public map<DWORD, CCMatchPremiumIPNode>{};

class CCMatchPremiumIPCache
{
private:
	CRITICAL_SECTION			m_csLock;
	CCMatchPremiumIPMap			m_PremiumIPMap;
	CCMatchPremiumIPMap			m_NotPremiumIPMap;
	int							m_nDBFailedCount;
	int							m_nFailedCheckCount;

	void Lock()		{ EnterCriticalSection(&m_csLock); }
	void Unlock()	{ LeaveCriticalSection(&m_csLock); }
public:
	CCMatchPremiumIPCache();
	~CCMatchPremiumIPCache();
	static CCMatchPremiumIPCache* GetInstance();

	bool CheckPremiumIP(DWORD dwIP, bool& outIsPremiumIP);
	void AddIP(DWORD dwIP, bool bPremiumIP);
	void OnDBFailed();
	void Update();
};

inline CCMatchPremiumIPCache* MPremiumIPCache()
{
	return CCMatchPremiumIPCache::GetInstance();
}




#endif