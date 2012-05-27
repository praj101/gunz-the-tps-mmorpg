#pragma once
#include "CCListBox.h"
#include "map"
#include "vector"
#include "CCUID.h"
#include "mmatchobject.h"
#include "ZEmblemInterface.h"
#include "ZApplication.h"

using namespace std;

class CCBitmap;
class CCScrollBar;

enum ePlayerState
{
	PS_LOGOUT = 0,
	PS_FIGHT,
	PS_WAIT,
	PS_LOBBY,
	PS_END,
};

/*
struct sPlayerInfo
{
	int Level;
	char szName[128];
	int state;
};
*/

class ZPlayerListItem : public CCListItem {
public:
	ZPlayerListItem() {
		m_PlayerUID = CCUID(0,0);
		m_Grade = CCMUGFREE;
		m_Color = sColor(0xFFCDCDCD);
	}

	void SetColor(sColor c) {
		m_Color = c;
	}

	const sColor GetColor() { 
		return m_Color; 
	}

public:

	CCUID				m_PlayerUID;
	CCMatchUserGradeID	m_Grade;		//이건 안쓰는 것 같다
	sColor				m_Color;


	char			m_szName[MATCHOBJECT_NAME_LENGTH];
	char			m_szClanName[CLAN_NAME_LENGTH];
	char			m_szLevel[128];

};

class ZLobbyPlayerListItem : public ZPlayerListItem{
protected:
	CCBitmap* m_pBitmap;
	CCBitmap* m_pBmpDTGradeIcon;
//	CCBitmap* m_pBitmapEmblem;
	unsigned int m_nClanID;

public:
	ePlayerState	m_nLobbyPlayerState;

public:
	ZLobbyPlayerListItem(const CCUID& puid, CCBitmap* pBitmap, unsigned int nClanID, const char* szLevel, const char* szName, 
		const char *szClanName, ePlayerState nLobbyPlayerState,CCMatchUserGradeID Grade, CCBitmap* pBmpDTGradeIcon)
	{
		m_pBitmap = pBitmap;
		m_pBmpDTGradeIcon = pBmpDTGradeIcon;
		m_nClanID = nClanID;
		ZGetEmblemInterface()->AddClanInfo(m_nClanID);
//		m_pBitmapEmblem = pBitmapEmblem;
		m_PlayerUID = puid;
		m_nLobbyPlayerState = nLobbyPlayerState;
		if (szLevel) strcpy(m_szLevel, szLevel);
		else m_szLevel[0] = NULL;
		if (szName) strcpy(m_szName, szName);
		else m_szName[0] = NULL;
		if(szClanName) strcpy(m_szClanName, szClanName);
		else m_szClanName[0] = NULL;
		m_Grade = Grade;
	}

	virtual ~ZLobbyPlayerListItem() {
		ZGetEmblemInterface()->DeleteClanInfo(m_nClanID);
	}

	ZLobbyPlayerListItem()
	{
		m_pBitmap = NULL;
		m_pBmpDTGradeIcon = NULL;
		m_nClanID = 0;
//		m_pBitmapEmblem = NULL;
		m_PlayerUID = CCUID(0,0);
		m_nLobbyPlayerState = (ePlayerState)0;
		m_szLevel[0] = 0;
		m_szName[0] = 0;
		m_szClanName[0] = 0;
		m_Grade = CCMUGFREE;
	}

	virtual const char* GetString()
	{
		return m_szName;
	}

	virtual const char* GetString(int i)
	{
		if(i==1) return m_szLevel;
		else if(i==3) return m_szName;
		else if(i==5) return m_szClanName;
		return NULL;
	}

	virtual CCBitmap* GetBitmap(int i)
	{
		if (i == 0) return m_pBitmap;
		else if (i == 2)
		{
			return m_pBmpDTGradeIcon;
		}
		else if (i == 4)
		{
			if ( strcmp( m_szClanName, "") == 0)
				return NULL;
			else
				return ZGetEmblemInterface()->GetClanEmblem(m_nClanID);
		}
		return NULL;
	}

	CCUID& GetUID() { return m_PlayerUID; }
};

class ZFriendPlayerListItem : public ZPlayerListItem{
protected:
	char		m_szLocation[128];
	CCBitmap*	m_pBitmap;

public:
	ePlayerState	m_nLobbyPlayerState;

public:
	ZFriendPlayerListItem(const CCUID& puid, CCBitmap* pBitmap, const char* szName, const char* szClanName,const char* szLocation, 
		ePlayerState nLobbyPlayerState,CCMatchUserGradeID Grade)
	{
		m_pBitmap = pBitmap;
		m_PlayerUID = puid;
		m_nLobbyPlayerState = nLobbyPlayerState;
		if (szName) strcpy(m_szName, szName);
		else m_szName[0] = NULL;
		if(szClanName) strcpy(m_szClanName, szClanName);
		else m_szClanName[0] = NULL;

		m_szLevel[0] = NULL;

		if (szLocation)
			strcpy(m_szLocation, szLocation);
		else
			m_szLocation[0] = NULL;

		m_Grade = Grade;
	}

	ZFriendPlayerListItem()
	{
		m_pBitmap = NULL;
		m_PlayerUID = CCUID(0,0);
		m_nLobbyPlayerState = (ePlayerState)0;
		m_szName[0] = PS_END;
		m_szLevel[0] = NULL;
		m_szLocation[0] = NULL;

		m_Grade = CCMUGFREE;
	}

	virtual const char* GetString()
	{
		return m_szName;
	}

	virtual const char* GetString(int i)
	{
		if(i==1) return m_szName;
		//else if(i==2) return m_szLocation;
		return NULL;
	}

	virtual CCBitmap* GetBitmap(int i)
	{
		if (i == 0) return m_pBitmap;
		return NULL;
	}

	CCUID& GetUID() { return m_PlayerUID; }
	const char* GetLocation() { return m_szLocation; }
};

class ZClanPlayerListItem : public ZPlayerListItem{
protected:
	CCBitmap* m_pBitmap;
	CCMatchClanGrade	m_ClanGrade;

public:
	ePlayerState	m_nLobbyPlayerState;

public:
	ZClanPlayerListItem(const CCUID& puid, CCBitmap* pBitmap, const char* szName, const char* szClanName,const char* szLevel, 
		ePlayerState nLobbyPlayerState,CCMatchClanGrade clanGrade)
	{
		m_pBitmap = pBitmap;
		m_PlayerUID = puid;
		m_nLobbyPlayerState = nLobbyPlayerState;
		if (szName)
			strcpy(m_szName, szName);
		else
			m_szName[0] = NULL;
		if (szLevel) strcpy(m_szLevel, szLevel);
		else m_szLevel[0] = NULL;
		if(szClanName) strcpy(m_szClanName, szClanName);
		else m_szClanName[0] = NULL;

		m_ClanGrade = clanGrade;
		m_Color = sColor(0xFFCDCDCD);
	}

	ZClanPlayerListItem()
	{
		m_pBitmap = NULL;
		m_PlayerUID = CCUID(0,0);
		m_nLobbyPlayerState = (ePlayerState)0;
		m_szName[0] = PS_END;
		m_szClanName[0] = PS_END;
		m_szLevel[0] = 0;
		m_Grade = CCMUGFREE;
		m_ClanGrade = CCG_NONE;
		m_Color = sColor(0xFFCDCDCD);
	}

	virtual const char* GetString()
	{
		return m_szName;
	}

	virtual const char* GetString(int i)
	{
		if(i==1) return m_szName;
		else if(i==3) return m_szClanName;
		return NULL;
	}

	virtual CCBitmap* GetBitmap(int i)
	{
		if (i == 0) return m_pBitmap;
		return NULL;
	}

	CCUID& GetUID() { return m_PlayerUID; }
};

enum eStagePlayerState
{
	SPS_NONE = 0,
	SPS_SHOP,
	SPS_EQUIP,
	SPS_READY,
	SPS_END
};

struct sStagePlayerInfo
{
	int Level;
	char szName[128];
	int state;
	bool isMaster;
	int	nTeam;
};


class ZStagePlayerListItem : public ZPlayerListItem{
public:
	int		m_nTeam;
	bool	m_bEnableObserver;			// 옵져버인지 아닌지... : 동환이가 추가
	unsigned int m_nClanID;

public:
	CCBitmap* m_pBitmap;
	CCBitmap* m_pBmpDTGradeIcon;
//	CCBitmap* m_pBitmapEmblem;

public:
	ZStagePlayerListItem(const CCUID& puid, CCBitmap* pBitmap, unsigned int nClanID, const char* szName, const char* szClanName, 
		const char* szLevel,CCMatchUserGradeID Grade, CCBitmap* pBmpDTGradeIcon)
	{
		m_pBitmap = pBitmap;
		m_pBmpDTGradeIcon = pBmpDTGradeIcon;
		m_nClanID = nClanID;
		ZGetEmblemInterface()->AddClanInfo(m_nClanID);
//		m_pBitmapEmblem = pBitmapEmblem;
		m_PlayerUID = puid;
		strcpy(m_szName, szName);
		if(szClanName) strcpy(m_szClanName, szClanName);
		else m_szClanName[0] = NULL;

		strcpy(m_szLevel, szLevel);
		m_nTeam = 0;
		m_Grade = Grade;
	}

	virtual ~ZStagePlayerListItem()
	{
		ZGetEmblemInterface()->DeleteClanInfo(m_nClanID);
	}

	ZStagePlayerListItem()
	{
		m_pBitmap = NULL;
		m_pBmpDTGradeIcon = NULL;
		m_nClanID = 0;
//		m_pBitmapEmblem = NULL;
		m_PlayerUID = CCUID(0,0);
		m_szName[0] = 0;
		m_szLevel[0] = 0;
		m_Grade = CCMUGFREE;
	}

	virtual const char* GetString()
	{
		return m_szName;
	}

	virtual const char* GetString(int i)
	{
		if(i==1) return m_szLevel;
		else if(i==3) return m_szName;
		else if(i==5) return m_szClanName;
		return NULL;
	}

	virtual CCBitmap* GetBitmap(int i)
	{
		if (i == 0) return m_pBitmap;
		else if (i == 2)
		{
			return m_pBmpDTGradeIcon;
		}
		else if (i == 4)
		{
			if ( strcmp( m_szClanName, "") == 0)
				return NULL;
			else
				return ZGetEmblemInterface()->GetClanEmblem(m_nClanID);
		}
		return NULL;
	}

	CCUID& GetUID() { return m_PlayerUID; }
};

class ZPlayerListBoxLook : public CCListBoxLook
{
public:
	virtual void OnItemDraw2(CCDrawContext* pDC, sRect& r, const char* szText, sColor color, bool bSelected, bool bFocus, int nAdjustWidth = 0);
	virtual void OnItemDraw2(CCDrawContext* pDC, sRect& r, CCBitmap* pBitmap, bool bSelected, bool bFocus, int nAdjustWidth);
	virtual void OnDraw(CCListBox* pListBox, CCDrawContext* pDC);

	virtual sRect GetClientRect(CCListBox* pListBox, sRect& r);
};

class ZPlayerListBox : public CCListBox
{
public:
	enum PLAYERLISTMODE {
		PLAYERLISTMODE_CHANNEL = 0,
		PLAYERLISTMODE_STAGE ,
		PLAYERLISTMODE_CHANNEL_FRIEND ,
		PLAYERLISTMODE_STAGE_FRIEND ,
		PLAYERLISTMODE_CHANNEL_CLAN ,
		PLAYERLISTMODE_STAGE_CLAN ,

		PLAYERLISTMODE_END,	// = count
	};

private:
//	CCBitmap*					m_pBitmap;
//	CCBitmap*					m_pBitmapIn;

	MBCCButton*					m_pButton;

//	map< CCUID, sPlayerInfo*>	mPlayers;
	vector<CCUID>				mPlayerOrder;

	int				mSelectedPlayer;
	int				mStartToDisplay;
	float			m_SlotWidth;
	float			m_SlotHeight;

	int				m_nOldW;
	PLAYERLISTMODE		m_nMode;


public:

	

protected:
	void SetupButton(const char *szOn, const char *szOff);

public:
	void InitUI(PLAYERLISTMODE nMode);
	void RefreshUI();

	PLAYERLISTMODE GetMode() { return m_nMode; }
	void SetMode(PLAYERLISTMODE mode);

//	void SetBitmap( CCBitmap* pBitmap );
//	CCBitmap* GetBitmap() {	return m_pBitmap; }
//	CCBitmap* GetBitmapIn() { return m_pBitmapIn; }

	// mode PLAYERLISTMODE_CHANNEL
	void AddPlayer(CCUID& puid, ePlayerState state, int  nLevel,char* szName, char* szClanName, unsigned int nClanID, CCMatchUserGradeID nGrade, int duelTournamentGrade );

	// mode PLAYERLISTMODE_STAGE
	void AddPlayer(CCUID& puid, CCMatchObjectStageState state, int nLevel, char* szName, char* szClanName, unsigned int nClanID, bool isMaster,CCMatchTeam nTeam, int duelTournamentGrade);

	// mode PLAYERLISTMODE_CHANNEL_FRIEND, PLAYERLISTMODE_STAGE_FRIEND
	void AddPlayer(ePlayerState state, char* szName, char* szLocation);

	// mode PLAYERLISTMODE_CHANNEL_CLAN
	void AddPlayer(CCUID& puid, ePlayerState state, char* szName, int  nLevel ,CCMatchClanGrade nGrade );

	void DelPlayer(CCUID& puid);
	void UpdatePlayer(CCUID& puid,CCMatchObjectStageState state, char* szName, int  nLevel ,bool isMaster,CCMatchTeam nTeam);
	void UpdatePlayer(CCUID& puid,CCMatchObjectStageState state, bool isMaster,CCMatchTeam nTeam);
	void UpdateEmblem(CCUID& puid);

	void UpdateList(int mode);

	ZPlayerListItem* GetUID(CCUID uid);
	const char* GetPlayerName( int nIndex);

	CCUID GetSelectedPlayerUID();
	void SelectPlayer(CCUID);

	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage);

//	CCUID	m_MyUID;
//	CCUID	m_uidChannel;
	int		m_nTotalPlayerCount;
	int		m_nPage;

	void SetWidth( float t ) { m_SlotWidth = t;	}
	void SetHeight( float t ) { m_SlotHeight = t; }

	PLAYERLISTMODE GetPlayerListMode()		{ return m_nMode; }
public:
	ZPlayerListBox(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZPlayerListBox();

	DECLARE_LOOK(ZPlayerListBoxLook)
	DECLARE_LOOK_CLIENT()

	void OnSize(int w,int h);

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);
	void AddTestItems();
};

////////////////////////////////////////////////////////////////////////////////////////

/*
class ZStagePlayerListBox : public CCListBox
{
private:
	CCBitmap*		m_pBitmap;

	map< CCUID, sStagePlayerInfo*>	mPlayers;
	vector<CCUID>					mPlayerOrder;

	int				mSelectedPlayer;
	int				mStartToDisplay;
	float			m_SlotWidth;
	float			m_SlotHeight;

	int				m_nOldW;
protected:

public:
	void SetBitmap( CCBitmap* pBitmap );
	CCBitmap* GetBitmap() {	return m_pBitmap; }

	void AddPlayer(CCMatchObjCache* pCache);
	void AddPlayer(CCUID& puid, CCMatchObjectStageState state, char* szName, int  nLevel ,bool isMaster,CCMatchTeam nTeam);
//	void AddPlayer(CCUID& puid, eStagePlayerState state, char* szName, int  nLevel ,bool isMaster,int nTeam);
	void DelPlayer(CCUID& puid);
	void UpdatePlayer(CCUID& puid,eStagePlayerState state, char* szName, int  nLevel ,bool isMaster,int nTeam);

	ZStagePlayerListItem* GetUID(CCUID uid);

	CCUID	m_MyUID;
	CCUID	m_uidChannel;
	int		m_nTotalPlayerCount;
	int		m_nPage;

	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);


	void SetWidth( float t ) { m_SlotWidth = t;	}
	void SetHeight( float t ) { m_SlotHeight = t; }

	void Resize(float x,float y);

	float OnReSize();

public:
	ZStagePlayerListBox(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZStagePlayerListBox();

	DECLARE_LOOK(ZPlayerListBoxLook)
	DECLARE_LOOK_CLIENT()
};
*/