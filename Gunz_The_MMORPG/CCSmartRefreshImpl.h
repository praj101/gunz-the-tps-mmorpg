#pragma once

#include "CCSmartRefresh.h"

class CCMatchObject;
class CCMatchChannel;
class CCMatchClan;


//// CCRefreshCategoryChannel ////
class CCRefreshCategoryChannelImpl : public CCRefreshCategory {
protected:
	CCMatchChannel*	m_pChannel;

protected:
	virtual bool OnUpdateChecksum(unsigned long nTick);

public:
	CCRefreshCategoryChannelImpl(CCMatchChannel* pChannel, int nCategory) : CCRefreshCategory(nCategory) 
	{
		m_pChannel = pChannel;
	}
	~CCRefreshCategoryChannelImpl()	{}

	CCMatchChannel* GetMatchChannel()				{ return m_pChannel; }
};


//// CCRefreshClientChannel ////
class CCRefreshClientChannelImpl : public CCRefreshClient {
protected:
	CCMatchObject*	m_pObject;

protected:
	virtual bool OnSync(unsigned long nChecksum);

public:
	void SetMatchObject(CCMatchObject* pObj)	{ m_pObject = pObj; }
	CCMatchObject* GetMatchObject()			{ return m_pObject; }
};


//// CCRefreshCategoryClanMember ////
class CCRefreshCategoryClanMemberImpl : public CCRefreshCategory {
protected:
	CCMatchClan*		m_pClan;

protected:
	virtual bool OnUpdateChecksum(unsigned long nTick);

public:
	CCRefreshCategoryClanMemberImpl(CCMatchClan* pClan, int nCategory) : CCRefreshCategory(nCategory) 
	{
		m_pClan = pClan;
	}
	~CCRefreshCategoryClanMemberImpl()	{}

	CCMatchClan* GetMatchClan()		{ return m_pClan; }
};


//// CCRefreshClientClanMemberImpl ////
class CCRefreshClientClanMemberImpl : public CCRefreshClient {
protected:
	CCMatchObject*	m_pObject;

protected:
	virtual bool OnSync(unsigned long nChecksum);

public:
	void SetMatchObject(CCMatchObject* pObj)	{ m_pObject = pObj; }
	CCMatchObject* GetMatchObject()			{ return m_pObject; }
};
