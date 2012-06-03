#pragma once

#include <vector>
#include <string>

using std::vector;
using std::string;


#include "CCAsyncDBJob.h"
#include "CCUID.h"


struct AsyncEventObj
{
	CCUID  uidUser;
	DWORD dwAID;
	DWORD dwCID;
};

typedef vector< AsyncEventObj > AsyncEventObjVec;


class CCAsyncDBJob_EventLog : public CCAsyncJob
{
public :
	CCAsyncDBJob_EventLog() : 
	   CCAsyncJob( CCASYNCJOB_PROBABILITYEVENTPERTIME, CCUID(0, 0) ) {}

	~CCAsyncDBJob_EventLog() {}
   
	const AsyncEventObjVec& GetEventObjList()	{ return m_vEventObj; }
	const string& GetEventName()				{ return m_strEventName; }
	const string& GetAnnounce()					{ return m_strAnnounce; }

	bool Input( AsyncEventObjVec& vEventObj, const string& strEventName, const string& strAnnounce );
	void Run( void* pContext );

private :
	AsyncEventObjVec	m_vEventObj;
	string				m_strEventName;
	string				m_strAnnounce;
};