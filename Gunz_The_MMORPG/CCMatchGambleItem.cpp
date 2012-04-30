#include "stdafx.h"
#include "CCMatchGambleItem.h"
#include "CCMatchGambleMachine.h"


const CCMatchCharGambleItem* CCMatchGambleItemManager::GetGambleItemByUID( const CCUID& uidItem ) const
{
	list< CCMatchCharGambleItem* >::const_iterator it, end;
	end = m_GambleItemList.end();
	for( it = m_GambleItemList.begin(); it != end; ++it )
	{
		if( uidItem == (*it)->GetUID()  )
			return (*it);
	}

	return NULL;

}

const CCMatchCharGambleItem* CCMatchGambleItemManager::GetGambleItemByIndex( const DWORD dwIndex ) const
{
	if( m_GambleItemList.size() > dwIndex )
	{
		list< CCMatchCharGambleItem* >::const_iterator it = m_GambleItemList.begin();
		for( DWORD i = 0; i < dwIndex; ++i )
			++it;

		return (*it);
	}
		
	return NULL;
}

const CCMatchCharGambleItem*	CCMatchGambleItemManager::GetGambleItemByCIID( const DWORD CIID ) const
{
	list< CCMatchCharGambleItem* >::const_iterator it, end;
	end = m_GambleItemList.end();
	for( it = m_GambleItemList.begin(); it != end; ++it )
	{
		if( CIID == (*it)->GetCIID()  )
			return (*it);
	}

	return NULL;
}

const CCMatchCharGambleItem*	CCMatchGambleItemManager::GetGambleItemByItemID( const DWORD dwItemID ) const
{
	list< CCMatchCharGambleItem* >::const_iterator it, end;
	end = m_GambleItemList.end();
	for( it = m_GambleItemList.begin(); it != end; ++it )
	{
		if( dwItemID == (*it)->GetGambleItemID() )
			return (*it);
	}

	return NULL;
}

const bool CCMatchGambleItemManager::AddGambleItem( const CCUID& uidItem, const DWORD dwCIID, const DWORD dwGambleItemID, const int nItemCount )
{
	if( NULL != GetGambleItemByUID(uidItem) ) return false;

	CCMatchCharGambleItem* pGambleItem = new CCMatchCharGambleItem( uidItem, dwCIID, dwGambleItemID, nItemCount );
	if( NULL == pGambleItem ) return false;

	m_GambleItemList.push_back( pGambleItem );
	return true;
}

void CCMatchGambleItemManager::DeleteGambleItem( const CCUID& uidItem )
{
	list< CCMatchCharGambleItem* >::iterator it, end;
	end = m_GambleItemList.end();
	for( it = m_GambleItemList.begin(); it != end; ++it )
	{
		if( uidItem == (*it)->GetUID() )
		{
			delete (*it);
			m_GambleItemList.erase( it );
			return;
		}
	}
}

bool CCMatchGambleItemManager::SetGambleItemCount( const CCUID& uidItem, int nItemCount)
{
	list< CCMatchCharGambleItem* >::iterator it, end;
	end = m_GambleItemList.end();
	for( it = m_GambleItemList.begin(); it != end; ++it )
	{
		CCMatchCharGambleItem *pGItem = (*it);
		if( uidItem == pGItem->GetUID() ) {			
			pGItem->SetItemCount(nItemCount);
			return true;
		}
	}

	return false;
}

bool CCMatchGambleItemManager::SetGambleItemCount( int nCIID, int nItemCount)
{
	list< CCMatchCharGambleItem* >::const_iterator it;
	for( it = m_GambleItemList.begin(); it != m_GambleItemList.end(); ++it ) {
		CCMatchCharGambleItem *pGItem = (*it);
		if( nCIID == pGItem->GetCIID()  ) {
			pGItem->SetItemCount(nItemCount);
			return true;
		}
	}

	return false;
}

void CCMatchGambleItemManager::Release()
{
	list< CCMatchCharGambleItem* >::iterator it, end;
	end = m_GambleItemList.end();
	for( it = m_GambleItemList.begin(); it != end; ++it )
	{
		delete (*it);
	}

	m_GambleItemList.clear();
}	
