#pragma once


#include "CCAsyncDBJob.h"
#include "CCMatchObject.h"


#include <vector>

using std::vector;


typedef vector<unsigned long int> ExpiredItemVec;


class CCAsyncDBJob_GetAccountItemList : public CCAsyncJob
{
public :
	CCAsyncDBJob_GetAccountItemList(const CCUID& uidOwner) : CCAsyncJob( CCASYNCJOB_GETACCOUNTITEMLIST, uidOwner ){}

	~CCAsyncDBJob_GetAccountItemList(){}

	void Input( const CCUID& uidPlayer, const DWORD dwAID )
	{
		m_uidPlayer	= uidPlayer;
		m_dwAID		= dwAID;
	}
	
	void Run( void* pContext );

	const CCUID&			GetPlayerUID()				{ return m_uidPlayer; }
	CCAccountItemNode*	GetAccountItemList()		{ return m_AccountItems; }
	const int			GetAccountItemCount()		{ return m_nItemCount; }
	ExpiredItemVec&		GetExpiredAccountItems()	{ return m_vExpiredItems; }

private :
	CCUID	m_uidPlayer;
	DWORD	m_dwAID;

	CCAccountItemNode	m_AccountItems[MAX_ACCOUNT_ITEM];
	int					m_nItemCount;
	ExpiredItemVec		m_vExpiredItems;
};