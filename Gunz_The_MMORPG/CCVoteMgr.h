#pragma once

#include "CCUID.h"
using namespace std;
#include <list>
#include <map>


enum MVOTE {
	MVOTE_GIVEUP,
	MVOTE_YES,
	MVOTE_NO
};


class CCVoter {
protected:
	CCUID	m_uidVoter;
public:
	CCVoter(CCUID uid)	{ m_uidVoter = uid; }
	CCUID GetID()		{ return m_uidVoter; }
};
class CCVoterMap : public map<CCUID, CCVoter*> {};


class CCVoteDiscuss {
protected:
	CCUID			m_uidStage;			// 스테이지UID
	string			m_strDiscuss;		// 안건
	unsigned long	m_nBeginTime;		// 발의된 시간
	list<CCUID>		m_YesVoterList;		// 찬성자
	list<CCUID>		m_NoVoterList;		// 반대자

public:
	CCVoteDiscuss(const CCUID& uidStage);
	virtual ~CCVoteDiscuss();

	CCUID GetStageUID()	{ return m_uidStage; }
	const char* GetDiscussName()	{ return m_strDiscuss.c_str(); }
	unsigned long GetBeginTime()	{ return m_nBeginTime; }
	size_t GetYesVoterCount()	{ return m_YesVoterList.size(); }
	size_t GetNoVoterCount()	{ return m_NoVoterList.size(); }

	bool CheckVoter(const CCUID& uid);	// 투표자인지 검사
	void Vote(const CCUID& uid, MVOTE nVote);
public:
	virtual bool OnJudge(bool bJudge) = 0;
	virtual string GetImplTarget() = 0;
};


class CCVoteMgr {
public:
	enum VOTEMGR_ERROR {
		VOTEMGR_ERROR_OK,
		VOTEMGR_ERROR_UNKNOWN,
		VOTEMGR_ERROR_VOTE_NODISCUSS,
		VOTEMGR_ERROR_VOTE_INPROGRESS,
		VOTEMGR_ERROR_VOTE_ALREADY_VOTED
	};

protected:
	CCVoterMap			m_VoterMap;
	CCVoteDiscuss*		m_pDiscuss;
	VOTEMGR_ERROR		m_nLastDiscussError;

protected:
	bool CheckDiscuss();
	void FinishDiscuss(bool bJudge);

public:
	CCVoteMgr();
	virtual ~CCVoteMgr();

	CCVoter* FindVoter(const CCUID& uid);
	void AddVoter(const CCUID& uid);
	void RemoveVoter(const CCUID& uid);	

	CCVoteDiscuss* GetDiscuss()		{ return m_pDiscuss; }
	VOTEMGR_ERROR GetLastError()	{ return m_nLastDiscussError; }
	void SetLastError(VOTEMGR_ERROR nError)	{ m_nLastDiscussError = nError; }

	bool CallVote(CCVoteDiscuss* pDiscuss);
	bool Vote(const CCUID& uid, MVOTE nVote);
	void Tick(unsigned long nClock);

	bool IsGoingOnVote() { return (0 != m_pDiscuss); }

	void StopVote( const CCUID& uidUser );
};

/*
void Client()
{
	CCVoteMgr	m_VoteMgr;

	m_VoteMgr.AddVoter( CCUID(0,1) );
	m_VoteMgr.AddVoter( CCUID(0,2) );
	m_VoteMgr.AddVoter( CCUID(0,3) );

	m_VoteMgr.CallVote(new CCVoteDiscussKick());

	m_VoteMgr.Vote(CCUID(0,1), MVOTE_YES);
	m_VoteMgr.Vote(CCUID(0,2), MVOTE_YES);
	m_VoteMgr.Vote(CCUID(0,3), MVOTE_NO);
}
*/