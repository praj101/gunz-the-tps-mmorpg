#ifndef _MASYNCDBJOB_FRIENDLIST_H
#define _MASYNCDBJOB_FRIENDLIST_H


#include "CCAsyncDBJob.h"



class CCAsyncDBJob_FriendList : public CCAsyncJob {
protected:
	CCUID			m_uid;
	
protected:	// Input Argument
	int				m_nCID;
protected:	// Output Result
	CCMatchFriendInfo*	m_pFriendInfo;
public:
	CCASYNCJOB_FRIENDLIST(const CCUID& uid, int nCID)
		: CCAsyncJob(CCASYNCJOB_FRIENDLIST, uid)
	{
		m_pFriendInfo = NULL;
		m_uid = uid;
		m_nCID = nCID;
	}
	virtual ~CCAsyncDBJob_FriendList()	{}

	const CCUID& GetUID()			{ return m_uid; }
	CCMatchFriendInfo* GetFriendInfo()			{ return m_pFriendInfo; }
	void SetFriendInfo(CCMatchFriendInfo* pFriendInfo)	{ m_pFriendInfo = pFriendInfo; }

	virtual void Run(void* pContext);
};





#endif