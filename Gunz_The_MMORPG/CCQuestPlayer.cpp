#include "stdafx.h"
#include "CCQuestPlayer.h"
#include "CCMatchStage.h"
#include "CCMatchObject.h"
#include "CCMatchServer.h"
#include "CCSharedCommandTable.h"
#include "CCMatchRuleQuest.h"
#include "CCQuestPlayer.h"
#include "CCQuestFormula.h"


CCQuestPlayerManager::CCQuestPlayerManager() : m_pStage(0)
{

}

CCQuestPlayerManager::~CCQuestPlayerManager()
{
	
}

void CCQuestPlayerManager::Create(CCMatchStage* pStage)
{
	m_pStage = pStage;

	for (CCUIDRefCache::iterator itorObj = pStage->GetObjBegin(); itorObj != pStage->GetObjEnd(); ++itorObj)
	{
		CCUID uidChar = (*itorObj).first;

		CCMatchObject* pObj = CCMatchServer::GetInstance()->GetObject(uidChar);
		if (IsAdminGrade(pObj) && pObj->CheckPlayerFlags(CCTD_PlayerFlags_AdminHide)) continue;
		

		AddPlayer(uidChar);
	}
}

void CCQuestPlayerManager::Destroy()
{
	Clear();
}

void CCQuestPlayerManager::AddPlayer(CCUID& uidPlayer)
{
	CCQuestPlayerInfo* pPlayerInfo = new CCQuestPlayerInfo();

	CCMatchObject* pPlayerObject = CCMatchServer::GetInstance()->GetObject(uidPlayer);
	if (IsEnabledObject(pPlayerObject))
	{
		int nPlayerLevel = pPlayerObject->GetCharInfo()->m_nLevel;
		int nQL = CCQuestFormula::CalcQL(nPlayerLevel);

		pPlayerInfo->Init(pPlayerObject, nQL);

		insert(value_type(uidPlayer, pPlayerInfo));
	}
}

void CCQuestPlayerManager::DelPlayer(CCUID& uidPlayer)
{
	CCQuestPlayerInfo* pDelPlayerInfo = NULL;

	iterator itor = find(uidPlayer);
	if (itor != end())
	{
		pDelPlayerInfo = (*itor).second;
		delete pDelPlayerInfo;

		erase(itor);
	}
}

void CCQuestPlayerManager::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		delete (*itor).second;
	}

	clear();
}

CCQuestPlayerInfo* CCQuestPlayerManager::GetPlayerInfo(const CCUID& uidPlayer)
{
	iterator itor = find(uidPlayer);
	if (itor != end())
	{
		return (*itor).second;
	}
	return NULL;
}