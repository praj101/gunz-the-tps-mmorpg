#include "stdafx.h"
#include "CCMatchObjCache.h"
#include "CCMatchObject.h"
#include "CCSharedCommandTable.h"
#include "CCCommandCommunicator.h"
#include "CCBlobArray.h"
#include "CCMatchTransDataType.h"
#include "CCMatchServer.h" 


CCMatchObjectCacheBuilder::CCMatchObjectCacheBuilder()
{
}

CCMatchObjectCacheBuilder::~CCMatchObjectCacheBuilder()
{
	Reset();
}

void CCMatchObjectCacheBuilder::AddObject(CCMatchObject* pObj)
{
	CCMatchObjCache* pCache = new CCMatchObjCache;
	int nItemID=0;
	CCMatchItemDesc* pItemDesc = NULL;

	CCMatchCharInfo* pCharInfo = pObj->GetCharInfo();
//	_ASSERT(pCharInfo);
	if (pCharInfo == NULL)
	{
		delete pCache;
		return;
	}

	CCMatchObjectDuelTournamentCharInfo* pDTCharInfo = pObj->GetDuelTournamentCharInfo();
	int nDTLastWeekGrade = pDTCharInfo ? pDTCharInfo->GetLastWeekGrade() : 0;

	pCache->SetInfo(pObj->GetUID(), pObj->GetName(), pCharInfo->m_ClanInfo.m_szClanName, 
					pCharInfo->m_nLevel, pObj->GetAccountInfo()->m_nUGrade, pObj->GetAccountInfo()->m_nPGrade
					, pCharInfo->m_nRank, pCharInfo->m_nTotalKillCount, pCharInfo->m_nTotalDeathCount, nDTLastWeekGrade );
	pCache->SetCLID(pObj->GetCharInfo()->m_ClanInfo.m_nClanID);

	CCMatchClan* pClan = CCMatchServer::GetInstance()->GetClanMap()->GetClan(pObj->GetCharInfo()->m_ClanInfo.m_nClanID);
	if (pClan)
		pCache->SetEmblemChecksum(pClan->GetEmblemChecksum());
	else
		pCache->SetEmblemChecksum(0);

	pCache->GetCostume()->nSex = pObj->GetCharInfo()->m_nSex;
	pCache->GetCostume()->nHair = pObj->GetCharInfo()->m_nHair;
	pCache->GetCostume()->nFace = pObj->GetCharInfo()->m_nFace;

	for (int i=0; i < MMCIP_END; i++)
	{
		if (!pObj->GetCharInfo()->m_EquipedItem.IsEmpty(CCMatchCharItemParts(i)))
		{
			pCache->GetCostume()->nEquipedItemID[i] =
				pObj->GetCharInfo()->m_EquipedItem.GetItem(CCMatchCharItemParts(i))->GetDescID();
		}
		else
		{
			pCache->GetCostume()->nEquipedItemID[i] = 0;
		}
	}

	pCache->SetFlags(pObj->GetPlayerFlags());

	m_ObjectCacheList.push_back(pCache);
}

void CCMatchObjectCacheBuilder::Reset()
{
	CCMatchObjCacheList::iterator itor;
	while ( (itor = m_ObjectCacheList.begin()) != m_ObjectCacheList.end()) {
		CCMatchObjCache* pObjCache = (*itor);
		m_ObjectCacheList.pop_front();
		delete pObjCache;
	}
}

MCommand* CCMatchObjectCacheBuilder::GetResultCmd(MATCHCACHEMODE nMode, MCommandCommunicator* pCmdComm)
{
	MCommand* pCmd = pCmdComm->CreateCommand(MC_MATCH_OBJECT_CACHE, CCUID(0,0));
	pCmd->AddParameter(new MCmdParamUChar(nMode));
	int nCount = (int)m_ObjectCacheList.size();
	void* pCacheArray = MMakeBlobArray(sizeof(CCMatchObjCache), nCount);
	int nIndex=0;
	for (CCMatchObjCacheList::iterator itor=m_ObjectCacheList.begin(); itor!=m_ObjectCacheList.end(); itor++) {
		CCMatchObjCache* pTrgCache = (CCMatchObjCache*)MGetBlobArrayElement(pCacheArray, nIndex++);
		CCMatchObjCache* pSrcCache = (*itor);

		
		pTrgCache->SetInfo(pSrcCache->GetUID(), pSrcCache->GetName(), pSrcCache->GetClanName(),
						   pSrcCache->GetLevel(), pSrcCache->GetUGrade(), pSrcCache->GetPGrade()
						   , pSrcCache->GetRank(), pSrcCache->GetKillCount(), pSrcCache->GetDeathCount(), pSrcCache->GetDTGrade() );

		pTrgCache->SetFlags(pSrcCache->GetFlags());
		pTrgCache->SetCLID(pSrcCache->GetCLID());
		pTrgCache->SetEmblemChecksum(pSrcCache->GetEmblemChecksum());

		pTrgCache->AssignCostume(pSrcCache->GetCostume());
	}
	pCmd->AddParameter(new MCmdParamBlob(pCacheArray, MGetBlobArraySize(pCacheArray)));
	MEraseBlobArray(pCacheArray);

	return pCmd;
}
