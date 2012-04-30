#include "stdafx.h"
#include "CCMatchDBGambleItem.h"



CCMatchGambleItem::CCMatchGambleItem( const DWORD dwGambleItemID
								   , const string& strName
								   , const string& strDescription
								   , const int nStartTimeMin
								   , const int nLifeTimeMin
								   , const DWORD dwPrice
								   , const bool bIsCash
								   , const bool bIsOpened ) 
								   : CCMatchDBBaseGambleItem( dwGambleItemID
															, strName
															, strDescription
															, dwPrice
															, bIsCash )
{    
	m_nStartTimeMin		= nStartTimeMin;;
	m_nLifeTimeMin		= nLifeTimeMin;
	m_dwTotalRate		= 0;
	m_bIsOpened			= bIsOpened;

	m_bIsNoTimeLimit	= (0 == nLifeTimeMin); // LifeTime이 0이면 무기한 아이템으로 판단.
}


CCMatchGambleItem::~CCMatchGambleItem()
{
}

bool CCMatchGambleItem::AddGambleRewardItem( CCMatchGambleRewardItem* pGRItem )
{
	if( NULL == pGRItem ) 
		return false;

	if( GetGambleItemID() != pGRItem->GetGambleItemID() )
		return false;

	_ASSERT( 1000 >= (m_dwTotalRate + pGRItem->GetRate()) );

	m_dwTotalRate += pGRItem->GetRate();

	pGRItem->SetRateRange( m_dwTotalRate );

	m_RewardItemList.push_back( pGRItem );

	return true;
}

const CCMatchGambleRewardItem* CCMatchGambleItem::GetGambleRewardItemByRate( const WORD wRate ) const
{
	vector< CCMatchGambleRewardItem* >::const_iterator it, end;
	end = m_RewardItemList.end();
	for( it = m_RewardItemList.begin(); it != end; ++it )
	{
		if( wRate < (*it)->GetRateRange() )
			return (*it);
	}

	return NULL;
}


void CCMatchGambleItem::Release()
{
	vector< CCMatchGambleRewardItem* >::const_iterator it, end;
	end = m_RewardItemList.end();
	for( it = m_RewardItemList.begin(); it != end; ++it )
		delete (*it);

	m_RewardItemList.clear();
}