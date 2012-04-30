#include "stdafx.h"
#include "CCMatchGameType.h"
#include "CCMatchMap.h"

CCMatchGameTypeMgr::CCMatchGameTypeMgr() : MBaseGameTypeCatalogue()
{

}

CCMatchGameTypeMgr::~CCMatchGameTypeMgr()
{

}

CCMatchGameTypeMgr* CCMatchGameTypeMgr::GetInstance()
{
	static CCMatchGameTypeMgr m_stGameTypeMgr;
	return &m_stGameTypeMgr;
}