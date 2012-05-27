#include "stdafx.h"

#include "zapplication.h"
#include "CCMatchObjCache.h"
#include ".\zplayerlistbox.h"
#include "CCBitmap.h"
#include "CCListBox.h"
#include "zpost.h"
#include "ZCharacterView.h"
#include "ZPlayerMenu.h"
#include "CCToolTip.h"
#include "ZButton.h"
#include "ZMyInfo.h"


#define PLAYER_SLOT_GAP 1

#define PLAYERLIST_ITEM_HEIGHT	23

bool GetUserGradeIDColor(CCMatchUserGradeID gid,sColor& UserNameColor,char* sp_name);
bool GetUserInfoUID(CCUID uid,sColor& _color,char* sp_name,CCMatchUserGradeID& gid);

void ZPlayerListBoxLook::OnItemDraw2(CCDrawContext* pDC, sRect& r, const char* szText, sColor color, bool bSelected, bool bFocus, int nAdjustWidth)
{
	if(szText==NULL) return;

	pDC->SetColor(color);

	sRect rtemp, rtemp2;
	rtemp2 = rtemp = pDC->GetClipRect();
	rtemp2.x	+= r.x;
	rtemp2.y	+= r.y;
	rtemp2.w	 = r.w;
	rtemp2.h	 = r.h;
	pDC->SetClipRect(rtemp2);
#ifdef COLORTEXT_SUPPORT
	pDC->TextMultiLine(r, szText,0,false);
#else
	pDC->Text(r.x, r.y+(r.h-pDC->GetFont()->GetHeight())/2, szText);
#endif
	pDC->SetClipRect(rtemp);
}


void ZPlayerListBoxLook::OnItemDraw2(CCDrawContext* pDC, sRect& r, CCBitmap* pBitmap, bool bSelected, bool bFocus, int nAdjustWidth)
{
	if(pBitmap==NULL) return;

	sRect rtemp, rtemp2;
	rtemp2 = rtemp = pDC->GetClipRect();
	rtemp2.w -= nAdjustWidth;
	pDC->SetClipRect(rtemp2);

	pDC->SetBitmap(pBitmap);
//	pDC->Draw(r.x, r.y);
	pDC->Draw(r, sRect(0,0,pBitmap->GetWidth(),pBitmap->GetHeight()));

	pDC->SetClipRect(rtemp);
}

sRect ZPlayerListBoxLook::GetClientRect(CCListBox* pListBox, sRect& r)
{
	return r;
}

float GetF(float _old,float _new)
{
	return _old/_new;
}

float GetF(float _new)
{
	return _new/800.f;
}


void ZPlayerListBoxLook::OnDraw(CCListBox* pListBox, CCDrawContext* pDC)
{
	((ZPlayerListBox*)pListBox)->UpdateList(((ZPlayerListBox*)pListBox)->GetPlayerListMode());

//	float fA = ((ZPlayerListBox*)pListBox)->OnReSize();
	int newW = RGetScreenWidth();
	float fA = GetF(newW);

	m_SelectedPlaneColor = sColor(180,220,180);

	int nItemHeight = 23*fA;//pListBox->GetItemHeight();
	int nShowCount = 0;

	sRect r = pListBox->GetClientRect();
	sPoint pos = pListBox->GetPosition();

	int nHeaderHeight = nItemHeight;

	sRect rr = pListBox->m_Rect;
	rr.x = 0;
	rr.y = 0;

	bool bShowClanCreateFrame = 
		((ZPlayerListBox*)pListBox)->GetMode()==ZPlayerListBox::PLAYERLISTMODE_CHANNEL_CLAN 
		&& !ZGetMyInfo()->IsClanJoined();

	// 바탕그려주고(동환이가 삭제...)
//	if(bShowClanCreateFrame) 
//		pDC->SetColor(32,32,32);
//	else
//		pDC->SetColor(60,60,60);
//	pDC->FillRectangle(rr.x,rr.y+1,rr.w,rr.h);

	pDC->SetColor(10,10,10);

	ZPlayerListBox::PLAYERLISTMODE pm = ((ZPlayerListBox*)pListBox)->GetPlayerListMode();

	int nMode = 0;// 0 : lobby 1 : stage

	if( pm == ZPlayerListBox::PLAYERLISTMODE_STAGE || pm == ZPlayerListBox::PLAYERLISTMODE_STAGE_FRIEND)
		nMode = 1;

	// 테두리 그려주고

	/*
	sRect cliprect = pDC->GetClipRect();//잠시풀어준다..

	pDC->SetClipRect(0,0,RGetScreenWidth(),RGetScreenHeight());

	pDC->SetColor(128,128,128);
	pDC->Rectangle( sRect(rr.x,rr.y,rr.w,rr.h) );
	*/

	/*
	//오른쪽 검은색으로 그려주고
	pDC->SetColor(0,0,0);
	pDC->FillRectangle(sRect(rr.x+rr.w-23*fA,rr.y+1,23*fA,rr.h-2));

	// 오른쪽에 이미지 그려주고

	if( ((ZPlayerListBox*)pListBox)->GetBitmap()) {
		CCBitmap* pBitmap = ((ZPlayerListBox*)pListBox)->GetBitmap();

		pDC->SetBitmap(pBitmap);
		pDC->Draw(sRect(rr.x+rr.w-pBitmap->GetWidth()*fA,rr.y+1,pBitmap->GetWidth()*fA,pBitmap->GetHeight()*fA), 
			sRect(0,0,pBitmap->GetWidth(),pBitmap->GetHeight()));
	}
	*/

//	pDC->SetClipRect(cliprect);

/*	if(!bShowClanCreateFrame)
	{
		pDC->SetColor(0,0,0);
		for(int i=0; i<pListBox->GetShowItemCount(); i++) {
			sPoint p;
			p.x = r.x;
			p.y = r.y+nItemHeight*i+1;
			pDC->HLine(1,p.y+nItemHeight-1,rr.w-1);
		}
	}
*/
	CCBitmap* pBaseBmp = NULL;

	for(int i=pListBox->GetStartItem(); i<pListBox->GetCount(); i++) {

		sPoint p;
		p.x = r.x;
		p.y = r.y+nHeaderHeight+nItemHeight*nShowCount;
//		p.x = 0;
//		p.y = 0+nHeaderHeight+nItemHeight*nShowCount;

//		cclog(" ------- p.y = %d %d \n",p.y,nItemHeight);

		CCListItem* pItem = pListBox->Get(i);
		bool bSelected = pItem->m_bSelected;
//		bool bSelected = (pListBox->IsSelected()) ? (pListBox->GetSelIndex()==i) : false;
		bool bFocused = (pListBox->IsFocus());

		int nFieldStartX = 0;

//		if(bSelected && bFocused) {//선택된아이템표시
		if(bSelected) {
//			pDC->SetColor(109,207,246);
//			pDC->FillRectangle(sRect(p.x,p.y+1,r.x+r.w,nItemHeight-1));
			pDC->SetColor(130,130,130);
			pDC->Rectangle(sRect(p.x+2,p.y+5,r.x+r.w-8,nItemHeight-4));		// 다시 그림(동환)
		}

		for(int j=0; j<max(pListBox->GetFieldCount(), 1); j++){

			int nTabSize = r.w;
			if(j<pListBox->GetFieldCount()) nTabSize = pListBox->GetField(j)->nTabSize;

			int nWidth = min(nTabSize, r.w-nFieldStartX);
			if(pListBox->m_bAbsoulteTabSpacing==false) nWidth = r.w*nTabSize/100;

			int nAdjustWidth = 0;

			if(pListBox->GetScrollBar()->IsVisible()){
				nAdjustWidth = pListBox->GetScrollBar()->GetRect().w + pListBox->GetScrollBar()->GetRect().w/2;
			}

//			sRect ir(p.x+nFieldStartX+4+5, p.y+4, nWidth-8, nItemHeight-8);
//			sRect irt(p.x+nFieldStartX+2+5, p.y+2, nWidth, nItemHeight);
			sRect ir(p.x+nFieldStartX+5, p.y+7, nWidth-7, nItemHeight-7);			// 다시 그림(동환)
			sRect irt(p.x+nFieldStartX, p.y+5, nWidth, nItemHeight);

			const char* szText = pItem->GetString(j);
			CCBitmap* pBitmap = pItem->GetBitmap(j);
			sColor color = pItem->GetColor();


			if(pBitmap!=NULL)//이미지 그리고
				OnItemDraw2(pDC, ir, pBitmap,  bSelected, bFocused, nAdjustWidth);

			if(szText!=NULL)// 텍스트 그리고
			{
//				if(bSelected && bFocused)
//				if(bSelected)
//					color = sColor(0,0,0);

				OnItemDraw2(pDC, irt, szText, color, bSelected, bFocused, nAdjustWidth);
			}

			nFieldStartX += nWidth;
			if(nFieldStartX>=r.w) break;
		}

		nShowCount++;

		if(nShowCount>=pListBox->GetShowItemCount()) break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_LOOK(ZPlayerListBox, ZPlayerListBoxLook)

ZPlayerListBox::ZPlayerListBox(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCListBox(szName, pParent, pListener)
{
	LOOK_IN_CONSTRUCTOR()
/*
	if(strcmp(szName,"StagePlayerList_")==0) {
		m_nMode = PLAYERLISTMODE_STAGE;
	}
	else 
		m_nMode = PLAYERLISTMODE_CHANNEL;
*/
	SetVisibleHeader(false);

	m_bAbsoulteTabSpacing = true;

//	m_MyUID = CCUID(0,0);
//	m_uidChannel = CCUID(0,0);
	m_nTotalPlayerCount = 0;
	m_nPage = 0;

	m_bVisible = true;
	m_bVisibleHeader = true;

//	AddField(" ", 23);//icon
//	AddField(" ",130);//name
//	AddField(" ", 23);//icon
//	AddField(" ", 32);//level

	SetItemHeight(PLAYERLIST_ITEM_HEIGHT);

//	m_pBitmap = NULL;
//	m_pBitmapIn = NULL;
	
	mSelectedPlayer = 0;
	mStartToDisplay = 0;
//	mPlayerOrder.reserve( sizeof(CCUID)*100 );
//	m_pScrollBar = new CCScrollBar( this, this );
//	ZApplication::GetGameInterface()->GetIDLResource()->InsertWidget("PlayerListScrollBar", m_pScrollBar );
	m_bAlwaysVisibleScrollbar = false;
	m_bHideScrollBar = true;
	m_pScrollBar->SetVisible(false);
	m_pScrollBar->Enable(false);

	m_pScrollBar->m_nDebugType = 3;
	m_nDebugType = 2;

	m_nOldW = RGetScreenWidth();

	SetListener(this);

	m_pButton = new ZButton(NULL,this,this);
	m_pButton->SetStretch(true);

	m_nMode = PLAYERLISTMODE_CHANNEL;
	InitUI(m_nMode);
}

ZPlayerListBox::~ZPlayerListBox()
{
	SAFE_DELETE(m_pButton);
}

void ZPlayerListBox::SetupButton(const char *szOn, const char *szOff)
{
	m_pButton->SetUpBitmap(CCBitmapManager::Get(szOff));
	m_pButton->SetDownBitmap(CCBitmapManager::Get(szOn));
	m_pButton->SetOverBitmap(CCBitmapManager::Get(szOff));
}

void ZPlayerListBox::InitUI(PLAYERLISTMODE nMode)
{
	int newW = RGetScreenWidth();
	float fA = GetF(newW);

	m_nMode = nMode;
	_ASSERT(nMode>=0 && nMode<PLAYERLISTMODE_END);

	RemoveAllField();

	// 필드 개수테이블 
	const int nFields[PLAYERLISTMODE_END] = { 6,6,2,2,4,4 }; 
	for(int i=0;i<nFields[nMode];i++) {
		AddField("",10);	// 일단 필드를 더해놓고 나중에 리사이즈한다
	}
	OnSize(0,0);

	bool bShowClanCreateFrame = false;

	switch(m_nMode) {
		case PLAYERLISTMODE_CHANNEL:	SetupButton("pltab_lobby_on.tga","pltab_lobby_off.tga");break;
		
		case PLAYERLISTMODE_STAGE: 		SetupButton("pltab_game_on.tga","pltab_game_off.tga");break;
		
		case PLAYERLISTMODE_CHANNEL_FRIEND:
		case PLAYERLISTMODE_STAGE_FRIEND: 	SetupButton("pltab_friends_on.tga","pltab_friends_off.tga");break;
		
		case PLAYERLISTMODE_CHANNEL_CLAN:
		case PLAYERLISTMODE_STAGE_CLAN: 	
		{
			SetupButton("pltab_clan_on.tga","pltab_clan_off.tga");
			bShowClanCreateFrame = !ZGetMyInfo()->IsClanJoined();
		}break;
	}

	// (좋지않은 구조) 클랜인데 클랜에 가입이 안되어있으면 생성 창을 보인다
	CCWidget *pFrame = ZGetGameInterface()->GetIDLResource()->FindWidget("LobbyPlayerListClanCreateFrame");
	CCButton* pButtonUp = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("LobbyChannelPlayerListPrev");
	CCButton* pButtonDn = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("LobbyChannelPlayerListNext");
	if( pFrame)
	{
		pFrame->Show(bShowClanCreateFrame);
		pButtonUp->Show(!bShowClanCreateFrame);
		pButtonDn->Show(!bShowClanCreateFrame);
	}
}

void ZPlayerListBox::RefreshUI()
{
	InitUI(GetMode());
}

void ZPlayerListBox::SetMode(PLAYERLISTMODE nMode)
{
	ZPlayerListBox* pWidget = (ZPlayerListBox*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("LobbyChannelPlayerList");
	if(pWidget) {
		pWidget->RemoveAll();
	}

	// cclog("ZPlayerListBox::SetMode %d \n" , nMode);

	InitUI(nMode);

	if(!ZGetGameClient()) return;
	if(nMode==PLAYERLISTMODE_CHANNEL) {
		if (ZGetGameClient()->IsConnected()) {
			//UpdateList(nMode);
			if(pWidget) {
				int nPage = pWidget->m_nPage;
				ZPostRequestChannelPlayerList(ZGetGameClient()->GetPlayerUID(),ZGetGameClient()->GetChannelUID(),nPage);
			}
		}
	}
	else if(nMode==PLAYERLISTMODE_STAGE) {
		if (ZGetGameClient()->IsConnected())
			ZPostRequestStagePlayerList(ZGetGameClient()->GetStageUID());
	}
	else if(nMode==PLAYERLISTMODE_CHANNEL_FRIEND || nMode==PLAYERLISTMODE_STAGE_FRIEND) {
		if (ZGetGameClient()->IsConnected())
			ZPostFriendList();
	}
	else if (nMode==PLAYERLISTMODE_CHANNEL_CLAN || nMode==PLAYERLISTMODE_STAGE_CLAN) {
		if (ZGetGameClient()->IsConnected())
			ZPostRequestClanMemberList(ZGetGameClient()->GetPlayerUID());
	}
}
/*
void ZPlayerListBox::SetBitmap( CCBitmap* pBitmap)
{
	m_pBitmap	= pBitmap;
}
*/

void ZPlayerListBox::AddTestItems()
{
/*
	switch (m_nMode)
	{
	case PLAYERLISTMODE_CHANNEL:
		AddPlayer(CCUID(0,0),PS_FIGHT,34,"아이디6글자1","클랜명여섯자",24401,CCMUGFREE,1);
		AddPlayer(CCUID(0,1),PS_WAIT ,10,"아이디6글자2","클랜명여섯자",24401,CCMUGFREE,2);
		AddPlayer(CCUID(0,2),PS_LOBBY,11,"아이디6글자3","클랜명여섯자",24401,CCMUGFREE,3);
		AddPlayer(CCUID(0,3),PS_LOBBY,99,"아이디6글자4","클랜명여섯자",24401,CCMUGFREE,4);
		AddPlayer(CCUID(0,4),PS_LOBBY, 3,"아이디6글자5","클랜명여섯자",24401,CCMUGFREE,5);
		AddPlayer(CCUID(0,5),PS_LOBBY, 1,"아이디6글자6","클랜명여섯자",24401,CCMUGFREE,6);
		AddPlayer(CCUID(0,6),PS_LOBBY,33,"아이디6글자7","클랜명여섯자",24401,CCMUGFREE,7);
		AddPlayer(CCUID(0,7),PS_LOBBY,45,"아이디6글자8","클랜명여섯자",24401,CCMUGFREE,8);
		AddPlayer(CCUID(0,8),PS_LOBBY,44,"아이디6글자9","클랜명여섯자",24401,CCMUGFREE,9);
		break;
	}
//*/
}

void GetRectMul(sRect* rect,sRect* org_rect,float f)
{
	rect->x = org_rect->x * f;
	rect->y = org_rect->y * f;
	rect->w = org_rect->w * f;
	rect->h = org_rect->h * f;
}

void ZPlayerListBox::OnSize(int w,int h)
{
	if(m_Fields.GetCount()==0) return;

	int newW = RGetScreenWidth();
	float fA = GetF(newW);

	m_nItemHeight = PLAYERLIST_ITEM_HEIGHT * fA;

//	m_pButton->SetBounds(1,1,m_Rect.w*.8f,m_nItemHeight+1);
	m_pButton->SetBounds(0, 0, m_Rect.w, (int)(28.0/*이미지 높이가 28이라서...*/*fA));		// 다시 그림(동환)

	switch(m_nMode) {
	case PLAYERLISTMODE_CHANNEL:
		{
			m_Fields.Get(0)->nTabSize = 23*fA;	//icon
			m_Fields.Get(1)->nTabSize = 16*fA;	//level
			m_Fields.Get(2)->nTabSize = 23*fA;	//icon (duel tournament grade)
			m_Fields.Get(3)->nTabSize = 94*fA;	//name
			m_Fields.Get(4)->nTabSize = 23*fA;	//icon
			m_Fields.Get(5)->nTabSize = 94*fA;	//clan name
		}
		break;
	case PLAYERLISTMODE_STAGE:
		{
			m_Fields.Get(0)->nTabSize = 23*fA;	//icon
			m_Fields.Get(1)->nTabSize = 16*fA;	//level
			m_Fields.Get(2)->nTabSize = 23*fA;	//icon (duel tournament grade)
			m_Fields.Get(3)->nTabSize = 94*fA;	//name
			m_Fields.Get(4)->nTabSize = 23*fA;	//icon
			m_Fields.Get(5)->nTabSize = 94*fA;	//clan name
		}
		break;
	case PLAYERLISTMODE_STAGE_FRIEND:
	case PLAYERLISTMODE_CHANNEL_FRIEND:
		{
			m_Fields.Get(0)->nTabSize = 23*fA;	//icon
			m_Fields.Get(1)->nTabSize = 72*fA;	//name
		}
		break;
	case PLAYERLISTMODE_CHANNEL_CLAN:
	case PLAYERLISTMODE_STAGE_CLAN:
		{
			m_Fields.Get(0)->nTabSize = 23*fA;	//icon
			m_Fields.Get(1)->nTabSize = 85*fA;	//name
			m_Fields.Get(2)->nTabSize = 23*fA;	//icon
			m_Fields.Get(3)->nTabSize = 90*fA;	//clan grade
		}
		break;
	};
	RecalcList();
}

// mode PLAYERLISTMODE_CHANNEL
void ZPlayerListBox::AddPlayer(CCUID& puid, ePlayerState state, int  nLevel,char* szName, char *szClanName, unsigned int nClanID, CCMatchUserGradeID nGrade, int duelTournamentGrade )
{
	if ( (int)strlen( szName) == 0)
		return;

	char szFileName[64] = "";
	char szLevel[64] = "";

	char* szRefName = NULL;

	sColor _color;
	char sp_name[256];
	bool bSpUser = false;

	if(GetUserGradeIDColor(nGrade,_color,sp_name)) {
//	if(GetUserInfoUID(puid,_color,sp_name,gid)) {
		sprintf(szLevel,"--");
		szRefName = sp_name;
		bSpUser = true;
	}
	else {
		sprintf(szLevel,"%2d",nLevel);
		szRefName = szName;
	}

	switch (state) {

		case PS_FIGHT	: strcpy(szFileName, "player_status_player.tga");	break;
		case PS_WAIT	: strcpy(szFileName, "player_status_game.tga");		break;
		case PS_LOBBY	: strcpy(szFileName, "player_status_lobby.tga");	break;
	}

	char szDTGradeIconFileName[64];
	GetDuelTournamentGradeIconFileName(szDTGradeIconFileName, duelTournamentGrade);
	CCBitmap* pBmpDTGradeIcon = CCBitmapManager::Get( szDTGradeIconFileName );

	//// 클랜 엠블럼
	//CCBitmap* pBitmapEmblem = NULL;
	//if ( strcmp( szClanName, "") != 0)
	//	pBitmapEmblem = ZGetEmblemInterface()->GetClanEmblem( nClanID );

	ZLobbyPlayerListItem* pItem = new ZLobbyPlayerListItem(puid, CCBitmapManager::Get(szFileName), nClanID, szLevel, szRefName, szClanName, state, nGrade, pBmpDTGradeIcon );

	if(bSpUser)
		pItem->SetColor(_color);

	CCListBox::Add( pItem );
}

// mode PLAYERLISTMODE_STAGE
void ZPlayerListBox::AddPlayer(CCUID& puid, CCMatchObjectStageState state, int nLevel, char* szName, char* szClanName, unsigned int nClanID, bool isMaster, CCMatchTeam nTeam, int duelTournamentGrade)
{
	if ( (int)strlen( szName) == 0)
		return;


	char szFileName[64] = "";
	char szFileNameState[64] = "";
	char szLevel[64] = "";

	char* szRefName = NULL;

	sColor _color;
	char sp_name[256];
	bool bSpUser = false;

	CCMatchUserGradeID gid = CCMUGFREE;

	if(GetUserInfoUID(puid,_color,sp_name,gid)){
		sprintf(szLevel,"--");
		szRefName = sp_name;
		bSpUser = true;
	}
	else {
		sprintf(szLevel,"%2d",nLevel);
		szRefName = szName;
	}

	CCBitmap* pBitmap = NULL;
	if(isMaster) {
		switch (state) {
			case MOSS_NONREADY	:
				if(nTeam == CCMT_RED)			strcpy(szFileName, "stg_status_master_red.tga");
				else if(nTeam == CCMT_BLUE)		strcpy(szFileName, "stg_status_master_blue.tga");
				else if(nTeam == CCMT_SPECTATOR)	strcpy(szFileName, "stg_status_master_observer.tga");
				else 							strcpy(szFileName, "stg_status_master_normal.tga");
				break;
			case MOSS_READY		:
				if(nTeam == CCMT_RED)			strcpy(szFileName, "stg_status_master_red_ready.tga");
				else if(nTeam == CCMT_BLUE)		strcpy(szFileName, "stg_status_master_blue_ready.tga");
				else if(nTeam == CCMT_SPECTATOR)	strcpy(szFileName, "stg_status_master_observer.tga");
				else 							strcpy(szFileName, "stg_status_master_normal_ready.tga");
				break;
			case MOSS_EQUIPMENT	: 
				if(nTeam == CCMT_RED)			strcpy(szFileName, "stg_status_master_red_equip.tga");
				else if(nTeam == CCMT_BLUE)		strcpy(szFileName, "stg_status_master_blue_equip.tga");
				else if(nTeam == CCMT_SPECTATOR)	strcpy(szFileName, "stg_status_master_observer.tga");
				else 							strcpy(szFileName, "stg_status_master_normal_equip.tga");
				break;
			default :
				strcpy(szFileName, " ");
				break;
		}
	}
	else {
		switch (state) {
			case MOSS_NONREADY	:
				if(nTeam == CCMT_RED)			strcpy(szFileName, "stg_status_member_red.tga");
				else if(nTeam == CCMT_BLUE)		strcpy(szFileName, "stg_status_member_blue.tga");	
				else if(nTeam == CCMT_SPECTATOR)	strcpy(szFileName, "stg_status_member_observer.tga");	
				else							strcpy(szFileName, "stg_status_member_normal.tga");	
				break;
			case MOSS_READY		: 
				if(nTeam == CCMT_RED)			strcpy(szFileName, "stg_status_member_red_ready.tga");
				else if(nTeam == CCMT_BLUE)		strcpy(szFileName, "stg_status_member_blue_ready.tga");	
				else if(nTeam == CCMT_SPECTATOR)	strcpy(szFileName, "stg_status_member_observer.tga");	
				else							strcpy(szFileName, "stg_status_member_normal_ready.tga");	
				break;
			case MOSS_EQUIPMENT	: 
				if(nTeam == CCMT_RED)			strcpy(szFileName, "stg_status_member_red_equip.tga");
				else if(nTeam == CCMT_BLUE)		strcpy(szFileName, "stg_status_member_blue_equip.tga");	
				else if(nTeam == CCMT_SPECTATOR)	strcpy(szFileName, "stg_status_member_observer.tga");	
				else							strcpy(szFileName, "stg_status_member_normal_equip.tga");	
				break;
		}
	}
	pBitmap = CCBitmapManager::Get(szFileName);


	char szDTGradeIconFileName[64];
	GetDuelTournamentGradeIconFileName(szDTGradeIconFileName, duelTournamentGrade);
	CCBitmap* pBmpDTGradeIcon = CCBitmapManager::Get( szDTGradeIconFileName );

	//// 클랜 엠블럼
	//CCBitmap* pBitmapEmblem = NULL;
	//if ( strcmp( szClanName, "") != 0)
	//	pBitmapEmblem = ZGetEmblemInterface()->GetClanEmblem( nClanID );

	ZStagePlayerListItem* pItem = new ZStagePlayerListItem(puid, pBitmap, nClanID, szRefName, szClanName, szLevel, gid, pBmpDTGradeIcon);

	if(bSpUser)
		pItem->SetColor(_color);

	CCListBox::Add( pItem );

	if( ZGetMyUID() == puid )
	{
		bool bBlue, bRed;
		bBlue = bRed = false;
		if( nTeam == CCMT_BLUE)	bBlue = true;
		if( nTeam == CCMT_RED)	bRed = true;
		
		CCButton* pButton = (CCButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("StageTeamBlue");
		pButton->SetCheck( bBlue );

		pButton = (CCButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("StageTeamRed");
		pButton->SetCheck( bRed );
	}
}

// mode PLAYERLISTMODE_CHANNEL_FRIEND, PLAYERLISTMODE_STAGE_FRIEND
void ZPlayerListBox::AddPlayer(ePlayerState state, char* szName, char* szLocation)
{
	if ( (int)strlen( szName) == 0)
		return;


	char szFileName[64] = "";
	switch (state) {
		case PS_FIGHT	: strcpy(szFileName, "player_status_player.tga");	break;
		case PS_WAIT	: strcpy(szFileName, "player_status_game.tga");		break;
		case PS_LOBBY	: strcpy(szFileName, "player_status_lobby.tga");	break;
	}

	CCListBox::Add(new ZFriendPlayerListItem(CCUID(0,0),CCBitmapManager::Get(szFileName), szName,NULL,szLocation,state,CCMUGFREE));
}

// mode PLAYERLISTMODE_CHANNEL_CLAN
void ZPlayerListBox::AddPlayer(CCUID& puid, ePlayerState state, char* szName, int nLevel ,CCMatchClanGrade nGrade )
{
	if ( (int)strlen( szName) == 0)
		return;


	char szFileName[64] = "";
	char szGradeName[64];

	char* szRefName = NULL;

	sColor _color = sColor(240,64,64);
//	char sp_name[256];
	bool bSpUser = false;

	switch(nGrade) {
		case CCG_MASTER : 
			sprintf(szGradeName, ZMsg( MSG_WORD_CLAN_MASTER));
			bSpUser = true;
			break;
		case CCG_ADMIN	: 
			sprintf(szGradeName, ZMsg( MSG_WORD_CLAN_ADMIN));
			bSpUser = true;
			break;
		default : sprintf(szGradeName, ZMsg( MSG_WORD_CLAN_MEMBER));
			break;
	}
	szRefName = szName;

	switch (state) {

		case PS_FIGHT	: strcpy(szFileName, "player_status_player.tga");	break;
		case PS_WAIT	: strcpy(szFileName, "player_status_game.tga");		break;
		case PS_LOBBY	: strcpy(szFileName, "player_status_lobby.tga");	break;
	}

	ZClanPlayerListItem* pItem = new ZClanPlayerListItem(puid, CCBitmapManager::Get(szFileName), szRefName, szGradeName, NULL, state, nGrade );

	if(bSpUser)
		pItem->SetColor(_color);

	CCListBox::Add( pItem );
}

void ZPlayerListBox::DelPlayer(CCUID& puid)
{
	ZPlayerListItem* pItem = NULL;

	for(int i=0;i<GetCount();i++) {
		pItem = (ZPlayerListItem*)Get(i);
		if(pItem->m_PlayerUID==puid){
			Remove(i);
			return;
		}
	}
}

// 나중에 바꾸자 바쁘다.. 새벽 5:16분..

ZPlayerListItem* ZPlayerListBox::GetUID(CCUID uid)
{
	ZPlayerListItem* pItem = NULL;

	for(int i=0;i<GetCount();i++) {
		pItem = (ZPlayerListItem*)Get(i);
		if(pItem->m_PlayerUID==uid)
			return pItem;
	}
	return NULL;
}


const char* ZPlayerListBox::GetPlayerName( int nIndex)
{
	ZPlayerListItem* pItem = (ZPlayerListItem*)Get( nIndex);

	if ( !pItem)
		return NULL;

	return pItem->m_szName;
}


// 위치는 좀 이상하지만~

static DWORD g_zplayer_list_update_time = 0;

#define ZPLAYERLIST_UPDATE_TIME 2000

void ZPlayerListBox::UpdateList(int mode)
{
	if (ZGetGameClient()->IsConnected() == false) return;

	DWORD this_time = timeGetTime();

	if( this_time < g_zplayer_list_update_time + ZPLAYERLIST_UPDATE_TIME)
		return;

	if(mode==PLAYERLISTMODE_CHANNEL) {
/*		ZPlayerListBox* pWidget = (ZPlayerListBox*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("LobbyChannelPlayerList");
		int nPage = pWidget->m_nPage;
		if(pWidget)
		{
			ZPostRequestChannelPlayerList(pWidget->m_MyUID,pWidget->m_uidChannel,nPage);
		}*/
	}
	else if(mode==PLAYERLISTMODE_STAGE) {
		// Do Nothing !!
	}
	else if(mode==PLAYERLISTMODE_CHANNEL_CLAN) {

		// test로 여기서 업데이트할때 클랜 리스트달라고도 계~속 요청한다. 테스트끝나면 삭제요망 - by bird
//#ifndef _PUBLISH
//			ZPostRequestClanMemberList(ZGetGameClient()->GetPlayerUID());				
//#endif
	}

	g_zplayer_list_update_time = this_time;
}


void ZPlayerListBox::UpdatePlayer(CCUID& puid,CCMatchObjectStageState state, bool isMaster,CCMatchTeam nTeam)
{
	ZStagePlayerListItem* pItem = (ZStagePlayerListItem*)GetUID(puid);

	if(pItem) {

		char szFileName[64] = "";
		char szFileNameState[64] = "";

		CCBitmap* pBitmap = NULL;
		CCBitmap* pBitmapState = NULL;

		const MSTAGE_SETTING_NODE* pStageSetting = ZGetGameClient()->GetMatchStageSetting()->GetStageSetting();

		if ( (nTeam != CCMT_SPECTATOR) && (ZGetGameTypeManager()->IsTeamGame(pStageSetting->nGameType) == false))
		{
			nTeam = CCMT_ALL;
		}

		if(isMaster) {
			switch (state) {
				case MOSS_NONREADY	:
					if(nTeam == CCMT_RED)			{ strcpy(szFileName, "stg_status_master_red.tga");				pItem->m_nTeam = 1;}
					else if(nTeam == CCMT_BLUE)		{ strcpy(szFileName, "stg_status_master_blue.tga");				pItem->m_nTeam = 2;}
					else if(nTeam == CCMT_SPECTATOR)	{ strcpy(szFileName, "stg_status_master_observer.tga");			pItem->m_nTeam = 3;}
					else 							{ strcpy(szFileName, "stg_status_master_normal.tga");			pItem->m_nTeam = 0;}
					break;
				case MOSS_READY		:
					if(nTeam == CCMT_RED)			{ strcpy(szFileName, "stg_status_master_red_ready.tga");		pItem->m_nTeam = 1;}
					else if(nTeam == CCMT_BLUE)		{ strcpy(szFileName, "stg_status_master_blue_ready.tga");		pItem->m_nTeam = 2;}
					else if(nTeam == CCMT_SPECTATOR)	{ strcpy(szFileName, "stg_status_master_observer.tga");			pItem->m_nTeam = 3;}
					else 							{ strcpy(szFileName, "stg_status_master_normal_ready.tga");		pItem->m_nTeam = 0;}
					break;
				case MOSS_EQUIPMENT	: 
					if(nTeam == CCMT_RED)			{ strcpy(szFileName, "stg_status_master_red_equip.tga");		pItem->m_nTeam = 1;}
					else if(nTeam == CCMT_BLUE)		{ strcpy(szFileName, "stg_status_master_blue_equip.tga");		pItem->m_nTeam = 2;}
					else if(nTeam == CCMT_SPECTATOR)	{ strcpy(szFileName, "stg_status_master_observer.tga");			pItem->m_nTeam = 3;}
					else 							{ strcpy(szFileName, "stg_status_master_normal_equip.tga");		pItem->m_nTeam = 0;}
					break;
				default :
					strcpy(szFileName, " ");
					break;
			}
		}
		else {
			switch (state) {
				case MOSS_NONREADY	:
					if(nTeam == CCMT_RED)			{ strcpy(szFileName, "stg_status_member_red.tga");				pItem->m_nTeam = 1;}
					else if(nTeam == CCMT_BLUE)		{ strcpy(szFileName, "stg_status_member_blue.tga");				pItem->m_nTeam = 2;}
					else if(nTeam == CCMT_SPECTATOR)	{ strcpy(szFileName, "stg_status_member_observer.tga");			pItem->m_nTeam = 3;}
					else 							{ strcpy(szFileName, "stg_status_member_normal.tga");			pItem->m_nTeam = 0;}
					break;
				case MOSS_READY		: 
					if(nTeam == CCMT_RED)			{ strcpy(szFileName, "stg_status_member_red_ready.tga");		pItem->m_nTeam = 1;}
					else if(nTeam == CCMT_BLUE)		{ strcpy(szFileName, "stg_status_member_blue_ready.tga");		pItem->m_nTeam = 2;}
					else if(nTeam == CCMT_SPECTATOR)	{ strcpy(szFileName, "stg_status_member_observer.tga");			pItem->m_nTeam = 3;}
					else 							{ strcpy(szFileName, "stg_status_member_normal_ready.tga");		pItem->m_nTeam = 0;}
					break;
				case MOSS_EQUIPMENT	: 
					if(nTeam == CCMT_RED)			{ strcpy(szFileName, "stg_status_member_red_equip.tga");		pItem->m_nTeam = 1;}
					else if(nTeam == CCMT_BLUE)		{ strcpy(szFileName, "stg_status_member_blue_equip.tga");		pItem->m_nTeam = 2;}
					else if(nTeam == CCMT_SPECTATOR)	{ strcpy(szFileName, "stg_status_member_observer.tga");			pItem->m_nTeam = 3;}
					else 							{ strcpy(szFileName, "stg_status_member_normal_equip.tga");		pItem->m_nTeam = 0;}
					break;
			}
		}

/*		switch (state) {

		case MOSS_NONREADY	: strcpy(szFileNameState, " ");							break;
		case MOSS_READY		: strcpy(szFileNameState, "char_stat_ready.tga");		break;
		case MOSS_SHOP		: strcpy(szFileNameState, "char_stat_shop.tga");		break;
		case MOSS_EQUIPMENT	: strcpy(szFileNameState, "char_stat_equip.tga");		break;
		}
*/
		pBitmap = CCBitmapManager::Get(szFileName);
//		pBitmapState = CCBitmapManager::Get(szFileNameState);

		pItem->m_pBitmap = pBitmap;
//		pItem->m_pBitmapState = pBitmapState;
//		char temp[256];
//		sprintf(temp,"UpdatePlayer (%s,%s)/%d \n",szFileName,szFileNameState,GetCount() );
//		OutputDebugString(temp);
	}	

	ZCharacterView* pCharView = (ZCharacterView*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Stage_Charviewer_");
	if( pCharView != 0 && puid == pCharView->m_Info.UID )
	{
		pCharView->m_Info.bMaster = isMaster;
		pCharView->m_Info.m_pMnTeam->Set_CheckCrc((int) nTeam);
		pCharView->m_Info.nStageState = state;
	}

	if( (nTeam != CCMT_SPECTATOR) && (ZGetMyUID() == puid ))
	{
		bool bBlue, bRed;
		bBlue = bRed = false;
		if( nTeam == CCMT_BLUE)	bBlue = true;
		if( nTeam == CCMT_RED)	bRed = true;
				
		CCButton* pButton = (CCButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("StageTeamBlue");
		pButton->SetCheck( bBlue );

		pButton = (CCButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("StageTeamRed");
		pButton->SetCheck( bRed );
	}
}


void ZPlayerListBox::UpdatePlayer(CCUID& puid,CCMatchObjectStageState state, char* szName, int  nLevel ,bool isMaster,CCMatchTeam nTeam)
{
	return;

	ZStagePlayerListItem* pItem = (ZStagePlayerListItem*)GetUID(puid);
	if(pItem) {

		char szFileName[64] = "";
		char szFileNameState[64] = "";
		char szLevel[64];

		CCBitmap* pBitmap = NULL;
		CCBitmap* pBitmapState = NULL;

		if(isMaster) {
			switch (state) {
				case MOSS_NONREADY	:
					if(nTeam == CCMT_RED)			strcpy(szFileName, "stg_status_master_red.tga");
					else if(nTeam == CCMT_BLUE)		strcpy(szFileName, "stg_status_master_blue.tga");
					else if(nTeam == CCMT_SPECTATOR)	strcpy(szFileName, "stg_status_master_observer.tga");
					else 							strcpy(szFileName, "stg_status_master_normal.tga");
					break;
				case MOSS_READY		:
					if(nTeam == CCMT_RED)			strcpy(szFileName, "stg_status_master_red_ready.tga");
					else if(nTeam == CCMT_BLUE)		strcpy(szFileName, "stg_status_master_blue_ready.tga");
					else if(nTeam == CCMT_SPECTATOR)	strcpy(szFileName, "stg_status_master_observer.tga");
					else 							strcpy(szFileName, "stg_status_master_normal_ready.tga");
					break;
				case MOSS_EQUIPMENT	: 
					if(nTeam == CCMT_RED)			strcpy(szFileName, "stg_status_master_red_equip.tga");
					else if(nTeam == CCMT_BLUE)		strcpy(szFileName, "stg_status_master_blue_equip.tga");
					else if(nTeam == CCMT_SPECTATOR)	strcpy(szFileName, "stg_status_master_observer.tga");
					else 							strcpy(szFileName, "stg_status_master_normal_equip.tga");
					break;
				default :
					strcpy(szFileName, " ");
					break;
			}
		}
		else {
			switch (state) {
				case MOSS_NONREADY	:
					if(nTeam == CCMT_RED)			strcpy(szFileName, "stg_status_member_red.tga");
					else if(nTeam == CCMT_BLUE)		strcpy(szFileName, "stg_status_member_blue.tga");
					else if(nTeam == CCMT_SPECTATOR)	strcpy(szFileName, "stg_status_member_observer.tga");
					else 							strcpy(szFileName, "stg_status_member_normal.tga");
					break;
				case MOSS_READY		: 
					if(nTeam == CCMT_RED)			strcpy(szFileName, "stg_status_member_red_ready.tga");
					else if(nTeam == CCMT_BLUE)		strcpy(szFileName, "stg_status_member_blue_ready.tga");
					else if(nTeam == CCMT_SPECTATOR)	strcpy(szFileName, "stg_status_member_observer.tga");
					else 							strcpy(szFileName, "stg_status_member_normal_ready.tga");
					break;
				case MOSS_EQUIPMENT	: 
					if(nTeam == CCMT_RED)			strcpy(szFileName, "stg_status_member_red_equip.tga");
					else if(nTeam == CCMT_BLUE)		strcpy(szFileName, "stg_status_member_blue_equip.tga");
					else if(nTeam == CCMT_SPECTATOR)	strcpy(szFileName, "stg_status_member_observer.tga");
					else 							strcpy(szFileName, "stg_status_member_normal_equip.tga");
					break;
			}
		}
/*
		switch (state) {

		case MOSS_NONREADY	: strcpy(szFileNameState, " ");							break;
		case MOSS_READY		: strcpy(szFileNameState, "char_stat_ready.tga");		break;
		case MOSS_SHOP		: strcpy(szFileNameState, "char_stat_shop.tga");		break;
		case MOSS_EQUIPMENT	: strcpy(szFileNameState, "char_stat_equip.tga");		break;
		}
*/
		pBitmap = CCBitmapManager::Get(szFileName);
//		pBitmapState = CCBitmapManager::Get(szFileNameState);

		pItem->m_pBitmap = pBitmap;
//		pItem->m_pBitmapState = pBitmapState;

		///////////////////////////////////////////////////////
/*
		sColor _color;
		char sp_name[256];

		if(GetUserInfoUID(puid,_color,sp_name,gid)) {
			sprintf(szLevel,"Lv ---");
			strcpy(pItem->m_szLevel,szLevel);
			strcpy(pItem->m_szName,sp_name);
		}
		else {
			sprintf(szLevel,"Lv %2d",nLevel);
			strcpy(pItem->m_szLevel,szLevel);
			strcpy(pItem->m_szName,szName);
		}
*/
		sprintf(szLevel,"Lv %2d",nLevel);
		strcpy(pItem->m_szLevel,szLevel);
		strcpy(pItem->m_szName,szName);

	}

	ZCharacterView* pCharView = (ZCharacterView*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Stage_Charviewer_");
	if( pCharView != 0 && puid == pCharView->m_Info.UID )
	{
		pCharView->m_Info.bMaster = isMaster;
		pCharView->m_Info.m_pMnTeam->Set_CheckCrc((int) nTeam);
		pCharView->m_Info.nStageState = state;
		pCharView->m_Info.nLevel = nLevel;
		pCharView->SetText(szName);
	}

	if( ZGetMyUID() == puid )
	{
		bool bBlue, bRed;
		bBlue = bRed = false;
		if( nTeam == CCMT_BLUE)	bBlue = true;
		if( nTeam == CCMT_RED)	bRed = true;

		CCButton* pButton = (CCButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("StageTeamBlue");
		pButton->SetCheck( bBlue );

		pButton = (CCButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("StageTeamRed");
		pButton->SetCheck( bRed );
	}
//	pCharView->m_Info.m_pMnTeam->SetWarpingAdd(GetTickCount());
}

bool ZPlayerListBox::OnCommand(CCWidget* pWidget, const char* szMessage)
{
	if(pWidget==m_pButton) {
		if( strcmp(szMessage, CCBTN_CLK_MSG)==0 ) {
			switch(GetMode()) {
				//채널안에서 로테이트
			case PLAYERLISTMODE_CHANNEL :
				SetMode(PLAYERLISTMODE_CHANNEL_FRIEND);
				break;
			case PLAYERLISTMODE_CHANNEL_FRIEND :
				SetMode(PLAYERLISTMODE_CHANNEL_CLAN);
				break;
			case PLAYERLISTMODE_CHANNEL_CLAN :
				SetMode(PLAYERLISTMODE_CHANNEL);
				break;
				// 스테이지 안에서..
			case PLAYERLISTMODE_STAGE :
//				SetMode(PLAYERLISTMODE_STAGE_FRIEND);
				break;
			case PLAYERLISTMODE_STAGE_FRIEND :
/*
				if(!ZGetMyInfo()->IsClanJoined())	// 클랜이 없으면 넘어간다
					SetMode(PLAYERLISTMODE_STAGE);
				else
					SetMode(PLAYERLISTMODE_STAGE_CLAN);
*/
				break;
			case PLAYERLISTMODE_STAGE_CLAN :
//				SetMode(PLAYERLISTMODE_STAGE);
				break;
			default:
				SetMode(PLAYERLISTMODE_CHANNEL);
				break;
			}
			return true;
		}
	}else
	if( strcmp(szMessage, "selected")==0 ) {

		if(m_nSelItem != -1) {
			ZStagePlayerListItem* pItem = (ZStagePlayerListItem*)Get(m_nSelItem);
			if(pItem) {
				CCUID uid = pItem->m_PlayerUID;

				// 플레이어를 클릭하면 좌측의 캐릭터뷰에 해당 캐릭터 이미지가
				// 뜨는거 막음. 아직 캐릭터 정보까지 변경 안되는 관계로...  -_-;(동환)
				ZCharacterView* pCharView = (ZCharacterView*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Stage_Charviewer");
				if(pCharView!=0) pCharView->SetCharacter( uid );
			}
		}
//		ZStagePlayerListItem* pItem = (ZStagePlayerListItem*) pWidget;
		return true;
	}
	else if(strcmp(szMessage, "selected2")==0) {
		// 차후 메뉴로 수정~
		if((GetKeyState(VK_MENU)&0x8000)!=0) {
//			ZStagePlayerListItem* pItem = (ZStagePlayerListItem*) pWidget;
			if(m_nSelItem != -1) {
				ZStagePlayerListItem* pItem = (ZStagePlayerListItem*)Get(m_nSelItem);
				if(pItem) {
					char temp[1024];
					sprintf(temp,"/kick %s", pItem->m_szName);
					ZPostStageChat(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID(), temp);
				}
			}
		}
	}

	return true;
}
bool ZPlayerListBox::OnEvent(CCEvent* pEvent, CCListener* pListener)
{
	sRect rtClient = GetClientRect();

	if(pEvent->nMessage==MWM_RBUTTONDOWN) {
		if(rtClient.InPoint(pEvent->sPos)==true) {
			int nSelItem = FindItem(pEvent->sPos);
			if (nSelItem != -1) {
				ZLobbyPlayerListItem* pItem = (ZLobbyPlayerListItem*)Get(nSelItem);

				bool bShow = true;

				sColor _color;
				char sp_name[256];
				CCMatchUserGradeID gid;

				if(pItem->GetUID() == ZGetMyUID() && 
					GetMode()!=PLAYERLISTMODE_CHANNEL_CLAN)	
					bShow = false;

				GetUserInfoUID(pItem->GetUID(),_color,sp_name,gid);

				if((gid==CCMUGDEVELOPER) || (gid==CCMUGADMIN)) { // 개발자나 관리자라면..
					bShow = false;
				}

				SetSelIndex(nSelItem);
				if (bShow) {

					ZPlayerMenu* pMenu = ZApplication::GetGameInterface()->GetPlayerMenu();
					pMenu->SetTargetName(pItem->GetString());
					pMenu->SetTargetUID(pItem->GetUID());

					switch(GetMode()) {
					case PLAYERLISTMODE_CHANNEL:
						pMenu->SetupMenu(ZPLAYERMENU_SET_LOBBY);
						break;
					case PLAYERLISTMODE_STAGE:
						pMenu->SetupMenu(ZPLAYERMENU_SET_STAGE);
						break;
					case PLAYERLISTMODE_CHANNEL_FRIEND:
						pMenu->SetupMenu(ZPLAYERMENU_SET_FRIEND);
						break;
					case PLAYERLISTMODE_STAGE_FRIEND:
						pMenu->SetupMenu(ZPLAYERMENU_SET_FRIEND);
						break;
					case PLAYERLISTMODE_CHANNEL_CLAN:
						if(pItem->GetUID() == ZGetMyUID())
							pMenu->SetupMenu(ZPLAYERMENU_SET_CLAN_ME);
						else
							pMenu->SetupMenu(ZPLAYERMENU_SET_CLAN);
						break;
					default:
						_ASSERT("Unknown PlayerMenu Setup");
					};

					// 위치를 조절해준다
/*					sPoint posItem;
					GetItemPos(&posItem, nSelItem);
					sPoint posMenu;
					posMenu.x = posItem.x + GetRect().w/2;
					posMenu.y = posItem.y + GetItemHeight()/2;
					posMenu=CCClientToScreen(this,posMenu);

					if(posMenu.y+pMenu->GetRect().h > CCGetWorkspaceHeight()) {
						posMenu.y-= pMenu->GetRect().h ;
					}

					pMenu->Show(posMenu.x, posMenu.y, true);
*/
					sPoint posItem;
					posItem = pEvent->sPos;
					sPoint posMenu = CCClientToScreen(this, posItem);
					
					if ( (posMenu.x + pMenu->GetClientRect().w) > (CCGetWorkspaceWidth() - 5))
						posMenu.x = CCGetWorkspaceWidth() - pMenu->GetClientRect().w - 5;

					if ( (posMenu.y + pMenu->GetClientRect().h) > (CCGetWorkspaceHeight() - 5))
						posMenu.y = CCGetWorkspaceHeight() - pMenu->GetClientRect().h - 5;

					pMenu->Show( posMenu.x, posMenu.y);
				}
				return true;
			}
		}
	}


	// 플레이어 정보 출력
	else if ( pEvent->nMessage == MWM_LBUTTONDBLCLK)
	{
		if ( rtClient.InPoint( pEvent->sPos) == true)
		{
			int nSelItem = FindItem( pEvent->sPos);

			if ( nSelItem != -1)
			{
#ifdef _DEBUG
				ZLobbyPlayerListItem* pItem = (ZLobbyPlayerListItem*)Get( nSelItem);
				ZPostRequestCharInfoDetail( ZGetMyUID(), pItem->m_szName);
#endif
			}
		}
	}


/*	if (GetToolTip()) {
		if ( (GetMode() == PLAYERLISTMODE_CHANNEL_FRIEND) || (GetMode() == PLAYERLISTMODE_STAGE_FRIEND) ) {
			if(rtClient.InPoint(pEvent->sPos)==true) {
				int nSelItem = FindItem(pEvent->sPos);
				if (nSelItem != -1) {
					ZFriendPlayerListItem* pItem = (ZFriendPlayerListItem*)Get(nSelItem);

					sPoint posItem;
					GetItemPos(&posItem, nSelItem);
					sPoint posToolTip;
					posToolTip.x = GetRect().x + posItem.x + GetRect().w/2;
					posToolTip.y = GetRect().y + posItem.y + GetItemHeight()/2;

					GetToolTip()->SetPosition(posToolTip.x, posToolTip.y);
					GetToolTip()->SetText(pItem->GetLocation());
				} else {
					GetToolTip()->Show(false);
				}
			}
		} else {
			GetToolTip()->Show(false);
		}
	}
*/
	return CCListBox::OnEvent(pEvent, pListener);;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/*
IMPLEMENT_LOOK(ZStagePlayerListBox, ZPlayerListBoxLook)

////////////////////////////////////////////////////////////////////////////////////////////////////

ZStagePlayerListBox::ZStagePlayerListBox(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCListBox(szName, pParent, pListener)
{
	LOOK_IN_CONSTRUCTOR()

	SetVisibleHeader(false);

	m_bAbsoulteTabSpacing = true;

	m_MyUID = CCUID(0,0);
	m_uidChannel = CCUID(0,0);
	m_nTotalPlayerCount = 0;
	m_nPage = 0;

	m_bVisible = true;

	AddField(" ", 23);//icon
	AddField(" ",130);//name
	AddField(" ", 23);//state icon
	AddField(" ", 32);//level

	SetItemHeight(23);

//	m_pBitmap = NULL;
	m_pBitmap = CCBitmapManager::Get("playerlist_tab_game.png");
	
	mSelectedPlayer = 0;
	mStartToDisplay = 0;
	mPlayerOrder.reserve( sizeof(CCUID)*100 );
//	m_pScrollBar = new CCScrollBar( this, this );
//	ZApplication::GetGameInterface()->GetIDLResource()->InsertWidget("PlayerListScrollBar", m_pScrollBar );

//	AddPlayer(CCUID(0,0), MOSS_READY		, "aaaaa", 10 ,true ,CCMT_RED );
//	AddPlayer(CCUID(0,1), MOSS_SHOP		, "bbbbb", 11 ,false,CCMT_BLUE);
//	AddPlayer(CCUID(0,2), MOSS_EQUIPMENT	, "ccccc", 12 ,false,CCMT_RED );
//	AddPlayer(CCUID(0,3), MOSS_NONREADY	, "ddddd", 13 ,false,CCMT_BLUE);

	m_bAlwaysVisibleScrollbar = false;
	m_bHideScrollBar = true;
	m_pScrollBar->SetVisible(false);
	m_pScrollBar->Enable(false);

	m_pScrollBar->m_nDebugType = 3;
	m_nDebugType = 2;

	m_nOldW = RGetScreenWidth();
}

ZStagePlayerListBox::~ZStagePlayerListBox()
{
}

void ZStagePlayerListBox::SetBitmap( CCBitmap* pBitmap)
{
	m_pBitmap	= pBitmap;
}

void ZStagePlayerListBox::Resize(float x,float y)
{
//	OnReSize();
}

float ZStagePlayerListBox::OnReSize()
{
	int newW = RGetScreenWidth();
	float fA = GetF(newW);

	m_nItemHeight = 23 * fA;

	MLISTFIELD* fi;

	for(int i=0;i<m_Fields.GetCount();i++) {
			
		fi = m_Fields.Get(i);

		if(i==0)		fi->nTabSize = 23 * fA;
		else if(i==1)	fi->nTabSize = 130 * fA;
		else if(i==2) 	fi->nTabSize = 23 * fA;
		else if(i==3) 	fi->nTabSize = 32 * fA;
	}

	return fA;
}

void ZStagePlayerListBox::DelPlayer(CCUID& puid)
{
	ZStagePlayerListItem* pItem = NULL;

	for(int i=0;i<GetCount();i++) {
		pItem = (ZStagePlayerListItem*)Get(i);
		if(pItem->m_PlayerUID==puid){
			Remove(i);
			return;
		}
	}
}

// 나중에 바꾸자 바쁘다.. 새벽 5:16분..

ZStagePlayerListItem* ZStagePlayerListBox::GetUID(CCUID uid)
{
	ZStagePlayerListItem* pItem = NULL;

	for(int i=0;i<GetCount();i++) {
		pItem = (ZStagePlayerListItem*)Get(i);
		if(pItem->m_PlayerUID==uid)
			return pItem;
	}
	return NULL;
}

void ZStagePlayerListBox::UpdatePlayer(CCUID& puid,eStagePlayerState state, char* szName, int  nLevel ,bool isMaster,int nTeam)
{
	ZStagePlayerListItem* pItem = GetUID(puid);
	if(pItem) {

		char szFileName[64] = "";
		char szFileNameState[64] = "";
		char szLevel[64];

		sprintf(szLevel,"Lv %2d",nLevel);

		CCBitmap* pBitmap = NULL;
		CCBitmap* pBitmapState = NULL;

		if(isMaster) {
				 if(nTeam == CCMT_RED)	strcpy(szFileName, "stg_status_master_red.tga");	
			else if(nTeam == CCMT_BLUE)	strcpy(szFileName, "stg_status_master_blue.tga");	
			else 						strcpy(szFileName, "stg_status_master_normal.tga");	
		}
		else {
			if(nTeam == CCMT_RED)		strcpy(szFileName, "stg_status_member_red.tga");	
			else if(nTeam == CCMT_BLUE)	strcpy(szFileName, "stg_status_member_blue.tga");	
			else						strcpy(szFileName, " ");	
		}

		switch (state) {

		case MOSS_NONREADY	: strcpy(szFileNameState, " ");						break;
		case MOSS_READY		: strcpy(szFileNameState, "char_stat_ready.tga");	break;
		case MOSS_SHOP		: strcpy(szFileNameState, "char_stat_shop.tga");	break;
		case MOSS_EQUIPMENT	: strcpy(szFileNameState, "char_stat_equip.tga");	break;
		}

		pBitmap = CCBitmapManager::Get(szFileName);
		pBitmapState = CCBitmapManager::Get(szFileNameState);

		pItem->m_pBitmap = pBitmap;
		pItem->m_pBitmapState = pBitmapState;
		
		sprintf(szLevel,"Lv %2d",nLevel);
		strcpy(pItem->m_szLevel,szLevel);
		strcpy(pItem->m_szName,szName);
	}
}

void ZStagePlayerListBox::AddPlayer(CCMatchObjCache* pCache)
{
	if(!pCache) return;

	AddPlayer(pCache->GetUID(), MOSS_NONREADY,pCache->GetName(), pCache->GetLevel(),false,CCMT_ALL);
}

void ZStagePlayerListBox::AddPlayer(CCUID& puid, CCMatchObjectStageState state, char* szName, int  nLevel ,bool isMaster,CCMatchTeam nTeam)
{
	char szFileName[64] = "";
	char szFileNameState[64] = "";
	char szLevel[64];

	sprintf(szLevel,"Lv %2d",nLevel);

	CCBitmap* pBitmap = NULL;
	CCBitmap* pBitmapState = NULL;

	if(isMaster) {
			 if(nTeam == CCMT_RED)	strcpy(szFileName, "stg_status_master_red.tga");	
		else if(nTeam == CCMT_BLUE)	strcpy(szFileName, "stg_status_master_blue.tga");	
		else 						strcpy(szFileName, "stg_status_master_normal.tga");	
	}
	else {
		if(nTeam == CCMT_RED)		strcpy(szFileName, "stg_status_member_red.tga");	
		else if(nTeam == CCMT_BLUE)	strcpy(szFileName, "stg_status_member_blue.tga");	
		else						strcpy(szFileName, " ");	
	}

	switch (state) {

		case MOSS_NONREADY	: strcpy(szFileNameState, " ");						break;
		case MOSS_READY		: strcpy(szFileNameState, "char_stat_ready.tga");	break;
		case MOSS_SHOP		: strcpy(szFileNameState, "char_stat_shop.tga");	break;
		case MOSS_EQUIPMENT	: strcpy(szFileNameState, "char_stat_equip.tga");	break;
	}

	pBitmap = CCBitmapManager::Get(szFileName);
	pBitmapState = CCBitmapManager::Get(szFileNameState);

//	CCListBox::Add(new ZLobbyPlayerListItem(puid, pBitmap, szName, szLevel));
	CCListBox::Add(new ZStagePlayerListItem(puid, pBitmap, pBitmapState, szName, szLevel));
}

bool ZStagePlayerListBox::OnEvent(CCEvent* pEvent, CCListener* pListener)
{
	if(pEvent->nMessage==MWM_MOUSEMOVE)	{
	}
	else if(pEvent->nMessage==MWM_CCButtonDOWN) {
		int k=0;
	}

	return CCListBox::OnEvent(pEvent, pListener);
}
*/
/*
void ZStagePlayerListBox::AddPlayer( CCMatchObjCache* pCache )
{
	CCUID uid = pCache->GetUID();

	sPlayerInfo* pInfo = new sPlayerInfo;
	strcpy( pInfo->szName, pCache->GetName() );
	pInfo->Level	=	pCache->GetLevel();

	CCListBox::Add(new ZLobbyPlayerListItem(uidItem, pIconBitmap, szName, szWeight, szSlot, szPrice));
}
*/

CCUID ZPlayerListBox::GetSelectedPlayerUID() 
{
	ZLobbyPlayerListItem* pItem = (ZLobbyPlayerListItem*)GetSelItem();
	if(!pItem) return CCUID(0,0);
    
	return pItem->GetUID();
}

void ZPlayerListBox::SelectPlayer(CCUID uid)
{
	for(int i=0;i<GetCount();i++)
	{
		ZLobbyPlayerListItem* pItem = (ZLobbyPlayerListItem*)Get(i);
		if(pItem->GetUID()==uid){
			SetSelIndex(i);
			return;
		}
	}
}

void ZPlayerListBox::MultiplySize( float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight )
{
	CCListBox::MultiplySize(byIDLWidth, byIDLHeight, byCurrWidth, byCurrHeight);

	OnSize(GetRect().w, GetRect().h);	// onsize안에서 커스텀하게 처리하는 부분들을 실행시키자
}