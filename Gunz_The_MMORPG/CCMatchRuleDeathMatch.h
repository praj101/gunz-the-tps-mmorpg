#ifndef _MMATCHRULE_DEATHMATCH_H
#define _MMATCHRULE_DEATHMATCH_H


#include "CCMatchRule.h"


class CCMatchRuleSoloDeath : public CCMatchRule {
protected:
	bool CheckKillCount(CCMatchObject* pOutObject);
	virtual void OnBegin();
	virtual void OnEnd();
	virtual void OnRoundTimeOut();
	virtual bool OnCheckRoundFinish();
	virtual bool RoundCount();
public:
	CCMatchRuleSoloDeath(CCMatchStage* pStage);
	virtual ~CCMatchRuleSoloDeath() { }
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_DEATHMATCH_SOLO; }
};

///////////////////////////////////////////////////////////////////////////////////////////////
class CCMatchRuleTeamDeath : public CCMatchRule {
protected:
	bool GetAliveCount(int* pRedAliveCount, int* pBlueAliveCount);
	virtual void OnBegin();
	virtual void OnEnd();
	virtual bool OnRun();
	virtual void OnRoundBegin();
	virtual void OnRoundEnd();
	virtual bool OnCheckRoundFinish();
	virtual void OnRoundTimeOut();
	virtual bool RoundCount();
	virtual bool OnCheckEnableBattleCondition();
public:
	CCMatchRuleTeamDeath(CCMatchStage* pStage);
	virtual ~CCMatchRuleTeamDeath()				{}
	virtual void CalcTeamBonus(CCMatchObject* pAttacker, CCMatchObject* pVictim,
								int nSrcExp, int* poutAttackerExp, int* poutTeamExp);
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_DEATHMATCH_TEAM; }
};


// Ãß°¡ by µ¿¼·
///////////////////////////////////////////////////////////////////////////////////////////////
class CCMatchRuleTeamDeath2 : public CCMatchRule {
protected:
	void GetTeamScore(int* pRedTeamScore, int* pBLueTeamScore);
	virtual void OnBegin();
	virtual void OnEnd();
	virtual bool OnRun();
	virtual void OnRoundBegin();
	virtual void OnRoundEnd();
	virtual bool OnCheckRoundFinish();
	virtual void OnRoundTimeOut();
	virtual bool RoundCount();
	virtual void OnGameKill(const CCUID& uidAttacker, const CCUID& uidVictim);

public:
	CCMatchRuleTeamDeath2(CCMatchStage* pStage);
	virtual ~CCMatchRuleTeamDeath2()				{}
	virtual void CalcTeamBonus(CCMatchObject* pAttacker, CCMatchObject* pVictim,
		int nSrcExp, int* poutAttackerExp, int* poutTeamExp);
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_DEATHMATCH_TEAM2; }
};



#endif