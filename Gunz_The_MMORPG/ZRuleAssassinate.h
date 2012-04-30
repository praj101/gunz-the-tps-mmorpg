#ifndef _ZRULE_ASSASSINATE_H
#define _ZRULE_ASSASSINATE_H


#include "ZRule.h"
#include "ZRuleDeathMatch.h"



class ZRuleAssassinate : public ZRuleTeamDeathMatch
{
private:
	void AssignCommander(const CCUID& uidRedCommander, const CCUID& uidBlueCommander);
public:
	ZRuleAssassinate(ZMatch* pMatch);
	virtual ~ZRuleAssassinate();
	virtual bool OnCommand(MCommand* pCommand);
	virtual void OnResponseRuleInfo(MTD_RuleInfo* pInfo);
};








#endif