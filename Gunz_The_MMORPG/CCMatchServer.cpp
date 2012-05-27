#include "stdafx.h"
#include "CCMatchServer.h"
#include "CCSharedCommandTable.h"
#include "CCErrorTable.h"
#include "CCBlobArray.h"
#include "CCObject.h"
#include "CCMatchObject.h"
#include "CCMatchItem.h"
#include "CCMatchNotify.h"
#include "Msg.h"
#include "CCMatchObjCache.h"
#include "CCMatchStage.h"
#include "CCMatchTransDataType.h"
#include "CCMatchFormula.h"
#include "CCMatchConfig.h"
#include "CCCommandCommunicator.h"
#include "CCMatchShop.h"
#include "CCDebug.h"
#include "CCMatchAuth.h"
#include "CCMatchStatus.h"
#include "CCAsyncDBJob.h"
#include "CCMatchWorldItemDesc.h"
#include "CCMatchQuestMonsterGroup.h"
#include "RTypes.h"
#include "CCMatchChatRoom.h"
#include "CCMatchUtil.h"
#include "CCLadderStatistics.h"
#include "CCMatchSchedule.h"
#include <winbase.h>
#include "CCMatchGameType.h"
#include "CCQuestFormula.h"
#include "CCQuestItem.h"
#include "CCSacrificeQItemTable.h"
#include "CCMatchPremiumIPCache.h"
#include "CCCommandBuilder.h"
#include "CCMatchLocale.h"
#include "CCMatchEvent.h"
#include "CCMatchEventManager.h"
#include "CCMatchEventFactory.h"
//#include "../../MatchServer/HSHIELD/AntiCpSvrFunc.h"
#include "CCMatchDBGambleItem.h"
#include "CCMatchCRC32XORCache.h"
#include "CCMatchCheckLoopTime.h"
#include "CCCrashDump.h"
#include "CCMatchDuelTournamentMgr.h"

#include "CCAsyncDBJob_FriendList.h"
#include "CCAsyncDBJob_UpdateCharInfoData.h"
#include "CCAsyncDBJob_GetLoginInfo.h"
#include "CCAsyncDBJob_UpdateCharItemInfoData.h"

#include "CCAsyncDBJob_GetBRDescription.h"
#include "CCAsyncDBJob_InsertCharBRInfo.h"
#include "CCAsyncDBJob_UpdateCharBRInfo.h"
#include "CCAsyncDBJob_RewardCharBR.h"

#include "CCMatchBuff.h"

// 메모리 침범을 찾기위해서 다음을 define 해준다. debug 모드에서만 사용가능하다
//#define _CHECK_MEMORY_CORRUPTION

#define DEFAULT_REQUEST_UID_SIZE		4200000000	///< UID 할당 요청 기본 단위
#define DEFAULT_REQUEST_UID_SPARE_SIZE	10000		///< UID 남은 갯수
#define DEFAULT_ASYNCPROXY_THREADPOOL	6
#define MAXUSER_WEIGHT					30

#define MAX_DB_QUERY_COUNT_OUT			5		// 쿼리가 5번이상 실패하면 Shutdown


#define MATCHSERVER_DEFAULT_UDP_PORT	7777

#define FILENAME_ITEM_DESC				"zitem.xml"
#define FILENAME_ITEM_DESC_LOCALE		"zitem_locale.xml"
#define FILENAME_BUFF_DESC				"zBuff.xml"
#define FILENAME_SHOP					"shop.xml"
#define FILENAME_CHANNEL				"channel.xml"
#define FILENAME_SHUTDOWN_NOTIFY		"shutdown.xml"
#define FILENAME_WORLDITEM_DESC			"worlditem.xml"
#define FILENAME_MONSTERGROUP_DESC		"monstergroup.xml"
#define FILENAME_CHANNELRULE			"channelrule.xml"

CCMatchServer* CCMatchServer::m_pInstance = NULL;
//extern void RcpLog(const char *pFormat,...);

////////////////////////////////////
void RcpLog(const char *pFormat,...)
{
	char szBuf[256];

	va_list args;

	va_start(args,pFormat);
	vsprintf(szBuf, pFormat, args);
	va_end(args);

	int nEnd = (int)strlen(szBuf)-1;
	if ((nEnd >= 0) && (szBuf[nEnd] == '\n')) {
		szBuf[nEnd] = NULL;
		strcat(szBuf, "\n");
	}
#ifdef _DEBUG
	OutputDebugString(szBuf);
#endif
}
////////////////////////////////////


/////////////////////////////////////////////////////////
class MPointerChecker
{
private:
	void* m_pPointer;
	bool	m_bPrinted;
public:
	MPointerChecker() : m_bPrinted(false), m_pPointer(0) {  }
	void Init(void* pPointer) { m_pPointer = pPointer; }
	void Check(void* pPointer, int nState, int nValue)
	{
		if ((pPointer != m_pPointer) && (!m_bPrinted))
		{
			m_bPrinted = true;
			cclog("### Invalid Pointer(%x, %x) - State(%d) , Value(%d) ###\n", m_pPointer, pPointer, nState, nValue);
		}
	}
};

#define NUM_CHECKPOINTER	3
static MPointerChecker g_PointerChecker[NUM_CHECKPOINTER];


void _CheckValidPointer(void* pPointer1, void* pPointer2, void* pPointer3, int nState, int nValue)
{
	if (pPointer1 != NULL) g_PointerChecker[0].Check(pPointer1, nState, nValue);
	if (pPointer2 != NULL) g_PointerChecker[1].Check(pPointer2, nState, nValue);
	if (pPointer3 != NULL) g_PointerChecker[2].Check(pPointer3, nState, nValue);
}



/////////////////////////////////////////////////////////

void CopyCharInfoForTrans(CCTD_CharInfo* pDest, CCMatchCharInfo* pSrcCharInfo, CCMatchObject* pSrcObject)
{
	memset(pDest, 0, sizeof(CCTD_CharInfo));

	if (pSrcCharInfo)
	{
		strcpy(pDest->szName, pSrcCharInfo->m_szName);
		strcpy(pDest->szClanName, pSrcCharInfo->m_ClanInfo.m_szClanName);
		
		pDest->nClanGrade = pSrcCharInfo->m_ClanInfo.m_nGrade;
		pDest->nClanContPoint = pSrcCharInfo->m_ClanInfo.m_nContPoint;

		pDest->nCharNum = (char)pSrcCharInfo->m_nCharNum;
		pDest->nLevel = (unsigned short)pSrcCharInfo->m_nLevel;
		pDest->nSex = (char)pSrcCharInfo->m_nSex;
		pDest->nFace = (char)pSrcCharInfo->m_nFace;
		pDest->nHair = (char)pSrcCharInfo->m_nHair;

		pDest->nXP = pSrcCharInfo->m_nXP;
		pDest->nBP = pSrcCharInfo->m_nBP;
		pDest->fBonusRate = pSrcCharInfo->m_fBonusRate;
		pDest->nPrize = (unsigned short)pSrcCharInfo->m_nPrize;
		pDest->nHP = (unsigned short)pSrcCharInfo->m_nHP;
		pDest->nAP = (unsigned short)pSrcCharInfo->m_nAP;
		pDest->nMaxWeight = (unsigned short)pSrcCharInfo->m_nMaxWeight;
		pDest->nSafeFalls = (unsigned short)pSrcCharInfo->m_nSafeFalls;
		pDest->nFR = (unsigned short)pSrcCharInfo->m_nFR;
		pDest->nCR = (unsigned short)pSrcCharInfo->m_nCR;
		pDest->nER = (unsigned short)pSrcCharInfo->m_nER;
		pDest->nWR = (unsigned short)pSrcCharInfo->m_nWR;

		for (int i = 0; i < MMCIP_END; i++) {
			if (pSrcCharInfo->m_EquipedItem.IsEmpty(CCMatchCharItemParts(i))) {
				pDest->nEquipedItemDesc[i]  = 0;
				pDest->nEquipedItemCount[i] = 0;
				pDest->uidEquipedItem[i] = CCUID(0, 0);
			}
			else {
				CCMatchItem* pItem = pSrcCharInfo->m_EquipedItem.GetItem(CCMatchCharItemParts(i));
				if( NULL == pItem ) {
					pDest->nEquipedItemDesc[i]  = 0;
					pDest->nEquipedItemCount[i] = 0;
					pDest->uidEquipedItem[i] = CCUID(0, 0);
				}
				else {
					CCMatchItemDesc* pItemDesc = pItem->GetDesc();
					if (pItemDesc) {
						pDest->nEquipedItemDesc[i]  = pItemDesc->m_nID;
						pDest->nEquipedItemCount[i] = pItem->GetItemCount();
						pDest->uidEquipedItem[i] = pSrcCharInfo->m_EquipedItem.GetItem((CCMatchCharItemParts)i)->GetUID();
					}
				}
			}

		}
	}


	if (pSrcObject) {
		pDest->nUGradeID = pSrcObject->GetAccountInfo()->m_nUGrade;
	} else {
		pDest->nUGradeID = CCMUGFREE;
	}

	pDest->nClanCLID = pSrcCharInfo->m_ClanInfo.m_nClanID;


	if (pSrcObject && pSrcObject->GetDuelTournamentCharInfo()) {
		pDest->nDTLastWeekGrade = pSrcObject->GetDuelTournamentCharInfo()->GetLastWeekGrade();
	} else {
		pDest->nDTLastWeekGrade = 0;
	}

}

void CopyCharInfoDetailForTrans(CCTD_CharInfo_Detail* pDest, CCMatchCharInfo* pSrcCharInfo, CCMatchObject* pSrcObject)
{
	memset(pDest, 0, sizeof(CCTD_CharInfo_Detail));

	if (pSrcCharInfo)
	{
		strcpy(pDest->szName, pSrcCharInfo->m_szName);
		strcpy(pDest->szClanName, pSrcCharInfo->m_ClanInfo.m_szClanName);
		pDest->nClanGrade = pSrcCharInfo->m_ClanInfo.m_nGrade;
		pDest->nClanContPoint = pSrcCharInfo->m_ClanInfo.m_nContPoint;

		pDest->nLevel = (unsigned short)pSrcCharInfo->m_nLevel;
		pDest->nSex = (char)pSrcCharInfo->m_nSex;
		pDest->nFace = (char)pSrcCharInfo->m_nFace;
		pDest->nHair = (char)pSrcCharInfo->m_nHair;
		pDest->nXP = pSrcCharInfo->m_nXP;
		pDest->nBP = pSrcCharInfo->m_nBP;

		pDest->nKillCount = pSrcCharInfo->m_nTotalKillCount;
		pDest->nDeathCount = pSrcCharInfo->m_nTotalDeathCount;


		unsigned long int nNowTime = CCMatchServer::GetInstance()->GetTickTime();

		// 접속시간
		pDest->nConnPlayTimeSec = CCGetTimeDistance(pSrcCharInfo->m_nConnTime, nNowTime) / 1000;
		pDest->nTotalPlayTimeSec = pDest->nConnPlayTimeSec + pSrcCharInfo->m_nTotalPlayTimeSec;

		// 아이템셋
		for (int i = 0; i < MMCIP_END; i++) {
			if (pSrcCharInfo->m_EquipedItem.IsEmpty(CCMatchCharItemParts(i))) {
				pDest->nEquipedItemDesc[i] = 0;
			} else {
				if( NULL == pSrcCharInfo->m_EquipedItem.GetItem(CCMatchCharItemParts(i)) ) {
					pDest->nEquipedItemDesc[i] = 0;
				} else {
					pDest->nEquipedItemDesc[i] = pSrcCharInfo->m_EquipedItem.GetItem(CCMatchCharItemParts(i))->GetDesc()->m_nID;
				}
			}
		}
	}


	if (pSrcObject) {
		pDest->nUGradeID = pSrcObject->GetAccountInfo()->m_nUGrade;
	} else {
		pDest->nUGradeID = CCMUGFREE;
	}

	pDest->nClanCLID = pSrcCharInfo->m_ClanInfo.m_nClanID;
}
//버프정보임시주석 
/*
void CopyCharBuffInfoForTrans(CCTD_CharBuffInfo* pDest, CCMatchCharInfo* pSrcCharInfo, CCMatchObject* pSrcObject)
{
	memset(pDest, 0, sizeof(CCTD_CharBuffInfo));

	if( pSrcCharInfo && pSrcObject ) 
	{
		int nIndex = 0;
		CCMatchObjectCharBuff* pCharBuffObj = pSrcObject->GetCharBuff();
		
		CCMatchShortBuffMap* pShortBuffMap = pCharBuffObj->GetShortBuffInfoMap();
		for(CCMatchShortBuffMap::iterator iter = pShortBuffMap->begin(); iter != pShortBuffMap->end(); iter++) 
		{
			CCMatchShortBuff* pInfo = iter->second;

			pDest->ShortBuffInfo[nIndex].uidBuff				= pInfo->GetBuffUID();
			pDest->ShortBuffInfo[nIndex].nBuffID				= pInfo->GetBuffID();
			pDest->ShortBuffInfo[nIndex].nBuffPeriod			= pInfo->GetBuffPeriod();
			pDest->ShortBuffInfo[nIndex++].nBuffPeriodRemainder	= pInfo->GetBuffPeriodRemainder(timeGetTime());

			if( nIndex > MAX_CHARACTER_SHORT_BUFF_COUNT ) {
				_ASSERT(0);
				break;
			}
		}
	}
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool IsExpiredBlockEndTime( const SYSTEMTIME& st )
{
	SYSTEMTIME stLocal;
	GetLocalTime( &stLocal );


	if( st.wYear < stLocal.wYear )
		return true;
	else if( st.wYear > stLocal.wYear )
		return false;

	if( st.wMonth < stLocal.wMonth )
		return true;
	else if( st.wMonth > stLocal.wMonth )
		return false;

	if( st.wDay < stLocal.wDay )
		return true;
	else if( st.wDay > stLocal.wDay )
		return false;

	if( st.wHour < stLocal.wHour )
		return true;
	else if( st.wHour > stLocal.wHour )
		return false;

	if( st.wMinute >= stLocal.wMinute )
		return false;
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CCMatchServer::CCMatchServer() : m_pScheduler( 0 ), m_pDTMgr(new CCMatchDuelTournamentMgr)
{
	_ASSERT(m_pInstance==NULL);
	m_pInstance = this;
	m_nTickTime = 0;
	m_dwBlockCount = 0;
	m_dwNonBlockCount = 0;

	m_This = (MATCHSERVER_UID);

	SetName("MATCHSERVER");	// For Debug
	SetDefaultChannelName("PUBLIC-");

	m_bCreated = false;

	m_pAuthBuilder = NULL;

	// m_pScheduler = 0;

	// 스트링 리소스는 제일 먼저 인스턴스를 생성해놔야 한다.
	CCMatchStringResManager::MakeInstance();

	// 포인터 침범 디버그코드
	m_checkMemory1 = m_checkMemory2 = m_checkMemory3 = m_checkMemory4 = m_checkMemory5 = m_checkMemory6 =
	m_checkMemory7 = m_checkMemory8 = m_checkMemory9 = m_checkMemory10 = m_checkMemory11 = m_checkMemory12 =
	m_checkMemory13 = m_checkMemory14 = m_checkMemory15 = m_checkMemory16 = m_checkMemory17 = m_checkMemory18 =
	m_checkMemory19 = m_checkMemory20 = m_checkMemory21 = CHECKMEMORYNUMBER;

#ifdef _HSHIELD
	if( MGetServerConfig()->IsUseHShield() )
		m_HSCheckCounter = 0L;
#endif

	memset(m_szMD5Value, 0, 16);
	SetFloodCheck(true);
}

static bool g_bPrintedInvalidMemory = false;

void CCMatchServer::CheckMemoryTest(int nState, int nValue)
{
#define CHECK(n) if(m_checkMemory##n!=CHECKMEMORYNUMBER) { g_bPrintedInvalidMemory=true; cclog("***WARNING !! m_checkMemory" #n " is corrupted. State(%d), Value(%d)\n", nState, nValue); }

	if (g_bPrintedInvalidMemory) return;

	CHECK(1);    
	CHECK(2);    
	CHECK(3);    
	CHECK(4);    
	CHECK(5);    
	CHECK(6);    
	CHECK(7);    
	CHECK(8);    
	CHECK(9);    
	CHECK(10);    
	CHECK(11);    
	CHECK(12);    
	CHECK(13);    
	CHECK(14);    
	CHECK(15);    
	CHECK(16);    
	CHECK(17);    
	CHECK(18);    
	CHECK(19);    
	CHECK(20);    
	CHECK(21);    
}

CCMatchServer::~CCMatchServer()
{
	delete m_pDTMgr;

	CheckMemoryTest();

	if (m_pAuthBuilder) {
		delete m_pAuthBuilder;
		m_pAuthBuilder = NULL;
	}

	Destroy();

}


bool CCMatchServer::LoadInitFile()
{
	if (!MGetServerConfig()->Create())
	{
		LOG(LOG_PROG, "Load Config File Failed\n");
		return false;
	}

	if( !InitLocale() ){
		LOG(LOG_PROG, "Locale 설정 실패.\n" );
		return false;
	}


	// 제한맵서버일 경우 플레이가능한 맵 화면에 출력
	if (MGetServerConfig()->IsResMap())
	{
		char szText[512];
		sprintf(szText, "Enable Maps: ");
		for (int i = 0; i < CCMATCH_MAP_MAX; i++)
		{
			if (MGetServerConfig()->IsEnableMap(CCMATCH_MAP(i)))
			{
				strcat(szText, MGetMapDescMgr()->GetMapName(i)); 
				strcat(szText, ", ");
			}
		}
		LOG(LOG_PROG, szText);
	}

	if (!CCMatchFormula::Create()) 
	{
		LOG(LOG_PROG, "Open Formula Table FAILED\n");
		return false;
	}
	if (!CCQuestFormula::Create()) 
	{
		LOG(LOG_PROG, "Open Quest Formula Table FAILED\n");
		return false;
	}

	if (!MGetMatchWorldItemDescMgr()->ReadXml(FILENAME_WORLDITEM_DESC))
	{
		Log(LOG_PROG, "Read World Item Desc Failed\n");
		return false;
	}

	if (!MGetMapDescMgr()->Initialize("map.xml"))
	{
		Log(LOG_PROG, "Read Map.xml Failed\n");
		return false;
	}
/*
	if (!MGetNPCGroupMgr()->ReadXml(FILENAME_MONSTERGROUP_DESC))
	{
		Log(LOG_PROG, "Read Monster Group Desc Failed");
		return false;
	}
*/
#ifdef _QUEST_ITEM
	if( !GetQuestItemDescMgr().ReadXml(QUEST_ITEM_FILE_NAME) )
	{
		Log( LOG_PROG, "Load quest item xml file failed." );
		return false;
	}
	if( !MSacrificeQItemTable::GetInst().ReadXML(SACRIFICE_TABLE_XML) )
	{
		Log( LOG_PROG, "Load sacrifice quest item table failed." );
		return false;
	}
#endif
	// 클랜전 서버일 경우만 실행하는 초기화
	if (MGetServerConfig()->GetServerMode() == CSM_CLAN)
	{
		GetLadderMgr()->Init();

#ifdef _DEBUG
		//GetLadderMgr()->GetStatistics()->PrintDebug();		
#endif
	}

	if (!MGetMapsWorldItemSpawnInfo()->Read())
	{
		Log(LOG_PROG, "Read World Item Spawn Failed\n");
		return false;
	}

	if (!MGetMatchItemDescMgr()->ReadXml(FILENAME_ITEM_DESC))
	{
		Log(LOG_PROG, "Read Item Descriptor Failed\n");
		return false;
	}
	if (!MGetMatchItemDescMgr()->ReadXml(FILENAME_ITEM_DESC_LOCALE))
	{
		Log(LOG_PROG, "Read Item Descriptor Locale Failed\n");
		return false;
	}

	InitItemCRC32Cache();

	//kimyhwan todok 일단 제거하였습니다
	//if( !MGetMatchBuffDescMgr()->ReadXml(FILENAME_BUFF_DESC) )
	//{
	//	Log(LOG_PROG, "Read Buff Descriptor Failed\n");
	//	return false;
	//}
	//else
	//{
	//	InitBuffCRC32Cache();
	//}
	//
	//if( !MGetMatchBuffDescMgr()->SetBuffName(MGetMatchItemDescMgr()) )
	//{
	//	Log(LOG_PROG, "Set Buff Name Failed In Buff Descriptor\n");
	//	return false;
	//}




	if(!GetQuest()->Create())
	{
		Log(LOG_PROG, "Read Quest Desc Failed\n");
		return false;
	}

	if (!MGetMatchShop()->Create(FILENAME_SHOP))
	{
		Log(LOG_PROG, "Read Shop Item Failed\n");
		return false;
	}
	if (!LoadChannelPreset()) 
	{
		Log(LOG_PROG, "Load Channel preset Failed\n");
		return false;
	}
	if (!m_MatchShutdown.LoadXML_ShutdownNotify(FILENAME_SHUTDOWN_NOTIFY))
	{
		Log(LOG_PROG, "Load Shutdown Notify Failed\n");
		return false;
	}
	if (!MGetChannelRuleMgr()->ReadXml(FILENAME_CHANNELRULE))
	{
		Log(LOG_PROG, "Load ChannelRule.xml Failed\n");
		return false;
	}

	unsigned long nItemChecksum = CCGetCCZFileChecksum(FILENAME_ITEM_DESC);
	SetItemFileChecksum(nItemChecksum);


	if( !InitEvent() )
	{
		Log(LOG_PROG, "init event failed.\n\n");
		return false;
	}

#ifdef _DEBUG
	CheckItemXML();
	CheckUpdateItemXML();	
#endif

	return true;
}


void CCMatchServer::InitItemCRC32Cache()
{
// 전혀 사용하지 않는거라 주석처리
/*	CCMatchItemDescMgr::iterator it = MGetMatchItemDescMgr()->begin();
	CCMatchItemDescMgr::const_iterator end = MGetMatchItemDescMgr()->end();
	CCMatchItemDesc* pItemDesc = NULL;
	CCMatchCRC32XORCache CRC32Cache;

	for( ; end != it; ++it )
	{
		pItemDesc = reinterpret_cast<CCMatchItemDesc*>( it->second );

		CRC32Cache.Reset();
		pItemDesc->CacheCRC32( CRC32Cache );

		m_ItemResourceCRC32.insert( map<int, DWORD>::value_type(pItemDesc->m_nID, CRC32Cache.GetXOR()) );
	}*/
}

/*
const DWORD CCMatchServer::GetItemCRC32Cache( const int nItemID )
{
	map< int, DWORD >::iterator itFind = m_ItemResourceCRC32.find( nItemID );
	if( m_ItemResourceCRC32.end() == itFind )
	{
		return 0;
	}

	return static_cast<DWORD>( itFind->second );
}
*/

void CCMatchServer::InitBuffCRC32Cache()
{
}

bool CCMatchServer::LoadChannelPreset()
{
	#define MTOK_DEFAULTCHANNELNAME		"DEFAULTCHANNELNAME"
	#define MTOK_DEFAULTRULENAME		"DEFAULTRULENAME"
	#define MTOK_CHANNEL				"CHANNEL"
	#define MTOK_CLAN_CHANNEL			"CLAN_CHANNEL"

	CCXmlDocument	xmlIniData;
	xmlIniData.Create();

	if (!xmlIniData.LoadFromFile(FILENAME_CHANNEL))
	{
		xmlIniData.Destroy();
		return false;
	}

	CCXmlElement rootElement, childElement;
	char szTagName[256];
	char szBuf[256];

	rootElement = xmlIniData.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		childElement = rootElement.GetChildNode(i);
		childElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, MTOK_CHANNEL))
		{
			char	szRuleName[256]		= "";
			int		nMaxPlayers			= 0;
			int		nLevelMin			= -1;
			int		nLevelMax			= -1;
			bool	bIsTicketChannel	= false;
			int		nTicketItemID		= -1;
			bool	bIsDuelTournament	= false;
			CCCHANNEL_TYPE nChannelType	= CCCHANNEL_TYPE_PRESET;

			childElement.GetAttribute(szBuf, "name");
			if (childElement.GetAttribute(szRuleName, "rule") == false)
				strcpy(szRuleName, GetDefaultChannelRuleName());
			childElement.GetAttribute(&nMaxPlayers, "maxplayers");
			childElement.GetAttribute(&nLevelMin, "levelmin");
			childElement.GetAttribute(&nLevelMax, "levelmax");
			childElement.GetAttribute(&bIsTicketChannel, "IsTicketChannel");
			childElement.GetAttribute(&nTicketItemID, "TicketItemID");
			childElement.GetAttribute(&bIsDuelTournament, "IsDuelTournament");

			if( bIsDuelTournament )
				nChannelType = CCCHANNEL_TYPE_DUELTOURNAMENT;

			CCUID uidChannel;
			ChannelAdd(MGetStringResManager()->GetStringFromXml(szBuf), 
				szRuleName, &uidChannel, nChannelType, nMaxPlayers, nLevelMin, nLevelMax,
				bIsTicketChannel, static_cast<DWORD>(nTicketItemID), bIsTicketChannel, szBuf);
		} 
		else if (!strcmp(szTagName, MTOK_DEFAULTCHANNELNAME)) 
		{
			childElement.GetAttribute(szBuf, "name");
			SetDefaultChannelName(MGetStringResManager()->GetStringFromXml(szBuf));
		} 
		else if (!strcmp(szTagName, MTOK_DEFAULTRULENAME)) 
		{
			childElement.GetAttribute(szBuf, "name");
			SetDefaultChannelRuleName(szBuf);
		}
		else if( !strcmp(szTagName, MTOK_CLAN_CHANNEL) )
		{
			bool	bIsTicketChannel	= false;
			int		nTicketItemID		= -1;

			childElement.GetAttribute(&bIsTicketChannel, "IsTicketChannel");
			childElement.GetAttribute(&nTicketItemID, "TicketItemID");

			GetChannelMap()->SetIsTicketClanChannel( bIsTicketChannel );
			GetChannelMap()->SetClanChannelTicketItemID( nTicketItemID );
		}
	}

	xmlIniData.Destroy();
	return true;
}

bool CCMatchServer::InitDB()
{
	CString str = m_MatchDBMgr.BuildDSNString(MGetServerConfig()->GetDB_DNS(), 
		                                      MGetServerConfig()->GetDB_UserName(), 
											  MGetServerConfig()->GetDB_Password());

	if (m_MatchDBMgr.Connect())
	{
		LOG(LOG_PROG, "DBMS connected\n");
	}
	else
	{
		LOG(LOG_PROG, "Can't Connect To DBMS\n");
		return false;
	}

	if( MGetServerConfig()->IsUseFilter() )
	{
		if( InitCountryFilterDB() )
			LOG(LOG_PROG, "InitCountryFilterDB.\n");
		else
		{
			LOG( LOG_PROG, "Fail to init country filter DB.\n" );
			return false;
		}
	}
	
	return true;
}

#include "CCLadderMgr.h"
#include "CCTeamGameStrategy.h"

LONG WINAPI TopLevelExceptionFilter(_EXCEPTION_POINTERS *pException)
{

	printf ("Crash!\n");

//	MessageBox(NULL,"test222", "heap corruption", MB_OK);
	cclog("TopLevelExceptionFilter...throw...");


	char szDumpFileName[ _MAX_DIR ]= {0,};
	SYSTEMTIME SystemTime;
	GetLocalTime( &SystemTime );
	sprintf( szDumpFileName, "Log/UnhandledDump_%d-%d-%d_%d-%d-%d.dmp"
		, SystemTime.wYear
		, SystemTime.wMonth
		, SystemTime.wDay
		, SystemTime.wHour
		, SystemTime.wMinute
		, SystemTime.wSecond );

	CrashExceptionDump( pException, szDumpFileName, true );

	return EXCEPTION_EXECUTE_HANDLER;
}

bool CCMatchServer::Create(int nPort)
{
	// unhandled exception
	::SetUnhandledExceptionFilter(TopLevelExceptionFilter);

	// set buffer overrun error handler /GS
//	SetSecurityErrorHandler(ReportBufferOverrun);

	srand(timeGetTime());

	m_NextUseUID.SetZero();
	m_NextUseUID.Increase(10);	// 10 아래의 UID는 사용안함

	SetupRCPLog(RcpLog);
// #ifdef _DEBUG
// 	m_RealCPNet.SetLogLevel(0);
// #else
	m_RealCPNet.SetLogLevel(0);
//#endif

	if (!LoadInitFile())	return false;
	if (!InitDB())			return false;

	if( MGetServerConfig()->IsItemConsistency() == true ) {
		if( CheckItemXMLFromDatabase() == false ) {
			_ASSERT(0);
			return false;
		}
	}

	m_AsyncProxy.Create(DEFAULT_ASYNCPROXY_THREADPOOL);
	m_Admin.Create(this);

	// 디비에 최대 접속인원 업데이트
	m_MatchDBMgr.UpdateServerInfo(MGetServerConfig()->GetServerID()
								, MGetServerConfig()->GetMaxUser()
								, MGetServerConfig()->GetServerName());

	if( nPort != MGetServerConfig()->GetServerPort() )
		nPort = MGetServerConfig()->GetServerPort();
	if(MServer::Create(nPort)==false) {
		return false;
	}

	
	MGetServerStatusSingleton()->Create(this);			///< 서버 상태 보여주는 클래스 초기화

	// 스케쥴러 초기화.
	if( !InitScheduler() ){
		LOG(LOG_PROG, "Match Server Scheduler Create FAILED\n" );
		return false;
	}

	CCMatchAntiHack::InitClientFileList();

	if( !InitGambleMachine() )
	{
		cclog( "init gamble machine fail.\n" );
		return false;
	}

	if( !InitBattletimeRewardMachine() )
	{
		cclog( "Init Battletime Reward Machine Fail.\n" );
		return false;
	}

//#if defined(LOCALE_NHNUSA) || defined(_DEBUG)
//	if( !m_HackingChatList.Init() )
//	{
//		cclog( "init hacking chat fail.\n" );
//		return false;
//	}
//#endif


	int nUDPPort = MATCHSERVER_DEFAULT_UDP_PORT;
	if( nUDPPort != MGetServerConfig()->GetServerUDPPort() )
		nUDPPort = MGetServerConfig()->GetServerUDPPort();
	if (m_SafeUDP.Create(true, nUDPPort)==false) {
		LOG(LOG_PROG, "Match Server SafeUDP Create FAILED (Port:%d)\n", MATCHSERVER_DEFAULT_UDP_PORT);
		return false;
	}

	// UDP network은 가장 마지막에 Init를 호출하고, 가장 처음 Release를 호출한다.
	m_SafeUDP.SetCustomRecvCallback(UDPSocketRecvEvent);
	

	if(OnCreate() == false) {		
		LOG(LOG_PROG, "Match Server create FAILED (Port:%d)\n", nPort);
		return false;
	}

	m_bCreated = true;
	LOG(LOG_PROG, "Match Server Created (Port:%d)\n", nPort);

	// 디버그용
	g_PointerChecker[0].Init(NULL);
	g_PointerChecker[1].Init(m_pScheduler);
	g_PointerChecker[2].Init(m_pAuthBuilder);

	return true;
}

void CCMatchServer::Destroy()
{
	m_bCreated = false;

	GetQuest()->Destroy();

	for (CCMatchObjectList::iterator ObjItor = m_Objects.begin(); 
		ObjItor != m_Objects.end(); ++ObjItor)
	{
		CCMatchObject* pObj = (*ObjItor).second;
		if (pObj)
		{
			CharFinalize(pObj->GetUID());
		}
	}

	m_ClanMap.Destroy();


	m_ChannelMap.Destroy();

	m_SafeUDP.Destroy();

	GetGambleMachine().Release();

	OnDestroy();
/*
	CCMatchChannelMap::iterator itorChannel = m_ChannelMap.begin();
	while(itorChannel != m_ChannelMap.end()) {
		CCUID uid = (*itorChannel).first;
		ChannelRemove(uid, &itorChannel);
	}
*/

	m_Admin.Destroy();
	m_AsyncProxy.Destroy();
	MGetMatchShop()->Destroy();
	m_MatchDBMgr.Disconnect();

	AgentClear();

	MServer::Destroy();

	CCMatchStringResManager::FreeInstance();

	LOG(LOG_PROG, "Match Server Destoryed\n");
}

void CCMatchServer::Shutdown()
{
	Log(LOG_PROG, "MatchServer Shutting down...\n");
}

bool CCMatchServer::OnCreate()
{
	if( MGetServerConfig()->IsEnabledDuelTournament() ) {		
		GetDTMgr()->Init();
		//OnAsyncRequestDuelTournamentGroupRankingInfo();
		//GetDTMgr()->InsertTestCase();
	}

	return true;
}
void CCMatchServer::OnDestroy()
{
	if( 0 != m_pScheduler ){
		m_pScheduler->Release();
		delete m_pScheduler;
		m_pScheduler = 0;
	}

	if( MGetServerConfig()->IsEnabledDuelTournament() ) {
		GetDTMgr()->Destory();
	}
}

void CCMatchServer::OnRegisterCommand(CCCommandManager* pCommandManager)
{
	CCCommandCommunicator::OnRegisterCommand(pCommandManager);
	MAddSharedCommandTable(pCommandManager, MSCT_MATCHSERVER);
	Log(LOG_PROG, "Command registeration completed\n");

}


void CCMatchServer::OnPrepareRun()
{	
	MServer::OnPrepareRun();
	MGetServerStatusSingleton()->AddCmdCount(m_CommandManager.GetCommandQueueCount());
}

int CCMatchServer::Connect(CCCommObject* pCommObj)
{
	if( pCommObj ) {
		cclog("CCMatchServer::Connect - Target IP(%s), Target Port(%d)",pCommObj->GetIPString(), pCommObj->GetPort());
	}

	return MServer::Connect(pCommObj);
}

int CCMatchServer::OnConnected(CCUID* pTargetUID, CCUID* pAllocUID, unsigned int nTimeStamp, CCCommObject* pCommObj)
{
	if( pCommObj == NULL ) {
		cclog("CCMatchServer::OnConnected - pTargetUID(%d%d), pAllocUID(%d%d)\n", pTargetUID->High, pTargetUID->Low, pAllocUID->High, pAllocUID->Low);
	} else {
		if( pCommObj->IsPassiveSocket() ) {
			cclog("CCMatchServer::OnConnected - pTargetUID(%d%d), pAllocUID(%d%d), IP(%s), Port(%d), Passive Socket\n"
				, pTargetUID->High, pTargetUID->Low, pAllocUID->High, pAllocUID->Low, pCommObj->GetIPString(), pCommObj->GetPort());
		} else {
			cclog("CCMatchServer::OnConnected - pTargetUID(%d%d), pAllocUID(%d%d), IP(%s), Port(%d), Active Socket\n"
				, pTargetUID->High, pTargetUID->Low, pAllocUID->High, pAllocUID->Low, pCommObj->GetIPString(), pCommObj->GetPort());
		}
		
	}

	cclog("CCMatchServer::OnConnected - We should not call func which is called 'CCCommandCommunicator::OnConnected'\n");
	return MOK;
	//return CCCommandCommunicator::OnConnected(pTargetUID, pAllocUID, nTimeStamp, pCommObj);
}

void CCMatchServer::OnRun()
{
//	CheckMemoryCorruption();

	/// space bar 누르면 메모리 침범 오류 나도록 테스트코드
// 	if(GetAsyncKeyState(VK_SPACE))
// 	{
// 		int * parray = new int[10];
// 		for (int i =10; i < 12; i++)
// 			parray[i] = i;
// 	}


//	MGetLocale()->PostLoginInfoToDBAgent(CCUID(1,1), "JM0000726991", "skarlfyd", 1);

#ifdef _DEBUG
//	Sleep(2000);
#endif
	MGetServerStatusSingleton()->SetRunStatus(100);
	// tick count
	SetTickTime(timeGetTime());

	MGetCheckLoopTimeInstance()->SetSchedulerTick();
	// 스케쥴러 목록 업데이트.
	if (m_pScheduler)
		m_pScheduler->Update();

	// PC방 IP캐쉬 업데이트
	MGetCheckLoopTimeInstance()->SetPremiumIPCacheTick();
	MPremiumIPCache()->Update();

	MGetServerStatusSingleton()->SetRunStatus(101);

	MGetCheckLoopTimeInstance()->SetObjectTick();


	// Update Objects
	CCUID uidDisUser;
	unsigned long int nGlobalClock = GetGlobalClockCount();
	unsigned long int nHShieldClock = GetGlobalClockCount();

	for(CCMatchObjectList::iterator i=m_Objects.begin(); i!=m_Objects.end();)
	{
		CCMatchObject* pObj = (*i).second;		
		pObj->Tick(nGlobalClock);

		if( pObj->GetCharInfo() ) {
			if (pObj->GetCharInfo()->GetDBCachingData()->IsRequestUpdate()) {
				UpdateCharDBCachingData(pObj);		///< XP, BP, KillCount, DeathCount 캐슁 업데이트
			}
		}			

		if( !pObj->m_DBJobQ.DBJobQ.empty() && !pObj->m_DBJobQ.bIsRunningAsyncJob )
		{
			CCAsyncJob* pAsyncJob = pObj->m_DBJobQ.DBJobQ.front();
			pObj->m_DBJobQ.DBJobQ.pop_front();
			pObj->m_DBJobQ.nLastJobID = pAsyncJob->GetJobID();
// #ifdef _DEBUG
// 			cclog( "async job id : %d\n", pAsyncJob->GetJobID() );
// #endif
			PostAsyncJob( pAsyncJob );
			pObj->m_DBJobQ.bIsRunningAsyncJob = true;

		}

		if( pObj->CheckDestroy(nGlobalClock) )  
		{
			uidDisUser = pObj->GetUID();

			// Notify Message 필요 -> 사용자 정리 - 해결(이쪽으로 들어오지 않음)
			Disconnect( uidDisUser );		
		}
		else if( pObj->GetDisconnStatusInfo().IsSendDisconnMsg() )
		{
			// DissconnectWait로 설정된 유저는 우선 클라이언트로 접속 종료를 알리고 
			// 다음 루프에서 디비를 업데이트하고 접속 종료를 함.
			CCCommand* pCmd = CreateCommand( MC_MATCH_DISCONNMSG, pObj->GetUID() );
			pCmd->AddParameter( new CCCmdParamUInt(pObj->GetDisconnStatusInfo().GetMsgID()) );
			Post( pCmd );

			// 접속 종료를 클라이언트에 통보된 상태로 설정.
			// 다음 루프에서 이 값을 검사해서 종료를 할건지 결정함.
			pObj->GetDisconnStatusInfo().SendCompleted();
		}
		else if( pObj->GetDisconnStatusInfo().IsDisconnectable(nGlobalClock) )
		{
			uidDisUser = pObj->GetUID();

			// Notify Message 필요 -> 사용자 정리 - 해결(그 전에 모두 처리하고 끊기만함)
			Disconnect( uidDisUser );
		}
		++i;
	}

	MGetServerStatusSingleton()->SetRunStatus(102);

	MGetCheckLoopTimeInstance()->SetStageTick();
	// Update Stages
	for(CCMatchStageMap::iterator iStage=m_StageMap.begin(); iStage!=m_StageMap.end();)
	{
		CCMatchStage* pStage = (*iStage).second;
		pStage->Tick(nGlobalClock);
		if (pStage->GetState() == STAGE_STATE_CLOSE)
		{
			StageRemove(pStage->GetUID(), &iStage);
			continue;
		}
		++iStage;
	}

	MGetServerStatusSingleton()->SetRunStatus(103);

	MGetCheckLoopTimeInstance()->SetChannelTick();
	// Update Channels
	m_ChannelMap.Update(nGlobalClock);

	MGetServerStatusSingleton()->SetRunStatus(104);

	MGetCheckLoopTimeInstance()->SetClanTick();
	// Update Clans
	m_ClanMap.Tick(nGlobalClock);
	
	MGetServerStatusSingleton()->SetRunStatus(105);

	MGetCheckLoopTimeInstance()->SetLadderTick();

	// Update Ladders - 클랜전서버일 경우에만 실행한다.
	if (MGetServerConfig()->GetServerMode() == CSM_CLAN)
	{
		GetLadderMgr()->Tick(nGlobalClock);
	}

	// Duel Tournament가 활성화 되었을 경우에만 실행한다.
	if( MGetServerConfig()->IsEnabledDuelTournament() == true ) {
		GetDTMgr()->Tick(nGlobalClock);
	}
	//////////////////////////////////////////////////////////////////////

	MGetServerStatusSingleton()->SetRunStatus(106);

	// Garbage Session Cleaning
	MGetCheckLoopTimeInstance()->SetPingTick();

#define COREERVAL_GARBAGE_SESSION_PING	(3 * 60 * 1000)	// 3 min
	static unsigned long tmLastGarbageSessionCleaning = nGlobalClock;
	if (nGlobalClock - tmLastGarbageSessionCleaning > COREERVAL_GARBAGE_SESSION_PING)
	{
		tmLastGarbageSessionCleaning = nGlobalClock;

		MemPool<CCCommandParameter>::GetCapacity();
		LOG(LOG_PROG, "GARBAGE SESSION CLEANING : m_This(%d%d), ClientCount=%d, SessionCount=%d, AgentCount=%d, CommandPool=%d, objHistoryCount=%d\n", 
			m_This.High, m_This.Low, GetClientCount(), GetCommObjCount(), GetAgentCount(),
			MemPool<CCCommand>::GetCapacity(),
			m_objectCommandHistory.GetObjectCount() );
		CCCommand* pNew = CreateCommand(MC_NET_PING, CCUID(0,0));
		pNew->AddParameter(new CCCmdParamUInt(GetGlobalClockCount()));
		RouteToAllConnection(pNew);
	}

//	CheckMemoryCorruption();

	MGetServerStatusSingleton()->SetRunStatus(107);

	MGetCheckLoopTimeInstance()->SetSessionCleanTick();
	
	
	// Garbage MatchObject Cleaning
#define COREERVAL_GARBAGE_SESSION_CLEANING	10*60*1000		// 10 min
	for(CCMatchObjectList::iterator i=m_Objects.begin(); i!=m_Objects.end(); i++)
	{
		CCMatchObject* pObj = (CCMatchObject*)((*i).second);
		if (pObj->GetUID() < CCUID(0,3)) continue;	// CCUID로 Client인지 판별할수 있는 코드 필요함
		if (GetTickTime() - pObj->GetTickLastPacketRecved() >= COREERVAL_GARBAGE_SESSION_CLEANING)
		{
			if( pObj->GetDisconnStatusInfo().GetStatus() == CCMDS_CONNECTED)
			{
				LOG(LOG_PROG, "TIMEOUT CLIENT CLEANING : %s(%u%u, %s) (ClientCnt=%d, SessionCnt=%d)", 
					pObj->GetName(), pObj->GetUID().High, pObj->GetUID().Low, pObj->GetIPString(), GetClientCount(), GetCommObjCount());
				pObj->GetDisconnStatusInfo().SetStatus(CCMDS_DISCONN_WAIT);
				pObj->GetDisconnStatusInfo().SetMsgID(MERR_CLEANING_CLIENT);
			}
			// ObjectRemove(uid, &i);
		}
	}

	MGetServerStatusSingleton()->SetRunStatus(108);

	MGetCheckLoopTimeInstance()->SetAsyncJopTick();
	ProcessAsyncJob();

	MGetServerStatusSingleton()->SetRunStatus(109);

	// Update Logs
	MGetCheckLoopTimeInstance()->SetServerLogTick();
	UpdateServerLog();

	MGetCheckLoopTimeInstance()->SetServerDBLogTick();
	UpdateServerStatusDB();
	
	MGetServerStatusSingleton()->SetRunStatus(110);

	MGetCheckLoopTimeInstance()->SetCustomIPListTick();
	// update custom ip list.
	if( 3600000 < (nGlobalClock - m_CountryFilter.GetLastUpdatedTime()) )
	{
		UpdateCustomIPList();
		m_CountryFilter.SetLastUpdatedTime( nGlobalClock );
	}
	
	MGetServerStatusSingleton()->SetRunStatus(111);

	MGetCheckLoopTimeInstance()->SetShutdownTick();
	// Shutdown...
	m_MatchShutdown.OnRun(nGlobalClock);
		
	MGetServerStatusSingleton()->SetRunStatus(112);

	// 초당 1번 정도 objectCommandHistory 와 connectionHistory 업데이트
	static DWORD dwLastCommandHistoryUpdate = nGlobalClock;
	int elapsed = nGlobalClock - dwLastCommandHistoryUpdate;
	if( 1000 < abs(elapsed) )
	{
		dwLastCommandHistoryUpdate = nGlobalClock;
		m_objectCommandHistory.Update( nGlobalClock );
		m_connectionHistory.Update( nGlobalClock );
	}
	
//	CheckMemoryCorruption();

	// TimeStamp가 바뀌었을 때.. 모든 케릭터 정보를 Refresh한다.
	if( GetDTMgr()->GetTimeStampChanged() && MGetServerConfig()->IsEnabledDuelTournament() ) {
		static DWORD dwLastRequestGetDTCharacterInfo = nGlobalClock;

		// 모든 케릭터 정보를 요청하다보면 부하가 걸릴 수 있으므로, 1.5초당 50개씩 요청!
		// 목표는 2분 안에 모두 실행!
		if( nGlobalClock - dwLastRequestGetDTCharacterInfo > 1500 ) {
			dwLastRequestGetDTCharacterInfo = nGlobalClock;

			bool bAllSameTimeStamp = true;			

			int nCount = 0;
			for(CCMatchObjectList::iterator i=m_Objects.begin(); i!=m_Objects.end(); i++) {
				CCMatchObject *pObj = (CCMatchObject*)((*i).second);
				CCMatchObjectDuelTournamentCharInfo* pInfo = pObj->GetDuelTournamentCharInfo();

				if( nCount == 50 ) break;
				if( pInfo != NULL ) 
				{
					// 타임 스탬프가 같은 케릭터가 있을 수도 있다. 다른 케릭터만 요청!
					if( GetDTMgr()->IsSameTimeStamp(pInfo->GetTimeStamp()) == false) {
						bAllSameTimeStamp = false;

						// 현재 게임 듀얼토너먼트 게임에 참여하고 있지 않은 케릭터만 정보 요청!
						if( pObj->GetDuelTournamentCharInfo()->IsJoinDuelTournament() == false ) {						
							LOG(LOG_PROG, "Request Character Info, Character Previous Info(%d%d)", pObj->GetUID().High, pObj->GetUID().Low);

							OnAsyncRequest_GetDuelTournamentCharacterInfo(pObj->GetUID(), pObj->GetCharInfo()->m_nCID);
							OnAsyncRequest_GetDuelTournamentPreviousCharacterInfo(pObj->GetUID(), pObj->GetCharInfo()->m_nCID);
							
							nCount++;
						}					
					}
				}
			}

			if( bAllSameTimeStamp ) { GetDTMgr()->SetTimeStampChanged(false); }
		}		
	}

	if( nGlobalClock - GetBattleTimeRewardMachine().GetLastUpdateTime() > MGetServerConfig()->GetBRDescriptionRefreshInterval() )
	{
		GetBattleTimeRewardMachine().SetLastUpdateTime(nGlobalClock);

		CCAsyncDBJob_GetBattleTimeRewardDescription *pJob = new CCAsyncDBJob_GetBattleTimeRewardDescription;
		CCMatchServer::GetInstance()->PostAsyncJob( pJob );
	}
}

void CCMatchServer::UpdateServerLog()
{
	if (!IsCreated()) return;

#define SERVER_LOG_TICK		(60000)	// 1분 (1000 * 60)

	static unsigned long int st_nElapsedTime = 0;
	static unsigned long int nLastTime = timeGetTime();
	unsigned long int nNowTime = timeGetTime();

	st_nElapsedTime += (nNowTime - nLastTime);

	if (st_nElapsedTime > SERVER_LOG_TICK)
	{
		st_nElapsedTime = 0;

		// 여기서 디비 업데이트
		m_MatchDBMgr.InsertServerLog(MGetServerConfig()->GetServerID(), 
									 (int)m_Objects.size(), (int)m_StageMap.size(), 
									 GetBlockCount(), GetNonBlockCount() );
		ResetBlockCount();
		ResetNonBlockCount();
	}

	nLastTime = nNowTime;
}

void CCMatchServer::UpdateServerStatusDB()
{
	if (!IsCreated()) return;

#define SERVER_STATUS_TICK		(30000)	// 30초 (1000 * 30)

	static unsigned long int st_nElapsedTime = 0;
	static unsigned long int nLastTime = timeGetTime();
	unsigned long int nNowTime = timeGetTime();

	st_nElapsedTime += (nNowTime - nLastTime);

	if (st_nElapsedTime > SERVER_STATUS_TICK)
	{
		st_nElapsedTime = 0;

		int nCurPlayer = (int)m_Objects.size();
		if (nCurPlayer > MGetServerConfig()->GetMaxUser()) 
			nCurPlayer = MGetServerConfig()->GetMaxUser();
	
		bool bResult = false;
		static int st_ErrCounter = 0;


#ifdef LOCALE_KOREA		
		int nNatePlayer = 0;

		for(CCMatchObjectList::iterator iter = m_Objects.begin(); iter != m_Objects.end(); iter++) 
		{
			CCMatchObject* pObj = (CCMatchObject*)iter->second;
			if( pObj->GetAccountInfo()->m_nCCode == 30 ) { nNatePlayer++; }
		}

		bResult = m_MatchDBMgr.UpdateServerStatus_Netmarble(MGetServerConfig()->GetServerID(), nCurPlayer, nNatePlayer);
#else
		bResult = m_MatchDBMgr.UpdateServerStatus(MGetServerConfig()->GetServerID(), nCurPlayer);
#endif

		if (bResult == false) 
		{
			LOG(LOG_PROG, "[CRITICAL ERROR] DB Connection Lost.\n");

			m_MatchDBMgr.Disconnect();

			InitDB();
			st_ErrCounter++;
			if (st_ErrCounter > MAX_DB_QUERY_COUNT_OUT) 
			{
				LOG(LOG_PROG, "[CRITICAL ERROR] UpdateServerStatusDB - Shutdown\n");
				Shutdown();
			}
		}
		else
		{
			st_ErrCounter = 0;
		}
	}

	nLastTime = nNowTime;
}

inline void CCMatchServer::RouteToListener(CCObject* pObject, CCCommand* pCommand)
{
	if (pObject == NULL) return;

	size_t nListenerCount = pObject->m_CommListener.size();
	if (nListenerCount <= 0) {
		delete pCommand;
		return;
	} else if (nListenerCount == 1) {
		CCUID TargetUID = *pObject->m_CommListener.begin();
		pCommand->m_Receiver = TargetUID;
		Post(pCommand);
	} else {
		int nCount = 0;
		for (list<CCUID>::iterator itorUID=pObject->m_CommListener.begin(); itorUID!=pObject->m_CommListener.end(); itorUID++) {
			CCUID TargetUID = *itorUID;

			CCCommand* pSendCmd;
			if (nCount<=0)
				pSendCmd = pCommand;
			else
				pSendCmd = pCommand->Clone();
			pSendCmd->m_Receiver = TargetUID;
			Post(pSendCmd);
			nCount++;
		}
	}
}

void CCMatchServer::RouteResponseToListener(CCObject* pObject, const int nCmdID, int nResult)
{
	CCCommand* pNew = CreateCommand(nCmdID, CCUID(0,0));
	pNew->AddParameter(new CCCmdParamInt(nResult));
	RouteToListener(pObject, pNew);
}

struct stRouteListenerNode
{
	DWORD				nUserContext;
	CCPacketCrypterKey	CryptKey;
	//SEED_ALG_INFO	CryptAlgInfo;
};

void CCMatchServer::RouteToAllConnection(CCCommand* pCommand)
{
	queue<stRouteListenerNode*>	ListenerList;

	// Queueing for SafeSend
	LockCommList();
		for(CCUIDRefCache::iterator i=m_CommRefCache.begin(); i!=m_CommRefCache.end(); i++){
			CCCommObject* pCommObj = (CCCommObject*)((*i).second);
			if (pCommObj->GetUID() < CCUID(0,3)) continue;	// CCUID로 Client인지 판별할수 있는 코드 필요함

			stRouteListenerNode* pNewNode = new stRouteListenerNode;
			pNewNode->nUserContext = pCommObj->GetUserContext();
			memcpy(&pNewNode->CryptKey, pCommObj->GetCrypter()->GetKey(), sizeof(CCPacketCrypterKey));
			ListenerList.push(pNewNode);
		}
	UnlockCommList();

	// Send the queue (each all session)
	int nCmdSize = pCommand->GetSize();

	if (nCmdSize <= 0)
	{
		while (!ListenerList.empty())
		{
			stRouteListenerNode* pNode = ListenerList.front();
			ListenerList.pop();
			delete pNode;
		}
		return;
	}

	char* pCmdData = new char[nCmdSize];
	int nSize = pCommand->GetData(pCmdData, nCmdSize);
	_ASSERT(nSize < MAX_PACKET_SIZE && nSize==nCmdSize);


	if (pCommand->m_pCommandDesc->IsFlag(MCCT_NON_ENCRYPTED))
	{
		while (!ListenerList.empty())
		{
			stRouteListenerNode* pNode = ListenerList.front();
			ListenerList.pop();

			SendMsgCommand(pNode->nUserContext, pCmdData, nSize, MSGID_RAWCOMMAND, NULL);

			delete pNode;
		}
	}
	else
	{
		while (!ListenerList.empty())
		{
			stRouteListenerNode* pNode = ListenerList.front();
			ListenerList.pop();

			SendMsgCommand(pNode->nUserContext, pCmdData, nSize, MSGID_COMMAND, &pNode->CryptKey);

			delete pNode;
		}
	}

	delete [] pCmdData;
	delete pCommand;
}

void CCMatchServer::RouteToAllClient(CCCommand* pCommand)
{
	for(CCMatchObjectList::iterator i=m_Objects.begin(); i!=m_Objects.end(); i++){
		CCMatchObject* pObj = (CCMatchObject*)((*i).second);
		if (pObj->GetUID() < CCUID(0,3)) continue;	// CCUID로 Client인지 판별할수 있는 코드 필요함
		
		CCCommand* pSendCmd = pCommand->Clone();
		pSendCmd->m_Receiver = pObj->GetUID();
		Post(pSendCmd);
	}	
	delete pCommand;
}

void CCMatchServer::RouteToChannel(const CCUID& uidChannel, CCCommand* pCommand)
{
	CCMatchChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) 
	{
		delete pCommand;
		return;
	}

	for (CCUIDRefCache::iterator i=pChannel->GetObjBegin(); i!=pChannel->GetObjEnd(); i++) {
		CCObject* pObj = (CCObject*)(*i).second;

		CCCommand* pSendCmd = pCommand->Clone();
		RouteToListener(pObj, pSendCmd);
	}
	delete pCommand;
}

void CCMatchServer::RouteToChannelLobby(const CCUID& uidChannel, CCCommand* pCommand)
{
	CCMatchChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) 
	{
		delete pCommand;
		return;
	}

	for (CCUIDRefCache::iterator i=pChannel->GetLobbyObjBegin(); i!=pChannel->GetLobbyObjEnd(); i++) 
	{
		CCObject* pObj = (CCObject*)(*i).second;

		CCCommand* pSendCmd = pCommand->Clone();
		RouteToListener(pObj, pSendCmd);
	}
	delete pCommand;
}

void CCMatchServer::RouteToStage(const CCUID& uidStage, CCCommand* pCommand)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) 
	{
		delete pCommand;
		return;
	}

	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {
//		CCObject* pObj = (CCObject*)(*i).second;

		CCUID uidObj = (CCUID)(*i).first;
		CCObject* pObj = (CCObject*)GetObject(uidObj);
		if (pObj) {
			CCCommand* pSendCmd = pCommand->Clone();
			RouteToListener(pObj, pSendCmd);
		} else {
			LOG(LOG_PROG, "WARNING(RouteToStage) : Not Existing Obj(%u:%u)\n", uidObj.High, uidObj.Low);
			i=pStage->RemoveObject(uidObj);	// RAONHAJE : 방에 쓰레기UID 남는것 발견시 로그&청소
			LogObjectCommandHistory(uidObj);
		}
	}
	delete pCommand;
}

void CCMatchServer::RouteToObjInStage(const CCUID& uidStage, const CCUID& uidTargetObj, CCCommand* pCommand)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) 
	{
		delete pCommand;
		return;
	}

	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {
		//		CCObject* pObj = (CCObject*)(*i).second;

		CCUID uidObj = (CCUID)(*i).first;
		if (uidObj == uidTargetObj) {
			CCObject* pObj = (CCObject*)GetObject(uidObj);
			if (pObj) {
				CCCommand* pSendCmd = pCommand->Clone();
				RouteToListener(pObj, pSendCmd);
			} else {
				LOG(LOG_PROG, "WARNING(RouteToStage) : Not Existing Obj(%u:%u)\n", uidObj.High, uidObj.Low);
				i=pStage->RemoveObject(uidObj);	// RAONHAJE : 방에 쓰레기UID 남는것 발견시 로그&청소
				LogObjectCommandHistory(uidObj);
			}
		}
	}
	delete pCommand;
}

void CCMatchServer::RouteToStageWaitRoom(const CCUID& uidStage, CCCommand* pCommand)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) 
	{
		delete pCommand;
		return;
	}

	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {

		CCUID uidObj = (CCUID)(*i).first;
		CCMatchObject* pObj = (CCMatchObject*)GetObject(uidObj);
		if (pObj) {
			if (! pObj->GetEnterBattle())
			{
				CCCommand* pSendCmd = pCommand->Clone();
				RouteToListener(pObj, pSendCmd);
			} 
		}
	}
	delete pCommand;
}

void CCMatchServer::RouteToBattle(const CCUID& uidStage, CCCommand* pCommand)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) 
	{
		delete pCommand;
		return;
	}

	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {
		//CCMatchObject* pObj = (CCMatchObject*)(*i).second;

		CCUID uidObj = (CCUID)(*i).first;
		CCMatchObject* pObj = (CCMatchObject*)GetObject(uidObj);
		if (pObj) {
			if (pObj->GetEnterBattle())
			{
				CCCommand* pSendCmd = pCommand->Clone();
				RouteToListener(pObj, pSendCmd);
			} 
		}else {
			LOG(LOG_PROG, "WARNING(RouteToBattle) : Not Existing Obj(%u:%u)\n", uidObj.High, uidObj.Low);
			i=pStage->RemoveObject(uidObj);	// RAONHAJE : 방에 쓰레기UID 남는것 발견시 로그&청소
			LogObjectCommandHistory(uidObj);
		}
	}
	delete pCommand;
}

void CCMatchServer::RouteToClan(const int nCLID, CCCommand* pCommand)
{
	CCMatchClan* pClan = FindClan(nCLID);
	if (pClan == NULL) 
	{
		delete pCommand;
		return;
	}

	for (CCUIDRefCache::iterator i=pClan->GetMemberBegin(); i!=pClan->GetMemberEnd(); i++) {
		CCObject* pObj = (CCObject*)(*i).second;

		CCCommand* pSendCmd = pCommand->Clone();
		RouteToListener(pObj, pSendCmd);
	}
	delete pCommand;
}

void CCMatchServer::ResponseRoundState(const CCUID& uidStage)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	CCMatchRule* pRule = pStage->GetRule();
	if (pRule == NULL) return;

	CCCommand* pCmd = CreateCommand(MC_MATCH_GAME_ROUNDSTATE, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterUID(pStage->GetUID()));
	pCmd->AddParameter(new CCCommandParameterInt(pRule->GetRoundCount()));
	pCmd->AddParameter(new CCCommandParameterInt(pRule->GetRoundState()));
	pCmd->AddParameter(new CCCommandParameterInt(pRule->GetRoundArg()));

	// 게임 안에 있는 플레이어에게만 전송
	RouteToBattle(uidStage, pCmd);
}

void CCMatchServer::ResponseRoundState(CCMatchObject* pObj, const CCUID& uidStage)
{
	if (pObj == NULL) return;
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	CCMatchRule* pRule = pStage->GetRule();
	if (pRule == NULL) return;

	CCCommand* pCmd = CreateCommand(MC_MATCH_GAME_ROUNDSTATE, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterUID(pStage->GetUID()));
	pCmd->AddParameter(new CCCommandParameterInt(pRule->GetRoundCount()));
	pCmd->AddParameter(new CCCommandParameterInt(pRule->GetRoundState()));
	pCmd->AddParameter(new CCCommandParameterInt(pRule->GetRoundArg()));

	RouteToListener(pObj, pCmd);
}

void CCMatchServer::NotifyMessage(const CCUID& uidChar, int nMsgID)
{
	CCMatchObject* pObj = GetObject(uidChar);
	if (pObj == NULL) return;
	
	CCCommand* pNew=new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_NOTIFY), CCUID(0,0), m_This);
	pNew->AddParameter(new CCCommandParameterUInt(nMsgID));
	RouteToListener(pObj, pNew);
}

int CCMatchServer::ObjectAdd(const CCUID& uidComm)
{
	CCMatchObject* pObj = new CCMatchObject(uidComm);
	pObj->UpdateTickLastPacketRecved();

	m_Objects.insert(CCMatchObjectList::value_type(pObj->GetUID(), pObj));
//	*pAllocUID = pObj->GetUID();

	//LOG("Character Added (UID:%d%d)", pObj->GetUID().High, pObj->GetUID().Low);

	return MOK;
}

int CCMatchServer::ObjectRemove(const CCUID& uid, CCMatchObjectList::iterator* pNextItor)
{
	CCMatchObjectList::iterator i = m_Objects.find(uid);
	if(i==m_Objects.end()) return MERR_OBJECT_INVALID;

	CCMatchObject* pObj = (*i).second;

	// move. OnRun() -> ObjectRemove. - by SungE 2007-05-03
	// 이전에 BlockType을 설정했다면 DB update flag가 설정된다.
	if( pObj->GetDisconnStatusInfo().IsUpdateDB() ) 
	{
		DWORD	dwCID = 0;
		string	strChannelName;

		if( NULL != pObj->GetCharInfo() )
			dwCID = pObj->GetCharInfo()->m_nCID;

		CCMatchChannel* pChannel = GetChannelMap()->Find( pObj->GetChannelUID() );
		if( NULL != pChannel )
			strChannelName = pChannel->GetName();

		CCAsyncDBJob_SetBlockHacking* pJob = new CCAsyncDBJob_SetBlockHacking(uid);

		pJob->Input( pObj->GetAccountInfo()->m_nAID
			, dwCID
			, pObj->GetDisconnStatusInfo().GetHackingType()
			, pObj->GetDisconnStatusInfo().GetBlockLevel()
			, pObj->GetDisconnStatusInfo().GetComment()
			, pObj->GetIPString()
			, pObj->GetDisconnStatusInfo().GetEndDate()
			, MGetServerConfig()->GetServerID()
			, strChannelName );

		// PostAsyncJob( pJob );

		pObj->m_DBJobQ.DBJobQ.push_back( pJob );

		// 업데이트하면 다시 DB update flag를 무효화 함.
		pObj->GetDisconnStatusInfo().UpdateDataBaseCompleted();
	}

	// Clear up the Object
	if (pObj->GetChatRooCCUID() != CCUID(0,0)) {
		CCMatchChatRoomMgr* pChatRoomMgr = GetChatRoomMgr();
		CCMatchChatRoom* pRoom = pChatRoomMgr->FindChatRoom(pObj->GetChatRooCCUID());
		if (pRoom)
			pRoom->RemovePlayer(pObj->GetUID());
	}

	if (pObj->GetStageUID()   != CCUID(0,0)){ StageLeaveBattle(pObj->GetUID(), true, true); }
	if (pObj->GetStageUID()   != CCUID(0,0)){ StageLeave(pObj->GetUID());	}
	if (pObj->GetChannelUID() != CCUID(0,0)){ ChannelLeave(pObj->GetUID(), pObj->GetChannelUID()); }
	
	// m_ClanMap에서도 삭제
	m_ClanMap.RemoveObject(pObj->GetUID(), pObj);

	CharFinalize( pObj->GetUID() );

	/* 비정상적으로 죽는 부분이라 디버깅을 위해서 나눠놨음.
	 CharInfo가 생성되기 전에 죽는 것인지 확인하는게 목적.
	*/
	// 소멸자에서 호출은 하지만 먼저 명시적으로 해주자.
	if( NULL != pObj->GetCharInfo() )
	{
		pObj->FreeCharInfo();
		pObj->FreeDuelTournamentInfo();
		pObj->FreeFriendInfo();
	}

	for( list<CCAsyncJob*>::iterator itAJob = pObj->m_DBJobQ.DBJobQ.begin()
		; itAJob != pObj->m_DBJobQ.DBJobQ.end()
		; itAJob = pObj->m_DBJobQ.DBJobQ.begin() )
	{
		CCAsyncJob* pJob = *itAJob;

		pObj->m_DBJobQ.DBJobQ.pop_front();

		PostAsyncJob( pJob );		
	}
	_ASSERT( pObj->m_DBJobQ.DBJobQ.empty() );
	
	delete pObj;
	pObj = NULL;

	CCMatchObjectList::iterator itorTemp = m_Objects.erase(i);
	if (pNextItor)
		*pNextItor = itorTemp;

	// dump object history test
	// m_objectCommandHistory.Dump(uid);

	return MOK;
}

CCMatchObject* CCMatchServer::GetObject(const CCUID& uid)
{
	CCMatchObjectList::iterator i = m_Objects.find(uid);
	if(i==m_Objects.end()) return NULL;
	return (*i).second;
}

CCMatchObject* CCMatchServer::GetPlayerByComCCUID(const CCUID& uid)
{
	for(CCMatchObjectList::iterator i=m_Objects.begin(); i!=m_Objects.end(); i++){
		CCMatchObject* pObj = ((*i).second);
		for (list<CCUID>::iterator j=pObj->m_CommListener.begin(); j!=pObj->m_CommListener.end(); j++){
			CCUID TargetUID = *j;
			if (TargetUID == uid)
				return pObj;
		}
	}
	return NULL;
}

CCMatchObject* CCMatchServer::GetPlayerByName(const char* pszName)
{
	for(CCMatchObjectList::iterator i=m_Objects.begin(); i!=m_Objects.end(); i++){
		CCMatchObject* pObj = ((*i).second);
		if (stricmp(pObj->GetName(), pszName) == 0)
			return pObj;
	}
	return NULL;
}

CCMatchObject* CCMatchServer::GetPlayerByAID(unsigned long int nAID)
{
	if (nAID == 0) return NULL;

	for(CCMatchObjectList::iterator i=m_Objects.begin(); i!=m_Objects.end(); i++)
	{
		CCMatchObject* pObj = ((*i).second);
		if (pObj->GetAccountInfo()->m_nAID == nAID)
			return pObj;
	}
	return NULL;
}





CCUID CCMatchServer::UseUID()
{
	LockUIDGenerate();
		CCUID ret = m_NextUseUID;
		m_NextUseUID.Increase();
	UnlockUIDGenerate();
	return ret;
}

void CCMatchServer::SetClientClockSynchronize(const CCUID& ComCCUID)
{
	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_CLOCK_SYNCHRONIZE), ComCCUID, m_This);
	pNew->AddParameter(new CCCommandParameterUInt(GetGlobalClockCount()));
	Post(pNew);
}

void CCMatchServer::Announce(const CCUID& ComCCUID, char* pszMsg)
{
	CCCommand* pCmd = CreateCommand(MC_MATCH_ANNOUNCE, ComCCUID);
	pCmd->AddParameter(new CCCmdParamUInt(0));
	pCmd->AddParameter(new CCCmdParamStr(pszMsg));
	Post(pCmd);
}

void CCMatchServer::Announce(CCObject* pObj, char* pszMsg)
{
	CCCommand* pCmd = CreateCommand(MC_MATCH_ANNOUNCE, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamUInt(0));
	pCmd->AddParameter(new CCCmdParamStr(pszMsg));
	RouteToListener(pObj, pCmd);
}

void CCMatchServer::AnnounceErrorMsg(const CCUID& ComCCUID, const int nErrorCode)
{
	// 다음 서버 패치때 Announce대신 ErrorCode로 클라이언트에 메세지를 전송할 수 있도록 만들 예정
}

void CCMatchServer::AnnounceErrorMsg(CCObject* pObj, const int nErrorCode)
{

}




void CCMatchServer::OnBridgePeer(const CCUID& uidChar, DWORD dwIP, DWORD nPort)
{
	CCMatchObject* pObj = GetObject(uidChar);
	if (pObj == NULL) return;
// 임시 Debug코드 ///////
// #ifdef _DEBUG
// if (strcmp(pObj->GetAccountName(), "라온하제4")==0)
// return;
// if (strcmp(pObj->GetAccountName(), "라온하제3")==0)
// return;
// #endif
/////////////////////////

/*	if (pObj->GetBridgePeer() == false) {
		char szMsg[128];
		sprintf(szMsg, "BridgePeer : Player[%d%d] Local(%s:%u) , Routed(%s:%u)", 
			uidChar.High, uidChar.Low, szLocalIP, nLocalPort, szIP, nPort);
		LOG(LOG_DEBUG, szMsg);
	}*/

	in_addr addr;
	addr.s_addr = dwIP;
	char* pszIP = inet_ntoa(addr);

	pObj->SetPeerAddr(dwIP, pszIP, (unsigned short)nPort);
	pObj->SetBridgePeer(true);
	pObj->SetPlayerFlag(CCTD_PlayerFlags_BridgePeer, true);

	ResponseBridgePeer(uidChar, 0);
}

CCMatchServer* CCMatchServer::GetInstance()
{
	return m_pInstance;
}

unsigned long int CCMatchServer::GetGlobalClockCount() const
{
	unsigned long int i = timeGetTime();
	return i;
}

unsigned long int CCMatchServer::ConvertLocalClockToGlobalClock(unsigned long int nLocalClock, unsigned long int nLocalClockDistance)
{
	return (nLocalClock + nLocalClockDistance);
}

unsigned long int CCMatchServer::ConvertGlobalClockToLocalClock(unsigned long int nGlobalClock, unsigned long int nLocalClockDistance)
{
	return (nGlobalClock - nLocalClockDistance);
}

void CCMatchServer::DebugTest()
{
#ifndef _DEBUG
	return;
#endif

///////////
	LOG(LOG_DEBUG, "DebugTest: Object List");
	for(CCMatchObjectList::iterator it=m_Objects.begin(); it!=m_Objects.end(); it++){
		CCMatchObject* pObj = (*it).second;
		LOG(LOG_DEBUG, "DebugTest: Obj(%d%d)", pObj->GetUID().High, pObj->GetUID().Low);
	}
///////////
}

void CCMatchServer::SendCommandByUDP(CCCommand* pCommand, char* szIP, int nPort)
{
	_ASSERT(0);	// 사용하지 않음
	// 현재는 1024 size이상 보낼 수 없다.
	const int BUF_SIZE = 1024;

	char* szBuf = new char[BUF_SIZE];
	int iMaxPacketSize = BUF_SIZE;

	CCPacketHeader a_PacketHeader;
	int iHeaderSize = sizeof(a_PacketHeader);
	int size = pCommand->GetData(szBuf + iHeaderSize, iMaxPacketSize - iHeaderSize);
	size += iHeaderSize;
	a_PacketHeader.nMsg = MSGID_COMMAND;
	a_PacketHeader.nSize = size;
	memcpy(szBuf, &a_PacketHeader, iHeaderSize);

	bool bRet = m_SafeUDP.Send(szIP, nPort, szBuf, size);
}

bool CCMatchServer::UDPSocketRecvEvent(DWORD dwIP, WORD wRawPort, char* pPacket, DWORD dwSize)
{
	static DWORD dwMonitorUDPIP = MGetServerConfig()->GetMonitorUDPIP();

	if( dwMonitorUDPIP == dwIP )
	{
		_ASSERT( 0 );
//		MGetMatchServer()->SafePushMonitorUDP( dwIP, wRawPort, pPacket, dwSize );
		return true;
	}

	if (dwSize < sizeof(CCPacketHeader)) return false;

	CCPacketHeader*	pPacketHeader;
	pPacketHeader = (CCPacketHeader*)pPacket;

	if ((dwSize < pPacketHeader->nSize) || 
		((pPacketHeader->nMsg != MSGID_COMMAND)&&(pPacketHeader->nMsg != MSGID_RAWCOMMAND))	) return false;

	CCMatchServer* pServer = CCMatchServer::GetInstance();
	pServer->ParseUDPPacket(&pPacket[sizeof(CCPacketHeader)], pPacketHeader, dwIP, wRawPort);
	return true;
}

void CCMatchServer::ParseUDPPacket(char* pData, CCPacketHeader* pPacketHeader, DWORD dwIP, WORD wRawPort)
{
	switch (pPacketHeader->nMsg)
	{
	case MSGID_RAWCOMMAND:
		{
			unsigned short nCheckSum = MBuildCheckSum(pPacketHeader, pPacketHeader->nSize);
			if (pPacketHeader->nCheckSum != nCheckSum) {
				static int nLogCount = 0;
				if (nLogCount++ < 100) {	// Log Flooding 방지
					cclog("CCMatchServer::ParseUDPPacket() -> CHECKSUM ERROR(R=%u/C=%u)\n", 
						pPacketHeader->nCheckSum, nCheckSum);
				}
				return;
			} else {
				unsigned short nTotalSize = 0;
				memcpy(&nTotalSize, pData, sizeof(nTotalSize));

				CCCommand* pCmd = new CCCommand();
				if( !pCmd->SetData(pData, &m_CommandManager) )
				{
					delete pCmd;
					cclog( "fail ParseUDPPacket.\n" );
					return;
				}

				if (pCmd->GetID() == MC_MATCH_BRIDGEPEER) {
					pCmd->m_Sender = CCUID(0,0);
					pCmd->m_Receiver = m_This;

					unsigned long nPort = ntohs(wRawPort);

					CCCommandParameterUInt* pParamIP = (CCCommandParameterUInt*)pCmd->GetParameter(1);
					CCCommandParameterUInt* pParamPort = (CCCommandParameterUInt*)pCmd->GetParameter(2);
					if (pParamIP==NULL || pParamIP->GetType()!=MPT_UINT)
					{
						delete pCmd;
						break;
					}
					if (pParamPort==NULL || pParamPort->GetType()!=MPT_UINT)
					{
						delete pCmd;
						break;
					}

					char pData[64];
					CCCommandParameterUInt(dwIP).GetData(pData, 64);
					pParamIP->SetData(pData);
					CCCommandParameterUInt(nPort).GetData(pData, 64);
					pParamPort->SetData(pData);

					PostSafeQueue(pCmd);
				} else {
					LOG(LOG_FILE, "CCMatchServer::ParseUDPPacket: NOT HANDLED COMMAND(%d)", pCmd->GetID());
				}
			}
		}
		break;
	case MSGID_COMMAND:
		{
// 			_ASSERT(0);
			// 서버상에 암호화된 UDP는 사용하지 않음
			Log(LOG_FILE, "CCMatchServer::ParseUDPPacket: Parse Packet Error");
		}
		break;
	default:
		{
// 			_ASSERT(0);
			Log(LOG_FILE, "CCMatchServer::ParseUDPPacket: Parse Packet Error");
		}

		break;
	}
}

void CCMatchServer::ResponseBridgePeer(const CCUID& uidChar, int nCode)
{
	CCMatchObject* pObj = GetObject(uidChar);
	if (pObj == NULL) return;

	CCCommand* pNew = CreateCommand(MC_MATCH_BRIDGEPEER_ACK, CCUID(0,0));
	pNew->AddParameter(new CCCmdParaCCUID(uidChar));
	pNew->AddParameter(new CCCmdParamInt(nCode));
	RouteToListener(pObj, pNew);
}

// 난입한 유저가 방안에 있는 다른 사람들 정보 달라고 요청했을때 방안의 유저정보를 알려준다
void CCMatchServer::ResponsePeerList(const CCUID& uidChar, const CCUID& uidStage)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	CCMatchObject* pObj = GetObject(uidChar);
	if (pObj == NULL) return;

	CCCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_PEERLIST, CCUID(0,0));
	pNew->AddParameter(new CCCommandParameterUID(pStage->GetUID()));

	// Battle에 들어간 사람만 List를 만든다.
	int nPeerCount = pStage->GetObjInBattleCount();

	void* pPeerArray = CCMakeBlobArray(sizeof(CCTD_PeerListNode), nPeerCount);
	int nIndex=0;
	for (CCUIDRefCache::iterator itor=pStage->GetObjBegin(); itor!=pStage->GetObjEnd(); itor++) {
		CCMatchObject* pObj = (CCMatchObject*)(*itor).second;
		if (pObj->GetEnterBattle() == false) continue;

		CCTD_PeerListNode* pNode = (CCTD_PeerListNode*)CCGetBlobArrayElement(pPeerArray, nIndex++);
		memset(pNode, 0, sizeof(CCTD_PeerListNode));
		
		pNode->uidChar = pObj->GetUID();
		pNode->dwIP = pObj->GetIP();
		pNode->nPort = pObj->GetPort();

		CopyCharInfoForTrans(&pNode->CharInfo, pObj->GetCharInfo(), pObj);
		//버프정보임시주석 CopyCharBuffInfoForTrans(&pNode->CharBuffInfo, pObj->GetCharInfo(), pObj);

		pNode->ExtendInfo.nPlayerFlags = pObj->GetPlayerFlags();
		if (pStage->GetStageSetting()->IsTeamPlay())	pNode->ExtendInfo.nTeam = (char)pObj->GetTeam();
		else								 			pNode->ExtendInfo.nTeam = 0;		
	}

	pNew->AddParameter(new CCCommandParameterBlob(pPeerArray, CCGetBlobArraySize(pPeerArray)));
	CCEraseBlobArray(pPeerArray);

	RouteToListener(pObj, pNew);
}


bool CCMatchServer::CheckBridgeFault()
{
	for (CCMatchObjectList::iterator i=m_Objects.begin(); i!=m_Objects.end(); i++) {
		CCMatchObject* pObj = (*i).second;
		if (pObj->GetBridgePeer() == false)
			return true;
	}
	return false;
}




void CCMatchServer::OnUserWhisper(const CCUID& uidComm, char* pszSenderName, char* pszTargetName, char* pszMessage)
{
	if (strlen(pszSenderName) < 2) return;
	if (strlen(pszTargetName) < 2) return;

	CCMatchObject* pObj = GetPlayerByComCCUID(uidComm);
	if (pObj == NULL) return;

	CCMatchObject* pTargetObj = GetPlayerByName(pszTargetName);
	if (pTargetObj == NULL) {
		NotifyMessage(pObj->GetUID(), MATCHNOTIFY_GENERAL_USER_NOTFOUND);
		return;
	}
	if (pTargetObj->CheckUserOption(MBITFLAG_USEROPTION_REJECT_WHISPER) == true) {
		NotifyMessage(pObj->GetUID(), MATCHNOTIFY_USER_WHISPER_REJECTED);
//		NotifyMessage(pTargetObj->GetUID(), MATCHNOTIFY_USER_WHISPER_IGNORED);
		return;
	}

	CCCommand* pCmd = CreateCommand(MC_MATCH_USER_WHISPER, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamStr(pObj->GetName()));
	pCmd->AddParameter(new CCCmdParamStr(pszTargetName));
	pCmd->AddParameter(new CCCmdParamStr(pszMessage));
	RouteToListener(pTargetObj, pCmd);
}

void CCMatchServer::OnUserWhere(const CCUID& uidComm, char* pszTargetName)
{
	if (strlen(pszTargetName) < 2) return;

	CCMatchObject* pObj = GetPlayerByComCCUID(uidComm);
	if (!IsEnabledObject(pObj)) return;

	CCMatchObject* pTargetObj = GetPlayerByName(pszTargetName);
	if (pTargetObj == NULL) {
		NotifyMessage(pObj->GetUID(), MATCHNOTIFY_GENERAL_USER_NOTFOUND);
		return;
	}

	if ((IsAdminGrade(pObj) == false) && (IsAdminGrade(pTargetObj) == true)) {
		NotifyMessage(pObj->GetUID(), MATCHNOTIFY_GENERAL_USER_NOTFOUND);
		return;
	}

	char szLog[256] = "";

	bool bUnknownChannel = true;

	CCMatchChannel* pChannel = FindChannel(pTargetObj->GetChannelUID());
	if (pChannel) {
		if (pTargetObj->GetPlace() == MMP_LOBBY)
		{
			bUnknownChannel = false;
			sprintf( szLog, "[%s] '%s'",
				pTargetObj->GetName(), 
				pChannel->GetName() );
		}
		else if ((pTargetObj->GetPlace() == MMP_STAGE) || (pTargetObj->GetPlace() == MMP_BATTLE))
		{
			CCMatchStage* pStage = FindStage( pTargetObj->GetStageUID() );
			if( 0 != pStage )
			{
				bUnknownChannel = false;
				sprintf( szLog, "[%s] '%s' , '(%d)%s'",
					pTargetObj->GetName(), 
					pChannel->GetName(), 
					pStage->GetIndex()+1,
					pStage->GetName() );
			}
		}
	}
	
	if (bUnknownChannel)
		sprintf(szLog, "%s , Unknown Channel", pTargetObj->GetName());

	Announce(pObj, szLog);
}

void CCMatchServer::OnUserOption(const CCUID& uidComm, unsigned long nOptionFlags)
{
	CCMatchObject* pObj = GetPlayerByComCCUID(uidComm);
	if (pObj == NULL) return;

	pObj->SetUserOption(nOptionFlags);
}

void CCMatchServer::OnChatRoomCreate(const CCUID& uidPlayer, const char* pszChatRoomName)
{
	CCMatchChatRoomMgr* pChatRoomMgr = GetChatRoomMgr();
	CCMatchChatRoom* pRoom = pChatRoomMgr->FindChatRoomByName(pszChatRoomName);
	if (pRoom) {
		NotifyMessage(uidPlayer, MATCHNOTIFY_CHATROOM_ALREADY_EXIST);	// Notify Already Exist
		return;
	}

	pRoom = pChatRoomMgr->AddChatRoom(uidPlayer, pszChatRoomName);
	if (pRoom == NULL) {
		NotifyMessage(uidPlayer, MATCHNOTIFY_CHATROOM_CREATE_FAILED);	// Notify Can't Create
		return;
	}

	if (pRoom->AddPlayer(uidPlayer) == true) {
//		LOG(LOG_PROG, "ChatRoom Created : '%s' ", pszChatRoomName);
		NotifyMessage(uidPlayer, MATCHNOTIFY_CHATROOM_CREATE_SUCCEED);
	} else {
		NotifyMessage(uidPlayer, MATCHNOTIFY_CHATROOM_JOIN_FAILED);		// Notify Join Failed
	}
}

void CCMatchServer::OnChatRoomJoin(const CCUID& uidComm, char* pszPlayerName, char* pszChatRoomName)
{
	CCMatchChatRoomMgr* pChatRoomMgr = GetChatRoomMgr();
	CCMatchChatRoom* pRoom = pChatRoomMgr->FindChatRoomByName(pszChatRoomName);

	// 2008.08.28 채팅룸 부수적 문제 해결 http://dev:8181/projects/gunz/ticket/158
	// 생성되지 않은 채팅방에 참가시도시 “존재하지 않는 채팅방입니다”라는 시스템메세지 출력
	if (pRoom == NULL) {
        NotifyMessage(uidComm, MATCHNOTIFY_CHATROOM_NOT_EXIST);			// Notify Does not Exist
		return;
	}
	
	CCMatchObject* pObj = GetPlayerByComCCUID(uidComm);
	if (pObj == NULL) return;

	if (pRoom->GetUserCount() > CHATROOM_MAX_ROOMMEMBER) {
		NotifyMessage(uidComm, MATCHNOTIFY_CHATROOM_USER_FULL);			// Notify Full Member
		return;
	}

	if (pRoom->AddPlayer(uidComm)) {
		// Notify Joinning to Participant
		CCCommand* pCmd = CreateCommand(MC_MATCH_CHATROOM_JOIN, CCUID(0,0));
		pCmd->AddParameter(new CCCmdParamStr(pObj->GetName()));
		pCmd->AddParameter(new CCCmdParamStr(pszChatRoomName));
		pRoom->RouteCommand(pCmd);
	} else {
		NotifyMessage(uidComm, MATCHNOTIFY_CHATROOM_JOIN_FAILED);		// Notify Join a room Failed
	}
}

void CCMatchServer::OnChatRoomLeave(const CCUID& uidComm, char* pszPlayerName, char* pszChatRoomName)
{
	CCMatchChatRoomMgr* pChatRoomMgr = GetChatRoomMgr();
	CCMatchChatRoom* pRoom = pChatRoomMgr->FindChatRoomByName(pszChatRoomName);
	if (pRoom == NULL)
		return;

	CCMatchObject* pObj = GetPlayerByComCCUID(uidComm);
	if (pObj == NULL) return;

	pRoom->RemovePlayer(uidComm);

	// 2008.08.28 채팅룸 개설후 파괴가 안돼 생기는 문제 http://dev:8181/projects/gunz/ticket/158
	// 해결방안 채팅룸 개설시 개설자가 꼭참여 되도록 로직이 되어있기 때문에 
	// 그 채팅룸에서 유저가 탈퇴시 유저Count를 확인후 채팅룸에 유저가 한명도없으면 채팅룸 파괴해준다.
	if(!pRoom->GetUserCount())
		pChatRoomMgr->RemoveChatRoom(pRoom->GetUID());

	// Notify to Player and Participant
	CCCommand* pCmd = CreateCommand(MC_MATCH_CHATROOM_LEAVE, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamStr(pObj->GetName()));
	pCmd->AddParameter(new CCCmdParamStr(pszChatRoomName));
	pRoom->RouteCommand(pCmd);
}

void CCMatchServer::OnChatRoomSelectWrite(const CCUID& uidComm, const char* pszChatRoomName)
{
	CCMatchObject* pPlayer = GetObject(uidComm);
	if (pPlayer == NULL) return;

	CCMatchChatRoomMgr* pChatRoomMgr = GetChatRoomMgr();
	CCMatchChatRoom* pRoom = pChatRoomMgr->FindChatRoomByName(pszChatRoomName);
	if (pRoom == NULL) {
		NotifyMessage(uidComm, MATCHNOTIFY_CHATROOM_NOT_EXIST);		// Notify Does not Exist
		return;
	}

	// 2008.08.28 채팅룸 부수적 문제 해결 http://dev:8181/projects/gunz/ticket/158
	// 채팅룸이 존재하지만 참가하지 않은 채팅룸을 선택하면 1)선택한 채팅룸에 들어가지못한다. 2)“채팅방에 참가되어 있지 않습니다”라는 시스템 메세지를 출력
	if(!pRoom->IsFindPlayer(uidComm)){ // 해당플레이어가 없으면
		NotifyMessage(uidComm, MATCHNOTIFY_CHATROOM_NOT_JOIN);
		return;
	}

	pPlayer->SetChatRooCCUID(pRoom->GetUID());
}

void CCMatchServer::OnChatRoomInvite(const CCUID& uidComm, const char* pszTargetName)
{
	if (strlen(pszTargetName) < 2) return;

	CCMatchObject* pPlayer = GetPlayerByComCCUID(uidComm);
	if (pPlayer == NULL) return;

	CCMatchChatRoomMgr* pChatRoomMgr = GetChatRoomMgr();
	CCMatchChatRoom* pRoom = pChatRoomMgr->FindChatRoom(pPlayer->GetChatRooCCUID());
	if (pRoom == NULL) {
		NotifyMessage(uidComm, MATCHNOTIFY_CHATROOM_NOT_EXIST);		// Notify Does not Exist
		return;
	}

	CCMatchObject* pTargetObj = GetPlayerByName(pszTargetName);

	if (pTargetObj == NULL) {
		NotifyMessage(pPlayer->GetUID(), MATCHNOTIFY_GENERAL_USER_NOTFOUND);
		return;
	}

	if( pTargetObj->GetEnterBattle() )  // 게임도중 초대금지
	{
		NotifyMessage(pPlayer->GetUID(), MATCHNOTIFY_USER_INVITE_REJECTED);
		return;
	}

	if (pTargetObj->CheckUserOption(MBITFLAG_USEROPTION_REJECT_INVITE) == true) {
		NotifyMessage(pPlayer->GetUID(), MATCHNOTIFY_USER_INVITE_REJECTED);
		NotifyMessage(pTargetObj->GetUID(), MATCHNOTIFY_USER_INVITE_IGNORED);
		return;
	}

	CCCommand* pCmd = CreateCommand(MC_MATCH_CHATROOM_INVITE, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamStr(pPlayer->GetName()));
	pCmd->AddParameter(new CCCmdParamStr(const_cast<char*>(pszTargetName)));
	pCmd->AddParameter(new CCCmdParamStr(const_cast<char*>(pRoom->GetName())));
	RouteToListener(pTargetObj, pCmd);

}

// RAONHAJE 임시코드
#ifdef _DEBUG
	#include "CCLexicalAnalyzer.h"
	bool StageFinish(CCMatchServer* pServer, const CCUID& uidPlayer, char* pszChat)
	{
		CCMatchObject* pChar = pServer->GetObject(uidPlayer);
		if (pChar == NULL)	return false;
//		if (pChar->GetPlace() != MMP_LOBBY) return false;
		CCMatchStage* pStage = pServer->FindStage(pChar->GetStageUID());
		if (pStage == NULL) return false;

		bool bResult = false;
		CCLexicalAnalyzer lex;
		lex.Create(pszChat);

		if (lex.GetCount() >= 1) {
			char* pszCmd = lex.GetByStr(0);
			if (pszCmd) {
				if (stricmp(pszCmd, "/finish") == 0) {
					pStage->GetRule()->DebugTest();
					bResult = true;
				}	// Finish
			}
		}

		lex.Destroy();
		return bResult;
	}
#endif

void CCMatchServer::OnChatRoomChat(const CCUID& uidComm, const char* pszMessage)
{
	CCMatchObject* pPlayer = GetObject(uidComm);
	if (pPlayer == NULL) return;

// #ifdef _DEBUG
// 	if (StageFinish(this, uidComm, const_cast<char*>(pszMessage)))
// 		return;
// #endif

	if (pPlayer->GetChatRooCCUID() == CCUID(0,0)) {
		NotifyMessage(uidComm, MATCHNOTIFY_CHATROOM_NOT_USING);		// Notify No ChatRoom
		return;
	}

	CCMatchChatRoomMgr* pChatRoomMgr = GetChatRoomMgr();
	CCMatchChatRoom* pRoom = pChatRoomMgr->FindChatRoom(pPlayer->GetChatRooCCUID());
	if (pRoom == NULL) {
		NotifyMessage(uidComm, MATCHNOTIFY_CHATROOM_NOT_EXIST);		// Notify Does not Exist
		return;
	}

	pRoom->RouteChat(pPlayer->GetUID(), const_cast<char*>(pszMessage));
}

void CCMatchServer::DisconnectObject(const CCUID& uidObject)
{
	CCMatchObject* pObj = GetObject(uidObject);
	if (pObj == NULL) return;

	Disconnect(pObj->GetCommListener());
}



//void CCMatchServer::InsertChatDBLog(const CCUID& uidPlayer, const char* szMsg)
//{
//	CCMatchObject* pObj = GetObject(uidPlayer);
//	if (pObj == NULL) return;
//	unsigned long int nNowTime = timeGetTime();
//
//	static int stnLogTop = 0;
//#define MAX_CHAT_LOG 1
//
//	static struct MCHATLOG
//	{
//		unsigned long int nCID;
//		char szMsg[256];
//		unsigned long int nTime;
//	} stChatLog[MAX_CHAT_LOG];
//
//	stChatLog[stnLogTop].nCID = pObj->GetCharInfo()->m_nCID;
//	if (strlen(szMsg) < 256) strcpy(stChatLog[stnLogTop].szMsg, szMsg); else strcpy(stChatLog[stnLogTop].szMsg, "");
//	stChatLog[stnLogTop].nTime = timeGetTime();
//	stnLogTop++;
//
//	// 일정 개수가 될때만 DB에 넣는다.
//	if (stnLogTop >= MAX_CHAT_LOG)
//	{
//		for (int i = 0; i < stnLogTop; i++)
//		{
//
//			if (!m_MatchDBMgr.InsertChatLog(stChatLog[i].nCID, stChatLog[i].szMsg, stChatLog[i].nTime))
//			{
//				LOG(LOG_PROG, "DB Query(InsertChatDBLog > InsertChatLog) Failed");
//			}
//		}
//		stnLogTop = 0;
//	}
//}



int CCMatchServer::ValidateMakingName(const char* szCharName, int nMinLength, int nMaxLength)
{
	// 캐릭터 이름의 최소-최대 길이를 검사한다.
	int nNameLen = (int)strlen( szCharName);
	if ( nNameLen < nMinLength)
		return MERR_TOO_SHORT_NAME;				// 캐릭터 이름이 넘 짧다

	if ( nNameLen > nMaxLength)
		return MERR_TOO_LONG_NAME;				// 캐릭터 이름이 넘 길다
	


	// 특정 국가에서 사용하는 특수 기호 중에 공백으로 표시될 만한 문자를 필터링한다.

#ifdef LOCALE_KOREA
	// 한국어에서는 2바이트 문자중에 0xC9xx 또는 0xFExx , 0xA1A1, 0xA4D4인 글자는 이름에 넣을 수 없다
	int nCur = 0;
	while (nCur < (nNameLen - 1))
	{
		unsigned char c1 = (unsigned char)szCharName[nCur];
		unsigned char c2 = (unsigned char)szCharName[nCur+1];

		if (c1 > 127)
		{
			if ((c1 == 0xc9) && (c2 > 127))
			{
				return MERR_WRONG_WORD_NAME;
			}
			if ((c1 == 0xfe) && (c2 > 127))
			{
				return MERR_WRONG_WORD_NAME;
			}
			if ((c1 == 0xa1) && (c2 == 0xa1))
			{
				return MERR_WRONG_WORD_NAME;
			}
			if ((c1 == 0xa4) && (c2 == 0xd4))
			{
				return MERR_WRONG_WORD_NAME;
			}

			nCur += 2;
		}
		else
		{
			nCur++;
		}
	}
#endif

	return MOK;
}



int CCMatchServer::ValidateChannelJoin(const CCUID& uidPlayer, const CCUID& uidChannel)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (! IsEnabledObject(pObj)) return MERR_CANNOT_JOIN_CHANNEL;
	CCMatchChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) return MERR_CANNOT_JOIN_CHANNEL;

	// 개발자나 영자는 레벨에 제한없음..
	if(!IsAdminGrade(pObj)) 
	{
		const DWORD dwTicketItemID = pChannel->GetTicketItemID();

		// 입장권을 사용하는 채널이면 입장권소유를 검사해 줘야 한다.
		if( MGetServerConfig()->IsUseTicket() && pChannel->IsUseTicket() && pChannel->IsTicketChannel() )
		{
			// 입장권 소유 검사만 함. 기간은 게임 로긴시 한번으로 정함.
			if( !pObj->GetCharInfo()->m_ItemList.IsHave(dwTicketItemID) )
				return MERR_CANNOT_JOIN_NEED_TICKET;
		}

		// 인원체크
		if ((int)pChannel->GetObjCount() >= pChannel->GetMaxPlayers())
		{
			return MERR_CANNOT_JOIN_CHANNEL_BY_MAXPLAYERS;
		}

		// 레벨 하한체크
		if ( (pChannel->GetLevelMin() > 0) && (pChannel->GetLevelMin() > pObj->GetCharInfo()->m_nLevel) )
		{
			return MERR_CANNOT_JOIN_CHANNEL_BY_LEVEL;
		}
		// 레벨 상한체크
		if ( (pChannel->GetLevelMax() > 0) && (pChannel->GetLevelMax() < pObj->GetCharInfo()->m_nLevel) )
		{
			return MERR_CANNOT_JOIN_CHANNEL_BY_LEVEL;
		}

		// 뉴비채널 체크
		if ((pChannel->GetRuleType() == CCCHANNEL_RULE_NEWBIE) && (pObj->IsNewbie() == false)) 
		{
			return MERR_CANNOT_JOIN_CHANNEL_BY_NEWBIE;
		}
	}

	return MOK;
}



DWORD EquipItemTotalWeightExceptionHandler( PEXCEPTION_POINTERS ExceptionInfo )
{
	char szStageDumpFileName[ _MAX_DIR ]= {0,};
	SYSTEMTIME SystemTime;
	GetLocalTime( &SystemTime );
	sprintf( szStageDumpFileName, "Log/EquipDump_%d-%d-%d_%d-%d-%d.dmp"
		, SystemTime.wYear
		, SystemTime.wMonth
		, SystemTime.wDay
		, SystemTime.wHour
		, SystemTime.wMinute
		, SystemTime.wSecond );

	return CrashExceptionDump( ExceptionInfo, szStageDumpFileName, true );
}


void GetCharEquipItemTotalWeight( CCMatchCharInfo* pCharInfo
								 , const CCMatchCharItemParts parts, CCMatchItem* pItem
								 , int& nWeight
								 , int& nMaxWeight )
{
	__try
	{
		pCharInfo->GetTotalWeight(&nWeight, &nMaxWeight);
	}
	__except(EquipItemTotalWeightExceptionHandler(GetExceptionInformation()))
	{
		cclog( "\nexception : equip item =====================\n" );
	
		cclog( "CharName(%d) : %s\n", pCharInfo->m_nCID, pCharInfo->m_szName );
		cclog( "parts : %d\n", parts );
		cclog( "ItemID(%d), CIID(%d)\n", pItem->GetDescID(), pItem->GetCIID() );

		cclog( "=====================\n\n" );
	}
}

int CCMatchServer::ValidateEquipItem(CCMatchObject* pObj, CCMatchItem* pItem, const CCMatchCharItemParts parts)
{
	if (! IsEnabledObject(pObj)) return MERR_UNKNOWN;
	if (pItem == NULL) return MERR_UNKNOWN;
	
	if (!IsEquipableItem(pItem->GetDescID(), pObj->GetCharInfo()->m_nLevel, pObj->GetCharInfo()->m_nSex))
	{
		return MERR_LOW_LEVEL;
	}

	// 무게 체크
	int nWeight = 0;
	int nMaxWeight = 0;

	CCMatchEquipedItem* pEquipedItem = &pObj->GetCharInfo()->m_EquipedItem;
	GetCharEquipItemTotalWeight( pObj->GetCharInfo(), parts, pItem,  nWeight, nMaxWeight );

	// 교체할 아이템의 무게를 뺀다.
	if (!pEquipedItem->IsEmpty(parts))
	{
		if (pEquipedItem->GetItem(parts)->GetDesc() != NULL)
		{
			nWeight -= pEquipedItem->GetItem(parts)->GetDesc()->m_nWeight.Ref();
			nMaxWeight -= pEquipedItem->GetItem(parts)->GetDesc()->m_nMaxWT.Ref();
		}
	}

	// 장착할 아이템의 무게를 더한다.
	if (pItem->GetDesc() != NULL)
	{
		nWeight += pItem->GetDesc()->m_nWeight.Ref();
	}
	
	if (nWeight > nMaxWeight)
	{
		return MERR_TOO_HEAVY;
	}


	// 양슬롯에 같은 무기를 장비하려는지 체크
	if ((parts == MMCIP_PRIMARY) || (parts == MMCIP_SECONDARY))
	{
		CCMatchCharItemParts tarparts = MMCIP_PRIMARY;
		if (parts == MMCIP_PRIMARY) tarparts = MMCIP_SECONDARY;

		if (!pEquipedItem->IsEmpty(tarparts))
		{
			CCMatchItem* pTarItem = pEquipedItem->GetItem(tarparts);
			if (pTarItem)
			{
				if (pTarItem->GetDescID() == pItem->GetDescID())
				{
					return MERR_CANNOT_EQUIP_EQUAL_ITEM;
				}
			}
		}
	}

	return MOK;
}

void CCMatchServer::OnNetPong(const CCUID& ComCCUID, unsigned int nTimeStamp)
{
	CCMatchObject* pObj = GetObject(ComCCUID);
	if (pObj) {
		pObj->UpdateTickLastPacketRecved();	// Last Packet Timestamp
	}
}

void CCMatchServer::OnHShieldPong(const CCUID& ComCCUID, unsigned int nTimeStamp)
{
	CCMatchObject* pObj = GetObject(ComCCUID);
	if (pObj) 
		pObj->UpdateLastHShieldMsgRecved();	// Last Packet Timestamp
}

void CCMatchServer::UpdateCharDBCachingData(CCMatchObject* pObject)
{
	if (!IsEnabledObject(pObject)) return;

	int	nAddedXP, nAddedBP, nAddedKillCount, nAddedDeathCount, nAddedPlayTime;

	nAddedXP			= pObject->GetCharInfo()->GetDBCachingData()->nAddedXP;
	nAddedBP			= pObject->GetCharInfo()->GetDBCachingData()->nAddedBP;
	nAddedKillCount		= pObject->GetCharInfo()->GetDBCachingData()->nAddedKillCount;
	nAddedDeathCount	= pObject->GetCharInfo()->GetDBCachingData()->nAddedDeathCount;
	nAddedPlayTime		= pObject->GetCharInfo()->GetDBCachingData()->nAddedPlayTime;

	if ((nAddedXP != 0) || (nAddedBP != 0) || (nAddedKillCount != 0) || (nAddedDeathCount != 0) || (nAddedPlayTime >= 0))
	{
		CCAsyncDBJob_UpdateCharInfoData* pJob = new CCAsyncDBJob_UpdateCharInfoData(pObject->GetUID());
		pJob->Input(pObject->GetCharInfo()->m_nCID, nAddedXP, nAddedBP, nAddedKillCount, nAddedDeathCount, nAddedPlayTime);
		pObject->m_DBJobQ.DBJobQ.push_back( pJob );
		pObject->GetCharInfo()->GetDBCachingData()->Reset();	///< 실패했는지는 알 수 없지만, 악용을 위해 Reset한다.
	}
}

void CCMatchServer::UpdateCharItemDBCachingData(CCMatchObject* pObject)
{
	if (!IsEnabledObject(pObject)) return;

	vector<CCUID> vecRemoveIteCCUIDList;

	CCMatchItemMap::iterator itBegin, itEnd;

	itBegin = pObject->GetCharInfo()->m_ItemList.begin();
	itEnd = pObject->GetCharInfo()->m_ItemList.end();

	CCAsyncDBJob_UpdateCharItemInfoData* pJob = new CCAsyncDBJob_UpdateCharItemInfoData(pObject->GetUID());

	for(; itBegin != itEnd; itBegin++) 
	{
		CCMatchItem* pItem = (*itBegin).second;

		if( pItem->IsNeedDBUpdate() ) 
		{
#ifdef _DEBUG
			cclog("해당 아이템(CIID:%d, Cnt:%d)은 업데이트가 필요합니다.\n", pItem->GetCIID(), pItem->GetItemCachingData()->nAddedItemCount);
#endif
			pJob->Input(pItem->GetCIID(), pItem->GetItemCachingData()->nAddedItemCount);
			pItem->DBUpdateDone();
		}

		/// 긴급 코드 - 디비에서 삭제될 소모성 아이템을 저장한다.
		if( pItem->GetDesc() && 
			pItem->GetDesc()->IsSpendableItem() && 
			pItem->GetItemCount() <= 0 ) {
			vecRemoveIteCCUIDList.push_back(pItem->GetUID());
		}
	}

	pObject->m_DBJobQ.DBJobQ.push_back( pJob );

	if (!vecRemoveIteCCUIDList.empty())
	{
		int nRemoveIteCCUIDListCount = (int)vecRemoveIteCCUIDList.size();
		for (int i = 0; i < nRemoveIteCCUIDListCount; i++)
		{
			CCUID uidItem = vecRemoveIteCCUIDList[i];
			CCMatchItem* pItem = pObject->GetCharInfo()->m_ItemList.GetItem(uidItem);
			if (pItem == NULL) continue;

			CCMatchCharItemParts nCheckParts = MMCIP_END;
			if (pObject->GetCharInfo()->m_EquipedItem.IsEquipedItem(uidItem, nCheckParts)) {
				pObject->GetCharInfo()->m_EquipedItem.Remove(nCheckParts);

				// 만약 스테이지 안이면 스테이지에 알려서 유저의 Look을 업데이트 해줘야 한다.
				if( FindStage(pObject->GetStageUID())  )
				{
					CCCommand* pCmd = CreateCommand( MC_MATCH_ROUTE_UPDATE_STAGE_EQUIP_LOOK, CCUID(0, 0) );
					pCmd->AddParameter(new CCCmdParaCCUID(pObject->GetUID()));
					pCmd->AddParameter(new CCCmdParamInt(nCheckParts));
					pCmd->AddParameter(new CCCmdParamInt(0));
					RouteToStage(pObject->GetStageUID(), pCmd);
				}
			}

			pObject->GetCharInfo()->m_ItemList.RemoveItem(uidItem);
		}

		ResponseCharacterItemList(pObject->GetUID());
	}
}

bool CCMatchServer::CheckItemXMLFromDatabase()
{
	map<int, CCMatchItemDescForDatabase*> ItemMapFromDatabase;

	if( m_MatchDBMgr.GetItemTable(ItemMapFromDatabase) == false ) {
		cclog("Can't not Get Item Information From Database. FAILED\n");
		return false;
	}

	bool bResult = true;
	int nResultCount = 0;

	cclog("========= Check validation of zitem.xml =========\n");	

	// ------------------------------------------------------------------------------------------

	cclog("== 1. Check Difference List From XML To DB\n");

	for(CCMatchItemDescMgr::iterator itBegin = MGetMatchItemDescMgr()->begin();
		itBegin != MGetMatchItemDescMgr()->end(); itBegin++)
	{
		CCMatchItemDesc *pItem = itBegin->second;
		if( ItemMapFromDatabase.find(pItem->m_nID) == ItemMapFromDatabase.end() )
		{
			cclog("     ItemID(%d) does not exist in Database\n", pItem->m_nID);
			bResult = false;
		}
	}

	// ------------------------------------------------------------------------------------------

	// 디비에만 있는 아이템이 존재할 수도 있습니다. 예비용으로 만들어둔거임..
	cclog("== 2. Check Difference List From DB To XML\n");

	for(map<int, CCMatchItemDescForDatabase*>::iterator itBegin = ItemMapFromDatabase.begin();
		itBegin != ItemMapFromDatabase.end(); itBegin++)
	{
		CCMatchItemDescForDatabase* pItem = itBegin->second;
		if( MGetMatchItemDescMgr()->GetItemDesc(pItem->m_nID) == NULL ) 
		{
			cclog("     ItemID(%d) does not exist in XML\n", pItem->m_nID);

			// 지울 땜 조심하세요~
			//FILE* fp = fopen("checkitem.sql", "wt");			
			//fprintf(fp, "DELETE Item WHERE ItemID=%d\n", pItem->m_nID);
		}
	}

	// ------------------------------------------------------------------------------------------

	cclog("== 3. Check Difference between XML and DB\n");

	for(map<int, CCMatchItemDescForDatabase*>::iterator itBegin = ItemMapFromDatabase.begin();
		itBegin != ItemMapFromDatabase.end(); itBegin++)
	{
		CCMatchItemDescForDatabase* pItem1 = itBegin->second;
		CCMatchItemDesc* pItem2 = MGetMatchItemDescMgr()->GetItemDesc(pItem1->m_nID);

		if( pItem2 != NULL ) 
		{
			if( !CompareMatchItem(pItem1, pItem2) ) 
			{
				bResult = false;
				nResultCount++;
			} 
		}
	}

	
	// ------------------------------------------------------------------------------------------


	if( bResult == false )
	{
		cclog("========= Failed =========\n");
		cclog("========= Check zitem.xml or Item table in Database =========\n\n");
	}

	for(map<int, CCMatchItemDescForDatabase*>::iterator itBegin = ItemMapFromDatabase.begin(); 
		itBegin != ItemMapFromDatabase.end(); itBegin++) 
	{
		CCMatchItemDescForDatabase *pItem1 = itBegin->second;
		delete pItem1;
	}

	ItemMapFromDatabase.clear();

	// ------------------------------------------------------------------------------------------

	return bResult;
}

bool CCMatchServer::CompareMatchItem(CCMatchItemDescForDatabase *pItem1, CCMatchItemDesc *pItem2)
{
	bool bResult = true;
	char *szMsg[256] = {0, };

	// ResSex의 경우, DB에는    3(All), 1(Male), 2(Female)이 들어가고, 
	//				  코드에는 -1(All), 0(Male), 1(Female)이 들어간다.
	if( ((pItem1->m_nResSex == 3 && pItem2->m_nResSex.Ref() == -1) 
		|| (pItem1->m_nResSex == 1 && pItem2->m_nResSex.Ref() == 0)
		|| (pItem1->m_nResSex == 2 && pItem2->m_nResSex.Ref() == 1)) == false)
	{
		cclog("%d(%s) - ResSex does not match\n", pItem1->m_nID, pItem2->m_pMItemName->Ref().m_szItemName);
		bResult = false;
	}

	if( pItem1->m_nResLevel != pItem2->m_nResLevel.Ref() ){
		cclog("%d(%s) - ResLevel does not match(DB : %d)\n", pItem1->m_nID, pItem2->m_pMItemName->Ref().m_szItemName, pItem1->m_nResLevel);
		bResult = false;
	}

	if( pItem1->m_nSlot != pItem2->m_nSlot ) {
		cclog("%d(%s) - Slot does not match(DB : %d)\n", pItem1->m_nID, pItem2->m_pMItemName->Ref().m_szItemName, (int)pItem1->m_nSlot);
		bResult = false;
	}

	if( pItem1->m_nWeight != pItem2->m_nWeight.Ref() ) {
		cclog("%d(%s) - Weight does not match(DB : %d)\n", pItem1->m_nID, pItem2->m_pMItemName->Ref().m_szItemName, pItem1->m_nWeight);
		bResult = false;
	}

	if( pItem1->m_nDamage != pItem2->m_nDamage.Ref() ) {
		cclog("%d(%s) - Damage does not match(DB : %d)\n", pItem1->m_nID, pItem2->m_pMItemName->Ref().m_szItemName, pItem1->m_nDamage);
		bResult = false;
	}

	if( pItem1->m_nDelay != pItem2->m_nDelay.Ref() ) {
		cclog("%d(%s) - Delay does not match(DB : %d)\n", pItem1->m_nID, pItem2->m_pMItemName->Ref().m_szItemName, pItem1->m_nDelay);
		bResult = false;
	}

	if( pItem1->m_nControllability != pItem2->m_nControllability.Ref() ) {
		cclog("%d(%s) - Controllability does not match(DB : %d)\n", pItem1->m_nID, pItem2->m_pMItemName->Ref().m_szItemName, pItem1->m_nControllability);
		bResult = false;
	}

	if( pItem1->m_nMagazine != pItem2->m_nMagazine.Ref() ) {
		cclog("%d(%s) - Magazine does not match(DB : %d)\n", pItem1->m_nID, pItem2->m_pMItemName->Ref().m_szItemName, pItem1->m_nMagazine);
		bResult = false;
	}

	if( pItem1->m_nReloadTime != pItem2->m_nReloadTime.Ref() ) {
		cclog("%d(%s) - ReloadTime does not match(DB : %d)\n", pItem1->m_nID, pItem2->m_pMItemName->Ref().m_szItemName, pItem1->m_nReloadTime);
		bResult = false;
	}

	if( pItem1->m_nMaxBullet != pItem2->m_nMaxBullet.Ref() ) {
		cclog("%d(%s) - MaxBullet does not match(DB : %d)\n", pItem1->m_nID, pItem2->m_pMItemName->Ref().m_szItemName, pItem1->m_nMaxBullet);
		bResult = false;
	}

	if( pItem1->m_nHP != pItem2->m_nHP.Ref() ) {
		cclog("%d(%s) - HP does not match(DB : %d)\n", pItem1->m_nID, pItem2->m_pMItemName->Ref().m_szItemName, pItem1->m_nHP);
		bResult = false;
	}

	if( pItem1->m_nAP != pItem2->m_nAP.Ref() ) {
		cclog("%d(%s) - AP does not match(DB : %d)\n", pItem1->m_nID, pItem2->m_pMItemName->Ref().m_szItemName, pItem1->m_nAP);
		bResult = false;
	}

	if( pItem1->m_bIsCashItem != pItem2->m_bIsCashItem ) {
		if( pItem1->m_bIsCashItem ) cclog("%d(%s) - IsCashItem does not match(DB : true)\n", pItem1->m_nID, pItem2->m_pMItemName->Ref().m_szItemName);
		else						cclog("%d(%s) - IsCashItem does not match(DB : false)\n", pItem1->m_nID, pItem2->m_pMItemName->Ref().m_szItemName);
		bResult = false;
	}

	if( pItem1->m_bIsSpendableItem != pItem2->m_bIsSpendableItem ) {
		if( pItem1->m_bIsSpendableItem ) cclog("%d(%s) - IsSpendableItem does not match(DB : true)\n", pItem1->m_nID, pItem2->m_pMItemName->Ref().m_szItemName);
		else							 cclog("%d(%s) - IsSpendableItem does not match(DB : false)\n", pItem1->m_nID, pItem2->m_pMItemName->Ref().m_szItemName);
		bResult = false;
	}

	return bResult;
}

// item xml 체크용 - 테스트
static bool CheckItemXML_ReadXML(map<unsigned long int, string>& ItemXmlMap, const char* szXmlFile)
{
	CCXmlDocument	xmlIniData;

	xmlIniData.Create();

	if (!xmlIniData.LoadFromFile(szXmlFile))
	{
		xmlIniData.Destroy();
		return false;
	}

	CCXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MICTOK_ITEM))
		{
			unsigned long int id;
			int n;
			char szItemName[256];
			chrElement.GetAttribute(&n, MICTOK_ID);
			id = n;
			chrElement.GetAttribute(szItemName, MICTOK_NAME);

			if (ItemXmlMap.find(id) != ItemXmlMap.end())
			{
				_ASSERT(0);	// 아이템 ID 중복
				char szTemp[256];
				sprintf(szTemp, "item xml 아이디 중복: %u\n", id);
				cclog(szTemp);
				return false;
			}
			ItemXmlMap.insert(map<unsigned long int, string>::value_type(id, string(szItemName)));
		}
	}

	xmlIniData.Destroy();
	return true;
}

bool CCMatchServer::CheckItemXML()
{
	map<unsigned long int, string>	ItemXmlMap;

	if (!CheckItemXML_ReadXML(ItemXmlMap, FILENAME_ITEM_DESC))
		return false;
	if (!CheckItemXML_ReadXML(ItemXmlMap, FILENAME_ITEM_DESC_LOCALE))
		return false;

	DWORD dwNomsgItemCount = 0;
	FILE* fp = fopen("item.sql", "wt");
	for (map<unsigned long int, string>::iterator itor = ItemXmlMap.begin();
		itor != ItemXmlMap.end(); ++itor)
	{
		char szTemp2[256];
		unsigned long int id = (*itor).first;
		size_t pos = (*itor).second.find( ":" );
		if( string::npos == pos ) 
		{
			ASSERT( 0 && "구분자를 찾지 못함. 문법오류." );
			continue;
		}

		string name = (*itor).second.c_str() + pos + 1;


		if( 0 == stricmp("nomsg", MGetStringResManager()->GetString(name)) )
		{
			++dwNomsgItemCount;
			// cclog( "nomsg Item : %s\n", name.c_str() );
		}
		
		sprintf(szTemp2, "INSERT INTO Item (ItemID, Name) Values (%u, '%s')\n", // id, name.c_str() );
			id, MGetStringResManager()->GetString(name) );

		fputs(szTemp2, fp);
	}

	cclog( "\n== Nomsg item count : %u ==\n\n", dwNomsgItemCount );

	fputs("\n\n--------------------------------------\n\n", fp);

	for (CCMatchItemDescMgr::iterator itor = MGetMatchItemDescMgr()->begin(); 
		itor != MGetMatchItemDescMgr()->end(); ++itor)
	{
		CCMatchItemDesc* pItemDesc = (*itor).second;

		int nIsCashItem = 0;
		int nResSex=1, nResLevel=0, nSlot=0, nWeight=0, nHP=0, nAP=0, nMaxWT=0, nIsSpendableItem=0;

		int nDamage=0, nDelay=0, nControl=0, nMagazine=0, nReloadTime=0, nSlugOutput=0, nMaxBullet=0;

		if (pItemDesc->IsCashItem())	nIsCashItem = 1; 
		else							nIsCashItem = 0;

		if (pItemDesc->IsSpendableItem())	nIsSpendableItem = 1; 
		else								nIsSpendableItem = 0;

		

		switch (pItemDesc->m_nResSex.Ref())
		{
		case 0: nResSex = 1; break;
		case 1: nResSex = 2; break;
		case -1: nResSex = 3; break;
		}

		nResLevel = pItemDesc->m_nResLevel.Ref();
		nWeight = pItemDesc->m_nWeight.Ref();
		nHP = pItemDesc->m_nHP.Ref();
		nAP = pItemDesc->m_nAP.Ref();
		nMaxWT = pItemDesc->m_nMaxWT.Ref();

		switch (pItemDesc->m_nSlot)
		{
		case MMIST_MELEE:		nSlot = 1; break;
		case MMIST_RANGE:		nSlot = 2; break;
		case MMIST_CUSTOM:		nSlot = 3; break;
		case MMIST_HEAD:		nSlot = 4; break;
		case MMIST_CHEST:		nSlot = 5; break;
		case MMIST_HANDS:		nSlot = 6; break;
		case MMIST_LEGS:		nSlot = 7; break;
		case MMIST_FEET:		nSlot = 8; break;
		case MMIST_FINGER:		nSlot = 9; break;
		case MMIST_EXTRA:		nSlot = 10; break;
		case MMIST_AVATAR :		nSlot = 11; break;
		case MMIST_COMMUNITY :	nSlot = 12; break;
		case MMIST_LONGBUFF :	nSlot = 13; break;
		}


		nDamage = pItemDesc->m_nDamage.Ref();
		nDelay = pItemDesc->m_nDelay.Ref();
		nControl = pItemDesc->m_nControllability.Ref();
		nMagazine = pItemDesc->m_nMagazine.Ref();
		nReloadTime = pItemDesc->m_nReloadTime.Ref();
		if (pItemDesc->m_bSlugOutput) nSlugOutput=1;
		nMaxBullet = pItemDesc->m_nMaxBullet.Ref();

		fprintf(fp, "UPDATE Item SET TotalPoint=0, BountyPrice=0, Damage=%d, Delay=%d, Controllability=%d, Magazine=%d, ReloadTime=%d, SlugOutput=%d, Gadget=0, SF=0, FR=0,CR=0,PR=0,LR=0, BlendColor=0, ModelName='', MaxBullet=%d, LimitSpeed=%d, IsCashItem=%d, \n",
			nDamage, nDelay, nControl, nMagazine, nReloadTime, nSlugOutput, nMaxBullet, 
			pItemDesc->m_nLimitSpeed.Ref(), nIsCashItem);

		fprintf(fp, "ResSex=%d, ResLevel=%d, Slot=%d, Weight=%d, HP=%d, AP=%d, MAXWT=%d, IsSpendableItem=%d, \n",
			nResSex, nResLevel, nSlot, nWeight, nHP, nAP, nMaxWT, nIsSpendableItem);

		fprintf(fp, "Description='%s', \n", pItemDesc->m_szDesc);
		fprintf(fp, "Name='%s' \n", pItemDesc->m_pMItemName->Ref().m_szItemName);

		// 이거 절대로 지우지 마세요. DB작업할때 대형 사고 날수 있습니다. - by SungE.
		fprintf(fp, "WHERE ItemID = %u\n\n", pItemDesc->m_nID );

		/*
		fprintf(fp, "UPDATE Item SET Slot = %d WHERE ItemID = %u AND Slot IS NULL\n", nSlot, pItemDesc->m_nID );
		*/
	}
	


	fclose(fp);

	///////////////////////////////////////////////////////////////////////////////////////////////
	// 캐쉬바운티 불문 모든 아이템을 상점에 올려볼 수 있는 shop.xml의 대체파일을 생성한다
	// 모든 아이템 썸네일 이미지를 게임상에서 확인해볼 수 있도록 하기 위함이다

	fp = fopen("shop_allitemtest.xml", "wt");
	if (fp)
	{
		fprintf(fp, "<?xml version=\"1.0\"?>\n");
		fprintf(fp, "<XML id=\"shop\">\n");

		int id;
		for (CCMatchItemDescMgr::iterator itor = MGetMatchItemDescMgr()->begin(); itor != MGetMatchItemDescMgr()->end(); ++itor)
		{
			id = itor->first;

			// 제외 아이템들
			if (id == 51 ||					// 대검
				id == 10001 ||				// 스나이퍼 라이플
				(300000 <= id && id <= 399999) ||	// 몬스터 무기 및 GM 상의
				(401000 <= id && id <= 499999) ||	// 이벤트 티켓
				id == 700001 ||						// 프리미엄 티켓
				(80000 <= id && id <= 100000) ||	// 롱버트 커뮤니티 아바타 아이템
				id == 800001						// 테스트 캐쉬 소모품
				)
				continue;

			fprintf(fp, "\t<SELL itemid=\"%d\" />\n", id);
		}
		for (CCQuestItemDescManager::iterator itor = GetQuestItemDescMgr().begin(); itor != GetQuestItemDescMgr().end(); ++itor)
		{
			id = itor->first;

			// 제외 아이템
			if (id == 210001) continue;

			fprintf(fp, "\t<SELL itemid=\"%d\" />\n", id);
		}
		fprintf(fp, "</XML>\n");

		fclose(fp);
	}

	return true;

}

// sql파일 생성을 위해서. 게임을 위해서 사용되지는 않음.
struct ix
{ 
	string id;
	string name;
	string desc;
};

bool CCMatchServer::CheckUpdateItemXML()
{
	// map<unsigned long int, string>	ItemXmlMap;
	
	map< string, ix > imName;
	map< string, ix > imDesc;

	CCXmlDocument	xmlIniData;

	xmlIniData.Create();

	if (!xmlIniData.LoadFromFile("strings.xml"))
	{
		xmlIniData.Destroy();
		return false;
	}

	CCXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, "STR"))
		{
			ix tix;
			char szID[256] = {0,};
			char szInfo[512] = {0,};

			chrElement.GetAttribute(szID, "id");
			chrElement.GetContents( szInfo );

			if( 0 == strncmp("ZITEM_NAME_", szID, 11) )
			{
				if( imName.end() != imName.find(szID) )
				{
					ASSERT( "중복" );
					continue;
				}

				tix.id = szID;
				tix.name = szInfo;

				imName.insert( map<string, ix>::value_type(szID, tix) );
			}
			else if( 0 == strncmp("ZITEM_DESC_", szID, 11) )
			{
				if( imDesc.end() != imDesc.find(szID) )
				{
					ASSERT( "중복" );
					continue;
				}

				tix.id = szID;
				tix.desc = szInfo;

				imDesc.insert( map<string, ix>::value_type(szID, tix) );
			}
			else
			{
				// ASSERT( 0 && "이상하다...." );
			}
		}
	}

	int ic, dc;
	ic = static_cast< int >( imName.size() );
	dc = static_cast< int >( imDesc.size() );

	xmlIniData.Destroy();

	map< string, ix >::iterator it, end;
	it = imName.begin();
	end = imName.end();
	FILE* fpName = fopen( "name.sql", "w" );
	for( ; it != end; ++it )
	{
		char szID[ 128 ];
		string a = it->second.name;
		strcpy( szID, it->second.id.c_str() + 11 );

		unsigned int nID = static_cast< unsigned long >( atol(szID) );
		int k = 0;

		fprintf( fpName, "UPDATE Item SET Name = '%s' WHERE ItemID = %d\n",
			it->second.name.c_str(), nID );
	}
	fclose( fpName );

	it = imDesc.begin();
	end = imDesc.end();
	FILE* fpDesc = fopen( "desc.sql", "w" );
	for( ; it != end; ++it )
	{
		char szID[ 128 ];
		string a = it->second.name;
		strcpy( szID, it->second.id.c_str() + 11 );

		unsigned int nID = static_cast< unsigned long >( atol(szID) );
		int k = 0;

		char szQ[ 1024 ] = {0,};
		fprintf( fpDesc, "UPDATE Item SET Description = '%s' WHERE ItemID = %d\n",
			it->second.desc.c_str(), nID );
	}
	fclose( fpDesc );
	
	return true;
}


unsigned long int CCMatchServer::GetStageListChecksum(CCUID& uidChannel, int nStageCursor, int nStageCount)
{
	CCMatchChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) return 0;

	unsigned long int nStageListChecksum = 0;
	int nRealStageCount = 0;

	for (int i = nStageCursor; i < pChannel->GetMaxPlayers(); i++)
	{
		if (nRealStageCount >= nStageCount) break;

		if (pChannel->IsEmptyStage(i)) continue;
		CCMatchStage* pStage = pChannel->GetStage(i);
		if ((pStage == NULL) || (pStage->GetState() == STAGE_STATE_CLOSE)) continue;

		nStageListChecksum += pStage->GetChecksum();

		nRealStageCount++;
	}

	return nStageListChecksum;
}




void CCMatchServer::BroadCastClanRenewVictories(const char* szWinnerClanName, const char* szLoserClanName, const int nVictories)
{
	CCCommand* pCmd = CreateCommand(MC_MATCH_BROADCAST_CLAN_RENEW_VICTORIES, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterString(szWinnerClanName));
	pCmd->AddParameter(new CCCommandParameterString(szLoserClanName));
	pCmd->AddParameter(new CCCommandParameterInt(nVictories));

	RouteToAllClient(pCmd);
}

void CCMatchServer::BroadCastClanInterruptVictories(const char* szWinnerClanName, const char* szLoserClanName, const int nVictories)
{
	CCCommand* pCmd = CreateCommand(MC_MATCH_BROADCAST_CLAN_INTERRUPT_VICTORIES, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterString(szWinnerClanName));
	pCmd->AddParameter(new CCCommandParameterString(szLoserClanName));
	pCmd->AddParameter(new CCCommandParameterInt(nVictories));

	RouteToAllClient(pCmd);
}

void CCMatchServer::BroadCastDuelRenewVictories(const CCUID& chanID, const char* szChampionName, const char* szChannelName, int nRoomNumber, const int nVictories)
{
	CCCommand* pCmd = CreateCommand(MC_MATCH_BROADCAST_DUEL_RENEW_VICTORIES, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterString(szChampionName));
	pCmd->AddParameter(new CCCommandParameterString(szChannelName));
	pCmd->AddParameter(new CCCommandParameterInt(nRoomNumber));
	pCmd->AddParameter(new CCCommandParameterInt(nVictories));

	RouteToChannel(chanID, pCmd);
}

void CCMatchServer::BroadCastDuelInterruptVictories(const CCUID& chanID, const char* szChampionName, const char* szInterrupterName, const int nVictories)
{
	CCCommand* pCmd = CreateCommand(MC_MATCH_BROADCAST_DUEL_INTERRUPT_VICTORIES, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterString(szChampionName));
	pCmd->AddParameter(new CCCommandParameterString(szInterrupterName));
	pCmd->AddParameter(new CCCommandParameterInt(nVictories));

	RouteToChannel(chanID, pCmd);
}


bool CCMatchServer::InitScheduler()
{
	// 스케쥴 업데이트시 커멘드를 포스트하기 위해서,
	//  CCMatchServer의 주소를 인자로 받아 멤버로 저장해둠.
	m_pScheduler = new CCMatchScheduleMgr( this );
	if( 0 == m_pScheduler )		
		return false;

	if( !m_pScheduler->Init() ){
		delete m_pScheduler;
		m_pScheduler = 0;
		return false;
	}

	// 검사 시간을 10초로 설정. 임시.
	m_pScheduler->SetUpdateTerm( 10 );

	// 상속한 클래스의 스케쥴 등록.
	if( !InitSubTaskSchedule() ){
		delete m_pScheduler;
		m_pScheduler = 0;
		return false;
	}

	return true;
}


bool CCMatchServer::InitLocale()
{
	if( MGetServerConfig()->IsComplete() )
	{
		
		MGetLocale()->Init( GetCountryID(MGetServerConfig()->GetLanguage().c_str()) );
	}
	else
	{
		ASSERT( 0 && "'CCMatchConfig' is must be completed befor init 'CCMatchLocale'." );
		return false;
	}

	MGetStringResManager()->Init( "", MGetLocale()->GetCountry() );	// 실행파일과 같은 폴더에 xml파일이 있다.

	return true;
}


bool CCMatchServer::InitCountryFilterDB()
{
	IPtoCountryList icl;
	BlockCountryCodeList bccl;
	CustomIPList cil;

	/* 실시간으로 caching하기로 함.
	if( !GetDBMgr()->GetIPtoCountryList(icl) )
	{
		ASSERT( 0 && "Fail to init IPtoCountryList.\n" );
		cclog( "Fail to init IPtoCountryList.\n" );
		return false;
	}
	*/

	if( !GetDBMgr()->GetBlockCountryCodeList(bccl) )
	{
		ASSERT( 0 && "Fail to init BlockCoutryCodeList.\n" );
		cclog( "Fail to init BlockCountryCodeList.\n" );
		return false;
	}

	if( !GetDBMgr()->GetCustomIPList(cil) )
	{
		ASSERT( 0 && "Fail to init CustomIPList.\n" );
		cclog( "Fail to init CustomIPList.\n" );
		return false;
	}

	if( !GetCountryFilter().Create(bccl, icl, cil) )
	{
		ASSERT( 0 && "Fail to create country filter.\n" );
		cclog( "Fail to create country filter.\n" );
		return false;
	}

	return true;
}


void CCMatchServer::SetUseCountryFilter()
{
	CCCommand* pCmd = CreateCommand( MC_LOCAL_UPDATE_USE_COUNTRY_FILTER, GetUID() );
	if( 0 != pCmd )
		Post( pCmd );
}


void CCMatchServer::UpdateIPtoCountryList()
{
	CCCommand* pCmd = CreateCommand( MC_LOCAL_GET_DB_IP_TO_COUNTRY, GetUID() );
	if( 0 != pCmd )
		Post( pCmd );
}


void CCMatchServer::UpdateBlockCountryCodeLsit()
{
	CCCommand* pCmd = CreateCommand( MC_LOCAL_GET_DB_BLOCK_COUNTRY_CODE, GetUID() );
	if( 0 != pCmd )
		Post( pCmd );
}


void CCMatchServer::UpdateCustomIPList()
{
	CCCommand* pCmd = CreateCommand( MC_LOCAL_GET_DB_CUSTOM_IP, GetUID() );
	if( 0 != pCmd )
		Post( pCmd );
}


void CCMatchServer::SetAccetpInvalidIP()
{
	CCCommand* pCmd = CreateCommand( MC_LOCAL_UPDATE_ACCEPT_INVALID_IP, GetUID() );
	if( 0 != pCmd )
		Post( pCmd );
}


bool CCMatchServer::CheckIsValidIP( const CCUID& ComCCUID, const string& strIP, string& strCountryCode3, const bool bUseFilter )
{
	switch( CheckIsValidCustomIP(ComCCUID, strIP, strCountryCode3, bUseFilter) )
	{
	case CIS_INVALID :
		{	// 서버에 해당 IP정보가 없으면
			switch( CheckIsNonBlockCountry(ComCCUID, strIP, strCountryCode3, bUseFilter) )
			{	// DB에서 해당 IP를 찾는다.
			case CCS_NONBLOCK :
				{
					return true;
				}
				break;

			case CCS_BLOCK :
				{
					return false;
				}
				break;

			case CCS_INVALID :
				{
					return MGetServerConfig()->IsAcceptInvalidIP();
				}
				break;

			default :
				{
					ASSERT( 0 );
				}
				break;
			}
		}
		break;

	case CIS_NONBLOCK :
		{
			return true;
		}
		break;

	case CIS_BLOCK :
		{
			return false;
		}
		break;

	default : 
		{
			ASSERT( 0 );
		}
		break;
	}
	
	return false;
}

const CUSTOM_IP_STATUS CCMatchServer::CheckIsValidCustomIP( const CCUID& ComCCUID, const string& strIP, string& strCountryCode3, const bool bUseFilter )
{
	string strComment;
	bool bIsBlock = false;

	if( !GetCountryFilter().GetCustomIP(strIP, bIsBlock, strCountryCode3, strComment) )
		return CIS_INVALID;

	if( bUseFilter && bIsBlock )
	{
		CCCommand* pCmd = CreateCommand(MC_RESPONSE_BLOCK_COUNTRYCODE, ComCCUID);
		if( 0 != pCmd )
		{
			pCmd->AddParameter( new CCCommandParameterString(strComment.c_str()) );
			Post( pCmd );
		}

		if( 3 != strCountryCode3.length() )
			strCountryCode3 = "er_";
		return CIS_BLOCK;
	}
	
	return CIS_NONBLOCK;
}


const COUNT_CODE_STATUS CCMatchServer::CheckIsNonBlockCountry( const CCUID& ComCCUID, const string& strIP, string& strCountryCode3, const bool bUseFilter )
{
	if( !bUseFilter )
		return CCS_NONBLOCK;

	if( GetCountryFilter().GetIPCountryCode(strIP, strCountryCode3) )
	{
		string strRoutingURL;

		if( GetCountryFilter().IsNotBlockCode(strCountryCode3, strRoutingURL) )
			return CCS_NONBLOCK;
		else 
		{
			CCCommand* pCmd = CreateCommand(MC_RESPONSE_BLOCK_COUNTRYCODE, ComCCUID);
			if( 0 != pCmd )
			{
				pCmd->AddParameter( new CCCommandParameterString(strRoutingURL.c_str()) );
				Post( pCmd );
			}
			return CCS_BLOCK;
		}
	}	
	else
	{
		DWORD dwIPFrom = 0;
		DWORD dwIPTo = 0;
		
		// IP를 포함하고 있는 범위의 정보를 DB에서 새로 가져옴.
		if( GetDBMgr()->GetIPContryCode(strIP, dwIPFrom, dwIPTo, strCountryCode3) )
		{
			// 새로운 IP범위를 리스트에 추가 함.
			if( !GetCountryFilter().AddIPtoCountry(dwIPFrom, dwIPTo, strCountryCode3) )
			{
				cclog( "CCMatchServer::CheckIsNonBlockCountry - add new IPtoCountry(f:%u, t%u, c:%s) fail.\n",
					dwIPFrom, dwIPTo, strCountryCode3.c_str() );
			}

			// 해당 IP가 블럭 국가의 IP인지 검사.
			string strRoutingURL;
			if( GetCountryFilter().IsNotBlockCode(strCountryCode3, strRoutingURL) )
				return CCS_NONBLOCK;
		}
		else
		{
			strCountryCode3 = "N/S";
			return CCS_INVALID;			
		}
		
		CCCommand* pCmd = CreateCommand(MC_RESPONSE_BLOCK_COUNTRYCODE, ComCCUID);
		if( 0 != pCmd )
		{
			pCmd->AddParameter( new CCCommandParameterString(strCountryCode3.c_str()) );
			Post( pCmd );
		}

		return CCS_BLOCK;		
	}

	return CCS_BLOCK;
}

bool CCMatchServer::InitEvent()
{
	if( !CCMatchEventDescManager::GetInstance().LoadEventXML(EVENT_XML_FILE_NAME) )
	{
		ASSERT( 0 && "fail to Load Event.xml" );
		cclog( "CCMatchServer::InitEvent - fail to Load %s\n", 
			EVENT_XML_FILE_NAME );
		return false;
	}

	if( !CCMatchEventFactoryManager::GetInstance().LoadEventListXML(EVENT_LIST_XML_FILE_NAME) )
	{
		ASSERT( 0 && "fail to load EventList.xml" );
		cclog( "CCMatchServer::InitEvent - fail to Load %s\n",	
			EVENT_LIST_XML_FILE_NAME );
		return false;
	}

	CCMatchEventFactoryManager::GetInstance().SetUsableState( MGetServerConfig()->IsUseEvent() );

	EventPtrVec EvnPtrVec;
	if( !CCMatchEventFactoryManager::GetInstance().GetEventList(CCMatchEvent::GAME_TYPE_ALL, ET_CUSTOM_EVENT, EvnPtrVec) )
	{
		ASSERT( 0 && "이벤트 리스트 생성 실패.\n" );
		cclog( "CCMatchServer::InitEvent - 리스트 생성 실패.\n" );
		CCMatchEventManager::ClearEventPtrVec( EvnPtrVec );
		return false;
	}
	m_CustomEventManager.ChangeEventList( EvnPtrVec );
	
	return true;
}


void CCMatchServer::CustomCheckEventObj( const DWORD dwEventID, CCMatchObject* pObj, void* pContext )
{
	m_CustomEventManager.CustomCheckEventObj( dwEventID, pObj, pContext );
}

void CCMatchServer::SendHShieldReqMsg()
{
	//{{RouteToAllClient HShieldReqMsg
	CCCommand* pCommand = CreateCommand(MC_HSHIELD_PING, CCUID(0,0));
	pCommand->AddParameter(new CCCmdParamUInt(GetGlobalClockCount()));


	unsigned long HSOption = ANTICPSVR_CHECK_GAME_MEMORY;
	m_HSCheckCounter++;
	if(m_HSCheckCounter == 3)
	{
		m_HSCheckCounter = 0;
		HSOption = ANTICPSVR_CHECK_ALL;
	}

	for(CCMatchObjectList::iterator i=m_Objects.begin(); i!=m_Objects.end(); i++)
	{
		CCMatchObject* pObj = (CCMatchObject*)((*i).second);

		// 이전 ReqMsg에 대한 응답(m_bHShieldMsgRecved)이 있는 클라이언트들에게만 보낸다. 그렇지 않은 경우 골치아픔. 새로 보냈는데 이전 ReqMsg에 대한 응답이 온다던가..
		if (pObj->GetUID() < CCUID(0,3) || !pObj->GetHShieldMsgRecved()) continue;	// CCUID로 Client인지 판별할수 있는 코드 필요함

		unsigned char* pbyReqMsg = pObj->GetHShieldInfo()->m_pbyReqMsg;
		unsigned char* pbyReqInfo = pObj->GetHShieldInfo()->m_pbyReqInfo;

		memset(pbyReqMsg, 0, sizeof(pbyReqMsg));
		memset(pbyReqInfo, 0, sizeof(pbyReqInfo));

		// 이 경우는 앞에서의 AnalyzeGuidAckMsg에서 에러가 났지만 접속이 끊기기 전에 타이머 타이밍이 맞아 이 루틴이 호출되는 경우이다.
		// 어차피 핵 판정 났으니 여기서 빠져나간다.
		if(!pObj->GetHShieldInfo()->m_bGuidAckPass)
		{
//			LOG(LOG_FILE, "%s's CrcInfo is NULL.", pObj->GetAccountName());
			break;
		}

		DWORD dwRet = MGetMatchServer()->HShield_MakeReqMsg(pObj->GetHShieldInfo()->m_pCRCInfo, pbyReqMsg, pbyReqInfo, HSOption);

		if(dwRet != ERROR_SUCCESS)
			LOG(LOG_FILE, "@MakeReqMsg - %s Making Req Msg Failed. (Error code = 0x%x, CrcInfo Address = 0x%x)", pObj->GetAccountName(), dwRet, pObj->GetHShieldInfo()->m_pCRCInfo);

		void* pBlob = CCMakeBlobArray(sizeof(unsigned char), SIZEOF_REQMSG);
		unsigned char* pCmdBlock = (unsigned char*)CCGetBlobArrayElement(pBlob, 0);
		CopyMemory(pCmdBlock, pbyReqMsg, SIZEOF_REQMSG);

		pCommand->AddParameter(new CCCmdParamBlob(pBlob, CCGetBlobArraySize(pBlob)));

		CCCommand* pSendCmd = pCommand->Clone();
		pSendCmd->m_Receiver = pObj->GetUID();
		Post(pSendCmd);

		pObj->SetHShieldMsgRecved(false);	// 새로 보냈으니 초기화해야지
		CCEraseBlobArray(pBlob);
		pCommand->ClearParam(1);
	}	

	delete pCommand;
}


bool CCMatchServer::IsEquipmentTypeItem( const CCMatchItemDesc* pItemDesc )
{
	if( 0 == pItemDesc ) return false;

	if( MMIT_TICKET == pItemDesc->m_nType.Ref() )
		return false;

	return true;
}

void CCMatchServer::RequestGameguardAuth( const CCUID& uidUser, const DWORD dwIndex, const DWORD dwValue1, const DWORD dwValue2, const DWORD dwValue3 )
{
	CCCommand* pCmd = CreateCommand( MC_REQUEST_GAMEGUARD_AUTH, uidUser );
	if( 0 == pCmd ) 
		return;

	pCmd->AddParameter( new CCCmdParamUInt(dwIndex) );
	pCmd->AddParameter( new CCCmdParamUInt(dwValue1) );
	pCmd->AddParameter( new CCCmdParamUInt(dwValue2) );
	pCmd->AddParameter( new CCCmdParamUInt(dwValue3) );

	PostSafeQueue( pCmd );

#ifdef _DEBUG
	cclog( "GAMEGUARD : send to client : index(%u), v1(%u), v2(%u), v3(%u)\n",
		dwIndex, dwValue1, dwValue2, dwValue3 );
#endif
}


void CCMatchServer::RequestFirstGameguardAuth( const CCUID& uidUser, const DWORD dwIndex, const DWORD dwValue1, const DWORD dwValue2, const DWORD dwValue3 )
{
	CCCommand* pCmd = this->CreateCommand( MC_REQUEST_FIRST_GAMEGUARD_AUTH, uidUser );
	if( 0 == pCmd ) 
		return;

	pCmd->AddParameter( new CCCmdParamUInt(dwIndex) );
	pCmd->AddParameter( new CCCmdParamUInt(dwValue1) );
	pCmd->AddParameter( new CCCmdParamUInt(dwValue2) );
	pCmd->AddParameter( new CCCmdParamUInt(dwValue3) );

	PostSafeQueue( pCmd );

#ifdef _DEBUG
	cclog( "GAMEGUARD : first send to client : index(%u), v1(%u), v2(%u), v3(%u)\n",
		dwIndex, dwValue1, dwValue2, dwValue3 );
#endif
}


bool CCMatchServer::InitGambleMachine()
{
	vector<CCMatchGambleItem*>			vGambleItemList;
	vector<CCMatchGambleRewardItem*>		vGambleRewardItemList;

	if( !GetDBMgr()->GetGambleItemList(vGambleItemList) )				return false;
	if( !GetDBMgr()->GetGambleRewardItemList(vGambleRewardItemList) )	return false;

	if( !GetGambleMachine().CreateGambleItemListWithGambleRewardList(vGambleItemList, vGambleRewardItemList) )
		return false;

	GetGambleMachine().SetLastUpdateTime( GetGlobalClockCount() );


	//////////// Init log ////////////////////
	vector<DWORD> GambleItemIndexVec;
	GetGambleMachine().GetItemVectorByCheckedItemTime( GambleItemIndexVec, GetGlobalClockCount() );

	cclog( "\nStart shop GItem list log.\n" );
	cclog( "GambleItemCount : %d.\n", int(GambleItemIndexVec.size()) );

	for( int i = 0; i < int(GambleItemIndexVec.size()); ++i )
	{
		const CCMatchGambleItem* pGItem = GetGambleMachine().GetGambleItemByIndex( GambleItemIndexVec[i] );
        if( NULL != pGItem )
		{
			cclog( "Shop GItem : %d\n", pGItem->GetGambleItemID() );
		}
		else
		{
			cclog( "Null GItem.\n" );
		}
	}
	cclog( "End shop GItem list log.\n\n" );
	//////////// Init log ////////////////////

	GetGambleMachine().WriteGambleItemInfoToLog();

	return true;
}


bool CCMatchServer::InitBattletimeRewardMachine()
{
	vector<CCMatchBRDescription*>	vBattletimeRewardDescription;
	vector<CCMatchBRItem*>			vBattletimeRewardItem;

	if( !GetDBMgr()->GetBattletimeRewardList(vBattletimeRewardDescription) ) return false;
	if( !GetDBMgr()->GetBattletimeRewardItemList(vBattletimeRewardItem) )			return false;

	CCMatchBRDescriptionMap BattletimeRewardDescriptionMap;
	MakeBattleTimeRewardDescriptionMap(vBattletimeRewardDescription, vBattletimeRewardItem, BattletimeRewardDescriptionMap);

	GetBattleTimeRewardMachine().SetBattleTimeRewardMachine(BattletimeRewardDescriptionMap);
	GetBattleTimeRewardMachine().SetLastUpdateTime( GetGlobalClockCount() );

	return true;
}

void CCMatchServer::MakeBattleTimeRewardDescriptionMap(vector<CCMatchBRDescription*>& vBattletimeRewardDescription, 
													  vector<CCMatchBRItem*>& vBattletimeRewardItem, 
													  CCMatchBRDescriptionMap& BattletimeRewardDescriptionMap)
{
	CCMatchBRDescriptionMap::iterator iter;

	// 1. Description 삽입하기
	for(int i = 0; i < (int)vBattletimeRewardDescription.size(); i++) 
	{
		CCMatchBRDescription* pDesc = vBattletimeRewardDescription[i];
		BattletimeRewardDescriptionMap.insert(pair<int, CCMatchBRDescription*>(pDesc->GetBRID(), pDesc));
	}

	// 2. 해당 Description에 맞는 RewardItem 삽입하기
	for(int i = 0; i < (int)vBattletimeRewardItem.size(); i++) 
	{
		CCMatchBRItem* pRewardItem = vBattletimeRewardItem[i];

		iter = BattletimeRewardDescriptionMap.find(pRewardItem->GetBRID());
		if(iter != BattletimeRewardDescriptionMap.end()) 
		{
			CCMatchBRDescription* pDesc = iter->second;
			pDesc->AddRewardItem(pRewardItem);
		}
	}

	// 3. 이상한 Description 빼주기
	for(iter = BattletimeRewardDescriptionMap.begin(); iter != BattletimeRewardDescriptionMap.end();)
	{
		CCMatchBRDescription* pDesc = iter->second;
		if( pDesc->GetTotalRate() != 1000 ) 
		{
			//_ASSERT(0);
			delete pDesc;
			iter = BattletimeRewardDescriptionMap.erase(iter);

			continue;
		}

		iter++;
	}

	// 4. CRC32 만들어주긔
	BattletimeRewardDescriptionMap.MakeCRC32();
}

void CCMatchServer::OnAsyncResponse_GetBR_Description(CCAsyncJob *pJobResult)
{
	CCAsyncDBJob_GetBattleTimeRewardDescription* pJob = (CCAsyncDBJob_GetBattleTimeRewardDescription*)pJobResult;

	if( CCASYNC_RESULT_SUCCEED != pJob->GetResult() ) {
		cclog("CCMatchServer::OnAsyncResponse_GetBR_Description - 실패\n");
		return;
	}

	CCMatchBRDescriptionMap BattletimeRewardDescriptionMap;
	MakeBattleTimeRewardDescriptionMap(pJob->GetBattleTimeRewardDescription(), pJob->GetBattleTimeRewardItem(), BattletimeRewardDescriptionMap);

	GetBattleTimeRewardMachine().SetBattleTimeRewardMachine(BattletimeRewardDescriptionMap);
}

void CCMatchServer::OnAsyncResponse_GetCharBRInfo(CCAsyncJob *pJobResult)
{
	CCAsyncDBJob_GetCharBRInfo* pJob = (CCAsyncDBJob_GetCharBRInfo*)pJobResult;

	if( CCASYNC_RESULT_SUCCEED != pJob->GetResult() ) {
		cclog("CCMatchServer::OnAsyncResponse_GetCharBRInfo - 실패\n");
		return;
	}

	CCMatchObject* pObj = GetObject(pJob->GetOwnerUID());
	if( pObj == NULL ) return;
	if( pObj->GetCharInfo() == NULL ) return;

	CCMatchCharBRInfo* pInfo = pObj->GetCharInfo()->GetBRInfoMap().Get(pJob->GetBRID());
	if( pInfo == NULL ) return;

	pInfo->SetBRInfo(pJob->GetCharBRInfo().GetBRID(), pJob->GetCharBRInfo().GetBRTID(), pJob->GetCharBRInfo().GetBattleTime(), 
		pJob->GetCharBRInfo().GetRewardCount(), pJob->GetCharBRInfo().GetKillCount());

	pInfo->SetCheckSkip(false);
	
#ifdef _DEBUG

	cclog("CID(%d), 의 BR Info(%d, %d, %d, %d)가 삽입되었습니다.\n", pJob->GetCID(), pJob->GetCharBRInfo().GetBRID(), 
		pJob->GetCharBRInfo().GetBRTID(), pJob->GetCharBRInfo().GetBattleTime(), pJob->GetCharBRInfo().GetKillCount());

#endif

}

void CCMatchServer::OnAsyncResponse_UpdateCharBRInfo(CCAsyncJob *pJobResult)
{
	CCAsyncDBJob_UpdateCharBRInfo* pJob = (CCAsyncDBJob_UpdateCharBRInfo*)pJobResult;

	if( CCASYNC_RESULT_SUCCEED != pJob->GetResult() ) {
		cclog("CCMatchServer::OnAsyncResponse_UpdateCharBRInfo - 실패\n");
		return;
	}
}

void CCMatchServer::OnAsyncResponse_RewardCharBR(CCAsyncJob *pJobResult)
{
	CCAsyncDBJob_RewardCharBR* pJob = (CCAsyncDBJob_RewardCharBR*)pJobResult;

	if( CCASYNC_RESULT_SUCCEED != pJob->GetResult() ) {
		cclog("CCMatchServer::OnAsyncResponse_RewardCharBR - 실패\n");
		return;
	}

	CCMatchObject* pObj = GetObject(pJob->GetOwnerUID());
	if( pObj == NULL ) return;
	if( pObj->GetCharInfo() == NULL ) return;

	const WORD			wRentHourPeriod	= pJob->GetRentHourPeriod();
	const DWORD			dwCIID			= pJob->GetCIID();
	const unsigned int	nItemID			= pJob->GetItemID();
	const int			nItemCount		= pJob->GetItemCnt();
	
	WORD wRentMinPeriod = RENT_PERIOD_UNLIMITED;
	if( RENT_PERIOD_UNLIMITED != pJob->GetRentHourPeriod() ) 
	{
		wRentMinPeriod = (pJob->GetRentHourPeriod() * 60) - 1;
	}

	if( NULL != MGetMatchItemDescMgr()->GetItemDesc(nItemID) )	//< Normal Item
	{
		CCMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);

		CCMatchItem* pItem = pObj->GetCharInfo()->m_ItemList.GetItemByCIID(dwCIID);
		if( pItem == NULL ) 
		{
			WORD wRentMinPeriod = RENT_PERIOD_UNLIMITED;
			if( RENT_PERIOD_UNLIMITED != pJob->GetRentHourPeriod() ) 
			{
				wRentMinPeriod = (wRentHourPeriod * 60) - 1;
			}

			pObj->GetCharInfo()->m_ItemList.CreateItem( CCMatchItemMap::UseUID(), dwCIID, nItemID,
				RENT_PERIOD_UNLIMITED != pJob->GetRentHourPeriod(), wRentMinPeriod, wRentHourPeriod, nItemCount);
		}
		else
		{
			if( pItem->GetDescID() == pJob->GetItemID() && pDesc->IsSpendableItem())
			{
				pItem->IncreaseCount(pJob->GetItemCnt());
			}
			else
			{
				_ASSERT(0);
				return;
			}
		}	
	} 
	else if( NULL != MGetMatchServer()->GetGambleMachine().GetGambleItemByGambleItemID(pJob->GetItemID()) )	//< Gamble Item
	{	
		const CCMatchCharGambleItem *pGItem = pObj->GetCharInfo()->m_GambleItemManager.GetGambleItemByCIID(pJob->GetCIID());
		if( pGItem == NULL )	pObj->GetCharInfo()->m_GambleItemManager.AddGambleItem( CCMatchItemMap::UseUID(), dwCIID, nItemID, nItemCount );
		else					pObj->GetCharInfo()->m_GambleItemManager.SetGambleItemCount(dwCIID, pGItem->GetItemCount() + nItemCount);
	} else { ASSERT( 0 ); return; }


#ifdef _DEBUG

	cclog("CID(%d), BattleTime Reward!! - Time(%d), ItemID(%d), ItemCnt(%d), ItemPeriod(%d)\n", 
		pJob->GetCID(), pJob->GetBattleTime(), pJob->GetItemID(), pJob->GetItemCnt(), pJob->GetRentHourPeriod());

#endif


	CCMatchBRDescription* pDesc = GetBattleTimeRewardMachine().GetBattleTimeRewardDescription(pJob->GetBRID());
	if( pDesc != NULL )
	{
		if( pDesc->GetRewardCount() == 0 )	
		{
			RouteCmdBattleTimeReward(pJob->GetOwnerUID(), pObj->GetStageUID(), pDesc->GetName().c_str(), pDesc->GetResetDesc().c_str(),
				pJob->GetItemID(), pJob->GetItemCnt(), pJob->GetRentHourPeriod(), -1);
		}
		else								
		{
			RouteCmdBattleTimeReward(pJob->GetOwnerUID(), pObj->GetStageUID(), pDesc->GetName().c_str(), pDesc->GetResetDesc().c_str(),
				pJob->GetItemID(), pJob->GetItemCnt(), pJob->GetRentHourPeriod(), pDesc->GetRewardCount() - pJob->GetRewardCount() - 1);
		}
	}

	ResponseCharacterItemList( pJob->GetOwnerUID() );
}

void CCMatchServer::RouteCmdBattleTimeReward(const CCUID& uidPlayer, CCUID& uidStage, const char* pszName, const char* pszResetDesc, int nItemID, int nItemCnt, int nRentHourPeriod, int nRemainRewardCnt)
{
	CCCommand* pNew = CreateCommand( MC_MATCH_REWARD_BATTLE_TIME, CCUID(0, 0) );

	pNew->AddParameter( new CCCmdParaCCUID(uidPlayer) );
	pNew->AddParameter( new CCCmdParamStr(pszName) );
	pNew->AddParameter( new CCCmdParamStr(pszResetDesc) );
	pNew->AddParameter( new CCCmdParamUInt(nItemID) );
	pNew->AddParameter( new CCCmdParamUInt(nItemCnt) );
	pNew->AddParameter( new CCCmdParamUInt(nRentHourPeriod) );
	pNew->AddParameter( new CCCmdParamInt(nRemainRewardCnt) );

	RouteToBattle(uidStage, pNew);
}

void CCMatchServer::OnAsyncRequest_RewardCharBP(const CCUID& uidPlayer, int nBRID, int nBRTID, int nRewardCount, int nBattleTime, int nKillCount, int nItemID, int nItemCnt, int nRentHourPeriod)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if( pObj == NULL ) return;
	if( pObj->GetCharInfo() == NULL ) return;

	bool bIsSpendable = false;

	CCMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if( pItemDesc != NULL ) 
	{
		bIsSpendable = pItemDesc->IsSpendableItem();
	} 
	else 
	{
		if( GetGambleMachine().GetGambleItemByGambleItemID(nItemID) != NULL ) 
		{
			bIsSpendable = true;
		} 
		else 
		{
			_ASSERT(0);
			return;
		}
	}

	CCAsyncDBJob_RewardCharBR* pJob = new CCAsyncDBJob_RewardCharBR(uidPlayer);
	pJob->Input(pObj->GetCharInfo()->m_nCID, nBRID, nBRTID, nRewardCount, nBattleTime, nKillCount, nItemID, nItemCnt, nRentHourPeriod, bIsSpendable);
	pObj->m_DBJobQ.DBJobQ.push_back(pJob);
}

void CCMatchServer::OnAsyncRequest_UpdateCharBRInfo(const CCUID& uidPlayer, int nBRID, int nBRTID, int nRewardCount, int nBattleTime, int nKillCount)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if( pObj == NULL ) return;
	if( pObj->GetCharInfo() == NULL ) return;

	CCAsyncDBJob_UpdateCharBRInfo* pJob = new CCAsyncDBJob_UpdateCharBRInfo(uidPlayer);
	pJob->Input(pObj->GetCharInfo()->m_nCID, nBRID, nBRTID, nRewardCount, nBattleTime, nKillCount);
	pObj->m_DBJobQ.DBJobQ.push_back(pJob);
}

void CCMatchServer::CheckMemoryCorruption()
{
#ifdef _CHECK_MEMORY_CORRUPTION
	if(_CrtCheckMemory() != 0) return;

	// 문제가 있으므로 덤프를 남긴다
	cclog("CheckMemoryCorruption...throw...");
	throw "heap corruption";
#endif
}

void CCMatchServer::Log(unsigned int nLogLevel, const char* szLog)
{
#ifdef _DEBUG
	if((nLogLevel & LOG_DEBUG) == LOG_DEBUG)
	{
		OutputDebugString(szLog);
		OutputDebugString("\n");
	}
#endif

	CTime theTime = CTime::GetCurrentTime();
	CString szTime = theTime.Format( "[%c] " );

	if((nLogLevel & LOG_FILE) == LOG_FILE)
	{
		char szTemp[1024];
		strcpy(szTemp, szTime);
		strcat(szTemp, szLog);
		strcat(szTemp, "\n");
		cclog(szTemp);
	}
}