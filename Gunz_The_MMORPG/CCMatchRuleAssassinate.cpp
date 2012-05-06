#include "stdafx.h"
#include "CCMatchRuleAssassinate.h"
#include "CCBlobArray.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// CCMatchRuleAssassinate //////////////////////////////////////////////////////////////////////
CCMatchRuleAssassinate::CCMatchRuleAssassinate(CCMatchStage* pStage) : CCMatchRuleTeamDeath(pStage)
{
	m_uidRedCommander = CCUID(0,0);
	m_uidBlueCommander = CCUID(0,0);
	m_bIsAdminCommander = false;
}


void CCMatchRuleAssassinate::ChooseAdminAsCommander()
{
	m_bIsAdminCommander = !m_bIsAdminCommander;

	CCMatchServer* pServer = CCMatchServer::GetInstance();
	CCMatchStage* pStage = GetStage();
	if (pServer==NULL || pStage==NULL) return;

	m_uidRedCommander = ChooseCommander(CCMT_RED);
	m_uidBlueCommander = ChooseCommander(CCMT_BLUE);
	if ( (m_uidRedCommander == CCUID(0,0)) || (m_uidBlueCommander == CCUID(0,0)) ) {
		// Wait the game
		SetRoundState(CCMATCH_ROUNDSTATE_FREE);
		return;
	}

	// Let players know the commander...
	CCCommand* pCmd = pServer->CreateCommand(MC_MATCH_ASSIGN_COMMANDER, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParaCCUID(m_uidRedCommander));
	pCmd->AddParameter(new CCCmdParaCCUID(m_uidBlueCommander));
	pServer->RouteToStage(pStage->GetUID(), pCmd);
}


const CCUID CCMatchRuleAssassinate::ChooseCommander(int nTeam)
{
	CCMatchStage* pStage = GetStage();
	if (pStage == NULL) return CCUID(0,0);

	int nRedAliveCount, nBlueAliveCount, nChooseTeamCount;
	if (GetAliveCount(&nRedAliveCount, &nBlueAliveCount) == false) return CCUID(0,0);
	if (nTeam == CCMT_RED) {
		if (nRedAliveCount <= 0) return CCUID(0,0);
		nChooseTeamCount = nRedAliveCount;
	}
	if (nTeam == CCMT_BLUE) {
		if (nBlueAliveCount <= 0) return CCUID(0,0);
		nChooseTeamCount = nBlueAliveCount;
	}
	

	if( m_bIsAdminCommander == true )
	{
		for(CCUIDRefCache::iterator itor=pStage->GetObjBegin(); itor!=pStage->GetObjEnd(); itor++) {
			
			CCMatchObject* pObj = (CCMatchObject*)(*itor).second;
			
			if (pObj->GetEnterBattle() == false) 
				continue;	// 배틀참가하고 있는 플레이어만 체크
			
			if (pObj->GetTeam() == nTeam && pObj->GetAccountInfo()->m_nUGrade == CCMUGADMIN) 
			{
				return pObj->GetUID();
			}
		}
	}


	MTime time;
	int nChoose = time.MakeNumber(1, nChooseTeamCount);

	int nCount = 0;
	for(CCUIDRefCache::iterator itor=pStage->GetObjBegin(); itor!=pStage->GetObjEnd(); itor++) {
		CCMatchObject* pObj = (CCMatchObject*)(*itor).second;
		if (pObj->GetEnterBattle() == false) continue;	// 배틀참가하고 있는 플레이어만 체크
		if (pObj->GetTeam() == nTeam) {
			nCount++;
			if (nCount == nChoose) {				
				return pObj->GetUID();
			}
		}
	}
	return CCUID(0,0);
}

void CCMatchRuleAssassinate::OnRoundBegin()
{
	CCMatchServer* pServer = CCMatchServer::GetInstance();
	CCMatchStage* pStage = GetStage();
	if (pServer==NULL || pStage==NULL) return;

	m_uidRedCommander = ChooseCommander(CCMT_RED);
	m_uidBlueCommander = ChooseCommander(CCMT_BLUE);
	if ( (m_uidRedCommander == CCUID(0,0)) || (m_uidBlueCommander == CCUID(0,0)) ) {
		// Wait the game
		SetRoundState(CCMATCH_ROUNDSTATE_FREE);
		return;
	}

	// Let players know the commander...
	CCCommand* pCmd = pServer->CreateCommand(MC_MATCH_ASSIGN_COMMANDER, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParaCCUID(m_uidRedCommander));
	pCmd->AddParameter(new CCCmdParaCCUID(m_uidBlueCommander));
	pServer->RouteToStage(pStage->GetUID(), pCmd);

//	OutputDebugString("Assassinate::OnRoundBegin() \n");
}

void CCMatchRuleAssassinate::OnRoundEnd()
{
	CCMatchRule::OnRoundEnd();
//	OutputDebugString("Assassinate::OnRoundEnd() \n");
}

bool CCMatchRuleAssassinate::OnCheckRoundFinish()
{
	CCMatchStage* pStage = GetStage();
	if (pStage == NULL) {
		SetRoundArg(CCMATCH_ROUNDRESULT_DRAW);
		return true;
	}

	CCMatchObject* pRedCommanderObj = CCMatchServer::GetInstance()->GetObject(m_uidRedCommander);
	if ( (pRedCommanderObj==NULL) ||
		 (pRedCommanderObj->GetStageUID() != pStage->GetUID()) ) {
		SetRoundArg(CCMATCH_ROUNDRESULT_BLUEWON);
		return true;
	}

	CCMatchObject* pBlueCommanderObj = CCMatchServer::GetInstance()->GetObject(m_uidBlueCommander);
	if ( (pBlueCommanderObj==NULL) ||
		 (pBlueCommanderObj->GetStageUID() != pStage->GetUID()) ) {
		SetRoundArg(CCMATCH_ROUNDRESULT_REDWON);
		return true;
	}

	if ( (pRedCommanderObj->CheckAlive() == false) && (pBlueCommanderObj->CheckAlive() == false) ) {
		SetRoundArg(CCMATCH_ROUNDRESULT_DRAW);
		return true;
	}
	if (pRedCommanderObj->CheckAlive() == false) {
		SetRoundArg(CCMATCH_ROUNDRESULT_BLUEWON);
		return true;
	}
	if (pBlueCommanderObj->CheckAlive() == false) {
		SetRoundArg(CCMATCH_ROUNDRESULT_REDWON);
		return true;
	}

	return false;
}

void* CCMatchRuleAssassinate::CreateRuleInfoBlob()
{
	void* pRuleInfoArray = CCMakeBlobArray(sizeof(CCTD_RuleInfo_Assassinate), 1);
	CCTD_RuleInfo_Assassinate* pRuleItem = (CCTD_RuleInfo_Assassinate*)CCGetBlobArrayElement(pRuleInfoArray, 0);
	memset(pRuleItem, 0, sizeof(CCTD_RuleInfo_Assassinate));
	
	pRuleItem->nRuleType = CCMATCH_GAMETYPE_ASSASSINATE;
	pRuleItem->uidRedCommander = m_uidRedCommander;
	pRuleItem->uidBlueCommander = m_uidBlueCommander;

	return pRuleInfoArray;
}


void CCMatchRuleAssassinate::CalcTeamBonus(CCMatchObject* pAttacker, CCMatchObject* pVictim,
							int nSrcExp, int* poutAttackerExp, int* poutTeamExp)
{
	if ((m_pStage == NULL) || (pAttacker == NULL) || (pVictim == NULL))
	{
		*poutAttackerExp = nSrcExp;
		*poutTeamExp = 0;
		return;
	}

	bool bVictimIsCommander = false;
	if (pVictim->GetTeam() == CCMT_RED)
	{
		if (m_uidRedCommander == pVictim->GetUID()) bVictimIsCommander = true;
	}
	else if (pVictim->GetTeam() == CCMT_BLUE)
	{
		if (m_uidBlueCommander == pVictim->GetUID()) bVictimIsCommander = true;
	}

	if (bVictimIsCommander)
	{
		*poutTeamExp = 0;
		*poutAttackerExp = nSrcExp * 2;		// 적장을 쓰러뜨리면 경험치의 200%를 받는다.
	}
	else
	{
		*poutTeamExp = (int)(nSrcExp * m_pStage->GetStageSetting()->GetCurrGameTypeInfo()->fTeamBonusExpRatio);
		*poutAttackerExp = (int)(nSrcExp * m_pStage->GetStageSetting()->GetCurrGameTypeInfo()->fTeamMyExpRatio);
	}
}
