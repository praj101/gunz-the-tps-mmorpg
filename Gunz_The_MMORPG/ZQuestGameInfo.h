#ifndef _ZQUEST_GAMEINFO_H
#define _ZQUEST_GAMEINFO_H

#include "CCBaseQuest.h"
#include "CCQuestLevel.h"
#include <vector>

// Added R350a
#include "RTypes.h"

using namespace std;

// 퀘스트의 게임 정보
class ZQuestGameInfo
{
private:
	bool							m_bInitialized;
	vector<CCQUEST_NPC>				m_NPCInfoVector;
	vector<CCQuestLevelSectorNode>	m_MapSectorVector;
	int								m_nQL;						// 현재 시나리오의 퀘스트 레벨
	float							m_fNPC_TC;
	int								m_nNPCCount;				// 한 섹터당 등장할 총 NPC수
	int								m_nNPCKilled;				// 한 섹터당 죽은 NPC 수
	int								m_nCurrSectorIndex;			// 현재 게임중인 섹터 인덱스
	int								m_nNumOfObtainQuestItem;	// 퀘스트 팀이 획득한 아이템 갯수
	vector<CCUID>					m_Bosses;					// 보스 UID
	rvector							m_vPortalPos;				// 포탈의 위치
	int								m_nMapsetID;
	unsigned int					m_nScenarioID;
	int								m_nRepeat;					// 시나리오 반복할 횟수 (서바이벌용)
	int								m_nRepeated;				// 현재 반복한 횟수 (서바이벌용)
public:
	ZQuestGameInfo();
	~ZQuestGameInfo();
	void Init(CCTD_QuestGameInfo* pMTDQuestGameInfo);
	void Final();
	void OnMovetoNewSector(int nSectorIndex, int nRepeated);				// 새로운 섹터로 이동

	// interface func
	int GetNPCInfoCount();
	int GetMapSectorCount();
	CCQUEST_NPC GetNPCInfo(int index);
	int GetMapSectorID(int index);
	int GetMapSectorLink(int index);
	bool IsInited();
	inline float GetNPC_TC();
	inline bool IsCurrSectorLastSector();			// 현재 섹터가 마지막 섹터인지 여부
	inline bool IsLastSectorInSurvival();			// 현재 섹터가 서바이벌 모드에서 모든 반복을 끝낸 마지막 섹터인지 여부
	int GetCurrSectorIndex()				{ return m_nCurrSectorIndex; }
	int GetCurrRepeatIndex()				{ return m_nRepeated; }
	int GetRepeatCount()					{ return m_nRepeat; }
	int GetNPCCount( void)					{ return m_nNPCCount; }
	int GetNPCKilled( void)					{ return m_nNPCKilled; }
	void ClearNPCKilled( void)				{ m_nNPCKilled = 0; }
	void IncreaseNPCKilled( void)			{ m_nNPCKilled++; }
	void SetQuestLevel( int nQL)			{ m_nQL = nQL; }
	void SetMapsetID( unsigned int nID)		{ m_nMapsetID = nID; }
	unsigned int GetMapsetID()				{ return m_nMapsetID; }
	void SetSenarioID( unsigned int nID)	{ m_nScenarioID = nID; }
	unsigned int GetSenarioID()				{ return m_nScenarioID; }
	int GetQuestLevel( void)				{ return m_nQL; }
	int GetNumOfObtainQuestItem( void)		{ return m_nNumOfObtainQuestItem; }
	void IncreaseObtainQuestItem( void)		{ m_nNumOfObtainQuestItem++; }
	vector<CCUID>& GetBosses()			{ return m_Bosses; }
	CCUID GetBoss();
	void SetPortalPos(rvector& pos)			{ m_vPortalPos = pos; }
	rvector GetPortalPos()					{ return m_vPortalPos; }
};



/////////////////////////////////////////////////////////////////////////////////////
inline int ZQuestGameInfo::GetNPCInfoCount() 
{ 
	return (int)m_NPCInfoVector.size(); 
}
inline int ZQuestGameInfo::GetMapSectorCount() 
{ 
	return (int)m_MapSectorVector.size(); 
}
inline CCQUEST_NPC ZQuestGameInfo::GetNPCInfo(int index) 
{ 
	return m_NPCInfoVector[index]; 
}
inline int ZQuestGameInfo::GetMapSectorID(int index) 
{ 
	return m_MapSectorVector[index].nSectorID; 
}
inline int ZQuestGameInfo::GetMapSectorLink(int index)
{
	return m_MapSectorVector[index].nNextLinkIndex;
}

inline bool ZQuestGameInfo::IsInited()
{
	return m_bInitialized; 
}

inline float ZQuestGameInfo::GetNPC_TC()
{
	return m_fNPC_TC;
}

bool ZQuestGameInfo::IsCurrSectorLastSector()
{
	if (GetMapSectorCount() <= (m_nCurrSectorIndex+1)) return true;
	return false;
}

bool ZQuestGameInfo::IsLastSectorInSurvival()
{
	if (GetMapSectorCount() <= m_nCurrSectorIndex+1 &&
		m_nRepeat <= m_nRepeated+1)
		return true;
	return false;
}

#endif