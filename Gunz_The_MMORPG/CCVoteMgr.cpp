#include "stdafx.h"
#include "CCVoteMgr.h"
#include <algorithm>
#include "CCMatchServer.h"
#include "CCSharedCommandTable.h"



/////////////////////////////////////////////////
// CCVoteDiscuss
CCVoteDiscuss::CCVoteDiscuss(const CCUID& uidStage)
{
	m_uidStage = uidStage;
	m_nBeginTime = CCMatchServer::GetInstance()->GetGlobalClockCount();
}

CCVoteDiscuss::~CCVoteDiscuss()
{
	//while(m_YesVoterList.size()) {
	//	m_YesVoterList.pop_front();
	//}
	//while(m_NoVoterList.size()) {
	//	m_NoVoterList.pop_front();
	//}

	m_YesVoterList.clear();
	m_NoVoterList.clear();
}

bool CCVoteDiscuss::CheckVoter(const CCUID& uid)
{
	list<CCUID>::iterator iYes = find(m_YesVoterList.begin(), m_YesVoterList.end(), uid);
	if (iYes!=m_YesVoterList.end())
		return true;

	list<CCUID>::iterator iNo = find(m_NoVoterList.begin(), m_NoVoterList.end(), uid);
	if (iNo!=m_NoVoterList.end())
		return true;

	return false;
}

void CCVoteDiscuss::Vote(const CCUID& uid, MVOTE nVote)
{
	if (CheckVoter(uid))
		return;		// already voted

	if (nVote == MVOTE_YES) {
		m_YesVoterList.push_back(uid);
	} else if (nVote == MVOTE_NO) {
		m_NoVoterList.push_back(uid);
	} else {
		_ASSERT("NEVER HAPPEND");
		return;
	}
}

/////////////////////////////////////////////////
// CCVoteMgr
CCVoteMgr::CCVoteMgr()
{
	m_pDiscuss = NULL;
}

CCVoteMgr::~CCVoteMgr()
{
	if (GetDiscuss()) {
		delete GetDiscuss();
		m_pDiscuss = NULL;
	}

	m_VoterMap.clear();
}

bool CCVoteMgr::CheckDiscuss()
{
	CCVoteDiscuss* pDiscuss = GetDiscuss();
	if (pDiscuss == NULL)
		return false;

	int nYesCount = (int)pDiscuss->GetYesVoterCount();
	int nNoCount = (int)pDiscuss->GetNoVoterCount();

//	char szLog[128]="";
//	sprintf(szLog, "VOTERESULT: Y(%f), N(%f)", (float)nYesCount, (float)m_VoterMap.size() * 0.5f);
//	CCMatchServer::GetInstance()->LOG(CCMatchServer::LOG_PROG, szLog);

	if ( (float)nYesCount > (float)m_VoterMap.size() * 0.66f )	// 2/3이상 찬성하면.
		return true;
	else
		return false;
}

void CCVoteMgr::FinishDiscuss(bool bJudge)
{
	if (GetDiscuss()) {
		MCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_MATCH_NOTIFY_VOTERESULT, CCUID(0,0));
		pCmd->AddParameter(new MCmdParamStr(GetDiscuss()->GetDiscussName()));
		pCmd->AddParameter(new MCmdParamInt(bJudge?1:0));
		CCMatchServer::GetInstance()->RouteToStage(GetDiscuss()->GetStageUID(), pCmd);

		GetDiscuss()->OnJudge(bJudge);

		delete GetDiscuss();
		m_pDiscuss = NULL;
	}
	SetLastError(VOTEMGR_ERROR_OK);
}

CCVoter* CCVoteMgr::FindVoter(const CCUID& uid)
{
	CCVoterMap::iterator i = m_VoterMap.find(uid);
	if (i != m_VoterMap.end())
		return (*i).second;
	else
		return NULL;
}

void CCVoteMgr::AddVoter(const CCUID& uid)
{
	if (FindVoter(uid) != NULL) {
		SetLastError(VOTEMGR_ERROR_OK);
		return;
	}

	CCVoter* pVoter = new CCVoter(uid);
	m_VoterMap.insert(CCVoterMap::value_type(uid, pVoter));

	SetLastError(VOTEMGR_ERROR_OK);
}

void CCVoteMgr::RemoveVoter(const CCUID& uid)
{
	CCVoterMap::iterator i = m_VoterMap.find(uid);
	if (i != m_VoterMap.end()) {
		CCVoter* pVoter = (*i).second;
		delete pVoter;
		m_VoterMap.erase(i);
	}

	SetLastError(VOTEMGR_ERROR_OK);
}

bool CCVoteMgr::CallVote(CCVoteDiscuss* pDiscuss)
{
	if (GetDiscuss()) {
		SetLastError(VOTEMGR_ERROR_VOTE_INPROGRESS);
		return false;
	}

	m_pDiscuss = pDiscuss;

	SetLastError(VOTEMGR_ERROR_OK);
	return true;
}

bool CCVoteMgr::Vote(const CCUID& uid, MVOTE nVote)
{
	if (GetDiscuss() == NULL) {
		SetLastError(VOTEMGR_ERROR_VOTE_NODISCUSS);
		return false;
	}

	CCVoteDiscuss* pDiscuss = GetDiscuss();
	if (pDiscuss->CheckVoter(uid)) {
		SetLastError(VOTEMGR_ERROR_VOTE_ALREADY_VOTED);
		return false;
	}

	pDiscuss->Vote(uid, nVote);
	SetLastError(VOTEMGR_ERROR_OK);

	return true;
}

void CCVoteMgr::Tick(unsigned long nClock)
{
	if (GetDiscuss() == NULL)
		return;

	if (CheckDiscuss() == true) {
		FinishDiscuss(true);
		return;
	} else {
		// 투표가능자가 없으면 투표 부결로 종료
		if (m_VoterMap.size() <= GetDiscuss()->GetYesVoterCount() + GetDiscuss()->GetNoVoterCount()) {
			FinishDiscuss(false);
			return;
		}
	}

	if (nClock - GetDiscuss()->GetBeginTime() > 60000) {	// 1분동안 미결정이면 종료
		FinishDiscuss(false);
		return;
	}
}


void CCVoteMgr::StopVote( const CCUID& uidUser )
{
	delete m_pDiscuss;
	m_pDiscuss = NULL;

	CCMatchObject* pObj = CCMatchServer::GetInstance()->GetObject( uidUser );
	if( !IsEnabledObject(pObj) )
		return;

	MCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand( MC_MATCH_VOTE_STOP, CCUID(0, 0) );
	if( 0 == pCmd )
		return;

	CCMatchStage* pStage = CCMatchServer::GetInstance()->FindStage( pObj->GetStageUID() );
	if( 0 == pStage )
		return;

	CCMatchServer::GetInstance()->RouteToStage( pStage->GetUID(), pCmd );
}