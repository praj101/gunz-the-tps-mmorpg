#ifndef _ZSURVIVAL_H
#define _ZSURVIVAL_H

#include "ZBaseQuest.h"

// 서바이벌 관련 전역 클래스
class ZSurvival : public ZBaseQuest
{
private:
	set<CCUID>	m_CharactersGone;	// 다음섹터로 이동한 캐릭터들

	ZQuestGameInfo		m_GameInfo;
	bool	m_Cheet[ZQUEST_CHEET_MAX];
	bool	m_bCreatedOnce;
	bool	m_bLoaded;
	bool	m_bIsQuestComplete;
	bool	m_bIsRoundClear;
	DWORD	m_tRemainedTime;					// 라운드가 끝나고 다음 라운드로 넘어가기까지 남은 시간
	float	m_fLastWeightTime;

	CCQuestCombatState	m_QuestCombatState;

	ZNPCInfoFromServerManager m_NPCInfoFromServerMgr;


#ifdef _QUEST_ITEM
	int				m_nRewardXP;				// 라운드당 받은 XP 누적
	int				m_nRewardBP;				// 라운드당 받은 BP 누적
	int				m_nReachedRound;			// 도달한 라운드 (결과 커맨드에서 세팅됨)
	int				m_nPoint;					// 게임 점수
	
	virtual bool OnRewardQuest( CCCommand* pCmd ) { return true; }		// 서바이벌에서 사용않음
	virtual bool OnNewMonsterInfo( CCCommand* pCmd );	// 몬스터 모감에 등록될 새로 습득한 몬스터 정보.
	bool OnSurvivalResult(CCCommand* pCmd);
	bool OnSurvivalRankingList(CCCommand* pCmd);
	bool OnSurvivalPrivateRanking(CCCommand* pCmd);

	virtual void GetMyObtainQuestItemList( int nRewardXP, int nRewardBP, void* pMyObtainQuestItemListBlob, void* pMyObtainZItemListBlob );

public :
	virtual int GetRewardXP( void)							{ return m_nRewardXP; }
	virtual int GetRewardBP( void)							{ return m_nRewardBP; }
			int GetReachedRound( void)						{ return m_nReachedRound; }
			int GetPoint( void)								{ return m_nPoint; }
	virtual bool IsQuestComplete( void)						{ return m_bIsQuestComplete; }
	virtual bool IsRoundClear( void)						{ return m_bIsRoundClear; }
	virtual DWORD GetRemainedTime( void)					{ return m_tRemainedTime; }

	virtual CCQuestCombatState GetQuestState()				{ return m_QuestCombatState; }

	virtual ZNPCInfoFromServerManager& GetNPCInfoFromServerMgr() { return m_NPCInfoFromServerMgr; }

#endif

	virtual bool OnNPCSpawn(CCCommand* pCommand);
	virtual bool OnNPCDead(CCCommand* pCommand);
	virtual bool OnPeerNPCDead(CCCommand* pCommand);
	virtual bool OnEntrustNPCControl(CCCommand* pCommand);
	virtual bool OnPeerNPCBasicInfo(CCCommand* pCommand);
	virtual bool OnPeerNPCHPInfo(CCCommand* pCommand);
	virtual bool OnPeerNPCAttackMelee(CCCommand* pCommand);
	virtual bool OnPeerNPCAttackRange(CCCommand* pCommand);
	virtual bool OnPeerNPCSkillStart(CCCommand* pCommand);
	virtual bool OnPeerNPCSkillExecute(CCCommand* pCommand);
	virtual bool OnPeerNPCBossHpAp(CCCommand* pCommand);
	virtual bool OnRefreshPlayerStatus(CCCommand* pCommand);
	virtual bool OnClearAllNPC(CCCommand* pCommand);
	virtual bool OnQuestRoundStart(CCCommand* pCommand);
	virtual bool OnQuestPlayerDead(CCCommand* pCommand);
	virtual bool OnQuestGameInfo(CCCommand* pCommand);
	virtual bool OnQuestCombatState(CCCommand* pCommand);
	virtual bool OnMovetoPortal(CCCommand* pCommand);
	virtual bool OnReadyToNewSector(CCCommand* pCommand);
	virtual bool OnSectorStart(CCCommand* pCommand);
	virtual bool OnObtainQuestItem(CCCommand* pCommand);
	virtual bool OnObtainZItem(CCCommand* pCommand);
	virtual bool OnSectorBonus(CCCommand* pCommand);
	virtual bool OnQuestCompleted(CCCommand* pCommand);
	virtual bool OnQuestFailed(CCCommand* pCommand);
	virtual bool OnQuestPing(CCCommand* pCommand);


	//ZQuestMap			m_Map;
	virtual void LoadNPCMeshes();
	virtual void LoadNPCSounds();
	virtual void MoveToNextSector();
	virtual void UpdateNavMeshWeight(float fDelta);
protected:
	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual bool OnCreateOnce();
	virtual void OnDestroyOnce();
public:
	ZSurvival();
	virtual ~ZSurvival();
public:
	virtual void OnGameCreate();
	virtual void OnGameDestroy();
	virtual void OnGameUpdate(float fElapsed);
	virtual bool OnCommand(CCCommand* pCommand);				///< 게임 이외에 날라오는 커맨드 처리
	virtual bool OnGameCommand(CCCommand* pCommand);			///< 게임중 날라오는 커맨드 처리

	virtual void SetCheet(ZQuestCheetType nCheetType, bool bValue);
	virtual bool GetCheet(ZQuestCheetType nCheetType);

	virtual void Reload();
	virtual bool Load();

	
	// interface
	virtual ZQuestGameInfo* GetGameInfo()		{ return &m_GameInfo; }

	// 상태에 상관없이 사용될수 있는 퀘스트 관련된 커맨드.
	virtual bool OnSetMonsterBibleInfo( CCCommand* pCmd );


	virtual bool OnPrePeerNPCAttackMelee(CCCommand* pCommand);	// 실제로 처리하는건 한타이밍 늦다
	
};




/////////////////////////////////////////////////////////////////////

inline void ZSurvival::SetCheet(ZQuestCheetType nCheetType, bool bValue) 
{ 
	m_Cheet[nCheetType] = bValue; 
}

inline bool ZSurvival::GetCheet(ZQuestCheetType nCheetType) 
{ 
	if (!ZIsLaunchDevelop()) return false;
	return m_Cheet[nCheetType];
}



#endif