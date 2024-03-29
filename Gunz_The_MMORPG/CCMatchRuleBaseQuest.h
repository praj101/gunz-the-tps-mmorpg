#pragma once

#include "CCMatchRule.h"
#include "CCMatchNPCObject.h"
#include "CCQuestPlayer.h"
#include "CCMatchQuestRound.h"


class CCMatchQuestGameLogInfoManager;

/// 퀘스트룰들의 부모 클래스
class CCMatchRuleBaseQuest : public CCMatchRule {
protected:
	CCMatchNPCManager			m_NPCManager;				///< NPC 관리자
	CCQuestPlayerManager			m_PlayerManager;			///< 플레이어 관리자
	bool						m_bQuestCompleted;			///< 퀘스트가 끝났는지 여부

	unsigned long int			m_nLastNPCSpawnTime;		// for test
	int							m_nNPCSpawnCount;			///< 스폰된 NPC수
	unsigned long int			m_nSpawnTime;				///< 마지막 NPC 스폰 시간
	int							m_nFirstPlayerCount;		///< 처음 게임시작시 플레이어 수

	unsigned long int			m_nLastNPCAssignCheckTime;	///< 마지막으로 NPC 재할당 체크를 한 시간
	unsigned long int			m_nLastPingTime;			///< 마지막으로 클라이언트 핑 잰 시간

protected:
	virtual void OnBegin();								///< 전체 게임 시작시 호출
	virtual void OnEnd();								///< 전체 게임 종료시 호출
	virtual bool OnRun();								///< 게임틱시 호출
	virtual void OnRoundBegin();						///< 라운드 시작할 때 호출
	virtual void OnRoundEnd();							///< 라운드 끝날 때 호출
	virtual bool OnCheckRoundFinish();					///< 라운드가 끝났는지 체크
	virtual void OnRoundTimeOut();						///< 라운드가 타임아웃으로 종료될 떄 OnRoundEnd() 전이다.
	virtual bool RoundCount();							///< 라운드 카운트. 모든 라운드가 끝나면 false를 반환한다.
	virtual bool OnCheckEnableBattleCondition();		///< 게임 가능한지 체크

	virtual void OnCommand(CCCommand* pCommand);			///< 퀘스트에서만 사용하는 커맨드 처리
protected:
	/// npc가 죽고 나서 드롭아이템이 있는지 체크하고 있으면 아이템을 드롭시킨다.
	/// @param uidPlayer	공격자 플레이어 UID
	/// @param pDropItem	드롭할 아이템
	/// @param pos			위치
	void CheckRewards(CCUID& uidPlayer, CCQuestDropItem* pDropItem, CCVector& pos);
	bool CheckPlayersAlive();								///< 플레이어가 모두 죽었는지 체크
	virtual void ProcessNPCSpawn() = 0;						///< NPC 스폰작업
	virtual bool CheckNPCSpawnEnable() = 0;					///< NPC가 스폰 가능한지 여부
	virtual void RouteGameInfo() = 0;						///< 클라이언트에 게임 정보 보내준다.
	virtual void RouteStageGameInfo() = 0;					///< 대기중 스테이지에서 바뀐 게임 정보를 보내준다.

	/// NPC를 스폰시킨다.
	/// @param nNPC			NPC 종류
	/// @param nPosIndex	스폰 위치
	CCMatchNPCObject* SpawnNPC(CCQUEST_NPC nNPC, int nPosIndex, bool bKeyNPC=false);

	virtual void OnCompleted();					///< 퀘스트 성공시 호출된다.
	virtual void OnFailed();					///< 퀘스트 실패시 호출된다.
	virtual void RouteCompleted() = 0;			///< 퀘스트 성공 메시지를 보낸다. - 리워드까지 함께 보낸다.
	virtual void RouteFailed() = 0;				///< 퀘스트 실패 메시지 보낸다.
	virtual void DistributeReward() = 0;		///< 퀘스트 성공시 리워드 배분

	void ReAssignNPC();
	void SendClientLatencyPing();
public:
	// 커맨드 처리 관련 함수

	/// NPC를 죽였을때 호출
	/// @param uidSender		메세지 보낸 플레이어
	/// @param uidKiller		죽인 플레이어
	/// @param uidNPC			죽은 NPC
	/// @param pos				NPC 위치
	virtual void OnRequestNPCDead(CCUID& uidSender, CCUID& uidKiller, CCUID& uidNPC, CCVector& pos);

	/// 플레이어 죽었을 때 호출
	/// @param uidVictim		죽은 플레이어 UID
	virtual void OnRequestPlayerDead(const CCUID& uidVictim);
	virtual void OnRequestTestNPCSpawn(int nNPCType, int nNPCCount);
	virtual void OnRequestTestClearNPC();

	/// 모든 플레이어 부활 및 HP, AP 초기화
	void RefreshPlayerStatus();
	/// 모든 NPC를 초기화
	void ClearAllNPC();

	/// 플레이어가 포탈로 이동했을 경우 호출된다.
	/// @param uidPlayer			이동한 플레이어 UID
	virtual void OnRequestMovetoPortal(const CCUID& uidPlayer) = 0;
	/// 포탈로 이동하고 나서 이동이 완료되었을 경우 호출된다.
	/// @param uidPlayer			플레이어 UID
	virtual void OnReadyToNewSector(const CCUID& uidPlayer) = 0;

	virtual void OnRequestTestSectorClear() = 0;
	virtual void OnRequestTestFinish() = 0;


	// 퀘스트때문에 추가된 가상 인터페이스.
	// 구조에대해 좀더 생각을 해봐야함. 너무 많은 인터페이스가 CCMatchRule까지 올라와 버렸음... - by 추교성.
	virtual void OnRequestDropSacrificeItemOnSlot( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID ) {}
	virtual void OnResponseDropSacrificeItemOnSlot( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID ) {}
	virtual void OnRequestCallbackSacrificeItem( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID ) {}
	virtual void OnResponseCallBackSacrificeItem( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )	{}
	virtual void OnRequestQL( const CCUID& uidSender ) {}
	virtual void OnResponseQL_ToStage( const CCUID& uidStage )	{}
	virtual void OnRequestSacrificeSlotInfo( const CCUID& uidSender ) {}
	virtual void OnResponseSacrificeSlotInfoToListener( const CCUID& uidSender ) {}
	virtual void OnResponseSacrificeSlotInfoToStage( const CCUID& uidStage )	{}
	virtual void OnChangeCondition() {}

	virtual bool							PrepareStart() { return true; }
	virtual void							PreProcessLeaveStage( const CCUID& uidLeaverUID );


	// 몬스터 바이블 추가 인터페이스.
	virtual void CheckMonsterBible( const CCUID& uidUser, const int nMonsterBibleIndex );
	virtual void PostNewMonsterInfo( const CCUID& uidUser, const char nMonIndex );
	
public:
	CCMatchRuleBaseQuest(CCMatchStage* pStage);			///< 생성자
	virtual ~CCMatchRuleBaseQuest();						///< 소멸자
	virtual void OnEnterBattle(CCUID& uidChar);			///< 게임중 난입할때 호출된다.
	virtual void OnLeaveBattle(CCUID& uidChar);			///< 게임중 나갔을때 호출된다.

	virtual void RefreshStageGameInfo() = 0;
};


void InsertNPCIDonUnique( vector<CCQUEST_NPC>& outNPCList, CCQUEST_NPC nNPCID );
void MakeJacoNPCList( vector<CCQUEST_NPC>& outNPCList, CCQuestScenarioInfoMaps& ScenarioInfoMaps );
void MakeSurvivalKeyNPCList( vector<CCQUEST_NPC>& outNPCList, CCQuestScenarioInfoMaps& ScenarioInfoMaps );
void MakeNomalNPCList( vector<CCQUEST_NPC>& outNPCList, CCQuestScenarioInfoMaps& ScenarioInfoMaps, CCMatchQuest* pQuest );
void CopyCCTD_NPCINFO( CCTD_NPCINFO* pDest, const CCQuestNPCInfo* pSource );
