#pragma once

/////////////////////////////////////////////////////////////////////////
// Purpose		: Blocking 작업을 여러 쓰레드로 나눠 Async 스럽게 굴린다.
// Last Update	: 2004-02-04 

#pragma warning(disable:4786)
#include <list>
#include <algorithm>
using namespace std;


enum CCASYNC_RESULT {
	CCASYNC_RESULT_SUCCEED,
	CCASYNC_RESULT_FAILED,
	CCASYNC_RESULT_TIMEOUT
};


class CCAsyncJob {
protected:
	int				m_nJobID;	// Job Type ID


	unsigned long	m_nPostTime;
	unsigned long	m_nFinishTime;

	CCASYNC_RESULT	m_nResult;

	CCUID			m_uidOwner;

public:
	CCAsyncJob(int nJobID, const CCUID& uidOwner) {
		m_nJobID = nJobID;
		m_uidOwner = uidOwner;
		m_nPostTime = 0;
		m_nFinishTime = 0;
	}
	virtual ~CCAsyncJob()	{}

	int GetJobID()							{ return m_nJobID; }
	const CCUID& GetOwnerUID()				{ return m_uidOwner; }
	unsigned long GetPostTime()				{ return m_nPostTime; }
	void SetPostTime(unsigned long nTime)	{ m_nPostTime = nTime; }
	unsigned long GetFinishTime()			{ return m_nFinishTime; }
	void SetFinishTime(unsigned long nTime)	{ m_nFinishTime = nTime; }

	CCASYNC_RESULT GetResult()				{ return m_nResult; }
	void SetResult(CCASYNC_RESULT nResult)	{ m_nResult = nResult; }

	virtual void Run(void* pContext) = 0;
};

class CCAsyncJobList : protected list<CCAsyncJob*> {
protected:
	CRITICAL_SECTION	m_csLock;
public:
	CCAsyncJobList()				{ InitializeCriticalSection(&m_csLock); }
	virtual ~CCAsyncJobList()	{ DeleteCriticalSection(&m_csLock); }

	void Lock()		{ EnterCriticalSection(&m_csLock); }
	void Unlock()	{ LeaveCriticalSection(&m_csLock); }

	// Unsafe Methods /////////////////////////////////////////////
	// ~Unsafe() 용도 이외엔 사용금지
	CCAsyncJobList::iterator GetBeginItorUnsafe()	{ return begin(); }
	CCAsyncJobList::iterator GetEndItorUnsafe()	{ return end(); }

	void AddUnsafe(CCAsyncJob* pJob) {
		push_back(pJob);
	}
	void RemoveUnsafe(CCAsyncJob* pJob, CCAsyncJobList::iterator* itorOut) {
		iterator i = find(begin(), end(), pJob);
		if (i != end()) {
			iterator itorTmp = erase(i);
			if (itorOut)
				*itorOut = itorTmp;
		}
	}
	CCAsyncJob* GetJobUnsafe() {
		if (begin() == end()) return NULL;
		CCAsyncJob* pReturn = *begin();
		pop_front();
		return pReturn;
	}
	int GetCount() { return (int)size(); }
};

#define MAX_THREADPOOL_COUNT 10

class CCAsyncProxy {
protected:
	HANDLE				m_hEventShutdown;
	HANDLE				m_hEventFetchJob;

	int					m_nThreadCount;
	HANDLE				m_ThreadPool[MAX_THREADPOOL_COUNT];

	CCAsyncJobList		m_WaitQueue;
	CCAsyncJobList		m_ResultQueue;

	CRITICAL_SECTION	m_csCrashDump;
	
protected:
	HANDLE GetEventShutdown()	{ return m_hEventShutdown; }
	HANDLE GetEventFetchJob()	{ return m_hEventFetchJob; }

	static DWORD WINAPI WorkerThread(LPVOID pJobContext);
	void OnRun();

public:
	CCAsyncProxy();
	virtual ~CCAsyncProxy();
	bool Create(int nThreadCount);
	void Destroy();
	
	int GetWaitQueueCount()		{ return m_WaitQueue.GetCount(); }
	int GetResultQueueCount()	{ return m_ResultQueue.GetCount(); }

	void PostJob(CCAsyncJob* pJob);
	CCAsyncJob* GetJobResult()	{
		CCAsyncJob* pJob = NULL;
		m_ResultQueue.Lock();
			pJob = m_ResultQueue.GetJobUnsafe();
		m_ResultQueue.Unlock();
		return pJob;
	}

	DWORD CrashDump( PEXCEPTION_POINTERS ExceptionInfo );
};
