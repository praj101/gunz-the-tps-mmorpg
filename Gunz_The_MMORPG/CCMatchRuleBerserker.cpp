#include "stdafx.h"
#include "CCMatchRuleBerserker.h"
#include "CCMatchTransDataType.h"
#include "CCBlobArray.h"
#include "CCMatchServer.h"

//////////////////////////////////////////////////////////////////////////////////
// CCMatchRuleBerserker ///////////////////////////////////////////////////////////
CCMatchRuleBerserker::CCMatchRuleBerserker(CCMatchStage* pStage) : CCMatchRuleSoloDeath(pStage), m_uidBerserker(0,0)
{

}

bool CCMatchRuleBerserker::OnCheckRoundFinish()
{
	return CCMatchRuleSoloDeath::OnCheckRoundFinish();
}

void CCMatchRuleBerserker::OnRoundBegin()
{
	m_uidBerserker = CCUID(0,0);
}

void* CCMatchRuleBerserker::CreateRuleInfoBlob()
{
	void* pRuleInfoArray = CCMakeBlobArray(sizeof(CCTD_RuleInfo_Berserker), 1);
	CCTD_RuleInfo_Berserker* pRuleItem = (CCTD_RuleInfo_Berserker*)CCGetBlobArrayElement(pRuleInfoArray, 0);
	memset(pRuleItem, 0, sizeof(CCTD_RuleInfo_Berserker));
	
	pRuleItem->nRuleType = CCMATCH_GAMETYPE_BERSERKER;
	pRuleItem->uidBerserker = m_uidBerserker;

	return pRuleInfoArray;
}

void CCMatchRuleBerserker::RouteAssignBerserker()
{	CCCommand* pNew = CCMatchServer::GetInstance()->CreateCommand(MC_MATCH_ASSIGN_BERSERKER, CCUID(0, 0));
	pNew->AddParameter(new CCCmdParamCCUID(m_uidBerserker));
	CCMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);
}


CCUID CCMatchRuleBerserker::RecommendBerserker()
{
	CCMatchStage* pStage = GetStage();
	if (pStage == NULL) return CCUID(0,0);

	int nCount = 0;
	for(CCUIDRefCache::iterator itor=pStage->GetObjBegin(); itor!=pStage->GetObjEnd(); itor++) {
		CCMatchObject* pObj = (CCMatchObject*)(*itor).second;
		if (pObj->GetEnterBattle() == false) continue;	// 배틀참가하고 있는 플레이어만 체크
		if (pObj->CheckAlive())
		{
			return pObj->GetUID();
		}
	}
	return CCUID(0,0);

}


void CCMatchRuleBerserker::OnEnterBattle(CCUID& uidChar)
{
}

void CCMatchRuleBerserker::OnLeaveBattle(CCUID& uidChar)
{
	if (uidChar == m_uidBerserker)
	{
		m_uidBerserker = CCUID(0,0);
		RouteAssignBerserker();
	}
}

void CCMatchRuleBerserker::OnGameKill(const CCUID& uidAttacker, const CCUID& uidVictim)
{
	// 희생자가 버서커이거나 현재 버서커가 한명도 없을때
	if ((m_uidBerserker == uidVictim) || (m_uidBerserker == CCUID(0,0)))
	{
		bool bAttackerCanBeBerserker = false;

		 // 공격자가 자신이 아닐 경우
		if (uidAttacker != uidVictim)
		{
			CCMatchObject* pAttacker = CCMatchServer::GetInstance()->GetObject(uidAttacker);

			// 공격자가 죽어있으면 버서커가 될 수 없다(러브샷)
			if ((pAttacker) && (pAttacker->CheckAlive()))
			{
				bAttackerCanBeBerserker = true;
			}
		}
		// 공격자가 자신일 경우 버서커는 아무도 되지 않는다.
		else if ((uidAttacker == CCUID(0,0)) || (uidAttacker == uidVictim))
		{
			bAttackerCanBeBerserker = false;
		}

		if (bAttackerCanBeBerserker) m_uidBerserker = uidAttacker;
		else m_uidBerserker = CCUID(0,0);

		RouteAssignBerserker();
	}
}

