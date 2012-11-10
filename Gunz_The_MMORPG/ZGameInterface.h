#ifndef _ZGAMEINTERFACE_H
#define _ZGAMEINTERFACE_H

#include "ZPrerequisites.h"
#include "ZInterface.h"
#include "ZCamera.h"
#include "ZMsgBox.h"
#include "ZLoading.h"
#include "ZInterfaceBackground.h"
//#include "ZChat.h"
//#include "ZQuest.h"
//#include "ZSurvival.h"
//#include "ZGameType.h"
//#include "ZTips.h"
//#include "ZCombatMenu.h"
#include "ZBitmapManager.h"
#include "CCTextArea.h"
#include "CCListBox.h"


#define LOGINSTATE_FADEIN				0
#define LOGINSTATE_SHOWLOGINFRAME		1
#define LOGINSTATE_STANDBY				2
#define LOGINSTATE_LOGINCOMPLETE		3
#define LOGINSTATE_FADEOUT				4

#if defined(_DEBUG) || defined(_RELEASE)
#define _CHATOUTPUT_ENABLE_CHAR_DAMAGE_INFO_				// 각 캐릭터 데미지 로그(테스트 작업용)
#endif

class ZLocatorList;
class ZGameInput;
class ZInterface;

enum ZChangeWeaponType;

class CCUserDataListItem : public CCDefaultListItem{
	int m_nUserData;
public:
	CCUserDataListItem(const char* szText, int nUserData)
		: CCDefaultListItem(szText){
			m_nUserData=nUserData;
		}

	int GetUserData() { return m_nUserData; }
};

class ZGameInterface : public ZInterface {
public:
	GunzState			m_nInitialState;
	bool				m_bTeenVersion;
	bool				m_bViewUI;
	bool				m_bTeamPlay;

	bool				m_bLoginTimeout;
	DWORD				m_dwLoginTimeout;

//	int					m_nSelectedCharacter;

	CCTextAreaLook		m_textAreaLookItemDesc;	// 상점에서 아이템 설명용 TextArea의 Look
	
protected:
	ZEffectManager*		m_pEffectManager;

	GunzState			m_nPreviousState;

	ZCombatInterface*	m_pCombatInterface;
	ZGameInput*			m_pGameInput;
	ZLoading*			m_pLoadingInterface;

	static ZGameClient*	m_spGameClient;
	ZGame*				m_pGame;
//	ZGameTypeManager	m_GameTypeManager;	
//	ZCombatMenu			m_CombatMenu;

	
	bool				m_bShowInterface;

	bool				m_bCursor;					///< 커서를 사용할 수 있는 상태
	LPDIRECT3DSURFACE9	m_pCursorSurface;

	DWORD				m_dwFrameMoveClock;

	ZIDLResource		m_IDLResource;

	GunzState			m_nState;			///< 현재 상태
	bool				m_bLogin;			///< Login 되었는가?

	bool				m_bLoading;
	bool				m_bWaitingArrangedGame;

	CCBitmap				*m_pMapThumbnail;///< 맵 썸네일

	ZMsgBox*				m_pMsgBox;
	ZMsgBox*				m_pConfirmMsgBox;
	ZInterfaceBackground*	m_pBackground;

	bool				m_bOnEndOfReplay;		// 리플레이 보구나면 플레이어의 Level Percent가 바뀌기 때문에 리플레이 시작 전에
	int					m_nLevelPercentCache;	// m_bOnEndOfReplay를 true로 셋한 다음 m_nLevelPercentCache에 현재 LevelPercent
												// 값을 저장해 놓구서 끝나면 다시 복원한다. 좀 안좋은 구조... 방법이 없음. -_-;

	unsigned long int	m_nDrawCount;

	bool			m_bReservedWeapon;
	int				m_nLoginState;
	DWORD			m_dwLoginTimer;
	DWORD			m_dwRefreshTime;
	int				m_nLocServ;

	CCBitmapR2*		m_pRoomListFrame;							// 게임방 리스트 프레임 이미지
	CCBitmapR2*		m_pBottomFrame;								// 하단 정보창 프레임 이미지
	CCBitmapR2*		m_pLoginBG;									// 로그인 배경 이미지
	CCBitmapR2*		m_pLoginPanel;								// 로그인 패널 이미지

	ZBitmapManager<int> m_ItemThumbnailMgr;							// 상점/장비창/듀얼토너먼트 게임중에 보여질 아이템 썸네일 매니저
	//↑적절한 시기마다 모두 언로드해서 메모리를 낭비하지 않도록 해야 합니다. (상점 나갈때, 장비창 나갈때, 듀얼토너먼트 게임 나갈때)

	ZLocatorList*	m_pLocatorList;
	ZLocatorList*	m_pTLocatorList;

	DWORD			m_dwTimeCount;								// 게임 경과시간 카운트. 청소년 자율규제 적용안 쓰불...
	DWORD			m_dwHourCount;								// 게임 경과시간(hour) 카운트. 청소년 자율규제 적용안 쓰불...

	DWORD			m_dwVoiceTime;								// 현재 출력중인 보이스 사운드 시간
	char			m_szCurrVoice[ 256];						// 현재 출력하는 보이스 파일 이름
	char			m_szNextVoice[ 256];						// 다음에 출력할 보이스 파일 이름
	DWORD			m_dwNextVoiceTime;							// 다음에 출력할 보이스 사운드 시간

	int				m_nRetryCount;

	bool			m_bReservedQuit;
	DWORD			m_dwReservedQuitTimer;

	bool			m_bReserveResetApp;							// for changing language

	static bool		m_bSkipGlobalEvent;

	DWORD			m_MyPort;

	DWORD			m_dErrMaxPalyerDelayTime;
	DWORD			m_bErrMaxPalyer;

	bool			m_bGameFinishLeaveBattle;		// 배틀에서 나갈때 스테이지 종료 여부

//	list<CCCommand*>	m_listDelayedGameCmd;

// _DUELTOURNAMENT
//	vector<DTPlayerInfo> m_vecDTPlayerInfo;
//	CCDUELTOURNAMENTTYPE m_eDuelTournamentType;

protected:
	static bool		OnGlobalEvent(CCEvent* pEvent);
	virtual bool	OnEvent(CCEvent* pEvent, CCListener* pListener);
	bool			OnDebugEvent(CCEvent* pEvent, CCListener* pListener);
	virtual bool	OnCommand(CCWidget* pWidget, const char* szMessage);
//	static bool		OnCommand(CCCommand* pCommand);

	bool ResizeWidget(const char* szName, int w, int h);
	bool ResizeWidgetRecursive( CCWidget* pWidget, int w, int h);
	void SetListenerWidget(const char* szName, CCListener* pListener);

	void UpdateCursorEnable();
	void UpdateDuelTournamentWaitMsgDots();

//	void LoadCustomBitmap();
	bool InitInterface(const char* szSkinName,ZLoadingProgress *pLoadingProgress = NULL);
	bool InitInterfaceListener();
	void FinalInterface();

	void LoadBitmaps(const char* szDir, const char* szSubDir, ZLoadingProgress *pLoadingProgress);

	void LeaveBattle();

	void OnGreeterCreate();
	void OnGreeterDestroy();

	void OnLoginCreate();
	void OnLoginDestroy();

	void OnDirectLoginCreate();
	void OnDirectLoginDestroy();

	void OnNetmarbleLoginCreate();
	void OnNetmarbleLoginDestroy();

	void OnGameOnLoginCreate();
	void OnGameOnLoginDestroy();

	void OnLobbyCreate();
	void OnLobbyDestroy();

	void OnStageCreate();
	void OnStageDestroy();

	void OnCharSelectionCreate();
	void OnCharSelectionDestroy();

	void OnCharCreationCreate();
	void OnCharCreationDestroy();

	void OnShutdownState();
	void OnUpdateGameMessage();

	void HideAllWidgets();

//	void OnResponseShopItemList( const vector< CCTD_ShopItemInfo*> &vShopItemList  , const vector<CCTD_GambleItemNode*>& vGItemList );
//	void OnResponseCharacterItemList(CCUID* puidEquipItem
//		, CCTD_ItemNode* pItemNodes
//		, int nItemCount
//		, CCTD_GambleItemNode* pGItemNodes
//		, int nGItemCount );

	void OnSendGambleItemList( void* pGItemArray, const DWORD dwCount );

	void OnDrawStateGame(CCDrawContext* pDC);
	void OnDrawStateLogin(CCDrawContext* pDC);
	void OnDrawStateLobbyNStage(CCDrawContext* pDC);
	void OnDrawStateCharSelection(CCDrawContext* pDC);

	void OnResponseServerStatusInfoList( const int nListCount, void* pBlob );
	void OnResponseBlockCountryCodeIP( const char* pszBlockCountryCode, const char* pszRoutingURL );

	// locator관련.
	void RequestServerStatusListInfo();

public:
	ZGameInterface(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	~ZGameInterface();

	static bool m_sbRemainClientConnectionForResetApp;	// 언어를 바꾸어서 리소스를 다시 로딩해야 할때 이걸 true로 해줘야 함

	bool OnCreate(ZLoadingProgress *pLoadingProgress);
	void OnDestroy();

	void OnInvalidate();
	void OnRestore();

	bool Update(float fElapsed);
	void OnDraw(CCDrawContext *pDC);

	void SetCursorEnable(bool bEnable);
	void OnResetCursor();
	bool IsCursorEnable() { return m_bCursor; }

	bool SetState(GunzState nState);
	GunzState GetState(){ return m_nState; }
	
	void UpdateBlueRedTeam();		// 동환이가 추가

	void ChangeToCharSelection();	///< 캐릭터 선택으로 이동

	bool ChangeInterfaceSkin(const char* szNewSkinName);

	/// 해당하는 이름을 아이템으로 가진 위젯의 Visible상태를 바꾼다.
	bool ShowWidget(const char* szName, bool bVisible, bool bModal=false);
	void SetTextWidget(const char* szName, const char* szText);
	void EnableWidget(const char* szName, bool bEnable);

	// 나중에 지울것..우선 당장 테스트를 위해
	void TestChangeParts(int mode);
	void TestChangePartsAll();
	void TestChangeWeapon(RVisualMesh* pVMesh = NULL);
	void TestToggleCharacter();

	void ChangeParts(int mode);
	void ChangeWeapon(ZChangeWeaponType nType);
	
	void Reload();

	void RespawnMyCharacter();	// 혼자테스트할때 클릭하면 되살아난다.

	void ReserveLeaveStage();	// 스테이지에서 나갈때 일정시간 흐른뒤 나간다
	void ReserveLeaveBattle();	// 대기방으로 나갈떄 일정시간 흐른뒤 나간다
	void FinishGame();

	void ReserveResetApp(bool b)	{ m_bReserveResetApp = b; }
	bool IsReservedResetApp()		{ return m_bReserveResetApp; }

	void SaveScreenShot();

	void ShowMessage(const char* szText, CCListener* pCustomListenter=NULL, int iMessageID=0);
	void ShowConfirmMessage(const char* szText, CCListener* pCustomListenter=NULL);
	void ShowMessage(int iMessageID);
	void ShowErrorMessage(int nErrorID);
	void ShowErrorMessage(const char* szErrorMsg, int nErrorID);
	
	void ShowInterface(bool bShowInterface);
	bool IsShowInterface() { return m_bShowInterface; }

	void SetTeenVersion(bool bt) { m_bTeenVersion = bt; }
	bool GetTeenVersion() { return m_bTeenVersion; }

	void OnCharSelect();


	// GunzState에 따른 Create/Destroy 핸들러
	bool OnGameCreate();
	void OnGameDestroy();
	void OnGameUpdate(float fElapsed);


	// 로비 UI 설정
	void OnArrangedTeamGameUI(bool bFinding);
	void OnDuelTournamentGameUI(bool bWaiting);

	void InitLobbyUIByChannelType();

	void InitLadderUI(bool bLadderEnable);
	void InitClanLobbyUI(bool bClanBattleEnable);
	void InitDuelTournamentLobbyUI(bool bEnableDuelTournamentUI);
//	void InitChannelFrame(CCCHANNEL_TYPE nChannelType);

//	bool InitLocatorList( CCZFileSystem* pFileSystem, const char* pszLocatorList );

	// 스테이지 UI 설정
	void SetMapThumbnail(const char* szMapName);
	void ClearMapThumbnail();
	void SerializeStageInterface();

	void EnableLobbyInterface(bool bEnable);
	void EnableStageInterface(bool bEnable);
	void ShowPrivateStageJoinFrame(const char* szStageName);

	void SetRoomNoLight( int d );


	// 상점및 장비
	void ShowEquipmentDialog(bool bShow=true);
	void ShowShopDialog(bool bShow=true);

	// 캐릭터 선택
	void ChangeSelectedChar( int nNum);


	// 리플레이
	void ShowReplayDialog( bool bShow);
	void ViewReplay( void);


	void ShowMenu(bool bEnable);
	void Show112Dialog(bool bShow);
	bool IsMenuVisible();

	bool OpenMiniMap();
	bool IsMiniMapEnable();

	void RequestQuickJoin();

	void EnableCharSelectionInterface(bool bEnable);

public:


	// 클랜생성이나 래더팀게임초대를 받을수 있는 상태인가 ?
	bool IsReadyToPropose();

	// 리플레이
	void OnReplay();

	// XTrap
	void OnRequestXTrapSeedKey(unsigned char *pComBuf);			// add sgk 0402

	void OnDisconnectMsg( const DWORD dwMsgID );
	void ShowDisconnectMsg( DWORD errStrID, DWORD delayTime );

	void OnAnnounceDeleteClan( const string& strAnnounce );

	// 퀘스트 아이템 아이콘 비트맵 얻기(쓰는덴 많은데 마땅히 둘데가 없어서... -_-;)
	CCBitmap* GetQuestItemIcon( int nItemID, bool bSmallIcon);

	// ZActionKey 입력중 GlobalEvent 무력화
	static bool CheckSkipGlobalEvent() { return m_bSkipGlobalEvent; }
	void SetSkipGlobalEvent(bool bSkip) { m_bSkipGlobalEvent = bSkip; }

	// 보이스 사운드 출력
	void OnVoiceSound();
	void PlayVoiceSound( char* pszSoundName, DWORD time=0);

	void SetAgentPing(DWORD nIP, DWORD nTimeStamp);

	void OnRequestGameguardAuth( const DWORD dwIndex, const DWORD dwValue1, const DWORD dwValue2, const DWORD dwValue3 );

	void SetErrMaxPlayerDelayTime(DWORD dDelayTime) { m_dErrMaxPalyerDelayTime = dDelayTime; }
	DWORD GetErrMaxPlayerDelayTime() { return m_dErrMaxPalyerDelayTime; }
	void SetErrMaxPlayer(bool bErrMaxPalyer) { m_bErrMaxPalyer = bErrMaxPalyer; }
	bool IsErrMaxPlayer() { return m_bErrMaxPalyer == 0 ? false : true; }

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);

// _DUELTOURNAMENT
//	void SetDuelTournamentCharacterList(CCDUELTOURNAMENTTYPE nType, const vector<DTPlayerInfo>& vecDTPlayerInfo);
//	const vector<DTPlayerInfo>& GetVectorDTPlayerInfo()	{ return m_vecDTPlayerInfo; }
//	void SetDuelTournamantType(CCDUELTOURNAMENTTYPE eType)		{ m_eDuelTournamentType = eType; }
//	CCDUELTOURNAMENTTYPE GetDuelTournamentType()		{ return m_eDuelTournamentType; }

//	void UpdateDuelTournamantMyCharInfoUI();
//	void UpdateDuelTournamantMyCharInfoPreviousUI();

// 릴레이맵
//	bool GetIsGameFinishLeaveBattle()			{ return m_bGameFinishLeaveBattle; }

	// 바깥에서 얻을만한 인터페이스들 (__forceinline 는 dll-injection 핵 방해를 위한 것)
	__forceinline ZGameClient* GetGameClient()			{ return m_spGameClient; }
	__forceinline ZGame* GetGame()						{ return m_pGame; }
	__forceinline ZCombatInterface* GetCombatInterface() { return m_pCombatInterface; }
//	__forceinline ZGameTypeManager* GetGameTypeManager()		{ return &m_GameTypeManager; }

	ZEffectManager* GetEffectManager()			{ return m_pEffectManager; }
	void SetGameClient(ZGameClient* pGameClient){ m_spGameClient = pGameClient; }
	
	ZIDLResource* GetIDLResource()			{ return &m_IDLResource; }
//	ZCombatMenu*	 GetCombatMenu()			{ return &m_CombatMenu; }

	ZBitmapManager<int>* GetItemThumbnailMgr()		{ return &m_ItemThumbnailMgr; }
};

#define BEGIN_WIDGETLIST(_ITEM, _IDLRESPTR, _CLASS, _INSTANCE)								\
{																							\
	CCWidgetList WidgetList;																	\
	(_IDLRESPTR)->FindWidgets(WidgetList, _ITEM);											\
	for (CCWidgetList::iterator itor = WidgetList.begin(); itor != WidgetList.end(); ++itor) \
{																							\
	if ((*itor) != NULL)																	\
{																							\
	_CLASS _INSTANCE = ((_CLASS)(*itor));

#define END_WIDGETLIST()		}}}


#define DEFAULT_INTERFACE_SKIN "Default"


#define WM_CHANGE_GAMESTATE		(WM_USER + 25)
void ZChangeGameState(GunzState state);		/// 쓰레드에 안전하기 위해서는 만듦


inline void GetDuelTournamentGradeIconFileName(char* out_sz, int grade)
{
	sprintf(out_sz, "dt_grade%d.png", grade);
}

char* GetItemSlotName( const char* szName, int nItem);
bool SetWidgetToolTipText(char* szWidget,const char* szToolTipText, CCAlignmentMode mam=CCD_LEFT|CCD_TOP);

#endif