#include "CCBaseQuest.h"
#include "CCSurvivalRankInfo.h"

/// 서버용 퀘스트 최고 관장 클래스
class CCMatchQuest : public CCBaseQuest
{
protected:
	virtual bool OnCreate();				///< 초기화
	virtual void OnDestroy();				///< 해제
	

	CCQuestNPCSetCatalogue		m_NPCSetCatalogue;						///< NPC Set 정보
	CCQuestScenarioCatalogue		m_ScenarioCatalogue;					///< 퀘스트 시나리오 정보
	CCQuestScenarioCatalogue		m_SurvivalScenarioCatalogue;			///< 서바이벌 시나리오 정보
	CCSurvivalRankInfo			m_SurvivalRankInfo;						///< 서바이벌 상위권 랭킹 목록

public:
	CCMatchQuest();														///< 생성자
	virtual ~CCMatchQuest();												///< 소멸자

	inline CCQuestNPCSetInfo* GetNPCSetInfo(int nID);					///< NPC Set 정보 반환
	inline CCQuestNPCSetInfo* GetNPCSetInfo(const char* szName);			///< NPC Set 정보 반환
	inline CCQuestScenarioCatalogue* GetScenarioCatalogue();				///< 퀘스트 시나리오 정보 반환
	inline CCQuestScenarioInfo*		GetScenarioInfo(int nScenarioID);	///< 퀘스트 시나리오 정보 반환
	inline CCQuestScenarioCatalogue* GetSurvivalScenarioCatalogue();				///< 서바이벌 시나리오 정보 반환
	inline CCQuestScenarioInfo*		GetSurvivalScenarioInfo(int nScenarioID);	///< 서바이벌 시나리오 정보 반환
	inline CCSurvivalRankInfo*		GetSurvivalRankInfo();						///< 서바이벌 랭킹 목록 반환

};




inline CCQuestNPCSetInfo* CCMatchQuest::GetNPCSetInfo(int nID)
{
	return m_NPCSetCatalogue.GetInfo(nID);
}

inline CCQuestNPCSetInfo* CCMatchQuest::GetNPCSetInfo(const char* szName)
{
	return m_NPCSetCatalogue.GetInfo(szName);
}

inline CCQuestScenarioCatalogue* CCMatchQuest::GetScenarioCatalogue()
{
	return &m_ScenarioCatalogue;
}

inline CCQuestScenarioInfo* CCMatchQuest::GetScenarioInfo(int nScenarioID)
{
	return m_ScenarioCatalogue.GetInfo(nScenarioID);
}

inline CCQuestScenarioCatalogue* CCMatchQuest::GetSurvivalScenarioCatalogue()
{
	return &m_SurvivalScenarioCatalogue;
}

inline CCQuestScenarioInfo* CCMatchQuest::GetSurvivalScenarioInfo(int nScenarioID)
{
	return m_SurvivalScenarioCatalogue.GetInfo(nScenarioID);
}

inline CCSurvivalRankInfo* CCMatchQuest::GetSurvivalRankInfo()
{
	return &m_SurvivalRankInfo;
}
