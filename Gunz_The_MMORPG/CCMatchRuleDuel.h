#ifndef _CCMATCHRULE_DUEL_H
#define _CCMATCHRULE_DUEL_H


#include "CCMatchRule.h"
#include <list>

using namespace std;

class CCMatchRuleDuel : public CCMatchRule {
private:
	list<CCUID>		WaitQueue;										///< 대기열 큐
	bool			isChangeChampion;								///< 승자가 바꼈는가
	bool			isRoundEnd;										///< 라운드 끝나는가
	bool			isTimeover;										///< 타임오버 됐는가
	int				nVictory;										///< 연승수
protected:	
	virtual bool RoundCount();										///< 라운드 카운트. 모든 라운드가 끝나면 false를 반환한다.

	virtual void OnBegin();											///< 전체 게임 시작시 호출
	virtual void OnEnd();											///< 전체 게임 종료시 호출
	virtual void OnRoundBegin();									///< 라운드 시작할 때 호출
	virtual void OnRoundEnd();										/// 라운드 끝날 때 호출
	virtual void OnRoundTimeOut();									///< 라운드가 타임아웃으로 종료될 떄 OnRoundEnd() 전이다.

	virtual bool OnCheckRoundFinish();								///< 라운드가 끝났는지 체크

	virtual void OnGameKill(const CCUID& uidAttacker, const CCUID& uidVictim);	///< 킬했을때 도전자의 킬인지 챔피언의 킬인지 체크

	virtual bool OnCheckEnableBattleCondition();					///< 게임 가능한지 체크

	/// 게임중 난입할때 호출된다.
	virtual void OnEnterBattle(CCUID& uidChar);
	// 게임중 나갔을때 호출된다.
	virtual void OnLeaveBattle(CCUID& uidChar);		

	void		 SpawnPlayers();									///< 플레이어들을 스폰시킨다.

	void		LogInfo();

	void		SendQueueInfo(bool isRoundEnd = false);				///< 플레이어들에게 큐 정보를 전달

public:
	int				GetVictory() { return nVictory;	}					///< 연승수 리턴
	CCUID			uidChampion;									///< 1위중인 사람
	CCUID			uidChallenger;									///< 도전자

	CCMatchRuleDuel(CCMatchStage* pStage);
	virtual ~CCMatchRuleDuel() { }
	virtual CCMATCH_GAMETYPE GetGameType() { return CCMATCH_GAMETYPE_DUEL; }
};


#endif