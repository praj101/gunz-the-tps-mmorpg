#pragma once
#include "MBaseChannelRule.h"

class MMatchChannelRuleMgr : public MChannelRuleMgr
{
public:
	MMatchChannelRuleMgr();
	virtual ~MMatchChannelRuleMgr();
	static MMatchChannelRuleMgr* GetInstance();
};


inline MMatchChannelRuleMgr* MGetChannelRuleMgr() 
{
	return MMatchChannelRuleMgr::GetInstance();
}

