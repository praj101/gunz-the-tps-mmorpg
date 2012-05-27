#ifndef _CCMATCHRULE_TRAINING_H
#define _CCMATCHRULE_TRAINING_H


#include "CCMatchRuleDeathMatch.h"

///////////////////////////////////////////////////////////////////////////////////////////////
class CCMatchRuleTraining : public CCMatchRuleSoloDeath {
public:
	CCMatchRuleTraining(CCMatchStage* pStage);
	virtual ~CCMatchRuleTraining()	{ }
	virtual CCMATCH_GAMETYPE GetGameType() { return CCMATCH_GAMETYPE_TRAINING; }
};






#endif