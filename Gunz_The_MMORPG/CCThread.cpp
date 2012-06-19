#include "stdafx.h"
#include "CCThread.h"

// CCThread class /////////////////////////////////////////////////////////////////////
CCThread::CCThread() 
{ 
	m_hThread = NULL;
	m_idThread = 0;
}

CCThread::~CCThread()
{
	if (m_hThread)
		Destroy();
}

void CCThread::Create()
{
//	OutputDebugString("<THREAD_CREATE/>\n");
	m_hThread = CreateThread(NULL, 0, ThreadProc, this, 0, &m_idThread); 
	OnCreate();
}

void CCThread::Destroy()
{
	OnDestroy();
	if (m_hThread) {
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
//	OutputDebugString("<THREAD_DESTROY/>\n");
}

DWORD WINAPI CCThread::ThreadProc(LPVOID pParam)
{
//	OutputDebugString("<MTHREAD_BEGIN>\n");
	CCThread* pThread = (CCThread*)pParam;

	pThread->Run();

//	OutputDebugString("</MTHREAD_END>\n");
	ExitThread(0);
	return (0);
}

// CCTime class ///////////////////////////////////////////////////////////////////////
DWORD CCTime::Random()
{
   register int lo, hi, test;

    hi   = seed/q;
    lo   = seed%q;

    test = a*lo - r*hi;

    if (test > 0)
	seed = test;
    else
	seed = test+ m;

    return seed;
}

int CCTime::MakeNumber(int nFrom, int nTo)
{
	if (nFrom > nTo) {
		int tmp = nFrom;
		nFrom = nTo;
		nTo = tmp;
	}
	return ((Random() % (nTo - nFrom + 1)) + nFrom);
}

void CCTime::GetTime(struct timeval *t)
{
  DWORD millisec = GetTickCount();

  t->tv_sec = (int) (millisec / 1000);
  t->tv_usec = millisec % 1000;
}

struct timeval CCTime::TimeSub(struct timeval Src1, struct timeval Src2)
{
	struct timeval null_time = {0, 0};
	struct timeval Result;

	if (Src1.tv_sec < Src2.tv_sec)
		return null_time;
	else if (Src1.tv_sec == Src2.tv_sec) {
		if (Src1.tv_usec < Src2.tv_usec)
			return null_time;
		else {
			Result.tv_sec = 0;
			Result.tv_usec = Src1.tv_usec - Src2.tv_usec;
			return Result;
		}
	} else {			/* Src->tv_sec > Src2->tv_sec */
		Result.tv_sec = Src1.tv_sec - Src2.tv_sec;
		if (Src1.tv_usec < Src2.tv_usec) {
			Result.tv_usec = Src1.tv_usec + 1000 - Src2.tv_usec;
			Result.tv_sec--;
		} else
			Result.tv_usec = Src1.tv_usec - Src2.tv_usec;
		return Result;
	}
}

struct timeval CCTime::TimeAdd(struct timeval Src1, struct timeval Src2)
{
	struct timeval Result;

	Result.tv_sec = Src1.tv_sec + Src2.tv_sec;
	Result.tv_usec = Src1.tv_usec + Src2.tv_usec;

	while (Result.tv_usec >= 1000000) {
		Result.tv_usec -= 1000000;
		Result.tv_sec++;
	}

	return Result;
}
