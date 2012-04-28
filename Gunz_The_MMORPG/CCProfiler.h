#pragma once
#pragma warning( disable : 4786 )

#include <stack>
#include <list>
using namespace std;

#define CCPROFILE_ITEM_NAME_LENGTH	64

// One Profile Item
struct CCPROFILEITEM{
	char	szName[CCPROFILE_ITEM_NAME_LENGTH];
	int		nStartTime;
	int		nEndTime;
};

// Accumulated Profile Log
struct CCPROFILELOG{
	char	szName[CCPROFILE_ITEM_NAME_LENGTH];
	int		nCount;
	int		nDepth;
	int		nTotalTime;
	int		nMaxTime;
	int		nMinTime;
};

// Temporary Profile Call Stack
class CCProfileStack : public stack<CCPROFILEITEM*>{
public:
	virtual ~CCProfileStack(void);
};

// One Loop Log
class CCProfileLoop : public list<CCPROFILELOG*>{
public:
	/*
	void AddProfile(char* szName, int nDepth);
	void SetProfile(int nTime);
	*/
	void AddProfile(CCPROFILELOG* pPL);

	int GetTotalTime(void);
};


// Profiler
class CCProfiler{
protected:
	CCProfileStack	m_ProfileStack;
	CCProfileLoop	m_ProfileLoop;

	bool			m_bEnableOneLoopProfile;
	CCProfileLoop*	m_pOneLoopProfile;
	CCProfileLoop*	m_pOneLoopProfileResult;

	char*			m_szFirstProfileName;		// 맨처음 시작한 프로파일 이름 ( Depth 0 )

public:
	CCProfiler(void);
	virtual ~CCProfiler(void);

	void BeginProfile(char* szProfileName);
	void EndProfile(char* szProfileName);

	bool FinalAnalysis(char* szFileName);

	int GetTotalTime(void);

	// One Loop Profiling
	void EnableOneLoopProfile(bool bEnable);
	bool IsOneLoopProfile(void);
	// if not enabled, return NULL
	CCProfileLoop* GetOneLoopProfile(void);

	// Accumulated Profile Result
	CCProfileLoop* GetProfile(void);
};


// Global Profiler
extern CCProfiler	g_DefaultProfiler;

// Instance 형태의 Profile
class CCProfileInstance{
	char	m_szProfileName[CCPROFILE_ITEM_NAME_LENGTH];
public:
	CCProfileInstance(char* szProfileName){
		g_DefaultProfiler.BeginProfile(szProfileName);
		strcpy(m_szProfileName, szProfileName);
	}
	virtual ~CCProfileInstance(void){
		g_DefaultProfiler.EndProfile(m_szProfileName);
	}
};


#ifdef _DO_NOT_USE_PROFILER

#define BEGINPROFILE(_szProfileName)	;
#define ENDPROFILE(_szProfileName)		;
#define FINALANALYSIS(_szFileName)		;
#define RUNWITHPROFILER(_Function)		_Function;
#define ENABLEONELOOPPROFILE(_bEnable)	;
#define GETONELOOPPROFILE()	0;
#define GETPROFILE();
#define PROFILEINSTANCE(_szProfileName)	;

#else

// Macro for Global Profiler
#define BEGINPROFILE(_szProfileName)	g_DefaultProfiler.BeginProfile(_szProfileName)
#define ENDPROFILE(_szProfileName)		g_DefaultProfiler.EndProfile(_szProfileName)
#define FINALANALYSIS(_szFileName)		g_DefaultProfiler.FinalAnalysis(_szFileName)
#define RUNWITHPROFILER(_Function)		BEGINPROFILE(#_Function); _Function; ENDPROFILE(#_Function);
#define ENABLEONELOOPPROFILE(_bEnable)	g_DefaultProfiler.EnableOneLoopProfile(_bEnable)
#define GETONELOOPPROFILE()				g_DefaultProfiler.GetOneLoopProfile()
#define GETPROFILE()					g_DefaultProfiler.GetProfile()

#define PROFILEINSTANCE(_szProfileName)	CCProfileInstance __ProfileInstance(_szProfileName);

#endif

// Simple Macro
#define _BP(_szProfileName)				BEGINPROFILE(_szProfileName)
#define _EP(_szProfileName)				ENDPROFILE(_szProfileName)
#define _RP(_Function)					RUNWITHPROFILER(_Function)
#define _FA(_szFileName)				FINALANALYSIS(_szFileName)
#define _PI(_szProfileName)				PROFILEINSTANCE(_szProfileName)


#endif
