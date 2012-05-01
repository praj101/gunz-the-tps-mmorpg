#include "stdafx.h"
#include "CCMatchChannelRule.h"

CCMatchChannelRuleMgr::CCMatchChannelRuleMgr() : CCChannelRuleMgr()
{

}

CCMatchChannelRuleMgr::~CCMatchChannelRuleMgr()
{

}

CCMatchChannelRuleMgr* CCMatchChannelRuleMgr::GetInstance()
{
	static CCMatchChannelRuleMgr m_stChannelRuleMgr;
	return &m_stChannelRuleMgr;
}