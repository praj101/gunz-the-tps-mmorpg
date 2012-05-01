#include "stdafx.h"
#include "CCMatchTransDataType.h"
#include "CCMath.h"

void Make_MTDItemNode(CCTD_ItemNode* pout, CCUID& uidItem, unsigned long int nItemID, int nRentMinutePeriodRemainder, int iMaxUseHour, int nCount)
{
	pout->uidItem = uidItem;
	pout->nItemID = nItemID;

	pout->nRentMinutePeriodRemainder = nRentMinutePeriodRemainder;		// 초단위
	pout->iMaxUseHour = iMaxUseHour;									// 최대 사용시간 추가
	pout->nCount = nCount;
}

void Make_MTDAccountItemNode(CCTD_AccountItemNode* pout, int nAIID, unsigned long int nItemID, int nRentMinutePeriodRemainder, int nCount)
{
	pout->nAIID = nAIID;
	pout->nItemID = nItemID;
	pout->nRentMinutePeriodRemainder = nRentMinutePeriodRemainder;		// 초단위
	pout->nCount = nCount;
}


void Make_MTDQuestItemNode( CCTD_QuestItemNode* pOut, const unsigned long int nItemID, const int nCount )
{
	if( 0 == pOut )
		return;

	pOut->m_nItemID			= nItemID;
	pOut->m_nCount			= nCount;
}


void Make_MTDWorldItem(CCTD_WorldItem* pOut, CCMatchWorldItem* pWorldItem)
{
	pOut->nUID = pWorldItem->nUID;
	pOut->nItemID = pWorldItem->nItemID;
	if ( (pWorldItem->nStaticSpawnIndex < 0) && (pWorldItem->nLifeTime > 0) )	
		pOut->nItemSubType = CCTD_Dynamic;
	else
		pOut->nItemSubType = CCTD_Static;
	
	pOut->x = (short)Roundf(pWorldItem->x);
	pOut->y = (short)Roundf(pWorldItem->y);
	pOut->z = (short)Roundf(pWorldItem->z);
}


void Make_MTDActivatedTrap(CCTD_ActivatedTrap *pOut, CCMatchActiveTrap* pTrapItem)
{
	pOut->uidOwner = pTrapItem->m_uidOwner;
	pOut->nItemID = pTrapItem->m_nTrapItemId;
	pOut->nTimeElapsed = MGetMatchServer()->GetGlobalClockCount() - pTrapItem->m_nTimeActivated;
	
	pOut->x = (short)Roundf(pTrapItem->m_vPosActivated.x);
	pOut->y = (short)Roundf(pTrapItem->m_vPosActivated.y);
	pOut->z = (short)Roundf(pTrapItem->m_vPosActivated.z);
}