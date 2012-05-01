#ifndef _ZRULE_DUEL_H
#define _ZRULE_DUEL_H

#include "ZRule.h"


class ZRuleDuel : public ZRule
{
public:
	CCTD_DuelQueueInfo QInfo;

	ZRuleDuel(ZMatch* pMatch);
	virtual ~ZRuleDuel();

	virtual bool OnCommand(CCCommand* pCommand);

	int	GetQueueIdx(const CCUID& uidChar);			// 0 : 챔피언   1 : 도전자  2~ : 관전자
};

#endif