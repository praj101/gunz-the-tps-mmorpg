#pragma once

#include "CCAsyncDBJob.h"

typedef struct _UpdateItem
{
	bool bIsSuccess;

	int nCIID;
	int	nChangeCnt;	
} UpdateItem;


class CCAsyncDBJob_UpdateCharItemInfoData : public CCAsyncJob {
protected:	
	list<UpdateItem *>	m_ItemList;

public:
	CCAsyncDBJob_UpdateCharItemInfoData(const CCUID& uidOwner) 
		: CCAsyncJob(CCASYNCJOB_UPDATE_CHARITEM_COUNT, uidOwner){}

	virtual ~CCAsyncDBJob_UpdateCharItemInfoData()	
	{
		RemoveAll();
	}

	bool Input(const int nCIID, const int nChangeCnt);
	virtual void Run(void* pContext);

	void RemoveAll();
};