#ifndef _MMATCHRULE_GLADIATOR_H
#define _MMATCHRULE_GLADIATOR_H


#include "CCMatchRuleDeathMatch.h"

///////////////////////////////////////////////////////////////////////////////////////////////
class CCMatchRuleSoloGladiator : public CCMatchRuleSoloDeath  {
public:
	CCMatchRuleSoloGladiator(CCMatchStage* pStage);
	virtual ~CCMatchRuleSoloGladiator() { }
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_GLADIATOR_SOLO; }
};

///////////////////////////////////////////////////////////////////////////////////////////////
class CCMatchRuleTeamGladiator : public CCMatchRuleTeamDeath {
public:
	CCMatchRuleTeamGladiator(CCMatchStage* pStage);
	virtual ~CCMatchRuleTeamGladiator()				{}
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_GLADIATOR_TEAM; }
};




#endif