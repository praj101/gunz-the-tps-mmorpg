#ifndef _ZCOMBATINTERFACE_H
#define _ZCOMBATINTERFACE_H

#include "ZInterface.h"
#include "CCPicture.h"
#include "CCEdit.h"
#include "CCListBox.h"
#include "CCLabel.h"
#include "CCAnimation.h"
#include "ZObserver.h"
#include "ZCombatChat.h"
#include "ZCrossHair.h"
#include "ZMiniMap.h"
#include "ZVoteInterface.h"

// Added R349a
#include "ZBandiCapturer.h"

_USING_NAMESPACE_REALSPACE2

class ZCharacter;
class ZScreenEffect;
class ZWeaponScreenEffect;
class ZMiniMap;
class ZCombatQuestScreen;

struct ZResultBoardItem {
	char szName[64];
	char szClan[CLAN_NAME_LENGTH];
	int nClanID;
	int nTeam;
	int nScore;
	int nKills;
	int nDeaths;
	int	nAllKill;
	int	nExcellent;
	int	nFantastic;
	int	nHeadShot;
	int	nUnbelievable;
	bool bMyChar;
	bool bGameRoomUser;

	ZResultBoardItem() { }
	ZResultBoardItem(const char *_szName, const char *_szClan, int _nTeam, int _nScore, int _nKills, int _nDeaths, bool _bMyChar = false, bool _bGameRoomUser = false) {
		strcpy(szName,_szName);
		strcpy(szClan,_szClan);
		nTeam = _nTeam;
		nScore = _nScore;
		nKills = _nKills;
		nDeaths = _nDeaths;
		// 필요하면 이것들도 만들자
		nAllKill = 0;
		nExcellent = 0;
		nFantastic = 0;
		nHeadShot = 0;
		nUnbelievable = 0;
		bMyChar = _bMyChar;
		bGameRoomUser = _bGameRoomUser;
	}
};

class ZResultBoardList : public list<ZResultBoardItem*>
{
public:
	void Destroy() { 
		while(!empty())
		{
			delete *begin();
			erase(begin());
		}
	}
};

struct DuelTournamentPlayer
{
	char m_szCharName[MATCHOBJECT_NAME_LENGTH];
	CCUID uidPlayer;
	int m_nTP;
	int nVictory;
	int nMatchLevel;
	int nNumber;

	float fMaxHP;
	float fMaxAP;
	float fHP;
	float fAP;
};

class ZCombatInterface : public ZInterface
{
private:
	float				m_fElapsed;
protected:
	ZWeaponScreenEffect*		m_pWeaponScreenEffect;
//	ZScoreBoard*		m_pScoreBoard;

	// 결과화면에 필요한것
	ZScreenEffect*		m_pResultPanel;
	ZScreenEffect*		m_pResultPanel_Team;
	ZResultBoardList	m_ResultItems;
	ZScreenEffect*		m_pResultLeft;
	ZScreenEffect*		m_pResultRight;

	int					m_nClanIDRed;			///< 클랜전일때
	int					m_nClanIDBlue;			///< 두 클랜 ID
	char				m_szRedClanName[32];	
	char				m_szBlueClanName[32];	///< 두 클랜의 이름

	ZCombatQuestScreen*	m_pQuestScreen;

	ZBandiCapturer*		m_Capture;					///< 동영상 캡쳐...by kammir 2008.10.02
	bool				m_bShowUI;

	ZObserver			m_Observer;			///< 옵져버 모드
	ZCrossHair			m_CrossHair;		///< 크로스 헤어
	ZVoteInterface		m_VoteInterface;

	ZIDLResource*		m_pIDLResource;

	CCLabel*				m_pTargetLabel;
	CCBitmap*			m_ppIcons[ZCI_END];		/// 칭찬 아이콘들
	CCBitmapR2*			m_pResultBgImg;
	
	bool				m_bMenuVisible;
	
	bool				m_bPickTarget;
	char				m_szTargetName[256];		// crosshair target 이름
	
	CCMatchItemDesc*		m_pLastItemDesc;

	int					m_nBulletSpare;
	int					m_nBulletCurrMagazine;
	int					m_nMagazine;

	int					m_nBulletImageIndex;
	int					m_nMagazineImageIndex;

	char				m_szItemName[256];
	
	bool				m_bReserveFinish;
	unsigned long int	m_nReserveFinishTime;

	bool				m_bDrawLeaveBattle;
	int					m_nDrawLeaveBattleSeconds;

	bool				m_bOnFinish;
	bool				m_bShowResult;
	bool				m_bIsShowUI;					// 모든 UI 감추기... by kammir 20081020 (유저의 선택사항)
	bool				m_bSkipUIDrawByRule;			// 게임룰이 필요하다면 UI 드로우를 끌 수 있도록

	bool				m_bDrawScoreBoard;
//	bool				m_bKickPlayerListVisible;		// 게임화면에 플레이어 리스트 보여준다

	float				m_fOrgMusicVolume;

	bool				m_bNetworkAlive;
	DWORD				m_dLastTimeTick;
	DWORD				m_dAbuseHandicapTick;

	void SetItemImageIndex(int nIndex);

	void SetItemName(const char* szName);
	void UpdateCombo(ZCharacter* pCharacter);
	
	void OnFinish();

	void GameCheckPickCharacter();

	// 화면에 그리는것과 관련된 펑션들
	void IconRelative(CCDrawContext* pDC,float x,float y,int nIcon);

	void DrawFriendName(CCDrawContext* pDC);			// 같은편 이름
	void DrawEnemyName(CCDrawContext* pDC);			// 적 이름
	void DrawAllPlayerName(CCDrawContext* pDC);		// 모든 팀 이름 표시 (Free Spectator)

	void DrawScoreBoard(CCDrawContext* pDC);			// 점수 화면 (tab키)
	void DrawDuelTournamentScoreBoard(CCDrawContext* pDC);						// 듀얼 토너먼트 대진표 화면 (tab키)
	void DrawPlayTime(CCDrawContext* pDC, float xPos, float yPos);	// 플레이 시간
	void DrawResultBoard(CCDrawContext* pDC);		// 게임 결과화면
	void DrawSoloSpawnTimeMessage(CCDrawContext* pDC);	// 쏠로 스폰전 타이머 메시지
	void DrawLeaveBattleTimeMessage(CCDrawContext* pDC);	// 게임에서 나갈때 기다리는 시간표시
//	void DrawVoteMessage(CCDrawContext* pDC);		// 투표가 진행중일때 메시지
//	void DrawKickPlayerList(CCDrawContext* pDC);		// kick 할 플레이어 선택하는 화면
	void GetResultInfo( void);

	void DrawTDMScore(CCDrawContext* pDC);

	void DrawNPCName(CCDrawContext* pDC);	// 디버그용

	void UpdateNetworkAlive(CCDrawContext* pDC);

public:
	ZCombatChat			m_Chat;
	ZCombatChat			m_AdminMsg;
	DWORD				m_nReservedOutTime;				// Finish 후에 밖으로 나가는 시간을 설정


	ZCombatInterface(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZCombatInterface();

	void OnInvalidate();
	void OnRestore();

	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual void OnDraw(CCDrawContext* pDC);	// 그리는 순서때문에 직접 그린다
	virtual void OnDrawCustom(CCDrawContext* pDC);
	virtual void DrawAfterWidgets(CCDrawContext* pDC);	//CCWidget에서 재정의한 함수, UI 위젯을 모두 그린 후 직접 DC에 추가로 그리기 위한 함수
	void		 DrawPont(CCDrawContext* pDC);
	void		 DrawMyNamePont(CCDrawContext* pDC);
	void		 DrawMyWeaponPont(CCDrawContext* pDC);
	void		 DrawScore(CCDrawContext* pDC);
	void		 DrawBuffStatus(CCDrawContext* pDC);
	void		 DrawFinish();
	int DrawVictory( CCDrawContext* pDC, int x, int y, int nWinCount, bool bGetWidth = false);

	virtual bool IsDone();

	void OnAddCharacter(ZCharacter *pChar);

	void Resize(int w, int h);

	void OutputChatMsg(const char* szMsg);
	void OutputChatMsg(sColor color, const char* szMsg);

	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);

	static CCFont *GetGameFont();
	sPoint GetCrosshairPoint() { return sPoint(CCGetWorkspaceWidth()/2,CCGetWorkspaceHeight()/2); }
	
	ZBandiCapturer*	GetBandiCapturer()			{ return m_Capture; }					///< 동영상 캡쳐...by kammir 2008.10.02

	void ShowMenu(bool bVisible = true);
	void ShowInfo(bool bVisible = true);
	void EnableInputChat(bool bInput=true, bool bTeamChat=false);

	void SetDrawLeaveBattle(bool bShow, int nSeconds);

	void ShowChatOutput(bool bShow);
	bool IsChat() { return m_Chat.IsChat(); }
	bool IsTeamChat() { return m_Chat.IsTeamChat(); }
	bool IsMenuVisible() { return m_bMenuVisible; }

	void Update(float fElapsed);
	void SetPickTarget(bool bPick, ZCharacter* pCharacter = NULL);

//	void ShowScoreBoard(bool bVisible = true);
//	bool IsScoreBoardVisible() { return m_pScoreBoard->IsVisible(); }

	void Finish();
	bool IsFinish();

	ZCharacter* GetTargetCharacter();
	CCUID		GetTargetUID();

	int GetPlayTime();

	void SetObserverMode(bool bEnable);
	bool GetObserverMode() { return m_Observer.IsVisible(); }
	ZObserver* GetObserver() { return &m_Observer; }
	ZCrossHair* GetCrossHair() { return &m_CrossHair; }

	ZVoteInterface* GetVoteInterface()	{ return &m_VoteInterface; }

	void ShowCrossHair(bool bVisible) {	m_CrossHair.Show(bVisible); 	}
	void OnGadget(CCMatchWeaponType nWeaponType);
	void OnGadgetOff();

	void SetSkipUIDraw(bool b) { m_bSkipUIDrawByRule = b; }
	bool IsSkupUIDraw() { return m_bSkipUIDrawByRule; }

	bool IsShowResult( void)  { return m_bShowResult; }
	bool IsShowUI( void)  { return m_bIsShowUI; }
	void SetIsShowUI(bool bIsShowUI)  { m_bIsShowUI = bIsShowUI; }
	bool IsShowScoreBoard()   { return m_bDrawScoreBoard; }
//	void SetKickPlayerListVisible(bool bShow = true) { m_bKickPlayerListVisible = bShow; }
//	bool IsKickPlayerListVisible() { return m_bKickPlayerListVisible; }

	bool IsNetworkalive()	{ return m_bNetworkAlive; }

	const char* GetRedClanName() const { return m_szRedClanName; }
	const char* GetBlueClanName() const { return m_szBlueClanName; }
};

void TextRelative(CCDrawContext* pDC,float x,float y,const char *szText,bool bCenter=false);

#endif