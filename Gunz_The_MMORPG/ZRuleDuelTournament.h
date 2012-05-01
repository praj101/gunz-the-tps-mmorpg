#ifndef _ZRULE_DUELTOURNAMENT_H
#define _ZRULE_DUELTOURNAMENT_H

#include "ZRule.h"

class ZRuleDuelTournament : public ZRule
{
public:
	MTD_DuelTournamentGameInfo m_DTGameInfo;
	MTD_DuelTournamentNextMatchPlayerInfo m_prevPlayerInfo;
	MTD_DuelTournamentNextMatchPlayerInfo m_nextPlayerInfo;

	int							m_nDTRoundCount;			// 우승하기위한 전체적인 라운드
	int							m_nDTPlayCount;				// 한판의 승패를 가리기위한 경기

	MDUELTOURNAMENTROUNDSTATE	m_eDTRoundState;
	DuelTournamentPlayer		m_QuaterFinalPlayer[8];
	DuelTournamentPlayer		m_DTChampion;

	float						m_fBlinkOpacity;
	
	bool						m_bSlideToCenter;			// UI 슬라이드 효과 방향
	float						m_fSlideElapsedTime;		// UI 슬라이드 효과 시작한 후 흐른 시간

	enum PRECOUNTDOWN_DETAIL {	// 프리카운트 상태를 세부적으로 구분함
		PCD_NOT_PRECOUNTDOWN,		// 프리카운트다운 상태가 아님
		PCD_WINLOSE,				// 이전 매치의 승자/패자를 표시
		PCD_NEXTMATCH,				// 다음 매치의 대전자 정보를 표시
	};

	bool						m_bFirstPreCountdown;
	PRECOUNTDOWN_DETAIL			m_ePreCountdownDetail;

	DWORD						m_dwTimeEnterPreCountDown;	// PRE_COUNTDOWN 상태에 진입한 시각

	void InitCharacterList();
	void SetPreCountdownDetail(PRECOUNTDOWN_DETAIL eDetail);

public:

	ZRuleDuelTournament(ZMatch* pMatch);
	virtual ~ZRuleDuelTournament();

	int	GetQueueIdx(const CCUID& uidChar);

	virtual bool OnCommand(MCommand* pCommand);
	virtual void AfterCommandProcessed( MCommand* pCommand );
	virtual void OnSetRoundState(CCMATCH_ROUNDSTATE roundState);
	virtual void OnUpdate(float fDelta);

	int GetDuelTournamentPlayCount() { return m_nDTPlayCount; }
	CCUID GetChampion() { return m_DTChampion.uidPlayer; }

	void OnDraw(CCDrawContext* pDC);

	void SetMatchPlayerInfoUI(const CCUID& uidPlayer1, const CCUID& uidPlayer2);
	void ShowMatchPlayerInfoUI(bool bShow);
	void ShowMatchPlayerInfoUI_OnlyNextMatch(bool bShow);
	void ShowWinLoseScreenEffect(MTD_DuelTournamentMatchResultInfo& DTGameResultInfo);

	void SetRoundResultInfo(MTD_DuelTournamentRoundResultInfo& DTGameResultInfo);
	void SetMatchResultInfo(MTD_DuelTournamentMatchResultInfo& DTGameResultInfo);
	void ShowMatchOrder(CCDrawContext* pDC, bool isResult, float fElapsed);
	DuelTournamentPlayer* GetPlayer(const CCUID& uid);
	void DrawVictorySymbol(CCDrawContext* pDC, CCUID uidPlayer1, CCUID uidPlayer2);

	void SetPlayerHpApForUI(const CCUID& uidChar, float fmaxhp, float fmaxap, float fhp, float fap);		// UI를 위해 그려주기만할 용도
	void GetPlayerHpApForUI(const CCUID& uidChar, float* fmaxhp, float* fmaxap, float* fhp, float* fap);		// UI를 위해 그려주기만할 용도
	int GetPlayerInfoIndex(const CCUID& uidChar);
	int GetPingValue(CCUID uiPlayer);		// 핑값 가져오기

private:
	void BeginPlayerInfoUISlideAni(bool bToCenter);
	void UpdateUISlideAni(float fElapsed);
	void DrawInverse(CCDrawContext* pDC, CCBitmap* pBitmap, float x, float y, float fRateX, float fRateY, int nMirror);
	void DrawHighlight(CCDrawContext* pDC, const sRect& rc);

	void RestoreStageExitButtonConfirmMessage();
	void SetVisiblePlayerInfoUI(bool bLeft, bool bShow);
};

inline ZCharacterView* GetWidgetCharViewLeft() {
	return (ZCharacterView*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatDT_CharviewerLeft");
}
inline ZCharacterView* GetWidgetCharViewRight() {
	return (ZCharacterView*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatDT_CharviewerRight");
}
inline ZCharacterView* GetWidgetCharViewResult() {
	return (ZCharacterView*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("DuelTournamentResult_Charviewer");
}

#endif