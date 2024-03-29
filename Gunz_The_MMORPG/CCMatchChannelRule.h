#pragma once
#include "CCBaseChannelRule.h"

class CCMatchChannelRuleMgr : public CCChannelRuleMgr
{
public:
	CCMatchChannelRuleMgr();
	virtual ~CCMatchChannelRuleMgr();
	static CCMatchChannelRuleMgr* GetInstance();
};


inline CCMatchChannelRuleMgr* CCGetChannelRuleMgr() 
{
	return CCMatchChannelRuleMgr::GetInstance();
}

