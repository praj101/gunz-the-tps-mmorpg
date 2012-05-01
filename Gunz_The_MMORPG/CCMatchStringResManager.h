#ifndef _CCMATCH_STRINGRES_MANAGER_H
#define _CCMATCH_STRINGRES_MANAGER_H

#include "CCBaseStringResManager.h"

class CCMatchStringResManager : public CCBaseStringResManager
{
public:
	CCMatchStringResManager();
	virtual ~CCMatchStringResManager();
	static void MakeInstance();
};


#endif