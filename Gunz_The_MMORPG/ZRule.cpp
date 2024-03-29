#include "stdafx.h"
#include "ZRule.h"
#include "ZMatch.h"
#include "ZRuleDeathMatch.h"
#include "ZRuleGladiator.h"
#include "ZRuleAssassinate.h"
#include "ZRuleTraining.h"
#include "ZRuleQuest.h"
#include "ZRuleSurvival.h"
#include "ZRuleBerserker.h"
#include "ZRuleDuel.h"
#include "ZRuleDuelTournament.h"

ZRule::ZRule(ZMatch* pMatch)
{
	m_pMatch = pMatch;				// 초기화 =_=
}

ZRule::~ZRule()
{

}

void ZRule::Update(float fDelta)
{
	OnUpdate(fDelta);
}

ZRule* ZRule::CreateRule(ZMatch* pMatch, CCMATCH_GAMETYPE nGameType)
{
	switch(nGameType)
	{
	case CCMATCH_GAMETYPE_DEATHMATCH_SOLO:
		{
			return (new ZRuleSoloDeathMatch(pMatch));
		}
		break;
	case CCMATCH_GAMETYPE_DEATHMATCH_TEAM:
		{
			return (new ZRuleTeamDeathMatch(pMatch));
		}
		break;
	case CCMATCH_GAMETYPE_GLADIATOR_SOLO:
		{
			return (new ZRuleSoloGladiator(pMatch));
		}
		break;
	case CCMATCH_GAMETYPE_GLADIATOR_TEAM:
		{
			return (new ZRuleTeamGladiator(pMatch));
		}
		break;
	case CCMATCH_GAMETYPE_ASSASSINATE:
		{
			return (new ZRuleAssassinate(pMatch));
		}
		break;
	case CCMATCH_GAMETYPE_TRAINING:
		{
			return (new ZRuleTraining(pMatch));
		}
		break;
#ifdef _QUEST
	case CCMATCH_GAMETYPE_SURVIVAL:
		{
			return (new ZRuleSurvival(pMatch));
		}
		break;
	case CCMATCH_GAMETYPE_QUEST:
		{
			return (new ZRuleQuest(pMatch));
		}
		break;
#endif
	case CCMATCH_GAMETYPE_BERSERKER:
		{
			return (new ZRuleBerserker(pMatch));
		}
		break;
	case CCMATCH_GAMETYPE_DEATHMATCH_TEAM2:
		{
			return (new ZRuleTeamDeathMatch2(pMatch));
		}
		break;
	case CCMATCH_GAMETYPE_DUEL:
		{
			return (new ZRuleDuel(pMatch));
		}
		break;

	case CCMATCH_GAMETYPE_DUELTOURNAMENT:
		{
			return (new ZRuleDuelTournament(pMatch));
		}
		break;

	default:
		{
			// 게임 타입이 없습니다.
			_ASSERT(0);
		}
	}
	return NULL;

}

bool ZRule::OnCommand(CCCommand* pCommand)
{
	return false;
}

void ZRule::AfterCommandProcessed( CCCommand* pCommand )
{

}

void ZRule::OnResponseRuleInfo(CCTD_RuleInfo* pInfo)
{

}