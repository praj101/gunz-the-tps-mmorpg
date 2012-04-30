#include "stdafx.h"
#include "CCMatchChannelRule.h"

CCMatchChannelRuleMgr::CCMatchChannelRuleMgr() : MChannelRuleMgr()
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