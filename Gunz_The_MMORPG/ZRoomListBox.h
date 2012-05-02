#pragma once
#include "mwidget.h"
#include "map"
#include "CCMatchRule.h"

using namespace std;

class MButton;
//class MScrollBar;
//class MStageListItem;


#define SMI_MAPNAME_LENGTH		128
#define SMI_ROOMNAME_LENGTH		128
#define	NUM_DISPLAY_ROOM		8

enum GameState
{
	GAME_PLAYING = 0,
	GAME_WAITING,
	GMAE_CLOSED,
	GAME_STATE_NUM,
};

struct sMapInfo
{
	bool		IsEmpty;
	int			RoomNumber;
	CCUID		uidStage;
	char		map_name[SMI_MAPNAME_LENGTH];
	char		room_name[SMI_ROOMNAME_LENGTH];
	int			nPeople;
	int			nMaxPeople;
	bool		bForcedEnter;
	bool		bLimitLevel;
	int			nMasterLevel;
	int			nLimitLevel;
	bool		bPrivate;
	GameState	roomState;
	CCMATCH_GAMETYPE		nGame_Type;
	
	sMapInfo()
	{
		uidStage			= CCUID(0,0);
		IsEmpty			= true;
		bForcedEnter	= false;
		bPrivate		= false;
	}
};
class ZRoomListBox :	public CCWidget
{
protected:
	int						m_nPrevStageCount;
	int						m_nNextStageCount;

	int						m_iNumRoom;
	float					m_RoomWidth;
	float					m_RoomHeight;
	sMapInfo				m_pMapInfo[NUM_DISPLAY_ROOM];
	map<string, CCBitmap*>	m_pMapImage;
	map<CCMATCH_GAMETYPE, CCBitmap*>	m_pIconImage;
	CCBitmap*				m_pRoomFrame;

	int						m_Selection;
	int						m_currPage;

	int						m_iGapWidth;
	int						m_iGapHeight;
	int						m_iGapCenter;

	//MScrollBar*		m_pScrollBar;

	CCUID					m_uidSelectedPrivateStageUID;
protected:
	virtual void	OnDraw( CCDrawContext* pDC );
	virtual bool	OnShow( void );
	virtual bool	OnCommand( CCWidget* pWidget, const char* szMassage );
	virtual bool	OnEvent(CCEvent* pEvent, CCListener* pListener);
protected:
	CCUID GetSelRoomUID();
	CCUID GetSelectedPrivateStageUID();
public:
	void	SetBannerImage(char* pBannerName, CCBitmap* pBitmap);
	void	SetIconImage(CCMATCH_GAMETYPE type, CCBitmap* pBitmap);
	void	SetFrameImage(CCBitmap* pBitmap){m_pRoomFrame = pBitmap;};
	void	SetWidth( float width ) { m_RoomWidth	= width; }
	void	SetHeight( float height ) { m_RoomHeight	= height; }
	void	SetRoomName( int i, char* pRoomName, sMapInfo* info );
	void	Resize( float w, float h );
	

	void Clear();
	void SetScroll(int nPrevStageCount, int nNextStageCount);

	struct _RoomInfoArg
	{
		int nIndex;
		int nRoomNumber;
		CCUID uidStage;
        char* szRoomName;
		char* szMapName;
		int nMaxPlayers;
		int nCurrPlayers;
		bool bPrivate;
		bool bForcedEntry;
        bool bLimitLevel;
		int nMasterLevel;
		int nLimitLevel;
        CCMATCH_GAMETYPE nGameType;
		STAGE_STATE nStageState;
	};
	void SetRoom(const _RoomInfoArg* pRoomInfo);
	void SetEmptyRoom(int nIndex);
	void SetPage();
	int GetFirstStageCursor();
	int GetLastStageCursor();
	void RequestSelStageJoin();
	void RequestSelPrivateStageJoin();
	const sMapInfo* GetSelMapInfo();
	void SetPrivateStageUID(CCUID& uidStage);
public:
	ZRoomListBox(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZRoomListBox(void);

};