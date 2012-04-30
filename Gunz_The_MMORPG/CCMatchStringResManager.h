#ifndef _MMATCH_STRINGRES_MANAGER_H
#define _MMATCH_STRINGRES_MANAGER_H

#include "CCBaseStringResManager.h"

class CCMatchStringResManager : public MBaseStringResManager
{
public:
	CCMatchStringResManager();
	virtual ~CCMatchStringResManager();
	static void MakeInstance();
};


#endif