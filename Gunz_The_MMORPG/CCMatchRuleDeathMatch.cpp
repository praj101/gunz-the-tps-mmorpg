#include "stdafx.h"
#include "CCMatchRuleDeathMatch.h"
#include "CCMatchTransDataType.h"
#include "CCBlobArray.h"

// TEAM DEATH RULE ///////////////////////////////////////////////////////////////
CCMatchRuleTeamDeath::CCMatchRuleTeamDeath(CCMatchStage* pStage) : CCMatchRule(pStage)
{
}

void CCMatchRuleTeamDeath::OnBegin()
{
}

void CCMatchRuleTeamDeath::OnEnd()
{
}

bool CCMatchRuleTeamDeath::OnRun()
{
	bool ret = CCMatchRule::OnRun();


	return ret;
}

void CCMatchRuleTeamDeath::OnRoundBegin()
{
	CCMatchRule::OnRoundBegin();
}

void CCMatchRuleTeamDeath::OnRoundEnd()
{
	if (m_pStage != NULL)
	{
		switch(m_nRoundArg)
		{
			case MMATCH_ROUNDRESULT_BLUE_ALL_OUT: m_pStage->OnRoundEnd_FromTeamGame(CCMT_RED);break;
			case MMATCH_ROUNDRESULT_RED_ALL_OUT: m_pStage->OnRoundEnd_FromTeamGame(CCMT_BLUE); break;
			case MMATCH_ROUNDRESULT_REDWON: m_pStage->OnRoundEnd_FromTeamGame(CCMT_RED); break;
			case MMATCH_ROUNDRESULT_BLUEWON: m_pStage->OnRoundEnd_FromTeamGame(CCMT_BLUE); break;
			case MMATCH_ROUNDRESULT_DRAW: break;
		}
	}

	CCMatchRule::OnRoundEnd();
}

bool CCMatchRuleTeamDeath::OnCheckEnableBattleCondition()
{
	// 선승제일 경우는 Free상태가 안된다.
	if (m_pStage->GetStageSetting()->IsTeamWinThePoint() == true)
	{
		return true;
	}

	int nRedTeam = 0, nBlueTeam = 0;
	int nPreRedTeam = 0, nPreBlueTeam = 0;
	int nStageObjects = 0;		// 게임안에 없고 스테이지에 있는 사람

	CCMatchStage* pStage = GetStage();
	if (pStage == NULL) return false;

	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
	{
		CCMatchObject* pObj = (CCMatchObject*)(*i).second;
		if ((pObj->GetEnterBattle() == false) && (!pObj->IsLaunchedGame()))
		{
			nStageObjects++;
			continue;
		}

		if (pObj->GetTeam() == CCMT_RED)
		{
			nRedTeam++;
		}
		else if (pObj->GetTeam() == CCMT_BLUE)
		{
			nBlueTeam++;
		}
	}

	if ( nRedTeam == 0 || nBlueTeam == 0)
	{
		return false;
	}

	return true;
}

// 만약 레드팀이나 블루팀에서 팀원이 0명일 경우는 false 반환 , true,false 모두 AliveCount 반환
bool CCMatchRuleTeamDeath::GetAliveCount(int* pRedAliveCount, int* pBlueAliveCount)
{
	int nRedCount = 0, nBlueCount = 0;
	int nRedAliveCount = 0, nBlueAliveCount = 0;
	(*pRedAliveCount) = 0;
	(*pBlueAliveCount) = 0;

	CCMatchStage* pStage = GetStage();
	if (pStage == NULL) return false;

	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
	{
		CCMatchObject* pObj = (CCMatchObject*)(*i).second;
		if (pObj->GetEnterBattle() == false) continue;	// 배틀참가하고 있는 플레이어만 체크

		if (pObj->GetTeam() == CCMT_RED)
		{
			nRedCount++;
			if (pObj->CheckAlive()==true)
			{
				nRedAliveCount++;
			}
		}
		else if (pObj->GetTeam() == CCMT_BLUE)
		{
			nBlueCount++;
			if (pObj->CheckAlive()==true)
			{
				nBlueAliveCount++;
			}
		}
	}

	(*pRedAliveCount) = nRedAliveCount;
	(*pBlueAliveCount) = nBlueAliveCount;

	if ((nRedAliveCount == 0) || (nBlueAliveCount == 0))
	{
		return false;
	}
	return true;
}

bool CCMatchRuleTeamDeath::OnCheckRoundFinish()
{
	int nRedAliveCount = 0;
	int nBlueAliveCount = 0;

	// 팀원이 0명인 팀이 있으면 false반환
	if (GetAliveCount(&nRedAliveCount, &nBlueAliveCount) == false)
	{
		int nRedTeam = 0, nBlueTeam = 0;
		int nStageObjects = 0;		// 게임안에 없고 스테이지에 있는 사람

		CCMatchStage* pStage = GetStage();

		for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
		{
			CCMatchObject* pObj = (CCMatchObject*)(*i).second;
			if ((pObj->GetEnterBattle() == false) && (!pObj->IsLaunchedGame()))
			{
				nStageObjects++;
				continue;
			}

			if (pObj->GetTeam() == CCMT_RED)		nRedTeam++;
			else if (pObj->GetTeam() == CCMT_BLUE)	nBlueTeam++;
		}

		if( nBlueTeam ==0 && (pStage->GetTeamScore(CCMT_BLUE) > pStage->GetTeamScore(CCMT_RED)) )
			SetRoundArg(MMATCH_ROUNDRESULT_BLUE_ALL_OUT);
		else if( nRedTeam ==0 && (pStage->GetTeamScore(CCMT_RED) > pStage->GetTeamScore(CCMT_BLUE)) )
			SetRoundArg(MMATCH_ROUNDRESULT_RED_ALL_OUT);
		else if ( (nRedAliveCount == 0) && (nBlueAliveCount == 0) )
			SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
		else if (nRedAliveCount == 0)
			SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
		else if (nBlueAliveCount == 0)
			SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
	}

	if (nRedAliveCount==0 || nBlueAliveCount==0) return true;
	else return false;
}

void CCMatchRuleTeamDeath::OnRoundTimeOut()
{
	int nRedAliveCount = 0;
	int nBlueAliveCount = 0;
	GetAliveCount(&nRedAliveCount, &nBlueAliveCount);

	if (nRedAliveCount > nBlueAliveCount)
		SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
	else if (nBlueAliveCount > nRedAliveCount)
		SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
	else SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
}

// 반환값이 false이면 게임이 끝난다.
bool CCMatchRuleTeamDeath::RoundCount() 
{
	if (m_pStage == NULL) return false;

	int nTotalRound = m_pStage->GetStageSetting()->GetRoundMax();
	m_nRoundCount++;

	if (m_pStage->GetStageSetting()->IsTeamWinThePoint() == false)
	{
		// 선승제가 아닐 경우
		if (m_nRoundCount < nTotalRound) return true;

	}
	else
	{
		// 선승제일 경우 

		// 팀원이 0명인 팀이 있어도 게임이 끝난다.
		int nRedTeamCount=0, nBlueTeamCount=0;
		m_pStage->GetTeamMemberCount(&nRedTeamCount, &nBlueTeamCount, NULL, true);

		if ((nRedTeamCount == 0) || (nBlueTeamCount == 0))
		{
			return false;
		}

		int nRedScore = m_pStage->GetTeamScore(CCMT_RED);
		int nBlueScore = m_pStage->GetTeamScore(CCMT_BLUE);
		
		// 래더게임에서 먼저 4승인 팀이 승리
		const int LADDER_WINNING_ROUNT_COUNT = 4;


		// 두팀이 모두 4승이 아니면 true반환
		if ((nRedScore < LADDER_WINNING_ROUNT_COUNT) && (nBlueScore < LADDER_WINNING_ROUNT_COUNT))
		{
			return true;
		}
	}

	return false;
}

void CCMatchRuleTeamDeath::CalcTeamBonus(CCMatchObject* pAttacker, CCMatchObject* pVictim,
								int nSrcExp, int* poutAttackerExp, int* poutTeamExp)
{
	if (m_pStage == NULL)
	{
		*poutAttackerExp = nSrcExp;
		*poutTeamExp = 0;
		return;
	}

	*poutTeamExp = (int)(nSrcExp * m_pStage->GetStageSetting()->GetCurrGameTypeInfo()->fTeamBonusExpRatio);
	*poutAttackerExp = (int)(nSrcExp * m_pStage->GetStageSetting()->GetCurrGameTypeInfo()->fTeamMyExpRatio);
}

//////////////////////////////////////////////////////////////////////////////////
// CCMatchRuleSoloDeath ///////////////////////////////////////////////////////////
CCMatchRuleSoloDeath::CCMatchRuleSoloDeath(CCMatchStage* pStage) : CCMatchRule(pStage)
{

}

void CCMatchRuleSoloDeath::OnBegin()
{

}
void CCMatchRuleSoloDeath::OnEnd()
{
}

bool CCMatchRuleSoloDeath::RoundCount()
{
	if (++m_nRoundCount < 1) return true;
	return false;
}

bool CCMatchRuleSoloDeath::CheckKillCount(CCMatchObject* pOutObject)
{
	CCMatchStage* pStage = GetStage();
	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
	{
		CCMatchObject* pObj = (CCMatchObject*)(*i).second;
		if (pObj->GetEnterBattle() == false) continue;

		if (pObj->GetKillCount() >= (unsigned int)pStage->GetStageSetting()->GetRoundMax())
		{
			pOutObject = pObj;
			return true;
		}
	}
	return false;
}

bool CCMatchRuleSoloDeath::OnCheckRoundFinish()
{
	CCMatchObject* pObject = NULL;

	if (CheckKillCount(pObject))
	{
		return true;
	}
	return false;
}

void CCMatchRuleSoloDeath::OnRoundTimeOut()
{
	SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
}




// 무한 팀데스매치 - 추가 by 동섭
//////////////////////////////////////////////////////////////////////////
CCMatchRuleTeamDeath2::CCMatchRuleTeamDeath2(CCMatchStage* pStage) : CCMatchRule(pStage)
{
}

void CCMatchRuleTeamDeath2::OnBegin()
{
	m_pStage->InitTeamKills();
}

void CCMatchRuleTeamDeath2::OnEnd()
{
}

bool CCMatchRuleTeamDeath2::OnRun()
{
	bool ret = CCMatchRule::OnRun();


	return ret;
}

void CCMatchRuleTeamDeath2::OnRoundBegin()
{
	CCMatchRule::OnRoundBegin();
}

void CCMatchRuleTeamDeath2::OnRoundEnd()
{
	if (m_pStage != NULL)
	{
		if (m_nRoundArg == MMATCH_ROUNDRESULT_REDWON) 
		{
			m_pStage->OnRoundEnd_FromTeamGame(CCMT_RED);
		} 
		else if (m_nRoundArg == MMATCH_ROUNDRESULT_BLUEWON) 
		{
			m_pStage->OnRoundEnd_FromTeamGame(CCMT_BLUE);
		} 
		else if (m_nRoundArg == MMATCH_ROUNDRESULT_DRAW) 
		{ 
			// Do Nothing
		}
	}

	CCMatchRule::OnRoundEnd();
}

// 만약 레드팀이나 블루팀에서 팀원이 0명일 경우는 false 반환 , true,false 모두 AliveCount 반환
void CCMatchRuleTeamDeath2::GetTeamScore(int* pRedTeamScore, int* pBlueTeamScore)
{
	(*pRedTeamScore) = 0;
	(*pBlueTeamScore) = 0;

	CCMatchStage* pStage = GetStage();
	if (pStage == NULL) return;

	(*pRedTeamScore) = pStage->GetTeamKills(CCMT_RED);
	(*pBlueTeamScore) = pStage->GetTeamKills(CCMT_BLUE);

	return;
}

bool CCMatchRuleTeamDeath2::OnCheckRoundFinish()
{
	int nRedScore, nBlueScore;
	GetTeamScore(&nRedScore, &nBlueScore);

	CCMatchStage* pStage = GetStage();

	if (nRedScore >= pStage->GetStageSetting()->GetRoundMax())
	{
		SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
		return true;
	}
	else if (nBlueScore >= pStage->GetStageSetting()->GetRoundMax())
	{
		SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
		return true;
	}

	return false;
}

void CCMatchRuleTeamDeath2::OnRoundTimeOut()
{
	if (!OnCheckRoundFinish())
		SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
}

// 반환값이 false이면 게임이 끝난다.
bool CCMatchRuleTeamDeath2::RoundCount() 
{
	if (++m_nRoundCount < 1) return true;
	return false;
}

void CCMatchRuleTeamDeath2::CalcTeamBonus(CCMatchObject* pAttacker, CCMatchObject* pVictim,
										int nSrcExp, int* poutAttackerExp, int* poutTeamExp)
{
	if (m_pStage == NULL)
	{
		*poutAttackerExp = nSrcExp;
		*poutTeamExp = 0;
		return;
	}

	*poutTeamExp = (int)(nSrcExp * m_pStage->GetStageSetting()->GetCurrGameTypeInfo()->fTeamBonusExpRatio);
	*poutAttackerExp = (int)(nSrcExp * m_pStage->GetStageSetting()->GetCurrGameTypeInfo()->fTeamMyExpRatio);
}




void CCMatchRuleTeamDeath2::OnGameKill(const CCUID& uidAttacker, const CCUID& uidVictim)
{
	CCMatchObject* pAttacker = CCMatchServer::GetInstance()->GetObject(uidAttacker);
	CCMatchObject* pVictim = CCMatchServer::GetInstance()->GetObject(uidVictim);

	if (m_pStage != NULL)
	{
//		if (pAttacker->GetTeam() != pVictim->GetTeam())
//		{
//			m_pStage->AddTeamKills(pAttacker->GetTeam());
//		}

		m_pStage->AddTeamKills(pVictim->GetTeam() == CCMT_BLUE ? CCMT_RED : CCMT_BLUE);		// 죽은사람 반대편팀 킬수 올림
	}
}