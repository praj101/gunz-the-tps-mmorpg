#include "stdafx.h"
#include "CCMatchQuest.h"

#define FILENAME_NPC_DESC				"npc.xml"
#define FILENAME_SCENARIO				"scenario.xml"
#define FILENAME_SURVIVALSCENARIO		"survivalScenario.xml"
#define FILENAME_QUESTMAP				"questmap.xml"
#define FILENAME_SURVIVALMAP			"survivalmap.xml"
#define FILENAME_NPCSET_DESC			"npcset.xml"
#define FILENAME_DROPTABLE				"droptable.xml"


CCMatchQuest::CCMatchQuest() : CCBaseQuest()
{

}

CCMatchQuest::~CCMatchQuest()
{

}

bool CCMatchQuest::OnCreate()
{
	if (!m_DropTable.ReadXml(FILENAME_DROPTABLE))
	{
		cclog("Droptable Read Failed");
		return false;
	}
	if (!m_NPCCatalogue.ReadXml(FILENAME_NPC_DESC))
	{
		cclog("Read NPC Catalogue Failed");
		return false;
	}

	ProcessNPCDropTableMatching();

	if (!m_NPCSetCatalogue.ReadXml(FILENAME_NPCSET_DESC))
	{
		cclog("Read NPCSet Catalogue Failed");
		return false;
	}

	if (!m_ScenarioCatalogue.ReadXml(FILENAME_SCENARIO))
	{
		cclog("Read Scenario Catalogue Failed");
		return false;
	}
	
	if (!m_MapCatalogue.ReadXml(FILENAME_QUESTMAP))
	{
		cclog("Read Questmap Catalogue Failed");
		return false;
	}

	if (!m_SurvivalScenarioCatalogue.ReadXml(FILENAME_SURVIVALSCENARIO))
	{
		cclog("Read Survival-Scenario Catalogue Failed");
		return false;
	}

	if (!m_SurvivalMapCatalogue.ReadXml(FILENAME_SURVIVALMAP))
	{
		cclog("Read Questmap Catalogue Failed");
		return false;
	}

#ifdef _DEBUG
	m_MapCatalogue.DebugReport();
	m_SurvivalMapCatalogue.DebugReport();
#endif

	// 최근 서바이벌 랭킹 리스트 요청
	CCMatchServer::GetInstance()->OnRequestSurvivalModeGroupRanking();
	return true;
}

void CCMatchQuest::OnDestroy()
{

}

