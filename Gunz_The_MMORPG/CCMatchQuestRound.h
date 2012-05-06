#pragma once
class CCMatchQuestRound
{
private:
	int		m_nRound;
public:
	CCMatchQuestRound();
	~CCMatchQuestRound();
	
	void Increase();
	void Reset();

	int ClearConditionNPCCount(int nPlayerCount);
	int SpawnTime();
	int MaxCurrNPCCount(int nPlayerCount);
	CCQUEST_NPC RandomNPC();
	int GetRandomSpawnPosIndex();

	int GetRound()		{ return m_nRound; }
};