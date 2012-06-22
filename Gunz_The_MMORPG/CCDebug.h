#pragma once

#include <Windows.h>
#include <crtdbg.h>

#define CCASSERT(x) _ASSERT(x)

#define CCLOGSTYLE_FILE 0x0001
#define CCLOGSTYLE_DEBUGSTRING 0x0002

#define CCLOG_DEFAULT_HISTORY_COUNT	10

#define cclog CCLog

void InitLog(int logmethodflags=CCLOGSTYLE_DEBUGSTRING, const char* pszLogFileName="system/cclog.txt");
void __cdecl CCLog(const char *pFormat,...);
const char* CCGetLogFileName();

#ifdef _WIN32
	void __cdecl CCMsg(const char *pFormat,...);
#endif

DWORD CCFilterException(LPEXCEPTION_POINTERS p);

void CCInstallSEH();

void CCInitProfile();
void CCBeginProfile(int nIndex,const char *szName);
void CCEndProfile(int nIndex);
void CCSaveProfile(const char *file);