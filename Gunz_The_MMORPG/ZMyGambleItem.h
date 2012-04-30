#ifndef _ZMYGAMBLEITEM_H
#define _ZMYGAMBLEITEM_H


#include "CCMatchBaseGambleItem.h"


#include <vector>

using std::vector;


class ZGambleItemDefine;


class ZMyGambleItem : public CCMatchBaseGambleItem
{
private :
	ZMyGambleItem() {}	

public :
	ZMyGambleItem( const CCUID& uidItem, const DWORD dwGambleItemID, const int nItemCount = 1);
	~ZMyGambleItem();

	const ZGambleItemDefine* GetDesc() const;
};



class ZMyGambleItemManager
{
private :
	vector< ZMyGambleItem* > m_GambleItemList;

public :
	ZMyGambleItemManager() {}
	~ZMyGambleItemManager();

	bool CreateGambleItem( const CCUID& uidItem, const DWORD dwGambleItemID, const int nitemCount = 1 );
	void DeleteGambleItem( const CCUID& uidItem );

	const DWORD				GetSize() const										{ return static_cast<DWORD>(m_GambleItemList.size()); }
	const ZMyGambleItem*	GetGambleItemByIndex( const DWORD dwIndex );
	const ZMyGambleItem*	GetGambleItem( const CCUID& uidItem );

	void Release();
};
#endif