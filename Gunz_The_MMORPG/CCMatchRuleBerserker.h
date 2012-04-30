#ifndef _MMATCHRULE_BERSERKER_H
#define _MMATCHRULE_BERSERKER_H


#include "CCMatchRule.h"
#include "CCMatchRuleDeathMatch.h"


class CCMatchRuleBerserker : public CCMatchRuleSoloDeath {
protected:
	// 멤버변수 ---------------------
	CCUID		m_uidBerserker;				// 현재 버서커인 플레이어

	// 함수 -------------------------
	bool CheckKillCount(CCMatchObject* pOutObject);
	virtual void OnRoundBegin();
	virtual bool OnCheckRoundFinish();
	void RouteAssignBerserker();
	CCUID RecommendBerserker();
public:
	CCMatchRuleBerserker(CCMatchStage* pStage);
	virtual ~CCMatchRuleBerserker() { }
	virtual void* CreateRuleInfoBlob();
	virtual void OnEnterBattle(CCUID& uidChar);			///< 게임중 난입할때 호출된다.
	virtual void OnLeaveBattle(CCUID& uidChar);			///< 게임중 나갔을때 호출된다.
	virtual void OnGameKill(const CCUID& uidAttacker, const CCUID& uidVictim);
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_BERSERKER; }
	CCUID& GetBerserker() { return m_uidBerserker; }
};

#endif