#ifndef _ZCHANNELRULE_H
#define _ZCHANNELRULE_H

#include "MBaseChannelRule.h"

class ZChannelRuleMgr : public CCChannelRuleMgr
{
public:
	ZChannelRuleMgr();
	virtual ~ZChannelRuleMgr();
	static ZChannelRuleMgr* GetInstance();
	CCChannelRule* GetCurrentRule();
};
inline ZChannelRuleMgr* ZGetChannelRuleMgr() { return ZChannelRuleMgr::GetInstance(); }

#endif