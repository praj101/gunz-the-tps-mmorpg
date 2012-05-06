#ifndef _MMATCHRULE_TRAINING_H
#define _MMATCHRULE_TRAINING_H


#include "CCMatchRuleDeathMatch.h"

///////////////////////////////////////////////////////////////////////////////////////////////
class MMatchRuleTraining : public MMatchRuleSoloDeath {
public:
	MMatchRuleTraining(MMatchStage* pStage);
	virtual ~MMatchRuleTraining()	{ }
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_TRAINING; }
};






#endif