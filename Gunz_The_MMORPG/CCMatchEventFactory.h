#ifndef _MMATCH_EVENT_FACTORY
#define _MMATCH_EVENT_FACTORY


#include "CCBaseGameType.h"
#include "CCMatchEventManager.h"


#include <vector>
#include <map>
#include <string>

using std::vector;
using std::map;
using std::string;


#define EVENT_LIST_XML_FILE_NAME "EventList.xml"


struct EventServerType
{
	DWORD				dwOrder;
	CCMatchServerMode	ServerType;
};


struct EventGameType
{
	DWORD			dwOrder;
	CCMATCH_GAMETYPE GameType;
};


struct EventData
{
	EventData()
	{
		dwEventListID	= 0;
		dwEventID		= 0;
		dwElapsedTime	= 0;
		dwPercent		= 0;
		dwRate			= 0;
		fXPBonusRatio	= 0.0f;
		fBPBonusRatio	= 0.0f;
	}

	DWORD					dwEventListID;
	DWORD					dwEventID;
	string					strName;
	string					strAnnounce;
	EVENT_TYPE				EventType;
	DWORD					dwGameType;
	DWORD					dwElapsedTime;
	DWORD					dwPercent;
	DWORD					dwRate;
	CCMatchServerMode		ServerType;
	SYSTEMTIME				Start;
	SYSTEMTIME				End;
	float					fXPBonusRatio;
	float					fBPBonusRatio;
	vector< EventPartTime > EventPartTimeVec;
};


typedef vector< EventData >			EventDataVec;
typedef map< DWORD, EventDataVec >	GameTypeEventMap;


class CCMatchEventFactory
{
public :
	CCMatchEventFactory();
	~CCMatchEventFactory();

	bool InsertEventData( const EventData& EvnData );

	bool GetEventList( const DWORD dwGameType, EventPtrVec& EvnPtrVec );

private :
	bool MakeEventList( const EventDataVec& EvnDataVec, EventPtrVec& epc );
	CCMatchEvent* CreateEvent( const DWORD dwEventID );

	void CreateFailMLog( CCMatchEvent* pEvent, const DWORD dwEventID );

private :
	EventDataVec		m_vAllGameTypeEventData;
	GameTypeEventMap	m_mGameTypeEvent;
};


class CCMatchEventFactoryManager
{
public :
	CCMatchEventFactoryManager();
	~CCMatchEventFactoryManager();

	bool LoadEventListXML( CCZFileSystem* pFileSystem, const string& strFileName );
	bool LoadEventListXML( const string& strFileName );

	bool		GetEventList( const DWORD dwGameType, const EVENT_TYPE EventType, EventPtrVec& EvnPtrVec );
	const WORD	GetLoadEventSize() const { return m_LoadEventSize; }

	void SetUsableState( const bool bIsUsable ) { m_bIsUsable = bIsUsable; }

	static CCMatchEventFactoryManager& GetInstance()
	{
		static CCMatchEventFactoryManager EventFactoryManager;
		return EventFactoryManager;
	}

private :
	CCMatchEventFactory& GetOnBeginEventFactory()	{ return m_OnBeginEventFactory; }
	CCMatchEventFactory& GetOnGameEventFactory()		{ return m_OnGameEventFactory; }
	CCMatchEventFactory& GetOnEndEventFactory()		{ return m_OnEndEventFactory; }
	CCMatchEventFactory& GetCustomEventFactory()		{ return m_CustomEventFactory; }

	bool InsertEvent( const EventData& EvnData );

	void ParseLocale( ::CCXmlElement& chrElement );
	void ParseEvent( ::CCXmlElement& chrElement );
	void ParseServerType( ::CCXmlElement& chrElement, vector< EventServerType >& vServerType );
	void ParseGameType( ::CCXmlElement& chrElement, vector< EventGameType >& vGameType );
	void ParseStartEndTime( ::CCXmlElement& chrElement, SYSTEMTIME& stTime );
	void ParseEventPartTime( ::CCXmlElement& chrElement, vector<EventPartTime>& EventPartTimeVec );
	
private :
	CCMatchEventFactory m_OnBeginEventFactory;	// 방을 만들어 게임을 시작하는 시점의 이벤트.
	CCMatchEventFactory m_OnGameEventFactory;	// 방을 만들어 게임을 시작한 후의 게임진행중 이벤트.
	CCMatchEventFactory m_OnEndEventFactory;		// 게임이 끝나는 시점의 이벤트.
	CCMatchEventFactory m_CustomEventFactory;	// 직접 호출하는 이벤트.

	bool m_bIsUsable;

	static WORD m_LoadEventSize;
};

#endif