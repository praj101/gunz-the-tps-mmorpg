#ifndef _ZRULE_QUEST_H
#define _ZRULE_QUEST_H



#include "ZRule.h"
#include "ZRuleBaseQuest.h"
#include "CCQuestConst.h"

class ZRuleQuest : public ZRuleBaseQuest
{
protected:
	CCQuestCombatState		m_nCombatState;
public:
	ZRuleQuest(ZMatch* pMatch);
	virtual ~ZRuleQuest();

	CCQuestCombatState GetCombatState() { return m_nCombatState; }
};







#endif