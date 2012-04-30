#pragma once

#include "CCBaseGameType.h"



class CCMatchGameTypeMgr : public MBaseGameTypeCatalogue
{
public:
	CCMatchGameTypeMgr();
	virtual ~CCMatchGameTypeMgr();
	static CCMatchGameTypeMgr* GetInstance();
};


inline CCMatchGameTypeMgr* MGetGameTypeMgr() 
{
	return CCMatchGameTypeMgr::GetInstance();
}
