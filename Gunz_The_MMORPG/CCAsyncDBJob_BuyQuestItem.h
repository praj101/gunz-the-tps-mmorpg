#pragma once


#include "CCAsyncDBJob.h"


class CCAsyncDBJob_BuyQuestItem : public CCAsyncJob
{
public :
	CCAsyncDBJob_BuyQuestItem(const CCUID& uidOwner) : CCAsyncJob( CCASYNCJOB_BUYQUESTITEM, uidOwner )
	{
	}

	~CCAsyncDBJob_BuyQuestItem() 
	{
	}


	void Input( const CCUID& uidPlayer, const DWORD dwCID, const int nItemCount, const int nPrice )
	{
		m_uidPlayer = uidPlayer;
		m_dwCID		= dwCID;
		m_nItemCount = nItemCount;
		m_nPrice	= nPrice * nItemCount;		
	}


	void Run(void* pContext);


	const CCUID&	GetPlayerUID()	{ return m_uidPlayer; }
	const int	GetPrice()		{ return m_nPrice; }
	const int	GetItemCount()	{ return m_nItemCount; }

private :
	CCUID	m_uidPlayer;
	DWORD	m_dwCID;
	int		m_nPrice;
	int		m_nItemCount;
};