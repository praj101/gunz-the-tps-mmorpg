#include "StdAfx.h"
#include "ZModule_Resistance.h"

// Added R350a
#include "ZObject.h"
#include "ZCharacterItem.h"

ZModule_Resistance::ZModule_Resistance()
{
}

ZModule_Resistance::~ZModule_Resistance()
{
}

void ZModule_Resistance::OnAdd()
{
	_ASSERT(MIsDerivedFromClass(ZObject,m_pContainer));
}

// 초기화 할때 저항치를 계산해 둔다
void ZModule_Resistance::InitStatus()
{
	m_nFR = 0;
	m_nCR = 0;
	m_nPR = 0;
	m_nLR = 0;

	// 캐릭터가 아니라면 적절한방법으로 구해놓는다
	ZObject *pThisObj = MStaticCast(ZObject,m_pContainer);
	ZCharacterItem *pItems = pThisObj->GetItems();
	for(int i=0;i<MMCIP_END;i++)
	{
		ZItem *pItem = pItems->GetItem((CCMatchCharItemParts)i);
		if(pItem) {
			CCMatchItemDesc *pDesc = pItem->GetDesc();
			if(pDesc) {
				m_nFR += pDesc->m_nFR.Ref();
				m_nCR += pDesc->m_nCR.Ref();
				m_nLR += pDesc->m_nLR.Ref();
				m_nPR += pDesc->m_nPR.Ref();
			}
		}
	}

	m_nFR = max(min(m_nFR,100),0);
	m_nCR = max(min(m_nCR,100),0);
	m_nPR = max(min(m_nPR,100),0);
	m_nLR = max(min(m_nLR,100),0);
}