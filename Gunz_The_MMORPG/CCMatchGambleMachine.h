#pragma once
#include <map>
#include <vector>


using std::map;
using std::vector;


// #define MAX_GAMBLE_RATE (1000 - 1) // 1000%가 최대지만 계산을 할때는 0부터 시작을 하기때문에 0 ~ 999까지가 된다.
static const DWORD MAX_GAMBLE_RATE = (1000 - 1); // 1000%가 최대지만 계산을 할때는 0부터 시작을 하기때문에 0 ~ 999까지가 된다.


class CCMatchGambleItem;
class CCMatchGambleRewardItem;


class CCMatchGambleMachine
{
private :
	map< DWORD, CCMatchGambleItem* >	m_GambleItemMap;
	vector< CCMatchGambleItem* >		m_GambleItemVec;
	DWORD							m_dwLastUpdateTime;

private :
	const CCMatchGambleRewardItem*	GetGambleRewardItem( const DWORD dwGambleItemID, const WORD wRate ) const;
									// 겜블 아이템 정책에서만 사용을 해야 한다.
	const bool						CheckGambleItemIsSelling( const int nStartTimeMin
															, const int nEndTimeMin
															, const int nCurTimeMin
															, const bool bIsNoTimeLimit  ) const;
	
public :
	CCMatchGambleMachine();
	~CCMatchGambleMachine(); 

									// Create함수가 호출되면 무조건 리스트를 초기화 하고 다시 구성한다.
									// 리스트 구성의 안전성을 위해서 하나씩 추가 하는 함수는 없음.
	bool							CreateGambleItemListWithGambleRewardList( vector<CCMatchGambleItem*>& vGambleItemList
																		, vector<CCMatchGambleRewardItem*>& vGambleRewardItemList );
	bool							CreateGambleItemList( vector<CCMatchGambleItem*>& vGambleItemList );


	void							Release();

	const DWORD						GetGambleItesSize() const { return static_cast<DWORD>(m_GambleItemVec.size()); }
	const CCMatchGambleItem*			GetGambleItemByIndex( const DWORD dwIndex ) const;
	const CCMatchGambleItem*			GetGambleItemByGambleItemID( const DWORD dwGambleItemID ) const;
	const CCMatchGambleItem*			GetGambleItemByName( const string& strGambleItemName ) const;

	const CCMatchGambleRewardItem*	Gamble( const DWORD dwGambleItemID ) const;

	// 겜블 아이템의 사용 시간을 검사해서 보내야 하는 겜블 아이템을 인자로 넘어온 백터에 담아준다. //
	void							GetItemVectorByCheckedItemTime(vector<DWORD>& outItemIndexVec, const DWORD dwCurTime) const;
	void							GetOpenedGambleItemList( vector<DWORD>& outGItemList ) const;
	const bool						IsItTimeoverEventGambleItem( const DWORD dwGambleItemID, const DWORD dwCurTime ) const;	
	const DWORD						GetLastUpdateTime() const { return m_dwLastUpdateTime; }

	void							SetLastUpdateTime( const DWORD dwCurTime ) { m_dwLastUpdateTime = dwCurTime; }

	void							WriteGambleItemInfoToLog() const;
};