#include "stdafx.h"
#include "CCQuestNPCSpawnTrigger.h"
#include "CCMath.h"
#include "CCQuestNPC.h"
#include "CCQuestConst.h"


CCQuestNPCSpawnTrigger::CCQuestNPCSpawnTrigger() : m_nLastTime(0), m_fRateSum(0.0f)
{
	
}

CCQuestNPCSpawnTrigger::~CCQuestNPCSpawnTrigger()
{

}

void CCQuestNPCSpawnTrigger::BuildNPCInfo(SpawnTriggerNPCInfoNode& NPCInfo)
{
	m_NPCInfo.push_back(NPCInfo);
	m_fRateSum += NPCInfo.fRate;
}

void CCQuestNPCSpawnTrigger::BuildCondition(SpawnTriggerInfo& Info)
{
	m_Info = Info;
}


bool CCQuestNPCSpawnTrigger::CheckSpawnEnable(int nCurrNPCCount)
{
	unsigned int nNowTime = timeGetTime();

	if (CCGetTimeDistance(m_nLastTime, nNowTime) < m_Info.nSpawnTickTime) return false;

	m_nLastTime = nNowTime;


	if ((nCurrNPCCount >= m_Info.nCurrMinNPCCount) && (nCurrNPCCount <= m_Info.nCurrMaxNPCCount))
	{
		MakeSpawnNPCs();

		return true;
	}

	return false;
}

void CCQuestNPCSpawnTrigger::MakeSpawnNPCs()
{
	m_NPCQueue.clear();
	m_NPCQueue.reserve(m_Info.nSpawnNPCCount);

	for (int i = 0; i < m_Info.nSpawnNPCCount; i++)
	{

		m_NPCQueue.push_back(GetRandomNPC());
	}
}

void CCQuestNPCSpawnTrigger::Clear()
{
	m_NPCInfo.clear();
	m_NPCQueue.clear();
	memset(&m_Info, 0, sizeof(SpawnTriggerInfo));
	m_nLastTime = 0;
	m_fRateSum = 0.0f;
}

CCQUEST_NPC CCQuestNPCSpawnTrigger::GetRandomNPC()
{
	float fRandNum = RandomNumber(0.001f, m_fRateSum);
	float f = 0.0f;

	for (vector<SpawnTriggerNPCInfoNode>::iterator itor = m_NPCInfo.begin(); itor != m_NPCInfo.end(); ++itor)
	{
		f += (*itor).fRate;

		if (fRandNum <= f) return (*itor).nNPCID;
	}

	CCQUEST_NPC nDefaultNPC = NPC_GOBLIN;
	if (!m_NPCInfo.empty()) nDefaultNPC = (*m_NPCInfo.begin()).nNPCID;

	return nDefaultNPC;
}