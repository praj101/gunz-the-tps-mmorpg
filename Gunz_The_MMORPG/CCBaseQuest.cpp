#include "stdafx.h"
#include "CCBaseQuest.h"




CCBaseQuest::CCBaseQuest() : m_bCreated(false)
{

}

CCBaseQuest::~CCBaseQuest()
{
	if (m_bCreated) Destroy();
}

bool CCBaseQuest::Create()
{
	if (m_bCreated) return true;

	bool ret = OnCreate();
	if (ret) m_bCreated = true;

	return ret;
}

void CCBaseQuest::Destroy()
{
	OnDestroy();
	m_bCreated = false;
}

bool CCBaseQuest::OnCreate()
{
	// 각각의 xml 로딩은 Client, Server 각각의 상속받은 클래스가 담당하도록 한다.

#ifdef _QUEST_ITEM
	return true;
#endif
	return false;
}

void CCBaseQuest::OnDestroy()
{
}


void CCBaseQuest::ProcessNPCDropTableMatching()
{
	for (CCQuestNPCCatalogue::iterator itor = m_NPCCatalogue.begin(); itor != m_NPCCatalogue.end(); ++itor)
	{
		CCQuestNPCInfo* pNPCInfo = (*itor).second;

		if (pNPCInfo->szDropTableName[0] != 0)
		{
			for (CCQuestDropTable::iterator itorDT = m_DropTable.begin(); itorDT != m_DropTable.end(); ++itorDT)
			{
				CCQuestDropSet* pDropSet = (*itorDT).second;
				if (!stricmp(pDropSet->GetName(), pNPCInfo->szDropTableName))
				{
					pNPCInfo->nDropTableID = pDropSet->GetID();
					break;
				}
			}
		}
	}
}