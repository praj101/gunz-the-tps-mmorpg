#pragma once

#include "CCZFileSystem.h"
#include "ZSoundEngine.h"
#include "ZDirectInput.h"
#include "CCDataChecker.h"
#include "ZLanguageConf.h"
#include "ZTimer.h"
//include "ZEmblemInterface.h"
#include "ZStageInterface.h"
//#include "ZSkill.h"
//#include "ZWorldManager.h"

#include "RAniEventInfo.h"

//class MMessenger;
class ZGame;
class ZGameInterface;
class ZLoadingProgress;
class ZOptionInterface;

enum GunzState{
	GUNZ_NA = 0,
	GUNZ_GAME = 1,
	GUNZ_LOGIN = 2,
	GUNZ_DIRECTLOGIN = 3,
	GUNZ_LOBBY = 4,
	GUNZ_STAGE = 5,
	GUNZ_GREETER = 6,
	GUNZ_CHARSELECTION = 7,
	GUNZ_CHARCREATION = 8,
	GUNZ_PREVIOUS = 10,
	GUNZ_SHUTDOWN = 11,
	GUNZ_BIRDTEST
};

class ZApplication 
{
public:
	enum ZLAUNCH_MODE {
		ZLAUNCH_MODE_DEBUG,
		ZLAUNCH_MODE_NETMARBLE,
		ZLAUNCH_MODE_GAMEON,
		ZLAUNCH_MODE_STANDALONE,
		ZLAUNCH_MODE_STANDALONE_DEVELOP,		// 개발자 모드		// 곧 사라질것임
		ZLAUNCH_MODE_STANDALONE_REPLAY,
		ZLAUNCH_MODE_STANDALONE_GAME,
		ZLAUNCH_MODE_STANDALONE_DUMMY,
		ZLAUNCH_MODE_STANDALONE_AI,
		ZLAUNCH_MODE_STANDALONE_QUEST,
		ZLAUNCH_MODE_NHNUSA,
	};

private:
	ZGameInterface*			m_pGameInterface;
	GunzState				m_nInitialState;			///< 초기 상태
	ZStageInterface*		m_pStageInterface;
	ZOptionInterface*		m_pOptionInterface;
	ZLAUNCH_MODE			m_nLaunchMode;
    char					m_szFileName[_MAX_PATH];	// 리플레이 혹은 standalone 맵이름
	char					m_szCmdLine[ 4096 ];			// 초기 커맨드라인 저장
	UINT					m_nTimerRes;
	CCDataChecker			m_GlobalDataChecker;
	bool					m_bLaunchDevelop;			///< 개발자버전 옵션
	bool					m_bLaunchTest;				///< 테스트버전 옵션

	void ParseStandAloneArguments(char* pszArgs);
protected:
	static ZApplication*	m_pInstance;	///< Singleton Instance
	static CCZFileSystem		m_FileSystem;
	static ZSoundEngine		m_SoundEngine;
//	static RMeshMgr			m_NPCMeshMgr;
//	static RMeshMgr			m_MeshMgr;
//	static RMeshMgr			m_WeaponMeshMgr;
	static ZTimer			m_Timer;
//	static ZEmblemInterface	m_EmblemInterface;
//	static ZSkillManager	m_SkillManager;				///< 스킬 매니저

	//애니메이션 이벤트 메니저...
//	static RAniEventMgr		m_AniEventMgr;

//	ZWorldManager			m_WorldManager;
	CCZFileCheckList			m_fileCheckList;
#ifdef _ZPROFILER
	ZProfiler				*m_pProfiler;
#endif

public:
	ZApplication();
	~ZApplication();

	const char* GetCmdLine() { return m_szCmdLine; }
	bool ParseArguments(const char* pszArgs);
	void SetInterface();
	void SetNextInterface();
	void CheckSound();
	void SetInitialState();
	bool GetSystemValue(const char* szField, char* szData);		///< 레지스트리의 값을 읽어옴
	void SetSystemValue(const char* szField, const char* szData);		///< 레지스트리에 저장

	void InitFileSystem();

	bool OnCreate(ZLoadingProgress *pLoadingProgress);
	void OnDestroy();
	bool OnDraw();
	void OnUpdate();
	void OnInvalidate();
	void OnRestore();

	static void ResetTimer();	// 첫 update 를 부르기 전에 불러준다.
	static void Exit();
	/// Singleton Instance
	__forceinline static ZApplication*		GetInstance();
	/// Singleton Current Interface
	__forceinline static ZGameInterface*	GetGameInterface();
	__forceinline static ZStageInterface*	GetStageInterface();
	__forceinline static ZOptionInterface*	GetOptionInterface();
	/// Singleton Zip File System
	__forceinline static CCZFileSystem*		GetFileSystem();
	/// Singleton ZGameClient
	__forceinline static ZTimer*			GetTimer();
	static ZSoundEngine*		GetSoundEngine();
//	static RMeshMgr*			GetNpcMeshMgr()			{ return &m_NPCMeshMgr;}
//	static RMeshMgr*			GetMeshMgr()			{ return &m_MeshMgr; }
//	static RMeshMgr*			GetWeaponMeshMgr()		{ return &m_WeaponMeshMgr; }
//	static ZEmblemInterface*	GetEmblemInterface()	{ return &m_EmblemInterface; }
//	static ZSkillManager*		GetSkillManager()		{ return &m_SkillManager; }

	//애니메이션 이벤트 메니저 얻어오는 부분
//	static RAniEventMgr*		GetAniEventMgr()		{ return &m_AniEventMgr;}

//	__forceinline ZWorldManager* GetWorldManager()		{ return &m_WorldManager; }

	__forceinline ZLAUNCH_MODE GetLaunchMode()			{ return m_nLaunchMode; }
	__forceinline void SetLaunchMode(ZLAUNCH_MODE nMode)	{ m_nLaunchMode = nMode; }
	__forceinline bool IsLaunchDevelop()					{ return m_bLaunchDevelop; }
	__forceinline bool IsLaunchTest()					{ return m_bLaunchTest; }

	__forceinline unsigned int GetFileListCRC()			{ return m_fileCheckList.GetCRC32(); }

	bool InitLocale();
	void PreCheckArguments();
};

__forceinline ZApplication* ZApplication::GetInstance()
{
	return m_pInstance;
}

__forceinline ZGameInterface* ZApplication::GetGameInterface()
{
	ZApplication* pApp = GetInstance();
	if(pApp==NULL) return NULL;
	return pApp->m_pGameInterface;		// 현재인터페이스가 ZGameInterface라고 가정한다. 세이프코드가 필요하다.
}
__forceinline ZStageInterface* ZApplication::GetStageInterface()
{
	ZApplication* pApp = GetInstance();
	if(pApp==NULL) return NULL;
	return pApp->m_pStageInterface;
}
__forceinline ZOptionInterface* ZApplication::GetOptionInterface()
{
	ZApplication* pApp = GetInstance();
	if(pApp==NULL) return NULL;
	return pApp->m_pOptionInterface;
}
__forceinline CCZFileSystem* ZApplication::GetFileSystem()
{
	return &m_FileSystem;
}
__forceinline ZTimer* ZApplication::GetTimer()
{
	return &m_Timer;
}