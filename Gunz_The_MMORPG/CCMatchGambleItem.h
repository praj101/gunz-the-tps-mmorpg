#pragma once
#include "CCMatchBaseGambleItem.h"


#include "CCUID.h"


#include <list>

using std::list;



class CCMatchCharGambleItem : public CCMatchBaseGambleItem
{
private :
	DWORD m_dwCIID;

public :
	CCMatchCharGambleItem( const CCUID& uidItem, const DWORD dwCIID, const DWORD dwGambleItemID, const int nItemCount) 
		: CCMatchBaseGambleItem( uidItem, dwGambleItemID, nItemCount )
	{
		m_dwCIID = dwCIID;
	}

	const DWORD GetCIID() const			{ return m_dwCIID; }
};


class CCMatchGambleItemManager
{
private :
	list< CCMatchCharGambleItem* > m_GambleItemList;

public :
	CCMatchGambleItemManager() {}
	~CCMatchGambleItemManager() { Release(); }

	const bool					AddGambleItem( const CCUID& uidItem, const DWORD dwCIID, const DWORD dwGambleItemID, const int nItemCount = 1 );

	const DWORD					GetCount() const								{ return static_cast<DWORD>(m_GambleItemList.size()); }

	const CCMatchCharGambleItem*	GetGambleItemByUID( const CCUID& uidItem ) const;
	const CCMatchCharGambleItem*	GetGambleItemByCIID( const DWORD CIID ) const;
	const CCMatchCharGambleItem*	GetGambleItemByIndex( const DWORD dwIndex ) const;
	const CCMatchCharGambleItem*	GetGambleItemByItemID( const DWORD dwItemID ) const;

	bool						SetGambleItemCount( int nCIID, int nItemCount);
	bool						SetGambleItemCount( const CCUID& uidItem, int nItemCount);	
	void						DeleteGambleItem( const CCUID& uidItem );
	void						Release();
};
