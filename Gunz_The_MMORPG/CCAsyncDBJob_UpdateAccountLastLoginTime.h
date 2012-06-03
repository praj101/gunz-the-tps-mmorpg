#pragma once

#include "CCAsyncDBJob.h"

class CCAsyncDBJob_UpdateAccountLastLoginTime : public CCAsyncJob
{
public :
	CCAsyncDBJob_UpdateAccountLastLoginTime(const CCUID& uidOwner) : CCAsyncJob( CCASYNCJOB_UPDATEACCOUNTLASTLOGINTIME, uidOwner )
	{
	}

	~CCAsyncDBJob_UpdateAccountLastLoginTime()
	{
	}

	void Input( const DWORD dwAID )
	{
		m_dwAID = dwAID;
	}

	void Run( void* pContext )
	{
		CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

		pDBMgr->UpdateAccountLastLoginTime( m_dwAID );
	}

private :
	DWORD m_dwAID;
};