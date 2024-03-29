#pragma once
#include "CCDuelTournamentMatchMaker.h"
#include "CCDuelTournamentTimeChecker.h"

#define MAX_DT_GROUP_RANKING_COUNT 100

class CCDuelTournamentMatchLauncher;

//////////////////////////////////////////////////////////////////////
// Duel Tournament의 관리 클래스이다.
// 역할 1. 듀얼 토너먼트 Match-Up을 시켜주는 MatchMaker 기능
// 역할 2. 듀얼 토너먼트의 Group Ranking을 관리해주는 기능(정해진 시간마다 DB에서 받아오는 기능)
//////////////////////////////////////////////////////////////////////

class CCMatchDuelTournamentMgr
{
	CCMatchObjectContainer m_matchObjectContainer;
	CCDuelTournamentTimeChecker m_TimeChecker;

	DWORD m_lastMatchedTick;		// 마지막으로 매치 처리한 시각
	DWORD m_nLastTimeCheckedTick;	// 마지막으로 TimeChecker를 실행한 시각
	bool m_bIsServiceTime;			// 서비스 시간에 대한 로그를 남기기 위해

protected:	
	DTRankingInfo m_GroupRankingBlob[MAX_DT_GROUP_RANKING_COUNT];
	CCDuelTournamentMatchMaker m_DTMatchMakers[CCDUELTOURNAMENTTYPE_MAX];
	CCDuelTournamentMatchLauncher* m_pDTMatchLauncher;

	void ClearGroupRanking();
	void LaunchMatch(CCDUELTOURNAMENTTYPE nType, CCDuelTournamentPickedGroup& vecUidPlayer);
public:
	CCMatchDuelTournamentMgr();
	~CCMatchDuelTournamentMgr();

	void Init();
	void Destory();

	bool AddPlayer(CCDUELTOURNAMENTTYPE nType, CCUID &uidPlayer);
	bool RemovePlayer(CCDUELTOURNAMENTTYPE nType, CCUID &uidPlayer);
	
	void Tick(unsigned long nTick);

	void AddGroupRanking(list<DTRankingInfo*>* pRankingList);

	void  SetTimeStamp(const char* szTimeStamp)		{ m_TimeChecker.SetTimeStamp(szTimeStamp); }
	char* GetTimeStamp()							{ return m_TimeChecker.GetTimeStamp(); }
	bool  IsSameTimeStamp(const char* szTimeStamp)  { return m_TimeChecker.IsSameTimeStamp(szTimeStamp); }

	bool GetTimeStampChanged()				{ return m_TimeChecker.GetTimeStampChanged(); }
	void SetTimeStampChanged(bool bValue)	{ m_TimeChecker.SetTimeStampChanged(bValue); }
};


// CCMatchDuelTournamentMgr는 이 클래스를 이용해서 MatchMaker가 뽑아낸 그룹을 실제로 경기시킨다
class CCDuelTournamentMatchLauncher
{
	DWORD m_dwLimitUserWaitTime;
	DWORD m_dwAcceptableTpGap;

public:
	CCDuelTournamentMatchLauncher() : m_dwLimitUserWaitTime(10000), m_dwAcceptableTpGap(10) {}

	void SetLimitUserWaitTime(DWORD n) { m_dwLimitUserWaitTime = n; }
	void SetAcceptableTpGap(DWORD n) { m_dwAcceptableTpGap = n; }

	void LaunchAvailableMatch(CCDUELTOURNAMENTTYPE nType, CCDuelTournamentMatchMaker& matchMaker, DWORD nCurTick);

protected:
	void LaunchMatchGroups(CCDUELTOURNAMENTTYPE nType, CCDuelTournamentPickedGroup& vecUidPlayer, CCDUELTOURNAMENTMATCHMAKINGFACTOR matchFactor);

	// 실제 CCMatchServer에게 경기 실행시키는 부분을 래핑하는 함수 (단위테스트 목적)
	virtual void LaunchMatch(CCDUELTOURNAMENTTYPE nType, CCDuelTournamentPickedGroup* pPickedGroup, CCDUELTOURNAMENTMATCHMAKINGFACTOR matchFactor);
};