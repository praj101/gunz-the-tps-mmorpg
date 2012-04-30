#include "stdafx.h"
#include "CCMatchPremiumIPCache.h"

#define MAX_PREMIUMIP_CACHE_DBFAILED		5
#define MAX_PREMIUMIP_CACHE_FAILEDCHECK		100
#define MAX_PREMIUMIP_CACHE_TICK			10000	// ms , 10 Sec
#define MAX_PREMIUMIP_CACHE_TIMEOUT			3600000	// ms , 1 Hour

CCMatchPremiumIPCache::CCMatchPremiumIPCache() : m_nDBFailedCount(0), m_nFailedCheckCount(0)
{
	InitializeCriticalSection(&m_csLock);
}

CCMatchPremiumIPCache::~CCMatchPremiumIPCache()
{
	DeleteCriticalSection(&m_csLock);
}

CCMatchPremiumIPCache* CCMatchPremiumIPCache::GetInstance()
{
	static CCMatchPremiumIPCache m_stPremiumIPCache;
	return &m_stPremiumIPCache;
}


bool CCMatchPremiumIPCache::CheckPremiumIP(DWORD dwIP, bool& outIsPremiumIP)
{
	CCMatchPremiumIPMap::iterator itor;
	bool bExist = false;
	bool bPremiumIP = false;

	Lock(); //////////////////////////////////////

	// 먼저 프리미엄IP가 아닌 세트에서 검사
	itor = m_NotPremiumIPMap.find(dwIP);
	if (itor != m_NotPremiumIPMap.end())
	{
		bPremiumIP = false;
		bExist = true;
	}
	else
	{
		// 프리미엄IP 세트에서 검사
		itor = m_PremiumIPMap.find(dwIP);
		if (itor != m_PremiumIPMap.end())
		{
			bPremiumIP = true;
			bExist = true;
		}
	}

	// 만약 DB가 멈춰져있는 상태라면 캐쉬에 있는거라고 간주하고 무조건 false를 반환한다.
	if ((m_nDBFailedCount > MAX_PREMIUMIP_CACHE_DBFAILED) && (m_nFailedCheckCount < MAX_PREMIUMIP_CACHE_FAILEDCHECK))
	{
		++m_nFailedCheckCount;
		bPremiumIP = false;
		bExist = true;
	}
	else
	{
		m_nFailedCheckCount = 0;
	}


	Unlock(); ////////////////////////////////////

	if (bExist) outIsPremiumIP = bPremiumIP;
	

	return bExist;
}

void CCMatchPremiumIPCache::AddIP(DWORD dwIP, bool bPremiumIP)
{
	Lock(); ///////////////////

	// db 검사후 IP를 추가하는 것이니 DB가 살아있다는 증거
	m_nDBFailedCount = 0;

	CCMatchPremiumIPNode node(dwIP, timeGetTime());

	if (bPremiumIP)
	{
		m_PremiumIPMap.insert(CCMatchPremiumIPMap::value_type(dwIP, node));
	}
	else
	{
		m_NotPremiumIPMap.insert(CCMatchPremiumIPMap::value_type(dwIP, node));
	}

	Unlock(); /////////////////
}


void CCMatchPremiumIPCache::OnDBFailed()
{
	Lock(); ///////////////////

	if (m_nDBFailedCount > MAX_PREMIUMIP_CACHE_DBFAILED) m_nDBFailedCount = 0;
	++m_nDBFailedCount;
	m_nFailedCheckCount = 0;

	Unlock(); /////////////////
}

void CCMatchPremiumIPCache::Update()
{
	static DWORD tmLastUpdate = 0;
	DWORD tmNow = timeGetTime();
	if (tmNow - tmLastUpdate < MAX_PREMIUMIP_CACHE_TICK)
	{
		return;
	}
	else
	{
		tmLastUpdate = tmNow;
	}

	Lock(); ///////////////////
	for (CCMatchPremiumIPMap::iterator i=m_PremiumIPMap.begin(); i!=m_PremiumIPMap.end();)
	{
		if (tmNow - ((*i).second).GetTime() >= MAX_PREMIUMIP_CACHE_TIMEOUT)
		{
			i=m_PremiumIPMap.erase(i);
			continue;
		}
		else
		{
			i++;
			continue;
		}
	}

	for (CCMatchPremiumIPMap::iterator i=m_NotPremiumIPMap.begin(); i!=m_NotPremiumIPMap.end();)
	{
		if (tmNow - ((*i).second).GetTime() >= MAX_PREMIUMIP_CACHE_TIMEOUT)
		{
			i=m_NotPremiumIPMap.erase(i);
			continue;
		}
		else
		{
			i++;
			continue;
		}
	}
	Unlock(); /////////////////
}
