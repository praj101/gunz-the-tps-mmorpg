#include "stdafx.h"
#include "CCAsyncDBJob_UpdateCharItemInfoData.h"

void CCAsyncDBJob_UpdateCharItemInfoData::RemoveAll()
{
	list<UpdateItem *>::iterator itBegin = m_ItemList.begin();
	for(; itBegin != m_ItemList.end(); itBegin++){
		UpdateItem *pItem = (*itBegin);
		delete pItem;
	}

	m_ItemList.clear();
}

bool CCAsyncDBJob_UpdateCharItemInfoData::Input(const int nCIID, const int nChangeCnt)
{
	if( nChangeCnt == 0 ) return false;

	UpdateItem* pItem = new UpdateItem;
	
	pItem->bIsSuccess	= false;
	pItem->nCIID		= nCIID;
	pItem->nChangeCnt	= nChangeCnt;

	m_ItemList.push_back(pItem);
	return true;
}

void CCAsyncDBJob_UpdateCharItemInfoData::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	list<UpdateItem *>::iterator itBegin = m_ItemList.begin();
	for(; itBegin != m_ItemList.end(); itBegin++)
	{
		UpdateItem *pItem = (*itBegin);

		if( pDBMgr->UpdateCharSpendItemCount(pItem->nCIID, pItem->nChangeCnt * (-1)) ) {
			pItem->bIsSuccess = true;
		}
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}
