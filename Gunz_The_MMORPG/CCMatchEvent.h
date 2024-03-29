#pragma once

#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::map;

#include "CCUID.h"


class CCMatchObject;


#define EVENT_XML_FILE_NAME "Event.xml"


#define EV_EVENT	"Event"
#define EV_EVENTID	"id"
#define EV_DESC		"Description"


#define EL_LOCALE			"Locale"
#define EL_COUNTRY			"country"
#define EL_EVENT			"Event"
#define EL_EVENT_LIST_ID	"id"
#define EL_EVENTID			"EventID"
#define EL_NAME				"name"
#define EL_EVENTTYPE		"event_type"
#define EL_ELAPSEDTIME		"elapsed_time"
#define EL_PERCENT			"percent"
#define EL_RATE				"rate"
#define EL_ANNOUNCE			"announce"
#define EL_SERVERTYPE		"ServerType"
#define EL_GAMETYPE			"GameType"
#define EL_STARTTIME		"StartTime"
#define EL_ENDTIME			"EndTime"
#define EL_XPBONUS_RATIO	"xp_bonus_ratio"
#define EL_BPBONUS_RATIO	"bp_bonus_ratio"
#define EL_PART_TIME		"PartTime"
#define		EL_ORDER			"order"
#define		EL_TYPE				"type"
#define		EL_YEAR				"year"
#define		EL_MONTH			"month"
#define		EL_DAY				"day"
#define		EL_HOUR				"hour"
#define		EL_START_HOUR		"start_hour"
#define		EL_END_HOUR			"end_hour"




enum EVENT_TYPE
{
	ET_BEGIN	= 0,		// : 게임을 시작할때.
	ET_ONGAME	= 1,		//  : 방을 만들어 게임을 시작을 해야 활성화되는 이벤트.
	ET_END		= 2,		// : 게임이 끝나후에.

	ET_CUSTOM_EVENT	= 100,	// : 직접 호출.

	ET_MAX,
};


enum EVENT_ID
{
	EID_PROBABILITY_PER_TIME = 1,
	EID_XP_BONUS,
	EID_BP_BONUS,
	EID_UINQUEPLAYERLIST,

	EID_END,
};


struct EventPartTime
{
	BYTE btStartHour;
	BYTE btEndHour;
};


class CCMatchEvent
{
public :
	const static DWORD		GAME_TYPE_ALL = 100;


	virtual ~CCMatchEvent();

	const DWORD GetEventID() const		{ return m_dwEventID; }
	const DWORD GetEventListID() const	{ return m_dwEventListID; }
	const string& GetName()				{ return m_strName; }
	// 정해진 이벤트 처리루틴외 등록을 위해서. 
	
	bool DoItNow( const DWORD dwCurTime );

	void Set( const DWORD dwEventListID, const DWORD dwEventType, const DWORD dwGameType, const DWORD dwCheckElapsedTime, const DWORD dwPercent, 
		const DWORD dwRate, const SYSTEMTIME& Start, const SYSTEMTIME& End, const string& strName, const string& strAnnounce,
		const float fXPBonusRatio, const float fBPBonusRatio, const vector<EventPartTime>& EventPartTimeVec );

	void StartNewEvent() { m_vEventObj.clear(); }
	void Run();
	void CheckEventObj( CCMatchObject* pObj, const DWORD dwCurTime );

	void Reset();

	void SetLastCheckTime( const DWORD dwCurTime );

	// 유저가 따로 초기화를 필요할때는 이것을 재정의 하면 된다.
	// CCMatchEvent클래스를 생성시에 이 함수가 호출되어 같이 검사된다.
	virtual bool InitEvent() { return true; }
	// 정해진 루틴이 아닌 따로 처리해 줘야하는 이벤트는 이 함수를 제정의해서 사용함.
	// 처리시 필요한 유저타입데이터는 pContext로 넘겨서 처리.
	virtual bool CustomCheckEventObj( CCMatchObject* pObj, void* pContext ) { return true; }

protected :
	CCMatchEvent() { Reset(); }
	CCMatchEvent( const DWORD dwEventID ) : m_dwEventID ( dwEventID ), m_dwLastCheckTime( 0 ) {}
	
	bool CheckEventTime();
	bool CheckElapsedTimeIsOverflow( const DWORD dwCurTime );
	bool CheckEventPartTime();

protected : 
	virtual void OnCheckEventObj( CCMatchObject* pObj, const DWORD dwCurTime ) {}
	virtual void OnRun() {}

protected :
	DWORD					m_dwEventListID;
	DWORD					m_dwEventID;
	DWORD					m_dwEventType;
	DWORD					m_dwGameType;
	DWORD					m_dwCheckElapsedTime;
	DWORD					m_dwLastCheckTime;
	DWORD					m_dwPercent;
	DWORD					m_dwRate;
	float					m_fXPBonusRatio;
	float					m_fBPBonusRatio;
	SYSTEMTIME				m_Start;
	SYSTEMTIME				m_End;
	string					m_strName;
	string					m_strAnnounce;
	vector< CCUID >			m_vEventObj;
	vector< EventPartTime >	m_EventPartTimeVec;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 

class CCMatchEventDesc
{
public :
	CCMatchEventDesc() {}
	~CCMatchEventDesc() {}

	void Set( const DWORD dwEventID, const string& strDesc );

	const DWORD GetEventID() const			{ return m_dwEventID; }
	const string& GetDescription() const	{ return m_strDesc; }

private :
	DWORD	m_dwEventID;
	string	m_strDesc;
};


class CCMatchEventDescManager : private map< DWORD, const CCMatchEventDesc* >
{
public :
	CCMatchEventDescManager() {}
	~CCMatchEventDescManager()
	{
		ClearEventDesc();
	}
	
	bool LoadEventXML( CCZFileSystem* pFileSystem, const string& strFileName );
	bool LoadEventXML( const string& strFileName );

	const CCMatchEventDesc* Find( const DWORD dwEventID );
	void ClearEventDesc();

	static CCMatchEventDescManager& GetInstance()
	{
		static CCMatchEventDescManager EventDescManager;
		return EventDescManager;
	}

private :
	void ParseEvent( CCXmlElement& chrElement );
	bool Insert( const DWORD dwEventID, CCMatchEventDesc* pEventDesc );
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 이 아래쪽으로 이벤트를 추가 한다. 추후에 이벤트가 많아지면 별도의 파일로 분리되어야 함.

class CCMatchProbabiltyEventPerTime : public CCMatchEvent
{
public :
	CCMatchProbabiltyEventPerTime() : 
	   CCMatchEvent( EID_PROBABILITY_PER_TIME ) {}
	virtual ~CCMatchProbabiltyEventPerTime();
	
protected :
	void OnCheckEventObj( CCMatchObject* pObj, const DWORD dwCurTime );
	void OnRun();

private :
};


class CCMatchXPBonusEvent : public CCMatchEvent
{
public :
	CCMatchXPBonusEvent() : 
	   CCMatchEvent( EID_XP_BONUS ) {}
   virtual ~CCMatchXPBonusEvent();

   bool CustomCheckEventObj( CCMatchObject* pObj, void* pContext );
};


class CCMatchBPBonusEvent : public CCMatchEvent
{
public :
	CCMatchBPBonusEvent() :
	   CCMatchEvent( EID_BP_BONUS ) {}
   virtual ~CCMatchBPBonusEvent();

   bool CustomCheckEventObj( CCMatchObject* pObj, void* pContext );
};


class CCMatchUniquePlayerListEvent : public CCMatchEvent
{
private :
	map< CCUID, DWORD > m_UniquePlayerList;

public :
	CCMatchUniquePlayerListEvent() : CCMatchEvent( EID_UINQUEPLAYERLIST ) {}
	~CCMatchUniquePlayerListEvent() {}

	void OnCheckEventObj( CCMatchObject* pObj, const DWORD dwCurTime );
	void OnRun();
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool CheckUsableEventTimeByStartTime( const SYSTEMTIME& StartTime );
bool CheckUsableEventTimeByEndTime( const SYSTEMTIME& EndTime );
bool CheckUsableEventPartTime( const EventPartTime& ept );
