#pragma once

#include "CCMatchNPCObject.h"

#include <map>
using std::map;

struct RewardZItemInfo
{
	unsigned int		nItemID;
	int					nRentPeriodHour;
	int					nItemCnt;
};

class CCQuestRewardZItemList : public list<RewardZItemInfo>
{
};

/// 퀘스트 룰에서 쓰이는 플레이 정보
struct CCQuestPlayerInfo
{
	// NPC Control 관련 /////////
	CCMatchObject*		pObject;					///< Object 정보
	unsigned long int	nNPCControlCheckSum;		///< NPC 조종 체크섬
	CCMatchNPCObjectMap	NPCObjects;					///< 조종중인 NPC
	bool				bEnableNPCControl;			///< NPC Control이 가능한지 여부

	/// NPC 관리 점수
	/// - 스코어가 낮을수록 우선순위가 높아짐
	int GetNPCControlScore()						
	{
		// 지금은 그냥 조종하는 NPC 개수
		return (int)(NPCObjects.size());
	}


	// 퀘스트 룰 관련 ///////////
	bool				bMovedtoNewSector;			///< 다음 섹터로 이동했는지 여부


	// 서바이벌 룰 관련 /////////



	// 보상 관련 ////////////////
	int						nQL;						///< QL
	int						nDeathCount;				///< 죽은 회수
	int						nUsedPageSacriItemCount;	///< 기본 희생 아이템 사용 개수(페이지)
	int						nUsedExtraSacriItemCount;	///< 추가 희생 아이템 사용 개수
	int						nXP;						///< 얻은 XP
	int						nBP;						///< 얻은 BP
	int						nKilledNpcHpApAccum;		///< 플레이어가 죽인 NPC AP,HP총합

	CCQuestItemMap			RewardQuestItemMap;			///< 얻은 퀘스트 아이템
	CCQuestRewardZItemList	RewardZItemList;


	// Log관련 ////////////////// - by 추교성.
	// char				szName[ 24 ];


	/// 초기화
	/// @param pObj		플레이어 오브젝트 정보
	/// @param a_nQL	플레이어 퀘스트 레벨
	void Init(CCMatchObject* pObj, int a_nQL)
	{
		pObject = pObj;
		bEnableNPCControl = true;
		nNPCControlCheckSum = 0;
		NPCObjects.clear();
		bMovedtoNewSector = true;

		nQL = a_nQL;
		nDeathCount = 0;
		nUsedPageSacriItemCount = 0;
		nUsedExtraSacriItemCount = 0;
		nXP = 0;
		nBP = 0;
		nKilledNpcHpApAccum = 0;

		RewardQuestItemMap.Clear();
		RewardZItemList.clear();
	}

	/// 생성자
	CCQuestPlayerInfo() : nXP(0), nBP(0), nKilledNpcHpApAccum(0)
	{
		
	}
};

/// 퀘스트룰의 플레이어 오브젝트 관리자
class CCQuestPlayerManager : public map<CCUID, CCQuestPlayerInfo*>
{
private:
	CCMatchStage* m_pStage;
	void AddPlayer(CCUID& uidPlayer);
public:
	CCQuestPlayerManager();										///< 생성자
	~CCQuestPlayerManager();										///< 소멸자
	void Create(CCMatchStage* pStage);							///< 초기화
	void Destroy();												///< 해제
	void DelPlayer(CCUID& uidPlayer);							///< 플레이어 삭제
	void Clear();												///< 초기화
	CCQuestPlayerInfo* GetPlayerInfo(const CCUID& uidPlayer);		///< 플레이어 정보 반환
};