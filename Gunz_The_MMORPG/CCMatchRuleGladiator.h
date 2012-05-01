#ifndef _CCMATCHRULE_GLADIATOR_H
#define _CCMATCHRULE_GLADIATOR_H


#include "CCMatchRuleDeathMatch.h"

///////////////////////////////////////////////////////////////////////////////////////////////
class CCMatchRuleSoloGladiator : public CCMatchRuleSoloDeath  {
public:
	CCMatchRuleSoloGladiator(CCMatchStage* pStage);
	virtual ~CCMatchRuleSoloGladiator() { }
	virtual CCMATCH_GAMETYPE GetGameType() { return CCMATCH_GAMETYPE_GLADIATOR_SOLO; }
};

///////////////////////////////////////////////////////////////////////////////////////////////
class CCMatchRuleTeamGladiator : public CCMatchRuleTeamDeath {
public:
	CCMatchRuleTeamGladiator(CCMatchStage* pStage);
	virtual ~CCMatchRuleTeamGladiator()				{}
	virtual CCMATCH_GAMETYPE GetGameType() { return CCMATCH_GAMETYPE_GLADIATOR_TEAM; }
};




#endif