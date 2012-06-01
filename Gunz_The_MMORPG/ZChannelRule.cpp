#include "stdafx.h"
#include "CCXml.h"
#include "CCZFileSystem.h"
#include "ZChannelRule.h"
#include "ZApplication.h"
#include "ZGameClient.h"

// Added
#include "ZGameInterface.h"

ZChannelRuleMgr::ZChannelRuleMgr() : CCChannelRuleMgr()
{

}

ZChannelRuleMgr::~ZChannelRuleMgr()
{

}

ZChannelRuleMgr* ZChannelRuleMgr::GetInstance()
{
	static ZChannelRuleMgr g_ChannelRuleMgr;
	return &g_ChannelRuleMgr;
}

CCChannelRule* ZChannelRuleMgr::GetCurrentRule()
{
	return GetRule(ZGetGameClient()->GetChannelRuleName());
}

