#include "stdafx.h"
#include "CCMatchGameType.h"
#include "CCMatchMap.h"

MMatchGameTypeMgr::MMatchGameTypeMgr() : MBaseGameTypeCatalogue()
{

}

MMatchGameTypeMgr::~MMatchGameTypeMgr()
{

}

MMatchGameTypeMgr* MMatchGameTypeMgr::GetInstance()
{
	static MMatchGameTypeMgr m_stGameTypeMgr;
	return &m_stGameTypeMgr;
}