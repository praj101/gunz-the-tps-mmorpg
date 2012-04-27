#pragma once

#include <Windows.h>
#include <crtdbg.h>

#define MASSERT(x) _ASSERT(x)

#define MLOGSTYLE_FILE 0x0001
#define MLOGSTYLE_DEBUGSTRING 0x0002

#define MLOG_DEFAULT_HISTORY_COUNT	10

#define mlog MLog

void InitLog(int logmethodflags=MLOGSTYLE_DEBUGSTRING, const char* pszLogFileName="mlog.txt");
void __cdecl MLog(const char *pFormat,...);
const char* CCGetLogFileName();

#ifdef _WIN32
	void __cdecl MMsg(const char *pFormat,...);
#endif

DWORD CCFilterException(LPEXCEPTION_POINTERS p);

void CCInstallSEH();

void CCInitProfile();
void CCBeginProfile(int nIndex,const char *szName);
void CCEndProfile(int nIndex);
void CCSaveProfile(const char *file);