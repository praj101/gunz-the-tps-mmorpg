#include "stdafx.h"

#include "ZApplication.h"
#include "ZInterfaceListener.h"
#include "CCWidget.h"
#include "CCEdit.h"
#include "CCComboBox.h"
#include "ZMapListBox.h"
#include "ZPost.h"
#include "CCMatchStage.h"
#include "ZConfiguration.h"
#include "CCSlider.h"
#include "CCMatchStage.h"
#include "ZCharacterView.h"
#include "ZCharacterViewList.h"
#include "ZCharacterSelectView.h"
#include "ZShop.h"
#include "ZMyItemList.h"
#include "ZMyInfo.h"
#include "ZStageSetting.h"
#include "CCChattingFilter.h"
#include "ZRoomListBox.h"
#include "ZPlayerListBox.h"
#include "CCDebug.h"
#include "ZChat.h"
#include "ZMsgBox.h"
#include "ZActionKey.h"
#include "ZPlayerSelectListBox.h"
#include "ZChannelListItem.h"
#include "CCTabCtrl.h"

#include "ZApplication.h"
#include "ZServerView.h"
#include "ZCharacterView.h"

#include "ZMonsterBookInterface.h"

#include "CCMatchGlobal.h"

#include "ZShopEquipInterface.h"
#include "ZShopEquipListbox.h"

#ifdef LOCALE_NHNUSA
#include "ZNHN_USA_Report.h"
#endif


// Chat Input Listener
class MChatInputListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, CCEdit_ENTER_VALUE)==true){
			const char* szCommand = pWidget->GetText();
			ZChatOutput(szCommand);

			char szChat[512];
			strcpy(szChat, pWidget->GetText());
			if (ZGetGameInterface()->GetChat()->Input(szChat))
			{
			}

			pWidget->SetText("");
			return true;
		}
		else if ((CCWidget::IsMsg(szMessage, CCEdit_CHAR_MSG)==true) || (CCWidget::IsMsg(szMessage, CCEdit_KEYDOWN_MSG)==true))
		{
			ZGetGameInterface()->GetChat()->FilterWhisperKey(pWidget);
		}

		return false;
	}
};
MChatInputListener	g_ChatInputListener;



class MHotBarButton : public CCButton{
protected:
	char	m_szCommandString[256];
protected:
	virtual bool OnDrop(CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString){
		m_pIcon = pBitmap;
		AttachToolTip(szString);
		strcpy(m_szCommandString, szItemString);
		//SetText(szString);
		return true;
	}

public:
	MHotBarButton(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL)
	: CCButton(szName, pParent, pListener){
		strcpy(m_szCommandString, "Command is not assigned");
	}
	virtual bool IsDropable(CCWidget* pSender){
		return true;
	}
	const char* GetCommandString(){
		return m_szCommandString;
	}
};


class MHotBarButtonListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, CCBTN_CLK_MSG)==true){
			MHotBarButton* pButton = (MHotBarButton*)pWidget;
			const char* szCommandString = pButton->GetCommandString();

			char szParse[256];
			ZGetGame()->ParseReservedWord(szParse, szCommandString);

			char szErrMsg[256];
			ZChatOutput(sColor(0xFFFFFFFF), szCommandString);
			if( ZGetGameClient()->Post(szErrMsg, 256, szParse)==false ){
				ZChatOutput(sColor(0xFFFFC600), szErrMsg);
			}

			return true;
		}
		return false;
	}
};
MHotBarButtonListener	g_HotBarButtonListener;

class CCLoginListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, CCBTN_CLK_MSG)==true)
		{
			ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();

			ZServerView* pServerList = (ZServerView*)pResource->FindWidget( "SelectedServer");
			if ( !pServerList)
				return false;

			ServerInfo* pServer = pServerList->GetSelectedServer();
			if ( pServer)
			{
				if ( pServer->nType == 0 )
					return false;

				if( !pServer->bIsLive )
					return false;

				CCWidget* pWidget = pResource->FindWidget( "LoginOK");
				if ( pWidget)
					pWidget->Enable( false);

				pWidget = pResource->FindWidget( "LoginFrame");
				if ( pWidget)
					pWidget->Show( false);

				pWidget = pResource->FindWidget( "Login_ConnectingMsg");
				if ( pWidget)
					pWidget->Show( true);


				ZGetGameInterface()->m_bLoginTimeout = true;
				ZGetGameInterface()->m_dwLoginTimeout = timeGetTime() + (90 * 1000);


				if ( pServer->nType == 7)	// Debug server Type1을 Type7로 변경
				{
					CCWidget* pAddr = pResource->FindWidget( "ServerAddress");
					CCWidget* pPort = pResource->FindWidget( "ServerPort");

					ZPostConnect( pAddr->GetText(), atoi(pPort->GetText()));		// Debug server
				}
				else
					ZPostConnect( pServer->szAddress, pServer->nPort);				// Game server

				CCLabel* pLabel = (CCLabel*)pResource->FindWidget( "LoginError");
				if ( pLabel)
					pLabel->SetText("");
			}
		}
		return false;
	}
};
CCLoginListener	g_LoginListener;

class CCLogoutListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, CCBTN_CLK_MSG)==true){

			cclog("CCLogoutListener !\n");
			// 체크후 로그아웃~
			/////////////////
			ZPostDisconnect();
			ZGetGameInterface()->SetState(GUNZ_LOGIN);
			return true;
		}
		return false;
	}
};
CCLogoutListener	g_LogoutListener;

class MExitListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, CCBTN_CLK_MSG)==true){

			cclog("MExitListener !\n");
			ZApplication::Exit();

			return true;
		}
		return false;
	}
};
MExitListener	g_ExitListener;

class CCChannelChatInputListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, CCEdit_ENTER_VALUE)==true){
			char szChat[512];
			if (strlen(pWidget->GetText()) < 255)
			{
				strcpy(szChat, pWidget->GetText());
				if (ZGetGameInterface()->GetChat()->Input(szChat))
				{
					pWidget->SetText("");
				}
			}
			return true;
		}
		else if ((CCWidget::IsMsg(szMessage, CCEdit_CHAR_MSG)==true) || (CCWidget::IsMsg(szMessage, CCEdit_KEYDOWN_MSG)==true))
		{
			ZGetGameInterface()->GetChat()->FilterWhisperKey(pWidget);
		}
		return false;
	}
};
CCChannelChatInputListener	g_ChannelChatInputListener;

class MStageChatInputListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, CCEdit_ENTER_VALUE)==true){
			char szChat[512];
			if (strlen(pWidget->GetText()) < 255)
			{
				strcpy(szChat, pWidget->GetText());
				if (ZGetGameInterface()->GetChat()->Input(szChat))
				{
					pWidget->SetText("");
				}
			}
			return true;
		}
		else if ((CCWidget::IsMsg(szMessage, CCEdit_CHAR_MSG)==true) || (CCWidget::IsMsg(szMessage, CCEdit_KEYDOWN_MSG)==true))
		{
			ZGetGameInterface()->GetChat()->FilterWhisperKey(pWidget);
		}

		return false;
	}
};
MStageChatInputListener	g_StageChatInputListener;


class MGameStartListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, CCBTN_CLK_MSG)==true){

			const MSTAGE_SETTING_NODE* pStageSetting = ZGetGameClient()->GetMatchStageSetting()->GetStageSetting();
			int nPlayerCnt = (int)ZGetGameClient()->GetMatchStageSetting()->m_CharSettingList.size();
			if(nPlayerCnt > pStageSetting->nMaxPlayers && nPlayerCnt!=0 && pStageSetting->nMaxPlayers!=0)
			{
				// 최대인원보다 방인원이 많을시 처리
				char szText[128] = {0,};
				sprintf(szText, "%s\n", ZErrStr(MERR_PERSONNEL_TOO_MUCH));
				ZGetGameInterface()->ShowMessage(szText);
				return false;
			}

			// 맵 등록이 안되어 있으면 게임 시작은 해주지 않는다.
			if(!ZApplication::GetStageInterface()->GetIsRelayMapRegisterComplete())
			{
				ZGetGameInterface()->ShowMessage(MSG_GAME_RELAYMAP_CONFIRM_BUTTON_PUSH);
				return true;
			}

			// 맵이 골라졌을때 시작 가능.
			if(ZGetGameClient()->GetMatchStageSetting()->GetMapName()[0]!=0)
			{
				ZApplication::GetStageInterface()->ChangeStageEnableReady( true);

				ZPostStageStart(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID());
			}
			else
			{
				ZGetGameInterface()->ShowMessage("선택하신 맵이 없습니다. 맵을 선택해 주세요.");
			}

			return true;
		}
		return false;
	}
};
MGameStartListener	g_GameStartListener;

class MMapChangeListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, CCBTN_CLK_MSG)==true){
			ZGetGameInterface()->ShowWidget("MapFrame", true, true);

			return true;
		}
		return false;
	}
};
MMapChangeListener	g_MapChangeListener;

class MMapSelectListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, CCBTN_CLK_MSG)==true){
			ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
			ZMapListBox* pWidget = (ZMapListBox*)pResource->FindWidget("MapList");
			char szMapName[_MAX_DIR];
			strcpy(szMapName, pWidget->GetSelItemString());
			if(szMapName!=NULL){
				ZApplication::GetStageInterface()->SetMapName(szMapName);
				ZPostStageMap(ZGetGameClient()->GetStageUID(), szMapName);

				// 현재 창을 닫는다.
				if(pWidget->GetParent()!=NULL) pWidget->GetParent()->Show(false);
			}

			return true;
		}
		return false;
	}
};
MMapSelectListener	g_MapSelectListener;



class MParentCloseListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, CCBTN_CLK_MSG)==true){
			if(pWidget->GetParent()!=NULL) pWidget->GetParent()->Show(false);
			return true;
		}
		return false;
	}
};
MParentCloseListener	g_ParentCloseListener;


class MStageCreateFrameCallerListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, CCBTN_CLK_MSG)==true){
			ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
			CCWidget* pFindWidget = pResource->FindWidget("StageCreateFrame");
			if(pFindWidget!=NULL) pFindWidget->Show(true, true);

			CCEdit* pPassEdit = (CCEdit*)pResource->FindWidget("StagePassword");
			if (pPassEdit!=NULL)
			{
				pPassEdit->SetMaxLength(STAGEPASSWD_LENGTH);
				pPassEdit->SetText("");
			}

			return true;
		}
		return false;
	}
};
MStageCreateFrameCallerListener	g_StageCreateFrameCallerListener;




class MSelectCharacterComboBoxListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage)
	{
		if(CCWidget::IsMsg(szMessage, MCMBBOX_CHANGED)==true)
		{
			ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();

			if (ZGetGameInterface()->GetCharacterSelectView() != NULL)
			{
				ZGetGameInterface()->GetCharacterSelectView()->SelectChar( ZCharacterSelectView::GetSelectedCharacter() );
			}

			return true;
		}

		return false;
	}
};
MSelectCharacterComboBoxListener	g_SelectCharacterComboBoxListener;

CCListener* ZGetChatInputListener()
{
	return &g_ChatInputListener;
}

CCListener* ZGetLoginListener()
{
	return &g_LoginListener;
}

CCListener* ZGetLogoutListener()
{
	return &g_LogoutListener;
}

CCListener* ZGetExitListener()
{
	return &g_ExitListener;
}

CCListener* ZGetChannelChatInputListener()
{
	return &g_ChannelChatInputListener;
}

CCListener* ZGetStageChatInputListener()
{
	return &g_StageChatInputListener;
}

CCListener* ZGetGameStartListener()
{
	return &g_GameStartListener;
}

CCListener* ZGetMapChangeListener()
{
	return &g_MapChangeListener;
}

CCListener* ZGetMapSelectListener()
{
	return &g_MapSelectListener;
}

CCListener* ZGetParentCloseListener()
{
	return &g_ParentCloseListener;
}


CCListener* ZGetStageCreateFrameCallerListener()
{
	return &g_StageCreateFrameCallerListener;
}

CCListener* ZGetSelectCharacterComboBoxListener()
{
	return &g_SelectCharacterComboBoxListener;
}


BEGIN_IMPLEMENT_LISTENER(ZGetMapListListener, MLB_ITEM_DBLCLK)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	ZMapListBox* pMapList = (ZMapListBox*)pResource->FindWidget("MapList");
	const char* pszSelItemString = pMapList->GetSelItemString();
	if (pszSelItemString) {
		char szMapName[_MAX_DIR];
		sprintf(szMapName, pszSelItemString);
		ZApplication::GetStageInterface()->SetMapName(szMapName);
		ZPostStageMap(ZGetGameClient()->GetStageUID(), szMapName);
		// 현재 창을 닫는다.
		if(pWidget->GetParent()!=NULL) pWidget->GetParent()->GetParent()->Show(false);
	}
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetStageListFrameCallerListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();

	/*
	CCWidget* pFindWidget = pResource->FindWidget("StageListFrame");
	if(pFindWidget!=NULL) pFindWidget->Show(true, true);
	*/
	ZGetGameClient()->StartStageList();
END_IMPLEMENT_LISTENER()


BEGIN_IMPLEMENT_LISTENER(ZGetStageCreateBtnListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pNameWidget = pResource->FindWidget("StageName");
	if(pNameWidget==NULL) return true;
	char szStageName[128], szStagePassword[128];
	bool bPrivate = false;
	strcpy(szStageName, pNameWidget->GetText());

	CCEdit* pPassEdit = (CCEdit*)pResource->FindWidget("StagePassword");
	if (pPassEdit)
	{
		if ((strlen(pPassEdit->GetText()) > 0) && (strlen(pPassEdit->GetText()) <= STAGEPASSWD_LENGTH))
			bPrivate = true;
		else 
			bPrivate = false;


		if (bPrivate == true)
		{
			strcpy(szStagePassword, pPassEdit->GetText());
		}
		else
		{
			memset(szStagePassword, 0, sizeof(szStagePassword));
		}
	}

	if(pWidget->GetParent()!=NULL) pWidget->GetParent()->Show(false);
	ZApplication::GetStageInterface()->ChangeStageButtons(false, true, false);

	MSTAGE_SETTING_NODE setting;
	setting.uidStage = CCUID(0, 0);
	memset(setting.szMapName, 0, sizeof(setting.szMapName));
	setting.nGameType = CCMATCH_GAMETYPE_DEATHMATCH_SOLO;
	setting.nRoundMax = 10;
	setting.nLimitTime = 10;
	setting.nMaxPlayers = 8;
	setting.bIsRelayMap = false;

	ZApplication::GetStageInterface()->ChangeStageGameSetting( &setting);

	if ( !MGetChattingFilter()->IsValidStr( szStageName, 1))
	{
		char szMsg[ 256 ];
		ZTransMsg( szMsg, MSG_WRONG_WORD_NAME, 1, MGetChattingFilter()->GetLastFilteredStr());
		ZGetGameInterface()->ShowMessage( szMsg );
	}
	else
	{
		// string strStageName = MGetChattingFilter()->AbuseWordPasser( szStageName );
		// memset( szStageName, 0, 128 );
		// strncpy( szStageName, &strStageName[0], strStageName.size() );
		ZGetGameInterface()->EnableLobbyInterface(false);
		ZPostStageCreate(ZGetGameClient()->GetPlayerUID(), szStageName, bPrivate, szStagePassword);
	}
END_IMPLEMENT_LISTENER()

// 비밀방 들어가기 확인버튼
BEGIN_IMPLEMENT_LISTENER(ZGetPrivateStageJoinBtnListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	ZRoomListBox* pRoomListBox = (ZRoomListBox*)pResource->FindWidget("Lobby_StageList");
	if (pRoomListBox)
	{
		pRoomListBox->RequestSelPrivateStageJoin();
	}

	CCWidget* pPrivateStageJoinFrame = pResource->FindWidget("PrivateStageJoinFrame");
	if (pPrivateStageJoinFrame)
	{
		pPrivateStageJoinFrame->Show(false);
	}
END_IMPLEMENT_LISTENER()


// Channel
BEGIN_IMPLEMENT_LISTENER(ZGetChannelListFrameCallerListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pFindWidget = pResource->FindWidget("ChannelListFrame");
	if(pFindWidget!=NULL) pFindWidget->Show(true, true);

	CCButton* pButton = (CCButton*)pResource->FindWidget("MyClanChannel");
	if ( pButton)
		pButton->Enable( ZGetMyInfo()->IsClanJoined());

	pButton = (CCButton*)pResource->FindWidget("ChannelList_DuelTournament");
	if ( pButton)
		pButton->Show( ZGetGameClient()->IsEnabledDuelTournament());

	CCCHANNEL_TYPE nCurrentChannelType = ZGetGameClient()->GetChannelType();
	ZGetGameInterface()->InitChannelFrame(nCurrentChannelType);
	ZGetGameClient()->StartChannelList(nCurrentChannelType);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetChannelListJoinButtonListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	MListBox* pChannelList = (MListBox*)pResource->FindWidget("ChannelList");
	ZChannelListItem* pItem = (ZChannelListItem*)pChannelList->GetSelItem();
	if (pItem) {
		ZGetGameClient()->RequestChannelJoin(pItem->GetUID());
	}
	if(pWidget->GetParent()!=NULL) pWidget->GetParent()->Show(false);
	ZGetGameClient()->StopChannelList();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetChannelListCloseButtonListener, CCBTN_CLK_MSG)
	if(pWidget->GetParent()!=NULL) pWidget->GetParent()->Show(false);
	ZGetGameClient()->StopChannelList();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetChannelListListener, MLB_ITEM_DBLCLK)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	MListBox* pChannelList = (MListBox*)pResource->FindWidget("ChannelList");
	ZChannelListItem* pItem = (ZChannelListItem*)pChannelList->GetSelItem();
	if (pItem) {
		ZGetGameClient()->RequestChannelJoin(pItem->GetUID());
	}
	if(pWidget->GetParent()!=NULL) pWidget->GetParent()->Show(false);
	ZGetGameClient()->StopChannelList();
END_IMPLEMENT_LISTENER()


// 스테이지 조인
BEGIN_IMPLEMENT_LISTENER(ZGetStageJoinListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	ZRoomListBox* pRoomListBox = (ZRoomListBox*)pResource->FindWidget("Lobby_StageList");
	if (pRoomListBox)
	{
		pRoomListBox->RequestSelStageJoin();

	}
END_IMPLEMENT_LISTENER()


// 기타 옵션
BEGIN_IMPLEMENT_LISTENER(ZGetStageSettingCallerListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("StageSettingFrame");
	if(pWidget!=NULL)
		pWidget->Show(true, true);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetStageSettingStageTypeListener, MCMBBOX_CHANGED)
	{
//		ZStageSetting::AdjustLimitTimeStageSettingDialog();
		ZStageSetting::InitStageSettingGameFromGameType();
		ZStageSetting::PostDataToServer();
	}
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetStageTeamRedListener, CCBTN_CLK_MSG)
	CCButton* pButton = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("StageTeamRed");
	if(pButton && !pButton->GetCheck() ) pButton->SetCheck(true);
	pButton = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("StageTeamBlue");
	if(pButton) pButton->SetCheck( false );
	ZPostStageTeam(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID(), MMT_RED);
END_IMPLEMENT_LISTENER()


BEGIN_IMPLEMENT_LISTENER(ZGetStageTeamBlueListener, CCBTN_CLK_MSG)
	CCButton* pButton = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("StageTeamBlue");
	if(pButton && !pButton->GetCheck() ) pButton->SetCheck(true);
	pButton = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("StageTeamRed");
	if(pButton) pButton->SetCheck( false );
	ZPostStageTeam(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID(), MMT_BLUE);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetStageReadyListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	
	bool bReady = false;
	CCButton* pReadyBtn = (CCButton*)pResource->FindWidget("StageReady");
	if(pReadyBtn) bReady=pReadyBtn->GetCheck();

	CCMatchObjectStageState nStageState;
	if (bReady)
		nStageState = MOSS_READY;
	else
		nStageState = MOSS_NONREADY;

	ZPostStageState(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID(), nStageState);
	ZApplication::GetStageInterface()->ChangeStageEnableReady(bReady);
END_IMPLEMENT_LISTENER()

// 관전 모드 버튼
BEGIN_IMPLEMENT_LISTENER(ZGetStageObserverBtnListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCButton* pObserverBtn = (CCButton*)pResource->FindWidget("StageObserverBtn");
	if ( pObserverBtn)
	{
		if ( pObserverBtn->GetCheck())
			ZPostStageTeam(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID(), MMT_SPECTATOR);
		else
		{
			CCButton* pBlueBtn = (CCButton*)pResource->FindWidget("StageTeamBlue");

			if ( ZGetGameInterface()->m_bTeamPlay) // 팀전 이면..
			{
				if ( pBlueBtn->GetCheck())
					ZPostStageTeam(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID(), MMT_BLUE);
				else
					ZPostStageTeam(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID(), MMT_RED);
			}
			else	// 개인전이면...
			{
				ZPostStageTeam(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID(), MMT_ALL);
			}
		}
	}
END_IMPLEMENT_LISTENER()

// 스태이지 변경시
BEGIN_IMPLEMENT_LISTENER(ZGetStageSettingChangedComboboxListener, MCMBBOX_CHANGED)
	ZStageSetting::PostDataToServer();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetStageSettingApplyBtnListener, CCBTN_CLK_MSG)
	ZStageSetting::ApplyStageSettingDialog();
END_IMPLEMENT_LISTENER()


BEGIN_IMPLEMENT_LISTENER(ZGetLobbyListener, CCBTN_CLK_MSG)
	ZPostStageLeave(ZGetGameClient()->GetPlayerUID());//, ZGetGameClient()->GetStageUID());
END_IMPLEMENT_LISTENER()


BEGIN_IMPLEMENT_LISTENER(ZGetLoginStateButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->SetState(GUNZ_LOGIN);
END_IMPLEMENT_LISTENER()


BEGIN_IMPLEMENT_LISTENER(ZGetGreeterStateButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->SetState(GUNZ_GREETER);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetBattleExitButtonListener, CCBTN_CLK_MSG)
	// 클랜전이 진행중이면 종료하지 않는다
	if ( !ZGetGameClient()->IsLadderGame())
	{
		if(pWidget->GetParent()!=NULL) pWidget->GetParent()->Show(false);
//		ZGetGameInterface()->SetCursorEnable(false);
		ZGetGameInterface()->ReserveLeaveBattle();
	}
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetStageExitButtonListener, CCBTN_CLK_MSG)
	if(pWidget->GetParent()!=NULL) pWidget->GetParent()->Show(false);
//	ZGetGameInterface()->SetCursorEnable(false);
	ZGetGameInterface()->ReserveLeaveStage();
END_IMPLEMENT_LISTENER()


BEGIN_IMPLEMENT_LISTENER(ZGetCombatMenuCloseButtonListener, CCBTN_CLK_MSG)
	if(pWidget->GetParent()!=NULL) pWidget->GetParent()->Show(false);
//	ZGetGameInterface()->SetCursorEnable(false);
END_IMPLEMENT_LISTENER()


BEGIN_IMPLEMENT_LISTENER(ZGetPreviousStateButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->SetState(GUNZ_PREVIOUS);
END_IMPLEMENT_LISTENER()



BEGIN_IMPLEMENT_LISTENER(ZGetShopCallerButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->ShowShopDialog();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetShopCloseButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->ShowShopDialog(false);
/*
	상점은 장비가 안 바뀐다~
	ZCharacterViewList* pVLL = ZGetCharacterViewList(GUNZ_LOBBY);
	if(pVLL)
		pVLL->ChangeCharacter();
	ZCharacterViewList* pVLS = ZGetCharacterViewList(GUNZ_STAGE);
	if(pVLS)
		pVLS->ChangeCharacter();
*/
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetEquipmentCallerButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->ShowEquipmentDialog();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetEquipmentCloseButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->ShowEquipmentDialog(false);

{	

	int nState = ZGetGameInterface()->GetState();

	if(nState==GUNZ_LOBBY)
	{
		ZCharacterViewList* pVLL = ZGetCharacterViewList(GUNZ_LOBBY);
		if(pVLL)
			pVLL->ChangeCharacterInfo();
	}
	else if(nState==GUNZ_STAGE)
	{
		ZCharacterViewList* pVLS = ZGetCharacterViewList(GUNZ_STAGE);
		if(pVLS)
			pVLS->ChangeCharacterInfo();
	}
}

END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetCharSelectionCallerButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->ChangeToCharSelection();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetQuickJoinButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->RequestQuickJoin();
END_IMPLEMENT_LISTENER()
BEGIN_IMPLEMENT_LISTENER(ZGetLobbyCharInfoCallerButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->ShowErrorMessage( MERR_NOT_SUPPORT );
END_IMPLEMENT_LISTENER()


BEGIN_IMPLEMENT_LISTENER(ZGetBuyButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->OnBuyButton();
END_IMPLEMENT_LISTENER()

//------------------------------------------------------------------------------------------------------

BEGIN_IMPLEMENT_LISTENER(ZGetSellButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->OnSellButton();
END_IMPLEMENT_LISTENER()

//------------------------------------------------------------------------------------------------------


BEGIN_IMPLEMENT_LISTENER(ZGetCountableItemTradeDlgCloseListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->GetItemCountDlg()->Close();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetCountableItemTradeDlgOkButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->GetItemCountDlg()->OnDlgDone();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetCountableItemTradeDlgCountUpButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->GetItemCountDlg()->AddCount(1);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetCountableItemTradeDlgCountDnButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->GetItemCountDlg()->AddCount(-1);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetCountableItemTradeDlgCountChangeListener, CCEdit_TEXT_CHANGED)
	ZGetGameInterface()->GetShopEquipInterface()->GetItemCountDlg()->OnEditBoxChanged();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetSellCashItemConfirmDlgOkButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->GetSellCashItemConfirmDlg()->OnOkButton();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetSellCashItemConfirmDlgCancelListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->GetSellCashItemConfirmDlg()->Close();
END_IMPLEMENT_LISTENER()

//------------------------------------------------------------------------------------------------------


BEGIN_IMPLEMENT_LISTENER(ZGetEquipButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->Equip();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetEquipmentSearchButtonListener, CCBTN_CLK_MSG)
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetSendAccountIteCCButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->OnSendAccountButton();	
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetBringAccountIteCCButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->OnBringAccountButton();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetShopCachRechargeButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->ShowErrorMessage( MERR_NOT_SUPPORT );
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetShopSearchCallerButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->ShowErrorMessage( MERR_NOT_SUPPORT );
END_IMPLEMENT_LISTENER()




// 맵선택

void PostMapname()
{
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCComboBox* pMapCombo = (CCComboBox*)pResource->FindWidget("MapSelection");
	const char* pszSelItemString = pMapCombo->GetSelItemString();

	if (pszSelItemString) 
	{
		char szMapName[_MAX_DIR];
		sprintf(szMapName, pszSelItemString);
/*
		// 완전 하드코딩... -_-;
		if ( strcmp( szMapName, "Island") == 0)
			strcpy( szMapName, "island");
		else if ( strcmp( szMapName, "Port") == 0)
			strcpy( szMapName, "port");
		else if ( strcmp( szMapName, "Snow Town") == 0)
			strcpy( szMapName, "Snow_Town");
*/
		ZApplication::GetStageInterface()->SetMapName(szMapName);
		ZPostStageMap(ZGetGameClient()->GetStageUID(), szMapName);

		/// 맵이 변경되면 릴레이맵 리스트 세팅중을 끝낸다.
		ZApplication::GetStageInterface()->SetIsRelayMapRegisterComplete(true);

		if(strcmp(CCMATCH_MAPNAME_RELAYMAP, pszSelItemString) == 0)
		{
			// 릴레이맵을 고르면 처음에 맨션맵 하나를 강제로 넣어준다
			void* pRelayMapListBlob = CCMakeBlobArray(sizeof(CCTD_RelayMap), 1);	// 블럭 만들기, 맵리스트 세팅
			CCTD_RelayMap* pRelayMapList = (CCTD_RelayMap*)CCGetBlobArrayElement(pRelayMapListBlob, 0);
			pRelayMapList->nMapID = MGetMapDescMgr()->GetMapID(CCMATCH_MAP_MANSION);

			ZPostStageRelayMapInfoUpdate(ZGetGameClient()->GetStageUID(), RELAY_MAP_TURN, RELAY_MAP_3REPEAT, pRelayMapListBlob);
			CCEraseBlobArray(pRelayMapListBlob);
		}
	}
}

BEGIN_IMPLEMENT_LISTENER(ZGetMapComboListener, MCMBBOX_CHANGED)
	PostMapname();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetSelectMapPrevButtonListener, CCBTN_CLK_MSG)

	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCComboBox* pComboBox = (CCComboBox*)pResource->FindWidget("MapSelection");

	if(pComboBox)
	{
		pComboBox->SetPrevSel();
		PostMapname();
	}

END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetSelectMapNextButtonListener, CCBTN_CLK_MSG)

	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCComboBox* pComboBox = (CCComboBox*)pResource->FindWidget("MapSelection");

	if(pComboBox)
	{
		pComboBox->SetNextSel();
		PostMapname();
	}

END_IMPLEMENT_LISTENER()


// 선택된 캐릭터 돌리며 보기 ( 카메라 돌리기 처리 )

BEGIN_IMPLEMENT_LISTENER(ZGetSelectCameraLeftButtonListener, CCBTN_CLK_MSG)
 
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetSelectCameraRightButtonListener, CCBTN_CLK_MSG)

END_IMPLEMENT_LISTENER()


BEGIN_IMPLEMENT_LISTENER(ZGetCreateCharacterLeftButtonListener, CCBTN_CLK_MSG)
	if(ZGetGameInterface()->GetCharacterSelectView())
		ZGetGameInterface()->GetCharacterSelectView()->CharacterLeft();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetCreateCharacterRightButtonListener, CCBTN_CLK_MSG)
	if(ZGetGameInterface()->GetCharacterSelectView())
		ZGetGameInterface()->GetCharacterSelectView()->CharacterRight();
END_IMPLEMENT_LISTENER()


// 캐릭터 선택, 생성 관련
static DWORD g_dwClockCharSelBtn = 0;
void CharacterSelect( int nNum)
{
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();

	// 더블 클릭시 캐릭터 바로 선택
	if ( (ZCharacterSelectView::GetSelectedCharacter() == nNum) && ((timeGetTime() - g_dwClockCharSelBtn ) <= 300))
	{
		ZGetGameInterface()->OnCharSelect();

		return ;
	}

	g_dwClockCharSelBtn = timeGetTime();

	ZGetGameInterface()->ChangeSelectedChar( nNum);
}

BEGIN_IMPLEMENT_LISTENER( ZGetSelectCharacterButtonListener0, CCBTN_CLK_MSG)
	CharacterSelect( 0);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER( ZGetSelectCharacterButtonListener1, CCBTN_CLK_MSG)
	CharacterSelect( 1);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER( ZGetSelectCharacterButtonListener2, CCBTN_CLK_MSG)
	CharacterSelect( 2);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER( ZGetSelectCharacterButtonListener3, CCBTN_CLK_MSG)
	CharacterSelect( 3);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetSelectCharacterButtonListener, CCBTN_CLK_MSG)
	if ( ZCharacterSelectView::GetNumOfCharacter())
	{
		if ( ZGetGameInterface()->GetCharacterSelectView() != NULL)
		{
			ZGetGameInterface()->OnCharSelect();

			CCWidget* pWidget = ZGetGameInterface()->GetIDLResource()->FindWidget( "CS_SelectCharDefKey");
			pWidget->Enable( false);
		}
	}
	else
	{
		ZGetGameInterface()->ShowMessage("해당 슬롯에 캐릭터가 없습니다.");
	}
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetShowCreateCharacterButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->SetState(GUNZ_CHARCREATION);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetDeleteCharacterButtonListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();

	char szName[256];
	sprintf( szName, "CharSel_Name%d", ZCharacterSelectView::GetSelectedCharacter() );
	CCLabel* pLabel = (CCLabel*)pResource->FindWidget( szName);
	
	if( ZCharacterSelectView::GetNumOfCharacter())
	{
		int ret = ZGetGameClient()->ValidateRequestDeleteChar();

		if (ret != ZOK)
		{
			ZGetGameInterface()->ShowMessage( ret );

			return true;
		}

		CCLabel* pCharNameLabel = (CCLabel*)pResource->FindWidget("CS_DeleteCharLabel");
		if (pCharNameLabel)
			pCharNameLabel->SetText( pLabel->GetText());

		CCEdit* pYesEdit = (CCEdit*)pResource->FindWidget("CS_DeleteCharNameEdit");
		if (pYesEdit)
			pYesEdit->SetText("");

		CCWidget* pWidget = pResource->FindWidget("CS_ConfirmDeleteChar");
		if (pWidget)
		{
			pWidget->Show(true, true);
			pWidget->SetFocus();
		}
	}
END_IMPLEMENT_LISTENER()

// 진짜 캐릭터 삭제
BEGIN_IMPLEMENT_LISTENER(ZGetConfirmDeleteCharacterButtonListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();

	char szName[256];
	sprintf( szName, "CharSel_Name%d", ZCharacterSelectView::GetSelectedCharacter() );
	CCLabel* pLabel = (CCLabel*)pResource->FindWidget( szName);

	if( ZCharacterSelectView::GetNumOfCharacter())
	{
		CCEdit* pYesEdit = (CCEdit*)pResource->FindWidget( "CS_DeleteCharNameEdit");
		if (pYesEdit)
		{
			if ( (!stricmp( pYesEdit->GetText(), ZMsg(MSG_MENUITEM_YES))) && (ZCharacterSelectView::GetSelectedCharacter() >= 0) )
			{
				// 클랜에 가입되어 있으면 캐릭터를 삭제할 수 없다.
				if ((ZCharacterSelectView::m_CharInfo[ZCharacterSelectView::GetSelectedCharacter()].m_bLoaded) && 
					( ZCharacterSelectView::m_CharInfo[ZCharacterSelectView::GetSelectedCharacter()].m_CharInfo.szClanName[0] == 0))
				{
					ZPostDeleteMyChar(ZGetGameClient()->GetPlayerUID(), ZCharacterSelectView::GetSelectedCharacter(), (char*)pLabel->GetText());

					//ZCharacterSelectView::SetSelectedCharacter(-1); 
				}
				else
					ZGetGameInterface()->ShowMessage( MSG_CLAN_PLEASE_LEAVE_FROM_CHAR_DELETE );
				}
			else
				ZGetGameInterface()->ShowMessage( MSG_CHARDELETE_ERROR );
		}

		CCWidget* pWidget = pResource->FindWidget( "CS_ConfirmDeleteChar");
		if ( pWidget)
			pWidget->Show( false);
	}
END_IMPLEMENT_LISTENER()

// 캐릭 삭제 확인창 닫기
BEGIN_IMPLEMENT_LISTENER(ZGetCloseConfirmDeleteCharButtonListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("CS_ConfirmDeleteChar");
	if (pWidget)
	{
		pWidget->Show(false);
	}
END_IMPLEMENT_LISTENER()



BEGIN_IMPLEMENT_LISTENER(ZGetCreateCharacterButtonListener, CCBTN_CLK_MSG)
	int nEmptySlotIndex=-1;
	for(int i=0;i<4;i++)
	{
		if(ZGetGameInterface()->GetCharacterSelectView()->IsEmpty(i))
		{
			nEmptySlotIndex=i;
			break;
		}
	}

	if(nEmptySlotIndex>=0)
	{
		ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
		CCEdit* pEdit = (CCEdit*)pResource->FindWidget("CC_CharName");
		CCComboBox* pSexCB, *pHairCB, *pFaceCB, *pCostumeCB;
		pSexCB = (CCComboBox*)pResource->FindWidget("CC_Sex");
		pHairCB = (CCComboBox*)pResource->FindWidget("CC_Hair");
		pFaceCB = (CCComboBox*)pResource->FindWidget("CC_Face");
		pCostumeCB = (CCComboBox*)pResource->FindWidget("CC_Costume");


		// 생성 조건인지 확인한다.
		if ( (pSexCB == NULL) || (pHairCB == NULL) || (pFaceCB == NULL) || (pCostumeCB == NULL) && (pEdit == NULL))
			return true;


		// 캐릭터 이름의 길이를 구한다.
		int nNameLen = (int)strlen( pEdit->GetText());

		if ( nNameLen <= 0)						// 이름을 이력하지 않았다.
		{
			ZGetGameInterface()->ShowErrorMessage( MERR_PLZ_INPUT_CHARNAME);
			return true;
		}
		else if ( nNameLen < MIN_CHARNAME)		// 이름이 너무 짧다.
		{
			ZGetGameInterface()->ShowErrorMessage( MERR_TOO_SHORT_NAME);
			return true;
		}
		else if ( nNameLen > MAX_CHARNAME)		// 이름이 제한 글자수를 넘었다.
		{
			ZGetGameInterface()->ShowErrorMessage( MERR_TOO_LONG_NAME);
			return true;
		}


		// 유효한 이름인지 검사함.
		bool bIsAbuse = MGetChattingFilter()->IsValidStr( pEdit->GetText(), 2, true);

		// 유효하지 않다면...
		if ( !bIsAbuse)
		{
			// 메시지를 출력하고 끝.
			char szMsg[ 256];
			ZTransMsg( szMsg, MSG_WRONG_WORD_NAME, 1, MGetChattingFilter()->GetLastFilteredStr());
			ZGetGameInterface()->ShowMessage( szMsg, NULL, MSG_WRONG_WORD_NAME);

			return true;
		}


		// 캐릭터 등록을 신청한다.
		ZPostCreateMyChar( ZGetGameClient()->GetPlayerUID(), nEmptySlotIndex, (char*)pEdit->GetText(), pSexCB->GetSelIndex(),
		                   pHairCB->GetSelIndex(), pFaceCB->GetSelIndex(), pCostumeCB->GetSelIndex());

	}
END_IMPLEMENT_LISTENER()


void SetCharacterInfoGroup(int n)
{
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCButton* pBtn = (CCButton*)pResource->FindWidget("ShowChar_infoGroup");
	if(pBtn)pBtn->SetCheck(n==0);
	pBtn = (CCButton*)pResource->FindWidget("ShowEquip_InfoGroup");
	if(pBtn)pBtn->SetCheck(n==1);

	CCWidget *pFrame=(CCFrame*)pResource->FindWidget("Char_infoGroup");
	if(pFrame) pFrame->Show(n==0);
	pFrame=(CCFrame*)pResource->FindWidget("Equip_InfoGroup");
	if(pFrame) pFrame->Show(n==1);
}

BEGIN_IMPLEMENT_LISTENER(ZGetShowCharInfoGroupListener, CCBTN_CLK_MSG)
SetCharacterInfoGroup(0);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetShowEquipInfoGroupListener, CCBTN_CLK_MSG)
SetCharacterInfoGroup(1);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetCancelCreateCharacterButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->SetState(GUNZ_CHARSELECTION);
END_IMPLEMENT_LISTENER()



BEGIN_IMPLEMENT_LISTENER(ZChangeCreateCharInfoListener, MCMBBOX_CHANGED)
	if (ZGetGameInterface()->GetCharacterSelectView() != NULL)
	{
		ZGetGameInterface()->GetCharacterSelectView()->OnChangedCharCostume();
	}
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetStageForcedEntryToGameListener, CCBTN_CLK_MSG)
	if(ZGetGameClient()->GetMatchStageSetting()->GetMapName()[0]!=0)
	{
		ZApplication::GetStageInterface()->ChangeStageEnableReady( true);

		ZPostRequestForcedEntry(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID());
	}
END_IMPLEMENT_LISTENER()

/*
void ShowPlayerListGroup(int i)
{
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("ChannelPlayerList_all");
	if(pWidget!=NULL) ((CCButton*)pWidget)->SetCheck(i==0?true:false);
	pWidget = pResource->FindWidget("ChannelPlayerList_friend");
	if(pWidget!=NULL) ((CCButton*)pWidget)->SetCheck(i==1?true:false);
	pWidget = pResource->FindWidget("ChannelPlayerList_clan");
	if(pWidget!=NULL) ((CCButton*)pWidget)->SetCheck(i==2?true:false);
}

BEGIN_IMPLEMENT_LISTENER(ZGetChannelPlayerOptionGroupAll, CCBTN_CLK_MSG)
ShowPlayerListGroup(0);
END_IMPLEMENT_LISTENER()
BEGIN_IMPLEMENT_LISTENER(ZGetChannelPlayerOptionGroupFriend, CCBTN_CLK_MSG)
ShowPlayerListGroup(1);
END_IMPLEMENT_LISTENER()
BEGIN_IMPLEMENT_LISTENER(ZGetChannelPlayerOptionGroupClan, CCBTN_CLK_MSG)
ShowPlayerListGroup(2);
END_IMPLEMENT_LISTENER()
*/

BEGIN_IMPLEMENT_LISTENER(ZGetAllEquipmentListCallerButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->SelectShopTab(0);
END_IMPLEMENT_LISTENER()
BEGIN_IMPLEMENT_LISTENER(ZGetMyAllEquipmentListCallerButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->SelectShopTab(1);
END_IMPLEMENT_LISTENER()
BEGIN_IMPLEMENT_LISTENER(ZGetCashEquipmentListCallerButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->SelectShopTab(2);
END_IMPLEMENT_LISTENER()


BEGIN_IMPLEMENT_LISTENER(ZGetEquipmentCharacterTabButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->SelectEquipmentTab(0);
END_IMPLEMENT_LISTENER()

// 중앙은행
BEGIN_IMPLEMENT_LISTENER(ZGetEquipmentAccountTabButtonListener, CCBTN_CLK_MSG)
	// 유져가 버튼 막 누르는 것을 방지하도록 한다.
//	static unsigned long int st_nLastTime = 0;
//	unsigned long int nNowTime = timeGetTime();
#define MIN_ACCOUNT_ITEM_REQUEST_TIME		2000

	ZGetGameInterface()->GetShopEquipInterface()->SelectEquipmentTab(1);

//	if ((nNowTime - st_nLastTime) > MIN_ACCOUNT_ITEM_REQUEST_TIME)
//	{
		ZPostRequestAccountItemList(ZGetGameClient()->GetPlayerUID());
//		st_nLastTime = nNowTime;
//	}
END_IMPLEMENT_LISTENER()

// 중앙은행에서 레벨 무시하고 가져오기 확인 (todok 수량 입력 때문에 구현하기가 빡세졌다.. 이게 꼭필요할까? 그냥 삭제하고 싶다)
class ZLevelConfirmListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, MMSGBOX_YES)==true){
			//int nCount = ZGetGameInterface()->m_nBringSpendableItemCount;
			//ZGetGameInterface()->GetShopEquipInterface()->BringAccountItem(nCount);
		} 
		pWidget->Show(false);
		return false;
	}
} g_LevelConfirmListener;

CCListener* ZGetLevelConfirmListenter()
{
	return &g_LevelConfirmListener;
}

/*
///////////////////////////////////////////////////////////////////////////
// 플레이어/친구 리스트 전환
// 누르면 채널 -> 친구 -> 클랜 -> 채널 로 순환한다

BEGIN_IMPLEMENT_LISTENER(ZGetListenerLobbyPlayerListTabChannel, CCBTN_CLK_MSG)
{
	ZPlayerListBox* pList = (ZPlayerListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("LobbyChannelPlayerList");
	if (pList) {
		pList->SetMode(ZPlayerListBox::PLAYERLISTMODE_CHANNEL_FRIEND);
		ZGetGameInterface()->SetupPlayerListButton(1);
	}
}
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetListenerLobbyPlayerListTabFriend, CCBTN_CLK_MSG)
{
	ZPlayerListBox* pList = (ZPlayerListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("LobbyChannelPlayerList");
	if (pList) {
		pList->SetMode(ZPlayerListBox::PLAYERLISTMODE_CHANNEL_CLAN);
		ZGetGameInterface()->SetupPlayerListButton(2);
	}    
}
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetListenerLobbyPlayerListTabClan, CCBTN_CLK_MSG)
{
	ZPlayerListBox* pList = (ZPlayerListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("LobbyChannelPlayerList");
	if (pList) {
		pList->SetMode(ZPlayerListBox::PLAYERLISTMODE_CHANNEL);
		ZGetGameInterface()->SetupPlayerListButton(0);
	}    
}
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetListenerGamePlayerListTabGame, CCBTN_CLK_MSG)
{
	ZPlayerListBox* pList = (ZPlayerListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("StagePlayerList_");
	CCButton* pBtnGame = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("StagePlayerListTabGame");
	CCButton* pBtnFriend = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("StagePlayerListTabFriend");
	if (pList && pBtnGame && pBtnFriend) {
//		pBtnGame->Show(false);
//		pBtnFriend->Show(true);
		pList->SetMode(ZPlayerListBox::PLAYERLISTMODE_STAGE_FRIEND);
	}    
}
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetListenerGamePlayerListTabFriend, CCBTN_CLK_MSG)
{
	ZPlayerListBox* pList = (ZPlayerListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("StagePlayerList_");
	CCButton* pBtnGame = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("StagePlayerListTabGame");
	CCButton* pBtnFriend = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("StagePlayerListTabFriend");
	if (pList && pBtnGame && pBtnFriend) {
//		pBtnGame->Show(true);
//		pBtnFriend->Show(false);
		pList->SetMode(ZPlayerListBox::PLAYERLISTMODE_STAGE);
	}    
}
END_IMPLEMENT_LISTENER()
*/
///////////////////////////////////////////////////////////////////////////
// 특정갯수단위로 플레이어 리스트 갱신
BEGIN_IMPLEMENT_LISTENER(ZGetPlayerListPrevListener, CCBTN_CLK_MSG)
{
	ZPlayerListBox* pWidget = (ZPlayerListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("LobbyChannelPlayerList");
	if (pWidget->GetMode() == ZPlayerListBox::PLAYERLISTMODE_CHANNEL_FRIEND) {
		int iStart = pWidget->GetStartItem();
		if (iStart > 0) 
			pWidget->SetStartItem(iStart-1);
		return true;
	}

	int nPage = pWidget->m_nPage;

	nPage--;

	if(nPage < 0) {
		return false;	
	}

	ZPostRequestChannelPlayerList(ZGetGameClient()->GetPlayerUID(),ZGetGameClient()->GetChannelUID(),nPage);

}
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetPlayerListNextListener, CCBTN_CLK_MSG)
{
	ZPlayerListBox* pWidget = (ZPlayerListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("LobbyChannelPlayerList");
	if (pWidget->GetMode() == ZPlayerListBox::PLAYERLISTMODE_CHANNEL_FRIEND) {
		int iStart = pWidget->GetStartItem();
		pWidget->SetStartItem(iStart+1);
		return true;
	}

	int nMaxPage = 0;
	if(pWidget->m_nTotalPlayerCount)
		nMaxPage = pWidget->m_nTotalPlayerCount/8;

	int nPage = pWidget->m_nPage;

	nPage++;

	if(nPage > nMaxPage) {
		return false;
	}

	ZPostRequestChannelPlayerList(ZGetGameClient()->GetPlayerUID(),ZGetGameClient()->GetChannelUID(),nPage);
	
}

END_IMPLEMENT_LISTENER()


BEGIN_IMPLEMENT_LISTENER(ZGetStagePlayerListPrevListener, CCBTN_CLK_MSG)
{
	ZPlayerListBox* pWidget = (ZPlayerListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("StagePlayerList_");
	if (pWidget->GetMode() == ZPlayerListBox::PLAYERLISTMODE_STAGE_FRIEND) {
		int iStart = pWidget->GetStartItem();
		if (iStart > 0) 
			pWidget->SetStartItem(iStart-1);
		return true;
	}

	pWidget->SetStartItem( pWidget->GetStartItem()-1 );
}
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetStagePlayerListNextListener, CCBTN_CLK_MSG)
{
	ZPlayerListBox* pWidget = (ZPlayerListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("StagePlayerList_");
	if (pWidget->GetMode() == ZPlayerListBox::PLAYERLISTMODE_STAGE_FRIEND) {
		int iStart = pWidget->GetStartItem();
		pWidget->SetStartItem(iStart+1);
		return true;
	}

	pWidget->SetStartItem( pWidget->GetStartItem()+1 );
}

END_IMPLEMENT_LISTENER()

////////////////////////////////////////////////////////////////////////////

BEGIN_IMPLEMENT_LISTENER(ZGetRoomListListener, MLIST_VALUE_CHANGED)
	ZRoomListBox* pWidget = (ZRoomListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Lobby_StageList");
	pWidget->SetPage();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetLobbyPrevRoomListButtonListener, CCBTN_CLK_MSG)
	ZGetGameClient()->RequestPrevStageList();	
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetLobbyNextRoomListPrevButtonListener, CCBTN_CLK_MSG)
	ZGetGameClient()->RequestNextStageList();
END_IMPLEMENT_LISTENER()

/*
BEGIN_IMPLEMENT_LISTENER(ZGetLobbyNextRoomNo1ButtonListener, CCBTN_CLK_MSG)
	ZGetGameClient()->RequestStageList(1);
	ZGetGameInterface()->SetRoomNoLight(1);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetLobbyNextRoomNo2ButtonListener, CCBTN_CLK_MSG)
	ZGetGameClient()->RequestStageList(2);
	ZGetGameInterface()->SetRoomNoLight(2);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetLobbyNextRoomNo3ButtonListener, CCBTN_CLK_MSG)
	ZGetGameClient()->RequestStageList(3);
	ZGetGameInterface()->SetRoomNoLight(3);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetLobbyNextRoomNo4ButtonListener, CCBTN_CLK_MSG)
	ZGetGameClient()->RequestStageList(4);
	ZGetGameInterface()->SetRoomNoLight(4);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetLobbyNextRoomNo5ButtonListener, CCBTN_CLK_MSG)
	ZGetGameClient()->RequestStageList(5);
	ZGetGameInterface()->SetRoomNoLight(5);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetLobbyNextRoomNo6ButtonListener, CCBTN_CLK_MSG)
	ZGetGameClient()->RequestStageList(6);
	ZGetGameInterface()->SetRoomNoLight(6);
END_IMPLEMENT_LISTENER()
*/
BEGIN_IMPLEMENT_LISTENER(ZGetLobbyNextRoomNoButtonListener, CCBTN_CLK_MSG)
	CCButton *pButton = (CCButton*)pWidget;
	int nIndexInGroup = pButton->GetIndexInGroup();
	_ASSERT(0<=nIndexInGroup && nIndexInGroup<6);
	nIndexInGroup++;	// 0~5 -> 1~6
	ZGetGameClient()->RequestStageList(nIndexInGroup);
	ZGetGameInterface()->SetRoomNoLight(nIndexInGroup);
END_IMPLEMENT_LISTENER()


BEGIN_IMPLEMENT_LISTENER( ZGetEquipmentShopCallerButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->ShowEquipmentDialog(false);
	ZGetGameInterface()->ShowShopDialog(true);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER( ZGetShopEquipmentCallerButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->ShowShopDialog(false);
	ZGetGameInterface()->ShowEquipmentDialog(true);
END_IMPLEMENT_LISTENER()

class ZMapListListener : public CCListener{									
	public:																	
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage)
	{	
		if(CCWidget::IsMsg(szMessage, MLB_ITEM_SEL)==true)
		{
			MListBox* pList = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("MapList");
			if(pList != NULL)
			{
				CCComboBox* pCombo = (CCComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("MapSelection");
				if(pCombo != NULL )
				{
					int si = pList->GetSelIndex();
					pCombo->SetSelIndex(si);
					PostMapname();
				}
			}

			return true;
		}
		if(CCWidget::IsMsg(szMessage, MLB_ITEM_DBLCLK)==true)
		{
			MListBox* pList = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("MapList");
			if(pList != NULL)
			{
				pList->Show(FALSE);
			}
			return true;
		}
		return false;
	}
} g_MapListListener;

CCListener* ZGetStageMapListSelectionListener()
{
	return &g_MapListListener;
}

BEGIN_IMPLEMENT_LISTENER(ZGetStageMapListCallerListener, CCBTN_CLK_MSG)
	MListBox* pList = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("MapList");
	pList->Show(TRUE);
END_IMPLEMENT_LISTENER();



void ZReport112FromListener()
{
/*
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCEdit* pReasonEdit = (CCEdit*)pResource->FindWidget("112_ConfirCCEdit");
	if (pReasonEdit)
	{
		if (strlen(pReasonEdit->GetText()) < 10)
		{
			char temp[256];
			ZTransMsg(temp, MSG_YOU_MUST_WRITE_MORE, 1, "10");
			ZGetGameInterface()->ShowMessage(temp, NULL, MSG_YOU_MUST_WRITE_MORE);
			return;
		}


		CCWidget* pWidget = pResource->FindWidget("112Confirm");
		if (pWidget)
		{
			pWidget->Show(false);
		}

//		if (ZGetGameInterface()->GetState() == GUNZ_GAME)
//		{
//			ZGetGameInterface()->SetCursorEnable(false);
//		}

		ZGetGameInterface()->GetChat()->Report112(pReasonEdit->GetText());
	}
*/
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();

	CCWidget* pWidget = pResource->FindWidget( "112Confirm");
	if ( !pWidget)
		return;

	CCComboBox* pCombo1 = (CCComboBox*)pResource->FindWidget( "112_ConfirmID");
	CCComboBox* pCombo2 = (CCComboBox*)pResource->FindWidget( "112_ConfirmReason");
	
	if ( !pCombo1 || !pCombo2)
		return;

	if ( ( pCombo2->GetSelIndex() < 0) || ( pCombo2->GetSelIndex() < 1))
		return;


	__time64_t long_time;
	_time64( &long_time);
	const struct tm* pLocalTime = _localtime64( &long_time);

	char szBuff[ 256];
	sprintf( szBuff, "%s\n%s\n%03d:%s\n%04d-%02d-%02d %02d:%02d:%02d\n",	ZGetMyInfo()->GetCharName(), pCombo1->GetSelItemString(),
																			100+pCombo2->GetSelIndex(), pCombo2->GetSelItemString(),
																			pLocalTime->tm_year+1900, pLocalTime->tm_mon+1, pLocalTime->tm_mday,
																			pLocalTime->tm_hour, pLocalTime->tm_min, pLocalTime->tm_sec);

	ZGetGameInterface()->GetChat()->Report112( szBuff);


	pWidget->Show( false);
}


BEGIN_IMPLEMENT_LISTENER(ZGet112ConfirCCEditListener, CCEdit_ENTER_VALUE)
	ZReport112FromListener();
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGet112ConfirmOKButtonListener, CCBTN_CLK_MSG)
	ZReport112FromListener();
END_IMPLEMENT_LISTENER();


BEGIN_IMPLEMENT_LISTENER(ZGet112ConfirmCancelButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->Show112Dialog(false);
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetClanSponsorAgreementConfirm_OKButtonListener, CCBTN_CLK_MSG)
	ZGetGameClient()->AnswerSponsorAgreement(true);

	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("ClanSponsorAgreementConfirm");
	if(pWidget!=NULL)
	{
		pWidget->Show(false);
	}
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetClanSponsorAgreementConfirm_CancelButtonListener, CCBTN_CLK_MSG)
	ZGetGameClient()->AnswerSponsorAgreement(false);

	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("ClanSponsorAgreementConfirm");
	if(pWidget!=NULL)
	{
		pWidget->Show(false);
	}
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetClanSponsorAgreementWait_CancelButtonListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("ClanSponsorAgreementWait");
	if(pWidget!=NULL)
	{
		pWidget->Show(false);
	}
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetClanJoinerAgreementConfirm_OKButtonListener, CCBTN_CLK_MSG)
	ZGetGameClient()->AnswerJoinerAgreement(true);

	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("ClanJoinerAgreementConfirm");
	if(pWidget!=NULL)
	{
		pWidget->Show(false);
	}
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetClanJoinerAgreementConfirm_CancelButtonListener, CCBTN_CLK_MSG)
	ZGetGameClient()->AnswerJoinerAgreement(false);

	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("ClanJoinerAgreementConfirm");
	if(pWidget!=NULL)
	{
		pWidget->Show(false);
	}
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetClanJoinerAgreementWait_CancelButtonListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("ClanJoinerAgreementWait");
	if(pWidget!=NULL)
	{
		pWidget->Show(false);
	}
END_IMPLEMENT_LISTENER();

// 클랜 생성 창
BEGIN_IMPLEMENT_LISTENER(ZGetLobbyPlayerListTabClanCreateButtonListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("ClanCreateDialog");
	if(pWidget!=NULL)
	{
		pWidget->Show(true,true);

		// 창단멤버로 선택가능한 사람 리스트를 요청한다
		unsigned long int nPlaceFilter = 0;
		SetBitSet(nPlaceFilter, MMP_LOBBY);

		ZPostRequestChannelAllPlayerList(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetChannelUID(),nPlaceFilter,
			MCP_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST_NONCLAN);
	}
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetClanCreateDialogOk, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("ClanCreateDialog");
	if(pWidget!=NULL)
	{
		pWidget->Show(false);

		ZPlayerSelectListBox *pPlayerList = (ZPlayerSelectListBox*)pResource->FindWidget("ClanSponsorSelect");
		if(pPlayerList)
		{
			char szSponsors[CLAN_SPONSORS_COUNT][MATCHOBJECT_NAME_LENGTH];
			char *ppSponsors[CLAN_SPONSORS_COUNT];
			int nCount = 0;
			for(int i=0;i<pPlayerList->GetCount();i++)
			{
				MListItem *pItem = pPlayerList->Get(i);
				if(pItem->m_bSelected) {
					if(nCount>=CLAN_SPONSORS_COUNT) break;
					strcpy(szSponsors[nCount],pItem->GetString());
					ppSponsors[nCount]=szSponsors[nCount];
					nCount ++;
				}
			}
			/*
#ifdef _DEBUG
			nCount = CLAN_SPONSORS_COUNT;
#endif
			*/

			// 정확한 수가 맞았으면..
			if ( nCount==CLAN_SPONSORS_COUNT)
			{
				CCEdit *pEditClanName= (CCEdit*)pResource->FindWidget("ClanCreate_ClanName");
				if ( !pEditClanName)
					return true;


				int nNameLen = (int)strlen( pEditClanName->GetText());

				if ( nNameLen <= 0)						// 이름을 이력하지 않았다.
				{
					ZGetGameInterface()->ShowErrorMessage( MERR_PLZ_INPUT_CHARNAME);
					return true;
				}
				else if ( nNameLen < MIN_CLANNAME)		// 이름이 너무 짧다.
				{
					ZGetGameInterface()->ShowErrorMessage( MERR_TOO_SHORT_NAME);
					return true;
				}
				else if ( nNameLen > MAX_CLANNAME)		// 이름이 제한 글자수를 넘었다.
				{
					ZGetGameInterface()->ShowErrorMessage( MERR_TOO_LONG_NAME);
					return true;
				}


				// 유효한지 검사한다.
				if( !MGetChattingFilter()->IsValidStr( pEditClanName->GetText(), 2, true) )
				{
					char szMsg[ 256 ];
					ZTransMsg( szMsg, MSG_WRONG_WORD_NAME, 1, MGetChattingFilter()->GetLastFilteredStr());
					ZGetGameInterface()->ShowMessage( szMsg, NULL, MSG_WRONG_WORD_NAME );
				}
				else if(pEditClanName)
				{
					char szClanName[CLAN_NAME_LENGTH]={0,};
					strcpy(szClanName,pEditClanName->GetText());
					ZGetGameClient()->RequestCreateClan(szClanName, ppSponsors);
				}
			}
			// 숫자가 모자르다
			else
			{
				char szMsgBox[256];
				char szArg[20];
				_itoa(CLAN_SPONSORS_COUNT, szArg, 10);

				ZTransMsg(szMsgBox, MSG_CLAN_CREATE_NEED_SOME_SPONSOR, 1, szArg);
				ZGetGameInterface()->ShowMessage(szMsgBox, NULL, MSG_CLAN_CREATE_NEED_SOME_SPONSOR);
			}
		}
	}
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetClanCreateDialogClose, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("ClanCreateDialog");
	if(pWidget!=NULL)
	{
		pWidget->Show(false);
	}
END_IMPLEMENT_LISTENER();

// 언어 변환 확인
class ZLanguageChangeConfirmListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		pWidget->Show(false);
		if(CCWidget::IsMsg(szMessage, MMSGBOX_YES)==true){
			ZGetGameInterface()->ReserveResetApp(true);
		}
		return false;
	}
} g_LanguageChangeConfirmListener;

CCListener* ZGetLanguageChangeConfirmListenter()
{
	return &g_LanguageChangeConfirmListener;
}

// 클랜 폐쇄 확인
class ZClanCloseConfirmListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, MMSGBOX_YES)==true){

			char szClanName[256];
			strcpy(szClanName, ZGetMyInfo()->GetClanName());
			// 서버에 폐쇄 요청
			ZPostRequestCloseClan(ZGetGameClient()->GetPlayerUID(), szClanName);
		} else {
		}
		pWidget->Show(false);
		return false;
	}
} g_ClanCloseConfirmListener;

CCListener* ZGetClanCloseConfirmListenter()
{
	return &g_ClanCloseConfirmListener;
}

// 클랜 탈퇴 확인
class ZClanLeaveConfirmListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, MMSGBOX_YES)==true){
			// 서버에 탈퇴 요청
			ZPostRequestLeaveClan(ZGetMyUID());
		} else {
		}
		pWidget->Show(false);
		return false;
	}
} g_ClanLeaveConfirmListener;

CCListener* ZGetClanLeaveConfirmListenter()
{
	return &g_ClanLeaveConfirmListener;
}

BEGIN_IMPLEMENT_LISTENER(ZGetArrangedTeamGameListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("ArrangedTeamGameDialog");
	if(pWidget!=NULL)
	{
		pWidget->Show(true,true);

		// 팀플레이 선수로 선택가능한 사람 리스트를 요청한다
		unsigned long int nPlaceFilter = 0;
		SetBitSet(nPlaceFilter, MMP_LOBBY);

		ZPostRequestChannelAllPlayerList(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetChannelUID(),nPlaceFilter,
			MCP_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST_MYCLAN);
	}
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetArrangedTeamDialogOkListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("ArrangedTeamGameDialog");
	if(pWidget!=NULL)
		pWidget->Show(false);

//	pWidget = pResource->FindWidget("LobbyFindClanTeam");
//	if(pWidget!=NULL)
//		pWidget->Show(true);

	// 게임 초대 메시지를 발송한다

	ZPlayerSelectListBox *pPlayerList = (ZPlayerSelectListBox*)pResource->FindWidget("ArrangedTeamSelect");
	if(pPlayerList)
	{
		const int nMaxInviteCount = max(MAX_LADDER_TEAM_MEMBER,MAX_CLANBATTLE_TEAM_MEMBER) - 1;

		char szNames[nMaxInviteCount][MATCHOBJECT_NAME_LENGTH];
		char *ppNames[nMaxInviteCount];
		int nCount = 0;
		for(int i=0;i<pPlayerList->GetCount();i++)
		{
			MListItem *pItem = pPlayerList->Get(i);
			if(pItem->m_bSelected) {
				if(nCount>=nMaxInviteCount) {
					nCount++;
					break;
				}
				strcpy(szNames[nCount],pItem->GetString());
				ppNames[nCount]=szNames[nCount];
				nCount ++;
			}
		}

		switch (ZGetGameClient()->GetServerMode())
		{
		case CSM_LADDER:
			{
				// 적절한 수의 팀이면
				if(0<nCount && nCount<=nMaxInviteCount) {
					ZGetGameClient()->RequestProposal(MPROPOSAL_LADDER_INVITE, ppNames, nCount);
				}else
				{
//					ZChatOutput(sColor(ZCOLOR_CHAT_SYSTEM), MGetErrorString(MSG_LADDER_INVALID_COUNT));
					ZChatOutput(sColor(ZCOLOR_CHAT_SYSTEM), 
						ZErrStr(MSG_LADDER_INVALID_COUNT) );
				}
			}
			break;
		case CSM_CLAN:
			{
				bool bRightMember = false;
				for (int i = 0; i < CCLADDERTYPE_MAX; i++)
				{
					if ((g_nNeedLadderMemberCount[i]-1) == nCount)
					{
						bRightMember = true;
						break;
					}
				}

				// 적절한 수의 팀이면

				if((0<nCount) && (bRightMember))
				{
					ZGetGameClient()->RequestProposal(MPROPOSAL_CLAN_INVITE, ppNames, nCount);
				} 
#ifdef _DEBUG	// 1명테스트
				else if (nCount == 0)
				{
					// 자기 자신만일때
					char szMember[1][MATCHOBJECT_NAME_LENGTH];
					char* ppMember[1];

					ppMember[0] = szMember[0];
					strcpy(szMember[0], ZGetMyInfo()->GetCharName());

					// Balance 옵션
					int nBalancedMatching = 0;
					ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
					CCButton* pButton = (CCButton*)pResource->FindWidget("BalancedMatchingCheckBox");
					if ((pButton) && (pButton->GetCheck()))
					{
						nBalancedMatching = 1;
					}

					ZPostLadderRequestChallenge(ppMember, 1, nBalancedMatching);
				}
#endif
				else
				{
					ZChatOutput(sColor(ZCOLOR_CHAT_SYSTEM), 
						ZMsg(MSG_LADDER_INVALID_COUNT) );
				}
			}
			break;
		}
	}
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetArrangedTeamDialogCloseListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("ArrangedTeamGameDialog");
	if(pWidget!=NULL)
		pWidget->Show(false);

	pWidget = pResource->FindWidget("LobbyFindClanTeam");
	if(pWidget!=NULL)
		pWidget->Show(false);
END_IMPLEMENT_LISTENER();


BEGIN_IMPLEMENT_LISTENER(ZGetProposalAgreementWait_CancelButtonListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("ProposalAgreementWait");
	if(pWidget!=NULL)
	{
		pWidget->Show(false);
	}
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetProposalAgreementConfirm_OKButtonListener, CCBTN_CLK_MSG)
	ZGetGameClient()->ReplyAgreement(true);

	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("ProposalAgreementConfirm");
	if(pWidget!=NULL)
	{
		pWidget->Show(false);
	}
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetProposalAgreementConfirm_CancelButtonListener, CCBTN_CLK_MSG)
	ZGetGameClient()->ReplyAgreement(false);

	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("ProposalAgreementConfirm");
	if(pWidget!=NULL)
	{
		pWidget->Show(false);
	}
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetArrangedTeamGame_CancelListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("LobbyFindClanTeam");
	if(pWidget!=NULL)
		pWidget->Show(false);

	ZPostLadderCancel();
END_IMPLEMENT_LISTENER();



BEGIN_IMPLEMENT_LISTENER(ZGetPrivateChannelEnterListener, CCBTN_CLK_MSG)
	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
	CCEdit* pEdit = (CCEdit*)pResource->FindWidget("PrivateChannelInput");
	if(pEdit!=NULL)
	{
		int nNameLen = (int)strlen( pEdit->GetText());

		if ( nNameLen <= 0)						// 이름을 이력하지 않았다.
		{
			ZGetGameInterface()->ShowErrorMessage( MERR_PLZ_INPUT_CHARNAME);
			return true;
		}
		else if ( nNameLen < MIN_CLANNAME)		// 이름이 너무 짧다.
		{
			ZGetGameInterface()->ShowErrorMessage( MERR_TOO_SHORT_NAME);
			return true;
		}
		else if ( nNameLen > MAX_CLANNAME)		// 이름이 제한 글자수를 넘었다.
		{
			ZGetGameInterface()->ShowErrorMessage( MERR_TOO_LONG_NAME);
			return true;
		}


		if( !MGetChattingFilter()->IsValidStr( pEdit->GetText(), 1))
		{
			char szMsg[ 256 ];
			ZTransMsg( szMsg, MSG_WRONG_WORD_NAME, 1, MGetChattingFilter()->GetLastFilteredStr());
			ZGetGameInterface()->ShowMessage( szMsg, NULL, MSG_WRONG_WORD_NAME );
		}
		else
		{
			ZPostChannelRequestJoinFromChannelName(ZGetMyUID(),CCCHANNEL_TYPE_USER,pEdit->GetText());

			CCWidget* pFindWidget = pResource->FindWidget("ChannelListFrame");
			if(pFindWidget!=NULL) pFindWidget->Show(false);
		}
	}
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetChannelList, CCBTN_CLK_MSG)
	CCButton *pButton = (CCButton*)pWidget;
	int nIndexInGroup = pButton->GetIndexInGroup();
	_ASSERT(0<=nIndexInGroup && nIndexInGroup<4);

	ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();

	CCCHANNEL_TYPE nChannelType = CCCHANNEL_TYPE_PRESET;
	// 0 = 일반 , 1 = 사설 , 2 = 클랜 채널 리스트 , 3 = 듀얼 토너먼트 채널 리스트를 요청한다
	switch(nIndexInGroup) {
	case 0 : nChannelType = CCCHANNEL_TYPE_PRESET; break;
	case 1 : nChannelType = CCCHANNEL_TYPE_USER; break;
	case 2 : nChannelType = CCCHANNEL_TYPE_CLAN; break;
	case 3 : nChannelType = CCCHANNEL_TYPE_DUELTOURNAMENT; break;

	default : _ASSERT(FALSE);
	}

	ZGetGameInterface()->InitChannelFrame(nChannelType);
	ZGetGameClient()->StartChannelList(nChannelType);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetMyClanChannel, CCBTN_CLK_MSG)
	if(ZGetMyInfo()->IsClanJoined())
	{
		ZPostChannelRequestJoinFromChannelName(ZGetMyUID(),CCCHANNEL_TYPE_CLAN,ZGetMyInfo()->GetClanName());

#ifdef LOCALE_NHNUSA
		GetNHNUSAReport().ReportJoinChannel();
#endif

		ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
		CCWidget* pFindWidget = pResource->FindWidget("ChannelListFrame");
		if(pFindWidget!=NULL) pFindWidget->Show(false);
	}
	else {
		// 클랜에 가입되어있지 않습니다 메시지 안내
		ZGetGameInterface()->ShowMessage( MSG_LOBBY_NO_CLAN );
	}
END_IMPLEMENT_LISTENER();


// Shop list frame open/close
BEGIN_IMPLEMENT_LISTENER(ZShopItemEquipmentTabOpen, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->OnArmorWeaponTabButtonClicked(0);
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZShopWeaponEquipmentTabOpen, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->OnArmorWeaponTabButtonClicked(1);
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZShopListFrameClose, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->SelectEquipmentFrameList(NULL, false);
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZShopListFrameOpen, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->SelectEquipmentFrameList(NULL, true);
END_IMPLEMENT_LISTENER();


// Equipment list frame open/close
BEGIN_IMPLEMENT_LISTENER(ZEquipItemEquipmentTabOpen, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->OnArmorWeaponTabButtonClicked(0);
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZEquipWeaponEquipmentTabOpen, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->OnArmorWeaponTabButtonClicked(1);
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZEquipListFrameClose, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->SelectEquipmentFrameList(NULL, false);
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZEquipListFrameOpen, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetShopEquipInterface()->SelectEquipmentFrameList(NULL, true);
END_IMPLEMENT_LISTENER();

// Equipment character view rotate
BEGIN_IMPLEMENT_LISTENER(ZEquipmetRotateBtn, CCBTN_CLK_MSG)
	ZCharacterView* pCharView = (ZCharacterView*)ZGetGameInterface()->GetIDLResource()->FindWidget( "EquipmentInformation");
	if ( pCharView)
	{
		pCharView->EnableAutoRotate( !pCharView->IsAutoRotate());
		
		MBCCButton* pButton = (MBCCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Equipment_CharacterRotate");
		if ( pButton)
		{
			if ( pCharView->IsAutoRotate())
			{
				pButton->SetUpBitmap(   CCBitmapManager::Get( "btn_rotate.tga"));
				pButton->SetDownBitmap( CCBitmapManager::Get( "btn_rotate.tga"));
				pButton->SetOverBitmap( CCBitmapManager::Get( "btn_rotate.tga"));
			}
			else
			{
				pButton->SetUpBitmap(   CCBitmapManager::Get( "btn_stop.tga"));
				pButton->SetDownBitmap( CCBitmapManager::Get( "btn_stop.tga"));
				pButton->SetOverBitmap( CCBitmapManager::Get( "btn_stop.tga"));
			}
		}
	}
END_IMPLEMENT_LISTENER();


// Replay
BEGIN_IMPLEMENT_LISTENER(ZReplayOk, CCBTN_CLK_MSG)
	ZGetGameInterface()->OnReplay();
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetReplayCallerButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->ShowReplayDialog( true);
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetReplayViewButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->ViewReplay();
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetReplayExitButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->ShowReplayDialog( false);
END_IMPLEMENT_LISTENER();

CCListener* ZGetReplayFileListBoxListener( void)
{
	class ListenerClass : public CCListener
	{
		public:
		virtual bool OnCommand( CCWidget* pWidget, const char* szMessage)
		{
			// Item select
			if ( CCWidget::IsMsg( szMessage, MLB_ITEM_SEL) == true)
			{
				CCWidget* pFindWidget = ZGetGameInterface()->GetIDLResource()->FindWidget( "Replay_View");
				if ( pFindWidget != NULL)
					pFindWidget->Enable( true);

                return true;
			}
			// Item double click
			else if ( CCWidget::IsMsg( szMessage, MLB_ITEM_DBLCLK) == true)
			{
				ZGetGameInterface()->ViewReplay();

                return true;
			}

			return false;
		}
	};
	static ListenerClass	Listener;
	return &Listener;
}


BEGIN_IMPLEMENT_LISTENER(ZGetLeaveClanOKListener, CCBTN_CLK_MSG)
	CCWidget* pWidget = (CCWidget*)ZGetGameInterface()->GetIDLResource()->FindWidget( "ConfirmLeaveClan");
	if ( pWidget)
		pWidget->Show( false);

	ZPostRequestLeaveClan(ZGetMyUID());

	ZPlayerListBox *pPlayerListBox = (ZPlayerListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "LobbyChannelPlayerList" );
	if(pPlayerListBox)
		pPlayerListBox->SetMode(ZPlayerListBox::PLAYERLISTMODE_CHANNEL_CLAN);
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER(ZGetLeaveClanCancelListener, CCBTN_CLK_MSG)
	CCWidget* pWidget = (CCWidget*)ZGetGameInterface()->GetIDLResource()->FindWidget( "ConfirmLeaveClan");
	if ( pWidget)
		pWidget->Show( false);
END_IMPLEMENT_LISTENER();

int g_lastPressedDuelTournamentGameBtn = 8;

// 듀얼 토너먼트 참가 버튼
BEGIN_IMPLEMENT_LISTENER(ZGetDuelTournamentGameButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->OnDuelTournamentGameUI(true);
	ZPostDuelTournamentRequestJoinGame( ZGetMyUID(), CCDUELTOURNAMENTTYPE_QUATERFINAL);
	g_lastPressedDuelTournamentGameBtn = 8;
END_IMPLEMENT_LISTENER();

// 듀얼 토너먼트 참가 테스트용 2인용 버튼
BEGIN_IMPLEMENT_LISTENER(ZGetDuelTournamentGame2TestButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->OnDuelTournamentGameUI(true);
	ZPostDuelTournamentRequestJoinGame( ZGetMyUID(), CCDUELTOURNAMENTTYPE_FINAL);
	g_lastPressedDuelTournamentGameBtn = 2;
END_IMPLEMENT_LISTENER();

// 듀얼 토너먼트 참가 테스트용 4인용 버튼
BEGIN_IMPLEMENT_LISTENER(ZGetDuelTournamentGame4TestButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->OnDuelTournamentGameUI(true);
	ZPostDuelTournamentRequestJoinGame( ZGetMyUID(), CCDUELTOURNAMENTTYPE_SEMIFINAL);
	g_lastPressedDuelTournamentGameBtn = 4;
END_IMPLEMENT_LISTENER();

// 듀얼 토너먼트 참가 취소 버튼
BEGIN_IMPLEMENT_LISTENER(ZGetDuelTournamentWaitCancelButtonListener, CCBTN_CLK_MSG)
	ZGetGameInterface()->OnDuelTournamentGameUI(false);

#ifdef _PUBLISH
	ZPostDuelTournamentRequestCancelGame( ZGetMyUID(), CCDUELTOURNAMENTTYPE_QUATERFINAL);
#else
	switch (g_lastPressedDuelTournamentGameBtn) {
		case 2 : ZPostDuelTournamentRequestCancelGame( ZGetMyUID(), CCDUELTOURNAMENTTYPE_FINAL); break;
		case 4 : ZPostDuelTournamentRequestCancelGame( ZGetMyUID(), CCDUELTOURNAMENTTYPE_SEMIFINAL); break;
		default : ZPostDuelTournamentRequestCancelGame( ZGetMyUID(), CCDUELTOURNAMENTTYPE_QUATERFINAL); break;
	}
#endif
		
END_IMPLEMENT_LISTENER();

// 스테이지 희생 아이템
BEGIN_IMPLEMENT_LISTENER( ZStageSacrificeItem0, CCBTN_CLK_MSG)
	ZApplication::GetStageInterface()->OnSacrificeItem0();
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER( ZStageSacrificeItem1, CCBTN_CLK_MSG)
	ZApplication::GetStageInterface()->OnSacrificeItem1();
END_IMPLEMENT_LISTENER();

// 스테이지 희생 아이템 선택 박스
BEGIN_IMPLEMENT_LISTENER( ZStagePutSacrificeItem, CCBTN_CLK_MSG)
	if ( !ZApplication::GetStageInterface()->m_SacrificeItem[ 0].IsExist())
		ZApplication::GetStageInterface()->OnDropSacrificeItem( 0);
	else if ( !ZApplication::GetStageInterface()->m_SacrificeItem[ 1].IsExist())
		ZApplication::GetStageInterface()->OnDropSacrificeItem( 1);
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER( ZStageSacrificeItemBoxOpen, CCBTN_CLK_MSG)
	ZApplication::GetStageInterface()->OpenSacrificeItemBox();
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER( ZStageSacrificeItemBoxClose, CCBTN_CLK_MSG)
	ZApplication::GetStageInterface()->CloseSacrificeItemBox();
END_IMPLEMENT_LISTENER();


// 릴레이맵
BEGIN_IMPLEMENT_LISTENER(ZGetRelayMapTypeListener, MCMBBOX_CHANGED)
	ZApplication::GetStageInterface()->PostRelayMapElementUpdate();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetRelayMapTurnCountListener, MCMBBOX_CHANGED)
	ZApplication::GetStageInterface()->PostRelayMapElementUpdate();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER( ZStageRelayMapBoxOpen, CCBTN_CLK_MSG)
	ZApplication::GetStageInterface()->OpenRelayMapBox();
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER( ZStageRelayMapBoxClose, CCBTN_CLK_MSG)
	ZApplication::GetStageInterface()->CloseRelayMapBox();
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER( ZGetRelayMapOKButtonListener, CCBTN_CLK_MSG)
	ZApplication::GetStageInterface()->PostRelayMapInfoUpdate();
END_IMPLEMENT_LISTENER();


// 게임 종료
BEGIN_IMPLEMENT_LISTENER( ZGetGameResultQuit, CCBTN_CLK_MSG)
	if ( ZGetGameClient()->IsLadderGame() || ZGetGameClient()->IsDuelTournamentGame())
		PostMessage(g_hWnd, WM_CHANGE_GAMESTATE, GUNZ_LOBBY, 0);
	else
		PostMessage(g_hWnd, WM_CHANGE_GAMESTATE, GUNZ_STAGE, 0);
END_IMPLEMENT_LISTENER();



// 테스트 버튼
BEGIN_IMPLEMENT_LISTENER( ZGetMonsterBookCaller, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetMonsterBookInterface()->OnCreate();
END_IMPLEMENT_LISTENER();


// 몬스터 도감
BEGIN_IMPLEMENT_LISTENER( ZGetMonsterInterfacePrevPage, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetMonsterBookInterface()->OnPrevPage();
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER( ZGetMonsterInterfaceNextPage, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetMonsterBookInterface()->OnNextPage();
END_IMPLEMENT_LISTENER();

BEGIN_IMPLEMENT_LISTENER( ZGetMonsterInterfaceQuit, CCBTN_CLK_MSG)
	ZGetGameInterface()->GetMonsterBookInterface()->OnDestroy();
END_IMPLEMENT_LISTENER();


// 등록
BEGIN_IMPLEMENT_LISTENER( ZGetRegisterListener, CCBTN_CLK_MSG)
//	ShellExecute( g_hWnd, "open", "IEXPLORE.EXE", "http://www.gunzonline.com/start.htm", NULL, SW_SHOW);
//	ShowWindow( g_hWnd, SW_SHOWMINIMIZED);
END_IMPLEMENT_LISTENER();
