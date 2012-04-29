#ifndef _MMATCHRULE_ASSASSINATE_H
#define _MMATCHRULE_ASSASSINATE_H


#include "MMatchRuleDeathMatch.h"

///////////////////////////////////////////////////////////////////////////////////////////////
class MMatchRuleAssassinate : public MMatchRuleTeamDeath {
private:
	CCUID		m_uidRedCommander;
	CCUID		m_uidBlueCommander;
	bool		m_bIsAdminCommander;
private:
	const CCUID ChooseCommander(int nTeam);
protected:
	virtual void OnRoundBegin();
	virtual void OnRoundEnd();
	virtual bool OnCheckRoundFinish();
public:
	MMatchRuleAssassinate(CCMatchStage* pStage);
	virtual ~MMatchRuleAssassinate()				{}
	virtual void* CreateRuleInfoBlob();
	virtual void CalcTeamBonus(CCMatchObject* pAttacker, CCMatchObject* pVictim,
								int nSrcExp, int* poutAttackerExp, int* poutTeamExp);
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_ASSASSINATE; }
	void ChooseAdminAsCommander();
};



#endif