#include "stdafx.h"
#include "CCXml.h"
#include "CCMatchLocale.h"
#include "CCMatchConfig.h"
#include "CCMatchEventFactory.h"


WORD CCMatchEventFactoryManager::m_LoadEventSize = 0;


CCMatchEventFactory::CCMatchEventFactory()
{
}


CCMatchEventFactory::~CCMatchEventFactory()
{
}


bool CCMatchEventFactory::InsertEventData( const EventData& EvnData )
{
	if( (CCMatchEvent::GAME_TYPE_ALL != EvnData.dwGameType) &&
		((0 > EvnData.dwGameType) && (8 < EvnData.dwGameType)) )
		return false;

	if( CCMatchEvent::GAME_TYPE_ALL == EvnData.dwGameType )
		m_vAllGameTypeEventData.push_back( EvnData );
	else
	{
		GameTypeEventMap::iterator itGameType = 
			m_mGameTypeEvent.find( EvnData.dwGameType );
		if( m_mGameTypeEvent.end() == itGameType )
		{
			EventDataVec edv;
			edv.push_back( EvnData );
			m_mGameTypeEvent.insert( GameTypeEventMap::value_type(EvnData.dwGameType, edv) );
		}
		else
			itGameType->second.push_back( EvnData );

		return true;
	}
	
	return false;
}


bool CCMatchEventFactory::GetEventList( const DWORD dwGameType, EventPtrVec& EvnPtrVec )
{
	if( (CCMatchEvent::GAME_TYPE_ALL != dwGameType) &&
		((0 > dwGameType) && (CCMATCH_GAMETYPE_MAX < dwGameType)) )
		return false;

	if( !m_vAllGameTypeEventData.empty() )
	{
		if( !MakeEventList(m_vAllGameTypeEventData, EvnPtrVec) )
			return false;
	}

	if( !m_mGameTypeEvent.empty() && CCMatchEvent::GAME_TYPE_ALL != dwGameType )
	{
		GameTypeEventMap::iterator itGameType = 
			m_mGameTypeEvent.find( dwGameType );
		if( m_mGameTypeEvent.end() != itGameType )
			return MakeEventList( itGameType->second, EvnPtrVec );
	}

	return true;
}


bool CCMatchEventFactory::MakeEventList( const EventDataVec& EvnDataVec, EventPtrVec& EvnPtrVec )
{
	bool bIsComplete = true;
	CCMatchEvent* pEvent;
	EventDataVec::const_iterator it, end;
	it = EvnDataVec.begin();
	for( end = EvnDataVec.end(); it != end; ++it )
	{
		if( !CheckUsableEventTimeByEndTime(it->End) )
			continue;

		pEvent = CreateEvent( it->dwEventID );
		if( 0 != pEvent )
		{
			if( pEvent->InitEvent() )
			{
				pEvent->Set( it->dwEventListID, it->EventType, it->dwGameType, 
					it->dwElapsedTime, it->dwPercent, it->dwRate, 
					it->Start, it->End, it->strName,
					it->strAnnounce,
					it->fXPBonusRatio,
					it->fBPBonusRatio,
					it->EventPartTimeVec );
				EvnPtrVec.push_back( pEvent );
				pEvent = 0;
			}
			else
			{
				cclog( "CCMatchEventFactory::MakeEventList - Init실패. ID:%u Name:%s.\n",
					pEvent->GetEventID(), pEvent->GetName().c_str() );
				delete pEvent;
				pEvent = 0;
				bIsComplete = false;
			}
		}
		else
			bIsComplete = false;
	}

	return bIsComplete;
}


CCMatchEvent* CCMatchEventFactory::CreateEvent( const DWORD dwEventID )
{
	switch( dwEventID )
	{
	case EID_PROBABILITY_PER_TIME :
		{
			CCMatchProbabiltyEventPerTime* pEvent = new CCMatchProbabiltyEventPerTime;
			if( 0 != pEvent )
			{
				if( dwEventID == pEvent->GetEventID() )
					return pEvent;
				else
				{
					CreateFailcclog( pEvent, dwEventID );
					delete pEvent;
					pEvent = 0;
					return 0;
				}
			}
			else 
				return 0;
		}
		break;

	case EID_XP_BONUS :
		{
			CCMatchXPBonusEvent* pEvent = new CCMatchXPBonusEvent;
			if( 0 != pEvent )
			{
				if( dwEventID == pEvent->GetEventID() )
					return pEvent;
				else
				{
					CreateFailcclog( pEvent, dwEventID );
					delete pEvent;
					pEvent = 0;
					return 0;
				}
			}
			else
				return 0;
		}
		break;

	case EID_BP_BONUS :
		{
			CCMatchBPBonusEvent* pEvent = new CCMatchBPBonusEvent;
			if( 0 != pEvent )
			{
				if( dwEventID == pEvent->GetEventID() )
					return pEvent;
				else
				{
					CreateFailcclog( pEvent, dwEventID );
					delete pEvent;
					pEvent = 0;
					return 0;
				}
			}
			else
				return 0;
		}
		break;

	case EID_UINQUEPLAYERLIST :
		{
			CCMatchUniquePlayerListEvent* pEvent = new CCMatchUniquePlayerListEvent;
			if( 0 != pEvent )
			{
				if( dwEventID == pEvent->GetEventID() )
					return pEvent;
				else
				{
					CreateFailcclog( pEvent, dwEventID );
					delete pEvent;
					pEvent = 0;
					return 0;
				}
			}
			else
				return NULL;
		}
		break;

	default :
		{
			ASSERT( 0 );
		}
		break;
	}

	return 0;
}


void CCMatchEventFactory::CreateFailcclog( CCMatchEvent* pEvent, const DWORD dwEventID )
{
	cclog( "Fail to create event : created event:%u, event:%u\n",
		pEvent->GetEventID(), dwEventID );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CCMatchEventFactoryManager::CCMatchEventFactoryManager()
{
	m_bIsUsable = false;
}


CCMatchEventFactoryManager::~CCMatchEventFactoryManager()
{
}


bool CCMatchEventFactoryManager::LoadEventListXML( CCZFileSystem* pFileSystem, const string& strFileName )
{
	CCXmlDocument	xmlIniData;
	xmlIniData.Create();

	//	<-----------------
	char *buffer;
	MZFile mzf;

	if(pFileSystem) 
	{
		if(!mzf.Open(strFileName.c_str(),pFileSystem)) 
		{
			if(!mzf.Open(strFileName.c_str())) 
			{
				xmlIniData.Destroy();
				return false;
			}
		}
	} 
	else 
	{
		if(!mzf.Open(strFileName.c_str()))
		{
			xmlIniData.Destroy();
			return false;
		}
	}

	buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer,mzf.GetLength());

	if(!xmlIniData.LoadFromMemory(buffer))
	{
		xmlIniData.Destroy();
		return false;
	}
	delete[] buffer;
	mzf.Close();
	//	<------------------

	CCXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(EL_LOCALE, szTagName))
		{
			ParseLocale( chrElement );
		}
	}

	xmlIniData.Destroy();
	return true;
}


bool CCMatchEventFactoryManager::LoadEventListXML( const string& strFileName )
{
	CCXmlDocument	xmlIniData;

	xmlIniData.Create();

	if (!xmlIniData.LoadFromFile(strFileName.c_str()))
	{
		xmlIniData.Destroy();
		return false;
	}

	CCXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();

	int iCount = rootElement.GetChildNodeCount();

	m_LoadEventSize = 0;

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(EL_LOCALE, szTagName))
		{
			ParseLocale( chrElement );
		}
	}

	xmlIniData.Destroy();
	return true;
}


void CCMatchEventFactoryManager::ParseLocale( ::CCXmlElement& chrElement )
{
	EventData ed;
	char szAttrName[ 128 ];
	char szAttrValue[ 256 ];
	const int nAttrCnt = chrElement.GetAttributeCount();
	for( int i = 0; i < nAttrCnt; ++i )
	{
		chrElement.GetAttribute( i, szAttrName, szAttrValue );
		
		if( 0 == stricmp(EL_COUNTRY, szAttrName) )
		{
			/*  // MBaseLocale.h // 
			enum MCountry
			{
				CCC_KOREA			= 82,		// 한국
				MC_US				= 1,		// 미국(인터네셔날)
				CCC_JAPAN			= 81,		// 일본
			};
			*/

			string strCountry;
			switch( CCGetLocale()->GetCountry() )
			{
			case CCC_KOREA :
				{
					strCountry = "kor";
				}
				break;

			case CCC_US :
				{
					strCountry = "us";
				}
				break;

			case CCC_JAPAN :
				{
					strCountry = "jpn";
				}
				break;

			case CCC_BRAZIL :
				{
					strCountry = "brz";
				}
				break;

			case CCC_INDIA :
				{
					strCountry = "ind";
				}
				break;

			default :
				{
					ASSERT( 0 );
				}
				break;
			}

			// 현제 서버랑 같은 국가 타입만 파싱함. 
            // if( 0 == stricmp(strCountry.c_str(), szAttrValue) )
			// 국가코드 구분 안함. 이미 국가별로 나눠줘 있음.
			if( true )
			{
				CCXmlElement chrNode;
				char szTag[ 128 ];
				const int nChrCnt = chrElement.GetChildNodeCount();
				for( int j = 0; j < nChrCnt; ++j )
				{
					chrNode = chrElement.GetChildNode( j );
					chrNode.GetTagName( szTag );

					if( 0 == stricmp(EL_EVENT, szTag) )
					{
						ParseEvent( chrNode );
					}
				}
			}
		}
	}
}


void CCMatchEventFactoryManager::ParseEvent( CCXmlElement& chrElement )
{
	char szAttrName[ 128 ];
	char szAttrValue[ 256 ];
	
	DWORD						dwEventListID = 0;
	DWORD						dwEventID = 0;
	string						strEventName;
	string						strAnnounce;
	EVENT_TYPE					EventType;
	DWORD						dwElapsedTime = 0;
	DWORD						dwPercent = 0;
	DWORD						dwRate = 0;
	vector< EventServerType >	vServerType;
	vector< EventGameType >		vGameType;
	SYSTEMTIME					Start, End;
	float						fXPBonusRatio = 0.0f;
	float						fBPBonusRatio = 0.0f;
	vector< EventPartTime >		EventPartTimeVec;

	memset( &Start, 0, sizeof(SYSTEMTIME) );
	memset( &End, 0, sizeof(SYSTEMTIME) );

	const int nAttrCnt = chrElement.GetAttributeCount();
	for( int i = 0; i < nAttrCnt; ++i )
	{
		chrElement.GetAttribute( i, szAttrName, szAttrValue );

		if( 0 == stricmp(EL_EVENT_LIST_ID, szAttrName) )
		{
			dwEventListID = static_cast< DWORD >( atoi(szAttrValue) );
			ASSERT( 0 < dwEventListID );
			continue;
		}

		if( 0 == stricmp(EL_EVENTID, szAttrName) )
		{
			dwEventID = static_cast< DWORD >( atol(szAttrValue) );
			if( NULL == CCMatchEventDescManager::GetInstance().Find(dwEventID) )
			{
				ASSERT( 0 && "Event.xml에 없는 Event ID입니다." );
				cclog( "CCMatchEventFactoryManager::ParseEvent - Event.xml에 없는 Event ID(%u)입니다.\n",
					dwEventID );
				return;
			}
			continue;
		}

		if( 0 == stricmp(EL_NAME, szAttrName) )
		{
			strEventName = MGetStringResManager()->GetString( string(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_EVENTTYPE, szAttrName) )
		{
			EventType = static_cast< EVENT_TYPE >( atoi(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_ELAPSEDTIME, szAttrName) )
		{
			dwElapsedTime = static_cast< DWORD >( atoi(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_PERCENT, szAttrName) )
		{
			dwPercent = static_cast< DWORD >( atol(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_RATE, szAttrName) )
		{
			dwRate = static_cast< DWORD >( atol(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_ANNOUNCE, szAttrName) )
		{
			strAnnounce = MGetStringResManager()->GetString( string(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_XPBONUS_RATIO, szAttrName) )
		{
			fXPBonusRatio = static_cast<float>( atoi(szAttrValue) ) / 100.0f;
			continue;
		}

		if( 0 == stricmp(EL_BPBONUS_RATIO, szAttrName) )
		{
			fBPBonusRatio = static_cast<float>( atoi(szAttrValue) ) / 100.0f;
			continue;
		}
	}

	CCXmlElement chrNode;
	char szTag[ 128 ];
	const int nChrNodeCnt = chrElement.GetChildNodeCount();

	EventPartTimeVec.clear();
	
	for( int j = 0; j < nChrNodeCnt; ++j )
	{
		chrNode = chrElement.GetChildNode( j );
		chrNode.GetTagName( szTag );

		if (szTag[0] == '#') continue;

		if( 0 == stricmp(EL_SERVERTYPE, szTag) )
		{
			ParseServerType( chrNode, vServerType );
			continue;
		}

		if( 0 == stricmp(EL_GAMETYPE, szTag) )
		{
			ParseGameType( chrNode, vGameType );
			continue;
		}

		if( 0 == stricmp(EL_STARTTIME, szTag) )
		{
			ParseStartEndTime( chrNode, Start );
			continue;
		}

		if( 0 == stricmp(EL_ENDTIME, szTag) )
		{
			ParseStartEndTime( chrNode, End );
			continue;
		}

		if( 0 == stricmp(EL_PART_TIME, szTag) )
		{
			ParseEventPartTime(chrNode, EventPartTimeVec );
			continue;
		}
	}

	// check start end time.
	if( !CheckUsableEventTimeByEndTime(End) )
	{
#ifdef _DEBUG
		cclog( "Time out Event(%u:%u.%u.%u.%u~%u.%u.%u.%u)\n", 
			dwEventID,
			Start.wYear, Start.wMonth, Start.wDay, Start.wHour,
			End.wYear, End.wMonth, End.wDay, End.wHour );
#endif
		return;
	}
	
	// check server type.
	vector< EventServerType >::iterator itSvrTyp, endSvrTyp;
	bool bUseEvent = false;
	endSvrTyp = vServerType.end();
	for( itSvrTyp = vServerType.begin(); itSvrTyp != endSvrTyp; ++itSvrTyp )
	{
		// 모든 서버에 적용.
		if( CSM_ALL == itSvrTyp->ServerType )
		{
			bUseEvent = true;
			continue;
		}

		// 현제 서버 타입에만 적용.
		if( MGetServerConfig()->GetServerMode() == itSvrTyp->ServerType )
		{
			bUseEvent = true;
			continue;
		}
	}

	ASSERT( (0 < Start.wYear) && (0 < Start.wMonth) && (0 < Start.wDay) && (0 <= Start.wHour) &&
			(0 < End.wYear) && (0 < End.wMonth) && (0 < End.wDay) && (0 <= End.wHour) );

	// check game type.
	if( bUseEvent )
	{
		EventData ed;
		vector< EventGameType >::iterator itGmTyp, endGmTyp;
		endGmTyp = vGameType.end();
		for( itGmTyp = vGameType.begin(); itGmTyp != endGmTyp; ++itGmTyp )
		{
			// insert event.

			ed.dwEventListID	= dwEventListID;
			ed.dwEventID		= dwEventID;
			ed.EventType		= EventType;
			ed.dwGameType			= itGmTyp->GameType;
			ed.ServerType		= MGetServerConfig()->GetServerMode();
			ed.dwElapsedTime	= dwElapsedTime;
			ed.dwPercent		= dwPercent;
			ed.dwRate			= dwRate;
			ed.Start			= Start;
			ed.End				= End;
			ed.strName			= strEventName;
			ed.strAnnounce		= strAnnounce;
			ed.fXPBonusRatio	= fXPBonusRatio;
			ed.fBPBonusRatio	= fBPBonusRatio;

			ed.EventPartTimeVec.swap( EventPartTimeVec );

			InsertEvent( ed );

			++m_LoadEventSize;
		}		
	}
}


void CCMatchEventFactoryManager::ParseServerType( CCXmlElement& chrElement, vector< EventServerType >& vServerType )
{
	EventServerType ServerType;
	char szAttrName[ 128 ];
	char szAttrValue[ 256 ];
	const int nAttrCnt = chrElement.GetAttributeCount();
	for( int i = 0; i < nAttrCnt; ++i )
	{
		chrElement.GetAttribute( i, szAttrName, szAttrValue );

		if( 0 == stricmp(EL_ORDER, szAttrName) )
		{
			ServerType.dwOrder = static_cast< DWORD >( atoi(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_TYPE, szAttrName) )
		{
			ServerType.ServerType = static_cast< CCMatchServerMode >( atoi(szAttrValue) );
			continue;
		}
	}

	vServerType.push_back( ServerType );
}


void CCMatchEventFactoryManager::ParseGameType( CCXmlElement& chrElement, vector< EventGameType >& vGameType )
{
	EventGameType GameType;
	char szAttrName[ 128 ];
	char szAttrValue[ 256 ];
	const int nAttrCnt = chrElement.GetAttributeCount();
	for( int i = 0; i < nAttrCnt; ++i )
	{
		chrElement.GetAttribute( i, szAttrName, szAttrValue );

		if( 0 == stricmp(EL_ORDER, szAttrName) )
		{
			GameType.dwOrder = static_cast< DWORD >( atol(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_TYPE, szAttrName) )
		{
			GameType.GameType = static_cast< CCMATCH_GAMETYPE >( atoi(szAttrValue) );
			continue;
		}
	}

	vGameType.push_back( GameType );
}


void CCMatchEventFactoryManager::ParseStartEndTime( CCXmlElement& chrElement, SYSTEMTIME& stTime )
{
	memset( &stTime, 0, sizeof(stTime) );

	char szAttrName[ 128 ];
	char szAttrValue[ 256 ];
	const int nAttrCnt = chrElement.GetAttributeCount();
	for( int i = 0; i < nAttrCnt; ++i )
	{
		chrElement.GetAttribute( i, szAttrName, szAttrValue );

		if( 0 == stricmp(EL_YEAR, szAttrName) )
		{
			stTime.wYear =  static_cast< WORD >( atoi(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_MONTH, szAttrName) )
		{
			stTime.wMonth = static_cast< WORD >( atoi(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_DAY, szAttrName) )
		{
			stTime.wDay = static_cast< WORD >( atoi(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_HOUR, szAttrName) )
		{
			stTime.wHour = static_cast< WORD >( atoi(szAttrValue) );
			continue;
		}
	}
}


void CCMatchEventFactoryManager::ParseEventPartTime( CCXmlElement& chrElement, vector<EventPartTime>& EventPartTimeVec )
{
	EventPartTime ept;

	char szAttrName[ 128 ];
	char szAttrValue[ 256 ];
	const int nAttrCnt = chrElement.GetAttributeCount();
	for( int i = 0; i < nAttrCnt; ++i )
	{
		chrElement.GetAttribute( i, szAttrName, szAttrValue );

		/*
		if( 0 == stricmp(EL_ORDER, szAttrName) )
		{
			continue;
		}
		*/

		if( 0 == stricmp(EL_START_HOUR, szAttrName) )
		{
			ept.btStartHour = static_cast< BYTE >( atoi(szAttrValue) );
			continue;
		}

		if( 0 == stricmp(EL_END_HOUR, szAttrName) )
		{
			ept.btEndHour = static_cast< BYTE >( atoi(szAttrValue) );
			continue;
		}
	}

#ifdef _DEBUG
	// 중복되는 범위가 있는지 검사.
	vector< EventPartTime >::iterator it, end;
	end = EventPartTimeVec.end();
	for( it = EventPartTimeVec.begin(); it != end; ++it )
	{
		if( (it->btStartHour <= ept.btStartHour) && (ept.btStartHour <= it->btEndHour) )
		{
			ASSERT( 0 );
		}
		
		if( (it->btStartHour <= ept.btEndHour) && (ept.btEndHour <= it->btEndHour) )
		{
			ASSERT( 0 );
		}
	}
#endif

	EventPartTimeVec.push_back( ept );
}


bool CCMatchEventFactoryManager::GetEventList( const DWORD dwGameType, const EVENT_TYPE EventType, EventPtrVec& EvnPtrVec )
{
	if( !m_bIsUsable )
		return true;

	switch( EventType )
	{
	case ET_BEGIN :
		{
			return GetOnBeginEventFactory().GetEventList( dwGameType, EvnPtrVec );
		}
		break;

	case ET_ONGAME :
		{
			return GetOnGameEventFactory().GetEventList( dwGameType, EvnPtrVec );
		}
		break;

	case ET_END :
		{
			return GetOnEndEventFactory().GetEventList( dwGameType, EvnPtrVec );
		}
		break;

	case ET_CUSTOM_EVENT :
		{
			return GetCustomEventFactory().GetEventList( dwGameType, EvnPtrVec );
		}
		break;

	default :
		{
			ASSERT( 0 && "정의되지 않은 이벤트 타입." );
			return false;
		}
		break;
	}
	return false;
}


bool CCMatchEventFactoryManager::InsertEvent( const EventData& EvnData )
{
	switch( EvnData.EventType )
	{
	case ET_BEGIN :
		{
			return GetOnBeginEventFactory().InsertEventData( EvnData );
		}
		break;

	case ET_ONGAME :
		{
			return GetOnGameEventFactory().InsertEventData( EvnData );
		}
		break;

	case ET_END :
		{
			return GetOnEndEventFactory().InsertEventData( EvnData );
		}
		break;

	case ET_CUSTOM_EVENT :
		{
			return GetCustomEventFactory().InsertEventData( EvnData );
		}
		break;

	default :
		{
			ASSERT( 0 && "정의되지 않은 이벤트 타입." );
			return false;
		}
		break;
	}
	return false;
}

