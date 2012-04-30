#pragma once

// 이곳에는 퀘스트관련 클라이언트와 서버가 공통으로 사용하는 것을 넣도록 한다.
//////////////////////////////////////////////////////////////////////////////

#include "CCQuestNPC.h"
#include "CCQuestMap.h"
#include "CCQuestScenario.h"
//#include "MSurvivalMap.h"
#include "CCQuestDropTable.h"

#include <map>
using namespace std;


struct CCQuestNPCInfo;

/// 퀘스트 월드레벨에서 사용하는 섹터 노드
struct CCQuestLevelSectorNode
{
	int		nSectorID;
	int		nNextLinkIndex;

	// 여기에 추가정보 들어갈듯
};

////////////////////////////////////////////////////////////////////////////////////////////
/// 서버와 클라이언트 퀘스트 최고 관장클래스의 공통 부모 클래스
class CCBaseQuest
{
private:
	bool m_bCreated;
protected:
	
	CCQuestMapCatalogue			m_MapCatalogue;								///< 퀘스트 맵 정보
	CCQuestMapCatalogue			m_SurvivalMapCatalogue;						///< 서바이벌 맵 정보
	CCQuestNPCCatalogue			m_NPCCatalogue;								///< NPC 정보
	CCQuestDropTable				m_DropTable;								///< 드롭 테이블 정보
	virtual bool OnCreate();												///< Create()호출시 불리는 함수
	virtual void OnDestroy();												///< Destroy()호출시 불리는 함수

	void ProcessNPCDropTableMatching();		// npc.xml의 Droptable을 매칭시킨다.
public:
	CCBaseQuest();															///< 생성자
	virtual ~MBaseQuest();													///< 소멸자
	inline CCQuestMapSectorInfo*		GetSectorInfo(int nSectorID);			///< 섹터 정보 반환 - 퀘스트
	inline CCQuestMapSectorInfo*		GetSurvivalSectorInfo(int nSectorID);	///< 섹터 정보 반환 - 서바이벌
	inline CCQuestNPCInfo*			GetNPCInfo(MQUEST_NPC nNPC);			///< NPC 정보 반환
	inline CCQuestNPCInfo*			GetNPCPageInfo( int nPage);				///< NPC 정보 반환
	//inline MSurvivalMapInfo*		GetSurvivalMapInfo(MSURVIVAL_MAP nMap);	///< 서바이벌 모드용 맵 정보 반환
	inline CCQuestDropTable*			GetDropTable();							///< 퀘스트 아이템 드롭 테이블 정보 반환
	bool Create();															///< 초기화
	int GetNumOfPage( void)			{ return (int)m_NPCCatalogue.size(); }
	void Destroy();															///< 해제

	CCQuestNPCInfo* GetNPCIndexInfo( int nMonsterBibleIndex )
	{
		return m_NPCCatalogue.GetIndexInfo( nMonsterBibleIndex );
	}

	inline CCQuestMapCatalogue* GetMapCatalogue();
	inline CCQuestMapCatalogue* GetSurvivalMapCatalogue();
	inline CCQuestNPCCatalogue* GetNPCCatalogue();
};





// inline functions //////////////////////////////////////////////////////////////////////////
inline CCQuestMapSectorInfo* CCBaseQuest::GetSectorInfo(int nSectorID)
{
	return m_MapCatalogue.GetSectorInfo(nSectorID);
}

inline CCQuestMapSectorInfo* CCBaseQuest::GetSurvivalSectorInfo(int nSectorID)
{
	return m_SurvivalMapCatalogue.GetSectorInfo(nSectorID);
}

inline CCQuestNPCInfo* CCBaseQuest::GetNPCInfo(CCQUEST_NPC nNPC)
{
	return m_NPCCatalogue.GetInfo(nNPC);
}

inline CCQuestNPCInfo* CCBaseQuest::GetNPCPageInfo( int nPage)
{
	return m_NPCCatalogue.GetPageInfo(nPage);
}

//inline MSurvivalMapInfo* MBaseQuest::GetSurvivalMapInfo(MSURVIVAL_MAP nMap)
//{
//	return m_SurvivalMapCatalogue.GetInfo(nMap);
//}

inline CCQuestDropTable* CCBaseQuest::GetDropTable()
{
	return &m_DropTable;
}

inline CCQuestMapCatalogue* CCBaseQuest::GetMapCatalogue()
{
	return &m_MapCatalogue;
}

inline CCQuestMapCatalogue* CCBaseQuest::GetSurvivalMapCatalogue()
{
	return &m_SurvivalMapCatalogue;
}

inline CCQuestNPCCatalogue* CCBaseQuest::GetNPCCatalogue()
{
	return &m_NPCCatalogue;
}
