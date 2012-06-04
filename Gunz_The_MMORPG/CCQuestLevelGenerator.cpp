#include "stdafx.h"
#include "CCQuestLevelGenerator.h"
#include "CCQuestLevel.h"
#include "CCMath.h"
#include "CCMatchServer.h"
//#include "MSacrificeQItemTable.h"

///////////////////////////////////////////////////////////////////////////////
CCQuestLevelGenerator::CCQuestLevelGenerator(CCMATCH_GAMETYPE eGameType) : 
m_eGameType(eGameType),
m_nPlayerQL(0), m_nMapsetID(-1), m_nScenarioID(-1)
{
	memset(m_nSacriQItemID, 0, sizeof(unsigned int) * MAX_SCENARIO_SACRI_ITEM);
	
	ASSERT(CCGetGameTypeMgr()->IsQuestDerived(m_eGameType));
}

CCQuestLevelGenerator::~CCQuestLevelGenerator()
{

}

void CCQuestLevelGenerator::BuildPlayerQL(int nPlayerQL)
{
	m_nPlayerQL = nPlayerQL;
}

void CCQuestLevelGenerator::BuildMapset(int nMapsetID)
{
	m_nMapsetID = nMapsetID;
}

void CCQuestLevelGenerator::BuildSacriQItem(unsigned int nItemID)
{
	for (int i = 0; i < MAX_SACRIFICE_SLOT_COUNT; i++)
	{
		if ((m_nSacriQItemID[i] == 0) || (i == (MAX_SCENARIO_SACRI_ITEM-1)))
		{
			m_nSacriQItemID[i] = nItemID;
			break;
		}
	}
}

CCQuestLevel* CCQuestLevelGenerator::MakeLevel()
{
	// 시나리오 결정
	m_nScenarioID = MakeScenarioID();


	// 주사위 굴림
	int dice = (int)Dice(1, SCENARIO_STANDARD_DICE_SIDES, 0);

	CCQuestLevel* pNewLevel = new CCQuestLevel();
	pNewLevel->Init(m_nScenarioID, dice, m_eGameType);

	return pNewLevel;
}


int CCQuestLevelGenerator::MakeScenarioID()
{
	CCQuestScenarioCatalogue* pScenarioCatalog = NULL;
	if (CCGetGameTypeMgr()->IsQuestOnly(m_eGameType))
	{
		pScenarioCatalog = CCMatchServer::GetInstance()->GetQuest()->GetScenarioCatalogue();
	}
	else if (CCGetGameTypeMgr()->IsSurvivalOnly(m_eGameType))
	{
		pScenarioCatalog = CCMatchServer::GetInstance()->GetQuest()->GetSurvivalScenarioCatalogue();
	}
	else
	{
		ASSERT(0);
		return 0;
	}

	int id = pScenarioCatalog->MakeScenarioID(m_nMapsetID, m_nPlayerQL, m_nSacriQItemID);

	// 시나리오가 없으면 기본 시나리오.
	if (!pScenarioCatalog->GetInfo(id))
	{
		id = pScenarioCatalog->GetDefaultStandardScenarioID();
	}

	return id;
}

int CCQuestLevelGenerator::ReturnScenarioID()
{
	return MakeScenarioID();
}




