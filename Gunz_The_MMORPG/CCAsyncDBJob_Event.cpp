#include "stdafx.h"
#include "CCAsyncDBJob_Event.h"


bool CCAsyncDBJob_EventLog::Input( AsyncEventObjVec& vEventObj, const string& strEventName, const string& strAnnounce )
{
	if( vEventObj.empty() ) 
		return false;

	m_vEventObj.swap( vEventObj );
	m_strEventName = strEventName;
	m_strAnnounce = strAnnounce;

	return true;
}


void CCAsyncDBJob_EventLog::Run( void* pContext )
{
	CCMatchDBMgr* pDBMgr = reinterpret_cast< CCMatchDBMgr* >( pContext );
	if( 0 == pDBMgr ) 
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	AsyncEventObjVec::iterator	it, end;

	end = m_vEventObj.end();
	for( it = m_vEventObj.begin(); it != end; ++it )
	{
		if( !pDBMgr->InsertEvent(it->dwAID, it->dwCID, m_strEventName) )
			it->uidUser = CCUID(0, 0);
	}
	
	SetResult(CCASYNC_RESULT_SUCCEED);
}