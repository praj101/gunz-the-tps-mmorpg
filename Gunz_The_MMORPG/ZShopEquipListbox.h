#pragma once
#include "CCMultiColListBox.h"
#include "ZShopEquipItem.h"

class ZShopEquipListItem : public MMultiColListItem
{
	ZShopEquipItem* m_pItemData;
	char m_szName[256];
	char m_szLevel[256];
	char m_szPrice[256];

public:
	ZShopEquipListItem(ZShopEquipItem* pItemData);
	virtual ~ZShopEquipListItem();

	ZShopEquipItem* GetItemData() { return m_pItemData; }

	virtual void OnDraw(sRect& r, CCDrawContext* pDC, bool bSelected, bool bMouseOver);
	virtual const char* GetString() { return m_szName; }

	virtual bool GetDragItem(CCBitmap** ppDragBitmap, char* szDragString, char* szDragItemString);
	virtual int GetSortHint();
	void GetIconRect(sRect& out, const sRect& rcItem);
	bool IsPtInRectToShowToolTip(sRect& rcItem, MPOINT& pt);
};


class ZShopEquipListbox : public MMultiColListBox
{
	int m_idxItemLastTooltip;
public:
	ZShopEquipListbox(const char* szName, CCWidget* pParent=NULL, CCListener* pListener=NULL);

	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);

	void SetupItemDescTooltip();
	char* GetItemDescTooltipName();

public:
#define MINT_EQUIPMENTLISTBOX	"EquipmentListBox"
	virtual const char* GetClassName(void){ return MINT_EQUIPMENTLISTBOX; }

};


void ShopPurchaseItemListBoxOnDrop(void* pSelf, CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString);
void ShopSaleItemListBoxOnDrop(void* pSelf, CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString);
void CharacterEquipmentItemListBoxOnDrop(void* pSelf, CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString);


CCListener* ZGetShopListFilterListener(void);
CCListener* ZGetEquipListFilterListener(void);

CCListener* ZGetShopSellItemListBoxListener(void);
//CCListener* ZGetCashShopItemListBoxListener(void);
CCListener* ZGetShopPurchaseItemListBoxListener(void);
CCListener* ZGetEquipmentMyItemListBoxListener(void);
CCListener* ZGetAccountItemListBoxListener(void);