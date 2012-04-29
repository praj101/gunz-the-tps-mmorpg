#include "stdafx.h"
#include "CCMatchClan.h"
#include "CCMatchServer.h"
#include "CCMatchObject.h"
#include "CCSmartRefreshImpl.h"
#include "CCMatchDBMgr.h"
#include "CCDebug.h"
#include "CCMatchUtil.h"

#define CCTICK_CLAN_RUN							500
#define CCTICK_CLAN_DBREFRESH_PERIOD_LIMIT		7200000			// (1000*60*120) - 2 hours
#define CCTICK_CLAN_EMPTY_PERIOD_LIMIT			600000			// (1000*60*10) - 10 secs


CCMatchClan::CCMatchClan(){
	Clear();
}

CCMatchClan::~CCMatchClan(){
}


void CCMatchClan::Clear(){
	m_nCLID = 0;
	m_szClanName[0] = 0;
	m_nSeriesOfVictories = 0;

	memset(&m_ClanInfoEx, 0, sizeof(ClanInfoEx));
	m_nDBRefreshLifeTime = 0;
	m_Members.clear();

	m_nEmptyPeriod = 0;
}

void CCMatchClan::InitClanInfoFromDB(){
	if (m_nCLID == 0) return;

	CCMatchDBMgr::CCDB_ClanInfo dbClanInfo;

	if (CCMatchServer::GetInstance()->GetDBMgr()->GetClanInfo(m_nCLID, &dbClanInfo)){
		InitClanInfoEx(dbClanInfo.nLevel, dbClanInfo.nTotalPoint, dbClanInfo.nPoint, dbClanInfo.nRanking,
			dbClanInfo.nWins, dbClanInfo.nLosses, dbClanInfo.nTotalMemberCount, dbClanInfo.szMasterName,
			dbClanInfo.szEmblemUrl, dbClanInfo.nEmblemChecksum);
	}
	else{
		mlog("DB Query(GetClanInfo) Failed\n");
	}

}

void CCMatchClan::Create(int nCLID, const char* szClanName){
	m_nCLID = nCLID;
	strcpy(m_szClanName, szClanName);
	m_SmartRefresh.AddCategory(new CCRefreshCategoryClanMemberImpl(this, 0));	// Category 0 로 전체목록 모두 담당
}

void CCMatchClan::InitClanInfoEx(const int nLevel, const int nTotalPoint, const int nPoint, const int nRanking,
		            const int nWins, const int nLosses, const int nTotalMemberCount, const char* szMaster,
					const char* szEmblemUrl, int nEmblemChecksum)
{
	m_ClanInfoEx.nLevel = nLevel;
	m_ClanInfoEx.nTotalPoint = nTotalPoint;
	m_ClanInfoEx.nPoint = nPoint;
	m_ClanInfoEx.nRanking = nRanking;
	m_ClanInfoEx.nWins = nWins;
	m_ClanInfoEx.nLosses = nLosses;
	m_ClanInfoEx.nTotalMemberCount = nTotalMemberCount;
	strcpy(m_ClanInfoEx.szMaster, szMaster);
	strcpy(m_ClanInfoEx.szEmblemUrl, szEmblemUrl);
	m_ClanInfoEx.nEmblemChecksum = nEmblemChecksum;
}

void CCMatchClan::AddObject(const CCUID& uid, CCMatchObject* pObj){
	m_Members.Insert(uid, (void*)pObj);

}

void CCMatchClan::RemoveObject(const CCUID& uid){
	CCUIDRefCache::iterator itor = m_Members.find(uid);
	if( m_Members.end() == itor )
		return;

	m_Members.erase(itor);
}


void CCMatchClan::Tick(unsigned long nClock){
	m_nDBRefreshLifeTime += CCTICK_CLAN_RUN;
	if (m_nDBRefreshLifeTime >= CCTICK_CLAN_DBREFRESH_PERIOD_LIMIT){
		InitClanInfoFromDB();
		m_nDBRefreshLifeTime = 0;
	}

	m_SmartRefresh.UpdateCategory(nClock);

	if (GetMemberCount() <= 0) m_nEmptyPeriod += CCTICK_CLAN_RUN; 
	else m_nEmptyPeriod = 0;
}

void CCMatchClan::SyncPlayerList(CCMatchObject* pObj, int nCategory){
	m_SmartRefresh.SyncClient(pObj->GetRefreshClientClanMemberImplement());
}

void CCMatchClan::InsertMatchedClanID(int nCLID)
{
	m_MatchedClanList.push_back(nCLID);
	if (m_MatchedClanList.size() >= 10){
		m_MatchedClanList.erase(m_MatchedClanList.begin());
	}
}

bool CCMatchClan::CheckLifePeriod(){
	if (GetMemberCount() > 0) 
		return true;

	if (m_nEmptyPeriod < CCTICK_CLAN_EMPTY_PERIOD_LIMIT)
		return true;

	return false;
}

/////////////////////////////////////////////////////////

CCMatchClanMap::CCMatchClanMap(){
	m_nLastTick = 0;
}

CCMatchClanMap::~CCMatchClanMap(){
}


void CCMatchClanMap::Destroy(){
	for (iterator itor = begin(); itor != end(); ++itor){
		CCMatchClan* pClan = (*itor).second;
		delete pClan;
	}
	clear();
}

void CCMatchClanMap::CreateClan(int nCLID, const char* szClanName){
	CCMatchClan* pNewClan = new CCMatchClan;
	pNewClan->Create(nCLID, szClanName);

	insert(value_type(nCLID, pNewClan));
	m_ClanNameMap.insert(map<std::string, CCMatchClan*>::value_type(string(szClanName), pNewClan));
}

void CCMatchClanMap::DestroyClan(int nCLID, CCMatchClanMap::iterator* pNextItor){
	iterator itor = find(nCLID);
	if (itor != end()){
		CCMatchClan* pClan = (*itor).second;


		map<std::string, CCMatchClan*>::iterator itorClanNameMap = m_ClanNameMap.find(string(pClan->GetName()));
		if (itorClanNameMap != m_ClanNameMap.end()){
			m_ClanNameMap.erase(itorClanNameMap);
		}

		delete pClan;
		CCMatchClanMap::iterator itorTemp = erase(itor);
		if (pNextItor) *pNextItor = itorTemp;
	}
}

void CCMatchClanMap::AddObject(const CCUID& uid, CCMatchObject* pObj){
	if (! IsEnabledObject(pObj)) 
		return;

	int nCLID = pObj->GetCharInfo()->m_ClanInfo.m_nClanID;
	if (nCLID == 0) return;

	// 임시코드... AddObject()하는 어느곳에서 잘못된 값을 가져와서 서버 크래쉬 유발시키기 때문에 일단 여기서 막음
	// 또한 AddObject()하는곳에서 log를 남겨 배포후 원인을 잡는 과정이 있을거임...20090224 by kammir
	if (nCLID <= -1) return;		// nCLID는 정수형
	if (nCLID >= 9000000) return;	// 현재(20090224) 한국 최고 클랜ID가 "1399559"이다.(DB에서는 1씩증가시킴)

	// 클랜이 없으면 새로 생성
	iterator itor = find(nCLID);
	if (itor == end()) {
		CreateClan(nCLID, pObj->GetCharInfo()->m_ClanInfo.m_szClanName);
	}

	itor = find(nCLID);
	if (itor != end()){
		CCMatchClan* pClan = (*itor).second;
		pClan->AddObject(uid, pObj);
	}
}

void CCMatchClanMap::RemoveObject(const CCUID& uid, CCMatchObject* pObj){
	if (! IsEnabledObject(pObj)) 
		return;

	int nCLID = pObj->GetCharInfo()->m_ClanInfo.m_nClanID;
	if (nCLID == 0) return;

	iterator itor = find(nCLID);
	if (itor != end()){
		CCMatchClan* pClan = (*itor).second;
		pClan->RemoveObject(uid);
	}
}

bool CCMatchClanMap::CheckTick(unsigned long nClock){
	if (CCGetTimeDistance(m_nLastTick, nClock) < CCTICK_CLAN_RUN) 
		return false;

	m_nLastTick = nClock;
	return true;
}


void CCMatchClanMap::Tick(unsigned long nClock){
	if (!CheckTick(nClock)) return;

	// Update Clans
	for(CCMatchClanMap::iterator iClan=begin(); iClan!=end();){
		CCMatchClan* pClan = (*iClan).second;
		pClan->Tick(nClock);

		if (pClan->CheckLifePeriod() == false) {
			DestroyClan(pClan->GetCLID(), &iClan);
			continue;
		}
		else{
			++iClan;
		}
	}
}

CCMatchClan* CCMatchClanMap::GetClan(const int nCLID){
	iterator itor = find(nCLID);
	if (itor != end()){
		return (*itor).second;
	}
	
	return NULL;
}

CCMatchClan* CCMatchClanMap::GetClan(const char* szClanName){
	map<string, CCMatchClan*>::iterator itor = m_ClanNameMap.find(string(szClanName));
	if (itor != m_ClanNameMap.end()){
		return (*itor).second;
	}

	return NULL;
}

