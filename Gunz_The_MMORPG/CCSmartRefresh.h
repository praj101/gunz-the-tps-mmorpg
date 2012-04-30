#ifndef _MSMARTREFRESH_H
#define _MSMARTREFRESH_H

//#pragma once

#include <map>
#include <string>
using namespace std;


class CCRefreshCategory {
protected:
	int				m_nCategory;
	unsigned long	m_nChecksum;

	unsigned long	m_nLastUpdateTick;

protected:

	void SetChecksum(unsigned long nChecksum)	{ m_nChecksum = nChecksum; }

	unsigned long GetLastUpdateTick()			{ return m_nLastUpdateTick; }
	void SetLastUpdateTick(unsigned long nTick)	{ m_nLastUpdateTick = nTick; }
	
	virtual bool OnUpdateChecksum(unsigned long nTick) = 0;

public:
	CCRefreshCategory(int nCategory);
	virtual ~CCRefreshCategory();

	int GetCategory()							{ return m_nCategory; }
	unsigned long GetChecksum()					{ return m_nChecksum; }

	inline bool UpdateChecksum(unsigned long nTick);
};
class CCRefreshCategoryMap : public map<int, CCRefreshCategory*>{};


class CCRefreshClient {
protected:
	int				m_nCategory;
	unsigned long	m_nChecksum;
	bool			m_bEnable;
	unsigned long	m_tmLastUpdated;

protected:
	virtual bool OnSync(unsigned long nChecksum) = 0;

public:
	CCRefreshClient();
	virtual ~CCRefreshClient();
	
	int GetCategory()								{ return m_nCategory; }
	void SetCategory(int nCategory)					{ m_nCategory = nCategory; }

	unsigned long GetChecksum()						{ return m_nChecksum; }
	void SetChecksum(unsigned long nChecksum)		{ m_nChecksum = nChecksum; }

	bool IsEnable()									{ return m_bEnable; }
	void Enable(bool bEnable)						{ m_bEnable = bEnable; }

	unsigned long GetLastUpdatedTime()				{ return m_tmLastUpdated; }
	void SetLastUpdatedTime(unsigned long tmTime)	{ m_tmLastUpdated = tmTime; }

	bool Sync(unsigned long nChecksum);
};


class CCSmartRefresh {
protected:
	CCRefreshCategoryMap		m_CategoryMap;

public:
	CCSmartRefresh();
	virtual ~CCSmartRefresh();
	void Clear();

	CCRefreshCategory* GetCategory(int nCategory);
	void AddCategory(CCRefreshCategory* pCategory);
	void UpdateCategory(unsigned int nTick);

//	void AddClient(CCRefreshClient* pClient);
//	void RemoveClient(CCRefreshClient* pClient);
	bool SyncClient(CCRefreshClient* pClient);	// Refresh되면:True, 변동없으면:False
};


#endif