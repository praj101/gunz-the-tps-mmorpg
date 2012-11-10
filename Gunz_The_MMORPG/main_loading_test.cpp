#include "stdafx.h"
//#include "../MatchServer/vld/vld.h"
#undef _HSHIELD
#undef _XTRAP
#undef _GAMEGUARD

//#define _INDEPTH_DEBUG_

#include "ZPrerequisites.h"
#include "ZConfiguration.h"
//#include "ZGameClient.h"
#include <windows.h>
#include <wingdi.h>
#include <mmsystem.h>
#include <shlwapi.h>
#include <shellapi.h>

#include "dxerr9.h"

#include "main.h"
#include "resource.h"
#include "VersionNo.h"
//#include "ZPost.h"

#include "Core4R2.h"
#include "ZApplication.h"
#include "RealSpace2.h"
#include "Core.h"
#include "ZGameInterface.h"
#include "RFrameWork.h"
#include "ZDirectInput.h"
#include "ZActionDef.h"
#include "ZInitialLoading.h"
#include "ZMsgBox.h"
#include "ZUtil.h"
#include "ZOptionInterface.h"
#include "ZStringResManager.h"
#include "RShaderMgr.h"
#include "RLenzFlare.h"
#include "ZLocale.h"
#include "CCSysInfo.h"	// Gets system details ad prints to log (Windows version, cpu, etc...)
#include "ZInput.h"
#include "Core4Gunz.h"


RMODEPARAMS	g_ModeParams={640,480,false,D3DFMT_R5G6B5};
//#endif

RRESULT RenderScene(void *pParam);

#define RD_STRING_LENGTH 512
char cstrReleaseDate[512] = "Alpha : 06/21/2012";

ZApplication		g_App;
CCDrawContextR2*	g_pDC = NULL;
CCFontR2*			g_pDefFont = NULL;
ZDirectInput		g_DInput;
extern ZInput*		g_pInput;
Core4Gunz			g_Core;

HRESULT GetDirectXVersionViaDxDiag( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter );

void zexit(int returnCode)
{
	exit(returnCode);
}

void _ZChangeGameState(int nIndex)
{
	GunzState state = GunzState(nIndex);
	cclog("Setting GunzState to %d\n", state);
	if (ZApplication::GetGameInterface())
	{
		ZApplication::GetGameInterface()->SetState(state);
	}
}

RRESULT OnCreate(void *pParam)
{
	/*
	//	Walk through in debug to trace all routes
	*/
	g_App.PreCheckArguments();

	sprintf( cstrReleaseDate, "Version : %s", ZGetSVNRevision().c_str()); //Possibly
	cclog(cstrReleaseDate); cclog("\n");

	g_DInput.Create(g_hWnd, FALSE, FALSE);
	g_pInput = new ZInput(&g_DInput);
	RSetGammaRamp(Z_VIDEO_GAMMA_VALUE);
	RSetRenderFlags(RRENDER_CLEAR_BACKBUFFER);

	ZGetInitialLoading()->Initialize(  1, 0, 0, RGetScreenWidth(), RGetScreenHeight(), 0, 0, 1024, 768 );

	cclog("InitialLoading success.\n");

	struct _finddata_t c_file;
	intptr_t hFile;
	char szFileName[256];
	#define FONT_DIR	"Font/"
	#define FONT_EXT	"ttf"
	if( (hFile = _findfirst(FONT_DIR"*."FONT_EXT, &c_file )) != -1L ){
		do{
			strcpy(szFileName, FONT_DIR);
			strcat(szFileName, c_file.name);
			AddFontResource(szFileName);
		}while( _findnext( hFile, &c_file ) == 0 );
		_findclose(hFile);
	}

	g_pDefFont = new CCFontR2;

	if( !g_pDefFont->Create("Default", Z_LOCALE_DEFAULT_FONT, DEFAULT_FONT_HEIGHT, 1.0f) )	{
		cclog("Fail to Create default font : CCFontR2 / main.cpp.. onCreate\n" );
		g_pDefFont->Destroy();
		SAFE_DELETE( g_pDefFont );
		g_pDefFont	= NULL;
	}
	g_pDC = new CCDrawContextR2(RGetDevice());

	if( ZGetInitialLoading()->IsUseEnable() )
	{
		ZGetInitialLoading()->AddBitmap( 0, "Interface/Default/LOADING/loading_adult.jpg" );
		ZGetInitialLoading()->AddBitmapBar( "Interface/Default/LOADING/loading.bmp" );
		ZGetInitialLoading()->SetText( g_pDefFont, 5, 5, cstrReleaseDate );
		ZGetInitialLoading()->AddBitmapGrade( "Interface/Default/LOADING/loading_grade_fifteen.jpg" );
		ZGetInitialLoading()->SetPercentage( 0.0f );
		ZGetInitialLoading()->Draw( MODE_FADEIN, 0 , true );
	}


	g_Core.Initialize(800, 600, g_pDC, g_pDefFont);
	Core::GetInstance()->SetHWND(RealSpace2::g_hWnd);

	cclog("interface Initialize success\n");

	ZLoadingProgress appLoading("application");
	if(!g_App.OnCreate(&appLoading))
	{
		ZGetInitialLoading()->Release();
		return R_ERROR_LOADING;
	}

	ZGetSoundEngine()->SetEffectVolume(Z_AUDIO_EFFECT_VOLUME);
	ZGetSoundEngine()->SetMusicVolume(Z_AUDIO_BGM_VOLUME);
	ZGetSoundEngine()->SetEffectMute(Z_AUDIO_EFFECT_MUTE);
	ZGetSoundEngine()->SetMusicMute(Z_AUDIO_BGM_MUTE);

	g_Core.SetWorkspaceSize(g_ModeParams.nWidth, g_ModeParams.nHeight);
	g_Core.GetMainFrame()->SetSize(g_ModeParams.nWidth, g_ModeParams.nHeight);
	ZGetOptionInterface()->Resize(g_ModeParams.nWidth, g_ModeParams.nHeight);

	// Default Key
	for(int i=0; i<ZACTION_COUNT; i++){
		ZACTIONKEYDESCRIPTION& keyDesc = ZGetConfiguration()->GetKeyboard()->ActionKeys[i];
		g_pInput->RegisterActionKey(i, keyDesc.nVirtualKey);
		if(keyDesc.nVirtualKeyAlt!=-1)
			g_pInput->RegisterActionKey(i, keyDesc.nVirtualKeyAlt);
	}

	g_App.SetInitialState();

//	ParseParameter(g_szCmdLine);

//	ZGetFlashBangEffect()->SetDrawCopyScreen(true);

	static const char *szDone = "Done.";
	ZGetInitialLoading()->SetLoadingStr(szDone);
	if( ZGetInitialLoading()->IsUseEnable() )
	{
#ifndef _FASTDEBUG
		ZGetInitialLoading()->SetPercentage( 100.f );
		ZGetInitialLoading()->Draw( MODE_FADEOUT, 0 ,true  );
#endif
		ZGetInitialLoading()->Release();
	}

	cclog("main : OnCreate() done\n");

	SetFocus(g_hWnd);

	return R_OK;
}




RRESULT OnDestroy(void *pParam)
{
	cclog("Destroy gunz\n");

	g_App.OnDestroy();

	SAFE_DELETE(g_pDefFont); 

	g_Core.Finalize();

	cclog("interface finalize.\n");

	SAFE_DELETE(g_pInput);
	g_DInput.Destroy();

	cclog("game input destroy.\n");

//	RGetShaderMgr()->Release();

	ZGetConfiguration()->Destroy();

	cclog("game gonfiguration destroy.\n");

	delete g_pDC;

	struct _finddata_t c_file;
	intptr_t hFile;
	char szFileName[256];
#define FONT_DIR	"Font/"
#define FONT_EXT	"ttf"
	if( (hFile = _findfirst(FONT_DIR"*."FONT_EXT, &c_file )) != -1L ){
		do{
			strcpy(szFileName, FONT_DIR);
			strcat(szFileName, c_file.name);
			RemoveFontResource(szFileName);
		}while( _findnext( hFile, &c_file ) == 0 );
		_findclose(hFile);
	}

	CCFontManager::Destroy();
	CCBitmapManager::Destroy();
	CCBitmapManager::DestroyAniBitmap();

	cclog("Bitmap manager destroy Animation bitmap.\n");


//	ZBasicInfoItem::Release(); 
//	ZGetStencilLight()->Destroy();
//	LightSource::Release();
	RBspObject::DestroyShadeMap();
	RDestroyLenzFlare();
	RAnimationFileMgr::GetInstance()->Destroy();
	ZStringResManager::ResetInstance();
	cclog("destroy gunz finish.\n");

	return R_OK;
}

RRESULT OnUpdate(void* pParam)
{
	__BP(100, "main::OnUpdate");

	g_pInput->Update();
	g_App.OnUpdate();

	const DWORD dwCurrUpdateTime = timeGetTime();
	__EP(100);

	return R_OK;
} 

RRESULT OnRender(void *pParam)
{
	__BP(101, "main::OnRender");
	if( !RIsActive() && RIsFullScreen() )
	{
		__EP(101);
		return R_NOTREADY;
	}

	g_App.OnDraw();


#ifdef _SMOOTHLOOP
	Sleep(10);
#endif

#ifndef _PUBLISH

	if(g_pDefFont) {
		static char __buffer[256];

		float fMs = 1000.f/g_fFPS;
		float fScore = 100-(fMs-(1000.f/60.f))*2;

		sprintf(__buffer, "GAME_STATE: %d  FPS : %3.3f %.3f점 (%.3f ms)", ZGetGameInterface()->GetState(),g_fFPS,fScore,fMs);
		g_pDefFont->m_Font.DrawText( CCGetWorkspaceWidth()-400,0,__buffer );
//		OutputDebugString(__buffer);
	}

#endif

	__EP(101);

	return R_OK;
}

RRESULT OnInvalidate(void *pParam)
{
	CCBitmapR2::m_dwStateBlock=NULL;
	g_App.OnInvalidate();
	return R_OK;
}

RRESULT OnRestore(void *pParam)
{
	for(int i=0; i<CCBitmapManager::GetCount(); i++){
		CCBitmapR2* pBitmap = (CCBitmapR2*)CCBitmapManager::Get(i);
		pBitmap->OnLostDevice();
	}

	g_App.OnRestore();

	return R_OK;
}

RRESULT OnActivate(void *pParam)
{
	if (ZGetGameInterface() && ZGetGameClient() && Z_ETC_BOOST)
		return R_OK;
//		ZGetGameClient()->PriorityBoost(true);
	return R_OK;
}

RRESULT OnDeActivate(void *pParam)
{
	if (ZGetGameInterface() && ZGetGameClient())
		return R_OK;
//		ZGetGameClient()->PriorityBoost(false);
	return R_OK;
}

RRESULT OnError(void *pParam)
{
	cclog("RealSpace::OnError(%d) \n", RGetLastError());

	switch (RGetLastError())
	{
	case RERROR_INVALID_DEVICE:
		{
			D3DADAPTER_IDENTIFIER9 *ai=RGetAdapterID();
			char szLog[512];
			ZTransMsg( szLog, MSG_DONOTSUPPORT_GPCARD, 1, ai->Description);

			int ret=MessageBox(NULL, szLog, ZMsg( MSG_WARNING), MB_YESNO);
			if(ret!=IDYES)
				return R_UNKNOWN;
		}
		break;
	case RERROR_CANNOT_CREATE_D3D:
		{
			ShowCursor(TRUE);

			char szLog[512];
			sprintf(szLog, ZMsg( MSG_DIRECTX_NOT_INSTALL));

			int ret=MessageBox(NULL, szLog, ZMsg( MSG_WARNING), MB_YESNO);
			if(ret==IDYES)
			{
				ShellExecute(g_hWnd, "open", ZMsg( MSG_DIRECTX_DOWNLOAD_URL), NULL, NULL, SW_SHOWNORMAL); 
			}
		}
		break;

	};

	return R_OK;
}

void SetModeParams()
{
	#ifdef _DEBUG
		g_ModeParams.bFullScreen = false;
	#else
		g_ModeParams.bFullScreen = ZGetConfiguration()->GetVideo()->bFullScreen;
	#endif

	g_ModeParams.nWidth = ZGetConfiguration()->GetVideo()->nWidth;
	g_ModeParams.nHeight = ZGetConfiguration()->GetVideo()->nHeight;
	ZGetConfiguration()->GetVideo()->nColorBits == 32 ? 
	g_ModeParams.PixelFormat = D3DFMT_X8R8G8B8 : g_ModeParams.PixelFormat = D3DFMT_R5G6B5 ;
}

void ResetAppResource()
{
	OnDestroy(0);

	ZGetConfiguration()->Destroy();
	ZGetConfiguration()->Load();

	SetModeParams();

	if( !ZApplication::GetInstance()->InitLocale() )
		CCLog("In changing language... InitLocale error !!!\n");

	ZGetConfiguration()->Load_StringResDependent();
	OnCreate(0);
	OnRestore(0);
}

int FindStringPos(char* str,char* word)
{
	if(!str || str[0]==0)	return -1;
	if(!word || word[0]==0)	return -1;

	int str_len = (int)strlen(str);
	int word_len = (int)strlen(word);

	char c;
	bool bCheck = false;

	for(int i=0;i<str_len;i++) {
		c = str[i];
		if(c == word[0]) {

			bCheck = true;

			for(int j=1;j<word_len;j++) {
				if(str[i+j]!=word[j]) {
					bCheck = false;
					break;
				}
			}

			if(bCheck) {
				return i;
			}
		}
	}
	return -1;
}

bool FindCrashFunc(char* pName)
{
//	Function Name
//	File Name 
	if(!pName) return false;

	FILE *fp;
	fp = fopen( "cclog.txt", "r" );
	if(fp==NULL)  return false;

	fseek(fp,0,SEEK_END);
	int size = ftell(fp);
	fseek(fp,0,SEEK_SET);

	char* pBuffer = new char [size];

	fread(pBuffer,1,size,fp);

	fclose(fp);

	// 우리 쏘스에서 찾는다.
	int posSource = FindStringPos(pBuffer,"ublish");
	if(posSource==-1) return false;

	int posA = FindStringPos(pBuffer+posSource,"Function Name");
//	int posB = FindStringPos(pBuffer,"File Name");	
	// filename 이 없는 경우도 있어서 이렇게 바꿨다
	int posB = posA + FindStringPos(pBuffer+posSource+posA,"\n");

	if(posA==-1) return false;
	if(posB==-1) return false;

	posA += 16;

//	int mesSize = posB-posA-6;
	int mesSize = posB-posA;
	memcpy(pName,&pBuffer[posA+posSource],mesSize);

	pName[mesSize] = 0;

	delete [] pBuffer;

	for(int i=0;i<mesSize;i++) {
		if(pName[i]==':') {
			pName[i] = '-';
		}
	}

	return true;
}

void HandleExceptionLog()
{
	#define ERROR_REPORT_FOLDER	"ReportError"

	extern char* logfilename;	// Instance on CCDebug.cpp

	// ERROR_REPORT_FOLDER 존재하는지 검사하고, 없으면 생성
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(ERROR_REPORT_FOLDER, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		if (!CreateDirectory("ReportError", NULL)) {
			MessageBox(g_hWnd, "ReportError 폴더를 생성할 수 없습니다.", APPLICATION_NAME , MB_ICONERROR|MB_OK);
			return;
		}
	} else 	{
		FindClose(hFind);
	}
}

long FAR PASCAL WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
		case WM_SYSCHAR:
			if(ZIsLaunchDevelop() && wParam==VK_RETURN)
			{
#ifndef _PUBLISH
				RFrame_ToggleFullScreen();
#endif
				return 0;
			}
			break;

		case WM_CREATE:
			if (strlen(Z_LOCALE_HOMEPAGE_TITLE) > 0)
			{
		//		ShowIExplorer(false, Z_LOCALE_HOMEPAGE_TITLE);
			}
			break;
		case WM_DESTROY:
			if (strlen(Z_LOCALE_HOMEPAGE_TITLE) > 0)
			{
		//		ShowIExplorer(true, Z_LOCALE_HOMEPAGE_TITLE);
			}
			break;
		case WM_SETCURSOR:
			if(ZApplication::GetGameInterface())
				ZApplication::GetGameInterface()->OnResetCursor();
			return TRUE; // prevent Windows from setting cursor to window class cursor

		case WM_ENTERIDLE:
			// 모달 대화상자가 코드를 블럭하고 있을 때 부모에게 보내는 idle 통지메시지
			// (일본 IME에 모달 대화상자가 있어서 넣었음)
			// 모달 대화상자로 업데이트 루프를 블럭해서 무적 어뷰즈로 악용되기 때문에 여기서 업데이트를 실행한다
			RFrame_UpdateRender();
			break;

			/*
		case  WM_LBUTTONDOWN:
			SetCapture(hWnd);
			return TRUE;
		case WM_LBUTTONUP:
			ReleaseCapture();
			return TRUE;
			*/
		case WM_KEYDOWN:
			{
				bool b = false;
			}
	}

	if(Core::GetInstance()->ProcessEvent(hWnd, message, wParam, lParam)==true)
	{
		if (ZGetGameInterface() && ZGetGameInterface()->IsReservedResetApp())	// for language changing
		{
			ZGetGameInterface()->ReserveResetApp(false);
			ResetAppResource();
		}

		return 0;
	}

	// thread safe하기위해 넣음
	if (message == WM_CHANGE_GAMESTATE)
	{
		_ZChangeGameState(wParam);
	}


	return DefWindowProc(hWnd, message, wParam, lParam);
}


bool CheckFileList()
{
	CCZFileSystem *pfs=ZApplication::GetFileSystem();
	CCZFile mzf;

	string strFileNameFillist(FILENAME_FILELIST);

	if(!mzf.Open(strFileNameFillist.c_str() ,pfs))
		return false;

	char *buffer;
	buffer=new char[mzf.GetLength()+1];
	mzf.Read(buffer,mzf.GetLength());
	buffer[mzf.GetLength()]=0;

	CCXmlDocument aXml;
	aXml.Create();
	if(!aXml.LoadFromMemory(buffer))
	{
		delete buffer;
		return false;
	}

	delete buffer;

	int iCount, i;
	::CCXmlElement		aParent, aChild;
	aParent = aXml.GetDocumentElement();
	iCount = aParent.GetChildNodeCount();

	char szTagName[256];
	for (i = 0; i < iCount; i++)
	{
		aChild = aParent.GetChildNode(i);
		aChild.GetTagName(szTagName);
		if(stricmp(szTagName,"FILE")==0)
		{
			char szContents[256],szCrc32[256];
			aChild.GetAttribute(szContents,"NAME");
			aChild.GetAttribute(szCrc32,"CRC32");

			if(stricmp(szContents,"config.xml")!=0)
			{
				unsigned int crc32_list = pfs->GetCRC32(szContents);
				unsigned int crc32_current;
				sscanf(szCrc32,"%x",&crc32_current);
			}
		}
	}

	return true;
}


enum RBASE_FONT{
	RBASE_FONT_GULIM = 0,
	RBASE_FONT_BATANG = 1,

	RBASE_FONT_END
};

static int g_base_font[RBASE_FONT_END];
static char g_UserDefineFont[256];

bool _GetFileFontName(char* pUserDefineFont)
{
	if(pUserDefineFont==NULL) return false;

	FILE* fp = fopen("_Font", "rt");
	if (fp) {
		fgets(pUserDefineFont,256, fp);
		fclose(fp);
		return true;
	}
	return false;
}


bool CheckFont()
{
	char FontPath[MAX_PATH];
	char FontNames[MAX_PATH+100];

	::GetWindowsDirectory(FontPath, MAX_PATH);

	strcpy(FontNames,FontPath);
	strcat(FontNames, "\\Fonts\\gulim.ttc");

	if (_access(FontNames,0) != -1)	{ g_base_font[RBASE_FONT_GULIM] = 1; }
	else							{ g_base_font[RBASE_FONT_GULIM] = 0; }

	strcpy(FontNames,FontPath);
	strcat(FontNames, "\\Fonts\\batang.ttc");

	if (_access(FontNames,0) != -1)	{ g_base_font[RBASE_FONT_BATANG] = 1; }
	else							{ g_base_font[RBASE_FONT_BATANG] = 0; }

	if(g_base_font[RBASE_FONT_GULIM]==0 && g_base_font[RBASE_FONT_BATANG]==0) {//둘다없으면..

		if( _access("_Font",0) != -1) { // 이미 기록되어 있다면..
			_GetFileFontName( g_UserDefineFont );
		}
		else {

			int hr = IDOK;
			if(hr==IDOK) {
				return true;
			}
			else {
				return false;
			}
		}
	}
	return true;
}


// 해당 텍스트 파일에 해당 글귀가 있으면 XTRAP 테스트 코드가 수행된다. (뒷구멍) //
void UpgradeMrsFile()
{
	char temp_path[ 1024];
	sprintf( temp_path,"*");

	FFileList file_list;
	GetFindFileListWin(temp_path,".mrs",file_list);
	file_list.UpgradeMrs();
}

HANDLE Mutex = 0;
DWORD g_dwMainThreadID;


// STEP1
int WINAPI WinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	InitLog(CCLOGSTYLE_DEBUGSTRING|CCLOGSTYLE_FILE);
	g_dwMainThreadID= GetCurrentThreadId();
	

	char szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);
	cclog("-------------------------------------------------------------\n");
	cclog("Current working directory: %s\n", szModuleFileName);
	cclog("-------------------------------------------------------------\n");
	srand( (unsigned)time( NULL ));
	cclog("GUNZ " STRFILEVER " launched. build ("__DATE__" "__TIME__") \n");
	char szDateRun[128]="";
	char szTimeRun[128]="";
	_strdate( szDateRun );
	_strtime( szTimeRun );
	cclog("Log time (%s %s)\n", szDateRun, szTimeRun);
	CCSysInfoLog();

	g_App.InitFileSystem();
	ZGetConfiguration()->Load();

	ZStringResManager::MakeInstance();
	if( !ZApplication::GetInstance()->InitLocale() )
	{
		CCLog("Locale Init error !!!\n");
		return false;
	}

	DWORD ver_major = 0;
	DWORD ver_minor = 0;
	TCHAR ver_letter = ' ';



	if(CheckFont()==false) {
		CCLog("CheckFont() failed.\n");
		return 0;
	}

	RSetFunction(RF_CREATE	,	OnCreate);
	RSetFunction(RF_RENDER	,	OnRender);
	RSetFunction(RF_UPDATE	,	OnUpdate);
	RSetFunction(RF_DESTROY ,	OnDestroy);
	RSetFunction(RF_INVALIDATE,	OnInvalidate);
	RSetFunction(RF_RESTORE,	OnRestore);
	RSetFunction(RF_ACTIVATE,	OnActivate);
	RSetFunction(RF_DEACTIVATE,	OnDeActivate);
	RSetFunction(RF_ERROR,		OnError);

	SetModeParams();

//	STEP2	
	cclog("Entering RMain()\n");
	const int nRMainReturn = RMain(APPLICATION_NAME,this_inst,prev_inst,cmdline,cmdshow,&g_ModeParams,WndProc,IDI_ICON1);
	if( 0 != nRMainReturn )
		return nRMainReturn;


	if( 0 != RInitD3D(&g_ModeParams) )
	{
		MessageBox(g_hWnd, "fail to initialize DirectX", NULL, MB_OK);
		cclog( "error init RInitD3D\n" );
		return 0;
	}
	cclog("Entering RRun()\n");
	const int nRRunReturn = RRun();
	ShowWindow(g_hWnd, SW_MINIMIZE);

	cclog("========================== DONE ===============================\n");

	ZStringResManager::FreeInstance();
	return 0;//nRRunReturn;
	}
