#include "stdafx.h"
#include "ZRuleDeathMatch.h"

// Added R347a
#include "ZGlobal.h"

ZRuleSoloDeathMatch::ZRuleSoloDeathMatch(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleSoloDeathMatch::~ZRuleSoloDeathMatch()
{





}
/////////////////////////////////////////////////////////////////////////////////////////

ZRuleTeamDeathMatch::ZRuleTeamDeathMatch(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleTeamDeathMatch::~ZRuleTeamDeathMatch()
{

}


/////////////////////////////////////////////////////////////////////////////////////////

ZRuleTeamDeathMatch2::ZRuleTeamDeathMatch2(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleTeamDeathMatch2::~ZRuleTeamDeathMatch2()
{

}

bool ZRuleTeamDeathMatch2::OnCommand(CCCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{

	case MC_MATCH_GAME_DEAD:
		{
			CCUID uidAttacker, uidVictim;

			pCommand->GetParameter(&uidAttacker, 0, MPT_UID);
			pCommand->GetParameter(&uidVictim, 2, MPT_UID);

			ZCharacter* pAttacker = ZGetGame()->m_CharacterManager.Find(uidAttacker);
			ZCharacter* pVictim = ZGetGame()->m_CharacterManager.Find(uidVictim);
			
			m_pMatch->AddTeamKills(pVictim->GetTeamID() == CCMT_BLUE ? CCMT_RED : CCMT_BLUE);
		}
		break;
	}

	return false;
}
