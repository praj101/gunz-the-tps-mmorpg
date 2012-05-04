#include "stdafx.h"
#include "CCMath.h"
#include "CCMatchGambleMachine.h"
#include "CCMatchDBGambleItem.h"
#include "CCDebug.h"



CCMatchGambleMachine::CCMatchGambleMachine()
{
	m_dwLastUpdateTime = 0;
}


CCMatchGambleMachine::~CCMatchGambleMachine()
{
}


const CCMatchGambleRewardItem* CCMatchGambleMachine::Gamble( const DWORD dwGambleItemID ) const
{
	return GetGambleRewardItem( dwGambleItemID, RandomNumber(0, MAX_GAMBLE_RATE) );
}


// 겜블 아이템의 사용 시간을 검사해서 보내야 하는 겜블 아이템을 인자로 넘어온 백터에 담아준다. //
void CCMatchGambleMachine::GetItemVectorByCheckedItemTime(vector<DWORD>& outItemIndexVec, const DWORD dwCurTime) const
{
	// 안에 아이템이 들어있으면 안된다.
	_ASSERT( outItemIndexVec.empty() );

	const int nCurTimeMin = static_cast<int>(dwCurTime / 60000);

	const DWORD VecSize = (DWORD)m_GambleItemVec.size();
	for (DWORD i = 0; i < VecSize; i++)
	{
		const CCMatchGambleItem* pGambleItem = m_GambleItemVec[i];
		if (pGambleItem == NULL)
		{
			ASSERT( 0 );
			continue;
		}

		// DB에 저장만 되고 아직 판매를 시작하지 않았음.
		if( !pGambleItem->IsOpened() )
			continue;

		// Cashitem은 보여주지 않는다.
		if( pGambleItem->IsCash() )
			continue;

		if( !CheckGambleItemIsSelling(
			pGambleItem->GetStartTimeMin()
			, pGambleItem->GetEndTimeMin()
			, nCurTimeMin
			, pGambleItem->IsNoTimeLimitItem()) )
			continue;

		outItemIndexVec.push_back(i);
	}
}

const CCMatchGambleRewardItem* CCMatchGambleMachine::GetGambleRewardItem( const DWORD dwGambleItemID, const WORD wRate ) const
{
	map< DWORD, CCMatchGambleItem* >::const_iterator itFind = m_GambleItemMap.find( dwGambleItemID );
	if( m_GambleItemMap.end() == itFind )
		return NULL;

	return itFind->second->GetGambleRewardItemByRate( wRate );
}


const CCMatchGambleItem* CCMatchGambleMachine::GetGambleItemByIndex( const DWORD dwIndex ) const
{
	if( dwIndex < m_GambleItemVec.size() )
		return m_GambleItemVec[ dwIndex ];

	return NULL;
}


bool CCMatchGambleMachine::CreateGambleItemListWithGambleRewardList( vector<CCMatchGambleItem*>& vGambleItemList
																, vector<CCMatchGambleRewardItem*>& vGambleRewardItemList )
{
	Release();

	DWORD dwGIID = 0;

	vector<CCMatchGambleItem*>::iterator itGI, endGI;
	vector<CCMatchGambleRewardItem*>::iterator itRI, endRI;

	endGI = vGambleItemList.end();
	endRI = vGambleRewardItemList.end();

	cclog( "Start GambleItem Init.\n" );

	for( itGI = vGambleItemList.begin(); itGI != endGI; ++itGI )
	{
		dwGIID = (*itGI)->GetGambleItemID();

		cclog( "GIID : %u\n", dwGIID );

		for( itRI = vGambleRewardItemList.begin(); itRI != endRI; ++itRI )
		{
			if( (*itRI)->GetGambleItemID() == dwGIID )
			{
				if( !(*itGI)->AddGambleRewardItem((*itRI)) )
					return false;

				cclog( "  GRIID : M(%u), F(%u), RentHourPeriod(%u), ItemCnt(%u).\n"
					, (*itRI)->GetItemIDMale(), (*itRI)->GetItemIDFemale()
					, (*itRI)->GetRentHourPeriod(), (*itRI)->GetItemCnt() );
			}
		}

		_ASSERT( 1000 == (*itGI)->GetTotalRate() );

		m_GambleItemMap.insert( map<DWORD, CCMatchGambleItem*>::value_type(dwGIID, (*itGI)) );
		m_GambleItemVec.push_back( (*itGI) );
	}

	cclog( "End GambleItem Init.\n" );

	vGambleItemList.clear();
	vGambleRewardItemList.clear();

	return true;
}


bool CCMatchGambleMachine::CreateGambleItemList( vector<CCMatchGambleItem*>& vGambleItemList )
{
	Release();

	vector<CCMatchGambleItem*>::iterator itGI, endGI;
	endGI = vGambleItemList.end();

	for( itGI = vGambleItemList.begin(); itGI != endGI; ++itGI )
	{
		m_GambleItemMap.insert( map<DWORD, CCMatchGambleItem*>::value_type((*itGI)->GetGambleItemID(), (*itGI)) );
		m_GambleItemVec.push_back( (*itGI) );
	}

	vGambleItemList.clear();

	return true;
}


void CCMatchGambleMachine::Release()
{
	vector< CCMatchGambleItem* >::iterator it, end;
	end = m_GambleItemVec.end();
	for( it = m_GambleItemVec.begin(); it != end; ++it )
	{
		(*it)->Release();
		delete (*it);
	}

	m_GambleItemMap.clear();
	m_GambleItemVec.clear();
}


const CCMatchGambleItem*	CCMatchGambleMachine::GetGambleItemByGambleItemID( const DWORD dwGambleItemID ) const
{
	map< DWORD, CCMatchGambleItem* >::const_iterator itFind = m_GambleItemMap.find( dwGambleItemID );
	if( m_GambleItemMap.end() == itFind )
		return NULL;

	return itFind->second;
}


const CCMatchGambleItem* CCMatchGambleMachine::GetGambleItemByName( const string& strGambleItemName ) const
{
	vector< CCMatchGambleItem* >::const_iterator it, end;
	end = m_GambleItemVec.end();
	for( it = m_GambleItemVec.begin(); it != end; ++it )
	{
		if( strGambleItemName == (*it)->GetName() )
			return (*it);
	}

	return NULL;
}


void CCMatchGambleMachine::GetOpenedGambleItemList( vector<DWORD>& outGItemList ) const
{
	const DWORD dwSize = GetGambleItesSize();
	for( DWORD i = 0; i < dwSize; ++i )
	{
		if( m_GambleItemVec[i]->IsOpened() )
			outGItemList.push_back( i );
	}
}


const bool CCMatchGambleMachine::IsItTimeoverEventGambleItem( const DWORD dwGambleItemID, const DWORD dwCurTime ) const
{
	const CCMatchGambleItem* pGItem = GetGambleItemByGambleItemID( dwGambleItemID );
	if( NULL == pGItem )
		return true;

	if( !pGItem->IsOpened() )
		return true;

	return !CheckGambleItemIsSelling( 
		pGItem->GetStartTimeMin()
		, pGItem->GetEndTimeMin() + MAX_BUYGAMBLEITEM_ELAPSEDTIME_MIN
		, static_cast<int>(dwCurTime / 60000)
		, pGItem->IsNoTimeLimitItem() );
}


const bool CCMatchGambleMachine::CheckGambleItemIsSelling( const int nStartTimeMin
														 , const int nEndTimeMin
														 , const int nCurTimeMin
														 , const bool bIsNoTimeLimit  ) const
{
	// 이 함수는 겜블 아이템 정책에 맞추어 만들어진 함수 입니다.
	// 꼭 겜블 아이템에만 적용을 해야 합니다.
	// 그 외의 상황에 대해서는 보장 못함! - by SungE 2007-06-22

	// 아직 판매까지 시간이 남았음.
	if( nCurTimeMin < nStartTimeMin )
		return false;

	// 무기한 아이템인지 검사함.
	if( bIsNoTimeLimit )
		return true;
	else if( nEndTimeMin < nCurTimeMin )
		return false;

	// 여기까지 내려오면 판매 기간에 있는 아이템이다.
	return true;
}



void CCMatchGambleMachine::WriteGambleItemInfoToLog() const
{
	vector< CCMatchGambleItem* >::const_iterator end			= m_GambleItemVec.end();
	vector< CCMatchGambleItem* >::const_iterator	it			= m_GambleItemVec.begin();
	const DWORD									dwCurTime	= MGetMatchServer()->GetGlobalClockCount();

	cclog( "\n=== Dump GambleItem Info. ===\n" );
	cclog( "Gamble item list.\n" );
	for( ; end != it; ++it )
	{
		CCMatchGambleItem* pGItem = (*it);

		cclog( "Gamble item. ID(%d), Name(%s), IsTimeover(%d).\n"
			, pGItem->GetGambleItemID()
			, pGItem->GetName().c_str()
			, IsItTimeoverEventGambleItem(pGItem->GetGambleItemID(), dwCurTime) );
	}

	cclog( "\nSend shop gamble item list.\n" );
	vector< DWORD > vShopGItemList;
	GetItemVectorByCheckedItemTime( vShopGItemList, dwCurTime );
	cclog( "Shop gamble item count : %d\n", int(vShopGItemList.size()) );
	vector< DWORD >::const_iterator endShop = vShopGItemList.end();
	vector< DWORD >::const_iterator	itShop	= vShopGItemList.begin();
	for( ; endShop != itShop; ++itShop )
	{
		const CCMatchGambleItem* pGItem = GetGambleItemByIndex( (*itShop) );
		if( NULL == pGItem )
		{
			_ASSERT( NULL != pGItem );
			cclog( "Invalid GItemIndex : %d\n", (*itShop) );
			continue;
		}

		cclog( "Shop gamble item. ID(%d), Name(%s).\n"
			, pGItem->GetGambleItemID()
			, pGItem->GetName().c_str() );
	}
	cclog( "=== Completed Dump GambleItem Info. ===\n\n" );
}