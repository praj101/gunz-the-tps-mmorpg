#include "stdafx.h"

#include "ZGameClient.h"
#include "zroomlistbox.h"
#include "CCButton.h"
#include "CCBitmap.h"
#include "RTypes.h"
#include "CCDrawContext.h"
#include "CCDebug.h"
#include "CCScrollBar.h"
#include "ZApplication.h"
#include "ZPost.h"

// Added R347a
#include "ZGameInterface.h"
// Added R350a
#include "CCEdit.h"

#define		ROOM_WIDTH					8
#define		ROOM_HEIGHT					7
const char* EMPTY_ROOM		=		"Empty";

template < class F, class S>
class string_key_equal
{
private:
	F string_key;
public:
	string_key_equal( const F& str ) : string_key(str) {}
	bool operator() (pair<const F, S> elem)
	{		return ( stricmp(elem.first.c_str(), string_key.c_str())== 0);		}
};

ZRoomListBox::ZRoomListBox(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCWidget(szName, pParent, pListener)
{
	m_nPrevStageCount = m_nNextStageCount = -1;
	//m_pMapInfo			= new sMapInfo[NUM_DISPLAY_ROOM];
	m_iNumRoom		= 0;
	m_RoomWidth		= 0;
	m_RoomHeight	= 0;
	m_pRoomFrame	= 0;
	m_Selection		= -1;
	m_currPage		=	0;
	m_iGapWidth		= ROOM_WIDTH;
	m_iGapHeight	= ROOM_HEIGHT;
	m_iGapCenter	= 10;
	m_uidSelectedPrivateStageUID	= CCUID(0,0);
}

ZRoomListBox::~ZRoomListBox()
{
	//SAFE_DELETE_ARRAY(m_pMapInfo);
	for( map<string, CCBitmap*>::iterator	iter = m_pMapImage.begin(); iter != m_pMapImage.end(); )
	{
		m_pMapImage.erase(iter++);
	}
}

void ZRoomListBox::OnDraw( CCDrawContext* pDC )
{
	CCBitmap* pBitmap;
	sRect		rect;
	map<string, CCBitmap*>::iterator iter;
	map<CCMATCH_GAMETYPE, CCBitmap*>::iterator iterIcon;
	int pressed_reposition = 0;

 	int index = 0;
	
	for( int i = 0; i < NUM_DISPLAY_ROOM; ++i )
	{
		bool	bRoomFull = false;

		if( m_Selection == i )			pressed_reposition = 1; 
		else							pressed_reposition = 0;

		const char*	mapName = CCGetMapDescMgr()->GetBannerName( m_pMapInfo[i].map_name);
		if( m_pMapInfo[i].IsEmpty )
		{
			continue;
		}
		rect = GetInitialClientRect();
		
		int width  = (int)( m_iGapWidth + ( m_RoomWidth + m_iGapWidth*2 + m_iGapCenter ) * ( index%2 ) );
		int height = (int)( m_iGapHeight + ( m_RoomHeight + m_iGapHeight ) * (int)(index*0.5f));
 		
 		if( m_pMapInfo[i].nPeople >= m_pMapInfo[i].nMaxPeople )
		{
			bRoomFull = true;
		}
		
		iter = find_if( m_pMapImage.begin(), m_pMapImage.end(), string_key_equal<string, CCBitmap*>(mapName));
		if( iter != m_pMapImage.end() )
		{
			pBitmap	= iter->second;
			if( pBitmap != 0 )
			{
				if( ( m_pMapInfo[i].roomState == GMAE_CLOSED ) || bRoomFull )	
 					pDC->SetBitmapColor(125,125,125,255);
				else					
 					pDC->SetBitmapColor(255,255,255,255);

				pDC->SetBitmap( pBitmap ); 
				pDC->Draw( width + pressed_reposition, height + pressed_reposition, m_RoomWidth, m_RoomHeight );
			}

			if(m_pRoomFrame!=0)
			{
				pDC->SetBitmap(m_pRoomFrame);
 				pDC->Draw(width + pressed_reposition, height + pressed_reposition, m_RoomWidth * 0.75, m_RoomHeight, 0, 0, 512, 32 );
			}
		}


		// infomation 그리기
		char szBuf[128];
		sRect r;

		// 방번호
 		r.x = width + m_RoomWidth*0.01f	+ pressed_reposition;
		r.y = height + m_RoomHeight*0.1f + pressed_reposition;
		r.w = m_RoomWidth*0.1;
		r.h = m_RoomHeight *0.5;
     	sprintf(szBuf,"%03d", m_pMapInfo[i].RoomNumber);

		pDC->SetFont( CCFontManager::Get("FONTc8b") );
		pDC->SetColor( 0,0,0);
		pDC->Text( sRect( r.x+1, r.y+1, r.w, r.h), szBuf);
		if(  m_pMapInfo[i].roomState == GMAE_CLOSED || bRoomFull )
			pDC->SetColor( 115,146,173 );
		else
			pDC->SetColor( 181, 247, 66 );
		pDC->Text( r, szBuf );

		// 인원수
		r.x = width + m_RoomWidth*0.01f + pressed_reposition;
		r.y = height + m_RoomHeight*0.5f + pressed_reposition;
		r.w = m_RoomWidth*0.1f;
		r.h = m_RoomHeight*0.5f;
		sprintf(szBuf,"%d/%d", m_pMapInfo[i].nPeople, m_pMapInfo[i].nMaxPeople );

		pDC->SetColor( 0,0,0);
		pDC->Text( sRect( r.x+1, r.y+1, r.w, r.h), szBuf);
		if(  m_pMapInfo[i].roomState == GMAE_CLOSED || bRoomFull )
			pDC->SetColor( 115,146,173 );
		else
			pDC->SetColor( 181, 247, 66 );
		pDC->Text( r, szBuf );


		// 방 이름
		pDC->SetFont( CCFontManager::Get("FONTc8b") );
 		r.x = width + m_RoomWidth*0.12 + pressed_reposition;
 		r.y = height /*+ m_RoomHeight*0.3*/ + pressed_reposition;
  		r.w = m_RoomWidth*0.75;
		r.h = m_RoomHeight/**0.85*/;
 		
		CCFont * pFont  = pDC->GetFont();
		char szBufTemp[SMI_MAPNAME_LENGTH];
		int strLength = 0;
		int RoomWidth = pFont->GetWidth(m_pMapInfo[i].room_name);
		if( RoomWidth > m_RoomWidth*0.7 )
		{
			while( strLength < 29 )
			{
				if( m_pMapInfo[i].map_name[strLength] == '0' ) // 이름의 끝
					strcpy( szBufTemp, m_pMapInfo[i].room_name );
				if( ((unsigned char)m_pMapInfo[i].room_name[strLength]) > 127 )
					strLength += 2;
				else
					++strLength;
			}
			strncpy( szBufTemp, m_pMapInfo[i].room_name, strLength*sizeof(char) );
			szBufTemp[strLength] = '\0';
			strcat( szBufTemp, "..."	);

			pDC->SetColor( 0,0,0);
			pDC->Text( sRect( r.x+1, r.y+1, r.w, r.h), szBufTemp, CCD_LEFT);
			if(  m_pMapInfo[i].roomState == GMAE_CLOSED || bRoomFull )
				pDC->SetColor( 115,146,173 );
			else
				pDC->SetColor( 255, 255, 255 );
			pDC->Text(r, szBufTemp, CCD_LEFT );
		}
		else
		{
			pDC->SetColor( 0,0,0);
			pDC->Text( sRect( r.x+1, r.y+1, r.w, r.h), m_pMapInfo[i].room_name, CCD_LEFT);
			if(  m_pMapInfo[i].roomState == GMAE_CLOSED || bRoomFull )
				pDC->SetColor( 115,146,173 );
			else
				pDC->SetColor( 255, 255, 255 );
			pDC->Text(r, m_pMapInfo[i].room_name, CCD_LEFT );
		}

		
		//레벨제한
		r.x = width + m_RoomWidth*0.75f + pressed_reposition;
		r.y = height + pressed_reposition;
		r.w = m_RoomWidth * 0.2f ;
		r.h = m_RoomHeight;

		if( m_pMapInfo[i].bLimitLevel )
		{
			char szBufTemp[64];
			sprintf( szBufTemp, "%d~%d", max(m_pMapInfo[i].nMasterLevel - m_pMapInfo[i].nLimitLevel,1), m_pMapInfo[i].nMasterLevel + m_pMapInfo[i].nLimitLevel);

			pDC->SetColor( 0,0,0);
			pDC->Text( sRect( r.x+1, r.y+1, r.w, r.h), szBufTemp);
			if( m_pMapInfo[i].roomState == GMAE_CLOSED || bRoomFull )
				pDC->SetColor( 100, 100, 100 );
			else
				pDC->SetColor( 181, 247, 66 );
			pDC->Text( r, szBufTemp );
		}
		
		if (m_pMapInfo[i].bPrivate)
		{
			// 비밀방이면 게임모드 아이콘대신 열쇠아이콘이 나온다.
			// 열쇠아이콘은 하드코드됨. 나중에 일반화해서 xml로 빼놔야할 듯..

			#define FN_ROOMLIST_PRIVATE_KEY		"in_key.png"
			pBitmap = CCBitmapManager::Get(FN_ROOMLIST_PRIVATE_KEY);
			if (pBitmap != NULL) 
			{
				float x, y;
				x = width + m_RoomWidth*0.9 + pressed_reposition;
				y = height + pressed_reposition + ((m_RoomHeight-pBitmap->GetHeight())/2);
				pDC->SetBitmap( pBitmap );
				pDC->Draw(x, y, pBitmap->GetWidth(), pBitmap->GetHeight());
			}
		}
		else
		{
			iterIcon = m_pIconImage.find( m_pMapInfo[i].nGame_Type );
			if( iterIcon != m_pIconImage.end() )
			{
				pBitmap = iterIcon->second;
				if( pBitmap != 0)
				{
 					r.x = width + m_RoomWidth*0.9 + pressed_reposition;
					r.y = height + m_RoomHeight/4.7 + pressed_reposition;

					// 아이콘
					if( m_pMapInfo[i].roomState == GMAE_CLOSED || bRoomFull )
					{
						pDC->SetBitmapColor(100,100,100,100);
					}
					else
					{
  						pDC->SetBitmapColor(255,255,255,255);
					}

					pDC->SetBitmap( pBitmap );
					pDC->Draw(r.x, height + pressed_reposition, m_RoomHeight, m_RoomHeight);
				}			
			}
		}


		// 플레이 중이면 플레이 아이콘을 표시한다.
		if ( m_pMapInfo[i].roomState == GAME_PLAYING)
		{
			// 역시 플레이 아이콘도 하드코딩... -_-;
			#define FN_ROOMLIST_PLAYICON		"icon_play.tga"
			pBitmap = CCBitmapManager::Get( FN_ROOMLIST_PLAYICON);
			if (pBitmap != NULL) 
			{
				float x, y;
				x = width  + (m_RoomWidth  * 0.955) + pressed_reposition;
				y = height + (m_RoomHeight * 0.54)  + pressed_reposition;
				pDC->SetBitmap( pBitmap );
				pDC->Draw(x, y, (m_RoomHeight * 0.5), (m_RoomHeight * 0.5));
			}
		}

		//왓구(테두리)
		if( i == m_Selection)
		{
			if( m_pMapInfo[i].roomState == GMAE_CLOSED || bRoomFull) pDC->SetColor(115,146,173);
			else pDC->SetColor( 181, 247, 66 );
		}
		else pDC->SetColor(128,128,128,255);
		pDC->Rectangle(width+pressed_reposition, height+pressed_reposition, m_RoomWidth, m_RoomHeight );

		++index;
	}
	pDC->SetOpacity( 255 );
	pDC->SetBitmapColor(255,255,255,255);
}

bool ZRoomListBox::OnShow()
{
	return true;
}

bool ZRoomListBox::OnCommand( CCWidget* pWidget, const char* szMassage )
{
	SetPage();
	return true;
}

bool ZRoomListBox::OnEvent( CCEvent* pEvent, CCListener* pListener )
{
	sRect r = GetInitialClientRect();

	sPoint ClickPos = pEvent->sPos;

	sRect	rect;

	switch(pEvent->iMessage)
	{
		if(r.InPoint(ClickPos)==true)
		{
			case CCWM_LBUTTONDOWN:
			{	
				for( int i = 0 ; i < NUM_DISPLAY_ROOM; ++i )
				{
					int width = (int)((m_RoomWidth + ROOM_WIDTH)*(i%2)+ ROOM_WIDTH);
					int height = (int)((m_RoomHeight + ROOM_HEIGHT)*((int)i/2) + ROOM_HEIGHT);
					rect.x = width;
					rect.y = height;
					rect.w = m_RoomWidth;
					rect.h = m_RoomHeight;
					if(	rect.InPoint( ClickPos ) )
					{
						if( m_pMapInfo[i].IsEmpty )
							continue;
						m_Selection	= i;
						SetPrivateStageUID(CCUID(0,0));

						return true;
					}
					m_Selection = -1;
				}
				return true;
			}
			case CCWM_LBUTTONDBLCLK:
			{
				for( int i = 0 ; i < NUM_DISPLAY_ROOM; ++i )
				{
					int width = (int)((m_RoomWidth + ROOM_WIDTH)*(i%2)+ ROOM_WIDTH);
					int height = (int)((m_RoomHeight + ROOM_HEIGHT)*((int)i/2) + ROOM_HEIGHT);
					rect.x = width;
					rect.y = height;
					rect.w = m_RoomWidth;
					rect.h = m_RoomHeight;
					if( rect.InPoint( ClickPos ) )
					{
						if( m_pMapInfo[i].IsEmpty )
							continue;
						m_Selection	= i;
						SetPrivateStageUID(CCUID(0,0));

						RequestSelStageJoin();
						return true;
					}
					m_Selection = -1;
				}
				return true;
			}
		}
	}
	return false;
}

void ZRoomListBox::SetRoomName( int i, char* pRoomName, sMapInfo* info )
{
	strcpy( m_pMapInfo[i].map_name,  info->map_name );
	strcpy( m_pMapInfo[i].room_name,  info->room_name );
	m_pMapInfo[i].nPeople		= info->nPeople;
	m_pMapInfo[i].RoomNumber	= info->RoomNumber;
	m_pMapInfo[i].nMaxPeople	= info->nMaxPeople;
	m_pMapInfo[i].roomState		= info->roomState;
}

void ZRoomListBox::SetBannerImage( char* pBannerName, CCBitmap* pBitmap )
{
	m_pMapImage.insert( map<string, CCBitmap*>::value_type( string(pBannerName), pBitmap) );
}
void ZRoomListBox::SetIconImage( CCMATCH_GAMETYPE type, CCBitmap* pBitmap )
{
	m_pIconImage.insert( map<CCMATCH_GAMETYPE, CCBitmap*>::value_type( type, pBitmap ) );
}

void ZRoomListBox::SetRoom(const _RoomInfoArg* pRoomInfo)
{
	int nIndex = pRoomInfo->nIndex; 

	if ((pRoomInfo->nIndex < 0) || (pRoomInfo->nIndex >= NUM_DISPLAY_ROOM)) return;

	m_pMapInfo[nIndex].IsEmpty = false;
	m_pMapInfo[nIndex].uidStage = pRoomInfo->uidStage;
	
	strcpy(m_pMapInfo[nIndex].map_name, pRoomInfo->szMapName);
	strcpy(m_pMapInfo[nIndex].room_name, pRoomInfo->szRoomName);
	m_pMapInfo[nIndex].nMaxPeople = pRoomInfo->nMaxPlayers;
	m_pMapInfo[nIndex].nPeople = pRoomInfo->nCurrPlayers;
	m_pMapInfo[nIndex].RoomNumber = pRoomInfo->nRoomNumber;
	m_pMapInfo[nIndex].nGame_Type	= pRoomInfo->nGameType;
	m_pMapInfo[nIndex].bForcedEnter = pRoomInfo->bForcedEntry;
	m_pMapInfo[nIndex].bLimitLevel	=	pRoomInfo->bLimitLevel;
	m_pMapInfo[nIndex].nMasterLevel = pRoomInfo->nMasterLevel;
	m_pMapInfo[nIndex].nLimitLevel = pRoomInfo->nLimitLevel;
	m_pMapInfo[nIndex].bPrivate = pRoomInfo->bPrivate;

 	if( pRoomInfo->nStageState == STAGE_STATE_STANDBY )
	{
		m_pMapInfo[nIndex].roomState = GAME_WAITING; // 대기중
	}
	//else if(	pRoomInfo->nStageState == STAGE_STATE_CLOSE && !pRoomInfo->bForcedEntry )
	//{
	//	m_pMapInfo[nIndex].roomState	= GMAE_CLOSED; // 난입 불가
	//}
	else if( pRoomInfo->nStageState == STAGE_STATE_RUN && pRoomInfo->bForcedEntry)
	{
		//m_pMapInfo[nIndex].roomState	= GAME_WAITING;
		m_pMapInfo[nIndex].roomState	= GAME_PLAYING	; // 난입 가능
	}
	else
	{
		m_pMapInfo[nIndex].roomState	= GMAE_CLOSED; // 난입 불가
	}

}

void ZRoomListBox::SetEmptyRoom(int nIndex)
{
	m_pMapInfo[nIndex].IsEmpty = true;
	m_pMapInfo[nIndex].uidStage = CCUID(0,0);
	m_pMapInfo[nIndex].bPrivate = false;
}


void ZRoomListBox::Clear()
{
	m_iNumRoom = 0;
	for (int i = 0; i < NUM_DISPLAY_ROOM; i++)
	{
		SetEmptyRoom(i);
	}
}

void ZRoomListBox::SetPage( )
{
	// 서버에 필요한 페이지를 요청한다
	//static int oldvalue = 0;
 //	int value = m_pScrollBar->GetValue();
 //	if( abs(oldvalue -value) == 1 )
	//{
	//	if( oldvalue > value )
	//		value -= 19;
	//	else
	//		value += 19;
	//	value = min( max( value, 0 ), 999 );
	//	m_pScrollBar->SetValue( value );
	//}
	//oldvalue = m_pScrollBar->GetValue();
	//int i =min(max((int)m_pScrollBar->GetValue()/100, 0),9);
	//if( i == m_currPage ) return;
	//
	//m_currPage = i;
	//cclog("%d 페이지 요청\n",i);
}

void ZRoomListBox::SetScroll(int nPrevStageCount, int nNextStageCount)
{
	m_nNextStageCount = nNextStageCount;
	m_nPrevStageCount = nPrevStageCount;
}

int ZRoomListBox::GetFirstStageCursor()
{
	for (int i = 0; i < NUM_DISPLAY_ROOM; i++)
	{
		if (m_pMapInfo[i].IsEmpty == false)
		{
			return m_pMapInfo[i].RoomNumber - 1;
			break;
		}
	}

	return 0;
}

int ZRoomListBox::GetLastStageCursor()
{
	for (int i = NUM_DISPLAY_ROOM-1; i >= 0; i--)
	{
		if (m_pMapInfo[i].IsEmpty == false)
		{
			return m_pMapInfo[i].RoomNumber - 1;
			break;
		}
	}

	return 0;
}

CCUID ZRoomListBox::GetSelRooCCUID()
{
	if ((m_Selection >= 0) && (m_Selection < NUM_DISPLAY_ROOM))
	{
		return m_pMapInfo[m_Selection].uidStage;
	}

	return CCUID(0,0);
}

CCUID ZRoomListBox::GetSelectedPrivateStageUID()
{
	return m_uidSelectedPrivateStageUID;
}

#include "ZMyInfo.h"
void ZRoomListBox::RequestSelStageJoin()
{
	CCUID uidChar = ZGetGameClient()->GetPlayerUID();

	if (GetSelRooCCUID() != CCUID(0,0))
	{
		if ( (ZGetMyInfo()->IsAdminGrade() == false) && (m_pMapInfo[m_Selection].bPrivate) )
		{
			char szStageName[SMI_ROOMNAME_LENGTH] = "";
			CCUID uidStage = CCUID(0,0);

			const sMapInfo* pSelRoomInfo = GetSelMapInfo();

			if (pSelRoomInfo != NULL)
			{
				strcpy(szStageName, pSelRoomInfo->room_name);
				uidStage = pSelRoomInfo->uidStage;
			}

			SetPrivateStageUID(uidStage);

			// 비밀방
			ZApplication::GetGameInterface()->ShowPrivateStageJoinFrame(szStageName);
		}
		else
		{
			ZPostRequestStageJoin(uidChar, GetSelRooCCUID());	// 공개방
			ZApplication::GetGameInterface()->EnableLobbyInterface(false);
		}
	}
}

void ZRoomListBox::RequestSelPrivateStageJoin()
{
	CCUID uidChar = ZGetGameClient()->GetPlayerUID();
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	CCUID uidStageUID = GetSelectedPrivateStageUID();

	if (uidStageUID == CCUID(0,0)) uidStageUID = GetSelRooCCUID();

	if (uidStageUID != CCUID(0,0))
	{
		CCEdit* pPassEdit = (CCEdit*)pResource->FindWidget("PrivateStagePassword");
		if (pPassEdit!=NULL)
		{
			char szPassword[256];
			strcpy(szPassword, pPassEdit->GetText());

			ZPostRequestPrivateStageJoin(uidChar, uidStageUID, szPassword);
			ZApplication::GetGameInterface()->EnableLobbyInterface(false);
		}
	}
}

void ZRoomListBox::Resize( float w, float h )
{
	m_RoomWidth	*= w;
	m_RoomHeight	*= h;
	//jintriple3 룸 리스트 해상도에 따라 위치가 살짝 변경되는 버그...
	float fGapCenter = (float)m_iGapCenter;
	float fGapWidth=  (float)m_iGapWidth;
	float fGapHeight = (float)m_iGapHeight;
	fGapCenter *= w;
	fGapWidth *= w;
	fGapHeight *=h;

	m_iGapCenter = (int)fGapCenter;
	if(fGapCenter - m_iGapCenter > 0.5f)
		m_iGapCenter++;

	m_iGapWidth = (int)fGapWidth;
	if(fGapWidth - m_iGapWidth > 0.5f)
		m_iGapWidth++;

	m_iGapHeight = (int)fGapHeight;
	if(fGapHeight - m_iGapHeight > 0.5f)
		m_iGapHeight++;

//	m_iGapCenter	*= w;
//	m_iGapWidth	*= w;
//	m_iGapHeight	*= h;
}

const sMapInfo* ZRoomListBox::GetSelMapInfo()
{ 
	if ((m_Selection >= 0) && (m_Selection < NUM_DISPLAY_ROOM)) return &m_pMapInfo[m_Selection];
	return NULL;
}


void ZRoomListBox::SetPrivateStageUID(CCUID& uidStage)
{
	m_uidSelectedPrivateStageUID = uidStage;
}