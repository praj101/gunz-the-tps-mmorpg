#ifndef _MASYNCDBJOB_FRIENDLIST_H
#define _MASYNCDBJOB_FRIENDLIST_H


#include "MAsyncDBJob.h"



class MAsyncDBJob_FriendList : public CCAsyncJob {
protected:
	CCUID			m_uid;
	
protected:	// Input Argument
	int				m_nCID;
protected:	// Output Result
	MMatchFriendInfo*	m_pFriendInfo;
public:
	MAsyncDBJob_FriendList(const CCUID& uid, int nCID)
		: CCAsyncJob(CCAsyncJob_FRIENDLIST, uid)
	{
		m_pFriendInfo = NULL;
		m_uid = uid;
		m_nCID = nCID;
	}
	virtual ~MAsyncDBJob_FriendList()	{}

	const CCUID& GetUID()			{ return m_uid; }
	MMatchFriendInfo* GetFriendInfo()			{ return m_pFriendInfo; }
	void SetFriendInfo(MMatchFriendInfo* pFriendInfo)	{ m_pFriendInfo = pFriendInfo; }

	virtual void Run(void* pContext);
};





#endif