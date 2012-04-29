#include "stdafx.h"
#include "ODBCRecordset.h"
#include "CCMatchChannel.h"
#include "CCCommand.h"
#include "CCMatchObject.h"
#include "CCMatchStage.h"
#include <math.h>
#include "CCSmartRefreshImpl.h"
//#include "MMatchMapSet.h"


#define CCTICK_CHANNEL_RUN					100
#define CCTICK_CHANNEL_UPDATECHECKSUM		2000
#define CCTICK_CHANNEL_EMPTY_PERIOD_LIMIT	60*1000		// 60 seconds


bool CCMatchChannel::Create(const CCUID& uid, const char* pszName, const char* pszRuleName, 
						CCCHANNEL_TYPE nType, int nMaxPlayers, int nLevelMin, int nLevelMax,
						const bool bIsTicketChannel, const DWORD dwTicketItemID, const bool bIsUseTicket, const char* pszNameStrResId)
{
	m_nEmptyPeriod		= 0;
	m_nMaxStages		= DEFAULT_CHANNEL_MAXSTAGES;
	m_uidChannel		= uid;
	strcpy(m_szChannelName, pszName);
	strcpy(m_szRuleName, pszRuleName);
	m_nChannelType		= nType;
	m_nMaxPlayers		= nMaxPlayers;
	m_nLevelMin			= nLevelMin;
	m_nLevelMax			= nLevelMax;
	m_nRuleType			= CCCHANNEL_RULE_NOVICE;
	m_bIsTicketChannel	= bIsTicketChannel;
	m_dwTicketItemID	= dwTicketItemID;
	m_bIsUseTicket		= bIsUseTicket;

	// 공식채널의 경우 클라이언트에서 번역된 이름으로 표시할 수 있도록 채널이름의 스트링리소스 ID를 저장했다가 채널리스트에 함께 보낸다
	if (pszNameStrResId) {
		_ASSERT(strlen(pszNameStrResId) < CHANNELNAME_STRINGRESID_LEN);
		strcpy(m_szChannelNameStrResId, pszNameStrResId);
	}
	else m_szChannelNameStrResId[0] = 0;

	//// Paged UserList and SmartRefreshing ////
	m_UserArray.Reserve(NUM_PLAYERLIST_NODE, nMaxPlayers);
	int nMaxPage = (int)ceil((float)nMaxPlayers/(float)NUM_PLAYERLIST_NODE);
	for (int i=0; i<nMaxPage; i++) {
		m_SmartRefresh.AddCategory(new CCRefreshCategoryChannelImpl(this, i));
	}

	for (int i = 0; i < m_nMaxStages; i++)
	{
		m_UnusedStageIndexList.push_back(i);
	}

	memset(m_pStages, 0, sizeof(CCMatchStage*) * MAX_CHANNEL_MAXSTAGES);


	// 채널룰에 대한 특성들 정의
	if (nType == CCCHANNEL_TYPE_PRESET || nType == CCCHANNEL_TYPE_DUELTOURNAMENT)
	{
		if (!stricmp(m_szRuleName, CCCHANNEL_RULE_NOVICE_STR))				m_nRuleType = CCCHANNEL_RULE_NOVICE;
		else if (!stricmp(m_szRuleName, CCCHANNEL_RULE_NEWBIE_STR))			m_nRuleType = CCCHANNEL_RULE_NEWBIE;
		else if (!stricmp(m_szRuleName, CCCHANNEL_RULE_ROOKIE_STR))			m_nRuleType = CCCHANNEL_RULE_ROOKIE;
		else if (!stricmp(m_szRuleName, CCCHANNEL_RULE_MASTERY_STR))		m_nRuleType = CCCHANNEL_RULE_MASTERY;
		else if (!stricmp(m_szRuleName, CCCHANNEL_RULE_ELITE_STR))			m_nRuleType = CCCHANNEL_RULE_ELITE;
		else if (!stricmp(m_szRuleName, CCCHANNEL_RULE_CHAMPION_STR))		m_nRuleType = CCCHANNEL_RULE_CHAMPION;
		else if (!stricmp(m_szRuleName, CCCHANNEL_RULE_QUEST_STR))			m_nRuleType = CCCHANNEL_RULE_QUEST;
		else if (!stricmp(m_szRuleName, CCCHANNEL_RULE_DUELTOURNAMENT_STR))	m_nRuleType = CCCHANNEL_RULE_DUELTOURNAMENT;
		else if (!stricmp(m_szRuleName, CCCHANNEL_RULE_SET1_STR))			m_nRuleType = CCCHANNEL_RULE_SET1;
		else if (!stricmp(m_szRuleName, CCCHANNEL_RULE_SET2_STR))			m_nRuleType = CCCHANNEL_RULE_SET2;
		else if (!stricmp(m_szRuleName, CCCHANNEL_RULE_SET3_STR))			m_nRuleType = CCCHANNEL_RULE_SET3;
	}

	// 룰이름이 "newbie"이면 뉴비채널
//	if ((nType == MCHANNEL_TYPE_PRESET) && (!stricmp(m_szRuleName, MCHANNEL_RULE_NEWBIE_STR)))
//	{
//		m_bNewbieChannel = true;
//	}

	return true;
}

void CCMatchChannel::Destroy(){
	m_ObjUIDCaches.clear();
	m_ObjUIDLobbyCaches.clear();
//	m_ObjStrCaches.clear();
}

bool CCMatchChannel::CheckTick(unsigned long nClock){
	if (nClock - m_nLastTick < CCTICK_CHANNEL_RUN) 
		return false;

	return true;
}

void CCMatchChannel::Tick(unsigned long nClock){
	if (IsChecksumUpdateTime(nClock))
		UpdateChecksum(nClock);

	m_SmartRefresh.UpdateCategory(nClock);

	if (GetObjCount() <= 0) m_nEmptyPeriod += CCTICK_CHANNEL_RUN; else m_nEmptyPeriod = 0;
	m_nLastTick = nClock;
}

bool CCMatchChannel::IsChecksumUpdateTime(unsigned long nTick){
	if (nTick - m_nLastChecksumTick > CCTICK_CHANNEL_UPDATECHECKSUM)
		return true;
	else
		return false;
}

void CCMatchChannel::UpdateChecksum(unsigned long nTick){
	m_nChecksum = (unsigned long)m_ObjUIDCaches.size() + m_uidChannel.Low;
	m_nLastChecksumTick = nTick;
}

bool CCMatchChannel::CheckLifePeriod(){
	CCCHANNEL_TYPE nType = GetChannelType();
	if (nType == CCCHANNEL_TYPE_PRESET || nType == CCCHANNEL_TYPE_DUELTOURNAMENT) 
		return true;

	if (GetObjCount() > 0) 
		return true;

	if (GetEmptyPeriod() < CCTICK_CHANNEL_EMPTY_PERIOD_LIMIT) 
		return true;

	return false;
}

void CCMatchChannel::AddObject(const CCUID& uid, CCMatchObject* pObj){
	m_ObjUIDCaches.Insert(uid, (void*)pObj); // Channel Cache
	//m_ObjStrCaches.insert(MObjectStrMap::value_type(string(pObj->GetCharInfo()->m_szName), pObj));
	
	m_UserArray.Add(pObj);

	JoinLobby(uid, pObj);
}

void CCMatchChannel::RemoveObject(const CCUID& uid){
	LeaveLobby(uid);

	CCUIDRefCache::iterator i = m_ObjUIDCaches.find(uid);
	if (i != m_ObjUIDCaches.end()){
		CCMatchObject* pObj = (CCMatchObject*)((*i).second);
		m_UserArray.Remove(pObj);

		m_ObjUIDCaches.erase(i); // Channel Cache
	}
	
/*
	if (pObj)
	{
		MObjectStrMap::iterator Itor2 = m_ObjStrCaches.find(string(pObj->GetCharInfo()->m_szName));
		if (Itor2 != m_ObjStrCaches.end())
		{
			m_ObjStrCaches.erase(Itor2);
		}
	}
*/	
}

void CCMatchChannel::JoinLobby(const CCUID& uid, const CCMatchObject* pObj){
	if (m_ObjUIDLobbyCaches.find(uid) == m_ObjUIDLobbyCaches.end()){
		m_ObjUIDLobbyCaches.Insert(uid, (void*)pObj); // Channel Cache
	}
	else{
		_ASSERT(0);
	}
}

void CCMatchChannel::LeaveLobby(const CCUID& uid){
	CCUIDRefCache::iterator LobbyObjItor = m_ObjUIDLobbyCaches.find(uid);
	if (LobbyObjItor != m_ObjUIDLobbyCaches.end()){
		m_ObjUIDLobbyCaches.erase(LobbyObjItor);
	}
	else{
		_ASSERT(0);
	}
}

bool CCMatchChannel::AddStage(CCMatchStage* pStage){
	if (m_UnusedStageIndexList.empty()) 
		return false;

	int nRecommendedStageIndex = *(m_UnusedStageIndexList.begin());
	m_UnusedStageIndexList.pop_front();
	if ((nRecommendedStageIndex < 0) || (nRecommendedStageIndex > m_nMaxStages)) 
		return false;

	pStage->SetOwnerChannel(GetUID(), nRecommendedStageIndex);

	if (m_pStages[nRecommendedStageIndex] != NULL){
		_ASSERT(0);
	}

	m_pStages[nRecommendedStageIndex] = pStage;

	return true;
}

bool LessCompStageIndexList(const int a, const int b){
	return (a<b);
}

void CCMatchChannel::RemoveStage(CCMatchStage* pStage){
	int nStageIndex = pStage->GetIndex();

	if ((nStageIndex >= 0) && (nStageIndex < m_nMaxStages)){
		list<int>::iterator it = lower_bound(m_UnusedStageIndexList.begin(), m_UnusedStageIndexList.end(), 
									nStageIndex, LessCompStageIndexList);

		m_UnusedStageIndexList.insert(it, nStageIndex);

		m_pStages[nStageIndex] = NULL;
	}
}

bool CCMatchChannel::IsEmptyStage(int nIndex){
	if ((nIndex < 0) || (nIndex > m_nMaxStages)) 
		return true;

	if (m_pStages[nIndex] == NULL) 
		return true;

	return false;
}

CCMatchStage* CCMatchChannel::GetStage(int nIndex){
	if ((nIndex < 0) || (nIndex > m_nMaxStages)) 
		return NULL;

	return m_pStages[nIndex];
}

int CCMatchChannel::GetPrevStageCount(int nStageIndex){
	if ((nStageIndex < 0) || (nStageIndex > m_nMaxStages)) 
		return 0;

	int nStageCount = 0;
	for (int i = 0; i < nStageIndex; i++){
		if (m_pStages[i] == NULL)  continue;
		if (m_pStages[i]->GetState() == STAGE_STATE_CLOSE) continue;

		nStageCount++;
	}
	return nStageCount;
}

int CCMatchChannel::GetNextStageCount(int nStageIndex){
	if ((nStageIndex < 0) || (nStageIndex > m_nMaxStages)) 
		return 0;

	int nStageCount = 0;
	for (int i = nStageIndex+1; i < m_nMaxStages; i++){
		if (m_pStages[i] == NULL)  continue;
		if (m_pStages[i]->GetState() == STAGE_STATE_CLOSE) continue;

		nStageCount++;
	}
	return nStageCount;
}

void CCMatchChannel::SyncPlayerList(CCMatchObject* pObj, int nPage){
	m_SmartRefresh.SyncClient(pObj->GetRefreshClientChannelImplement());
}


CCMatchChannel* CCMatchChannelMap::Find(const CCUID& uidChannel){
	iterator i = find(uidChannel);
	if(i==end()) 
		return NULL;

	CCMatchChannel* pChannel = (*i).second;
	return pChannel;
}

CCMatchChannel* CCMatchChannelMap::Find(const CCCHANNEL_TYPE nChannelType, const char* pszChannelName){
	if ((nChannelType < 0) || (nChannelType >= CCCHANNEL_TYPE_MAX)) return NULL;

	for(map<CCUID, CCMatchChannel*>::iterator i = m_TypesChannelMap[nChannelType].begin(); 
		i != m_TypesChannelMap[nChannelType].end(); i++)
	{
		CCMatchChannel* pChannel = (*i).second;
		if (strcmp(pChannel->GetName(), pszChannelName) == 0) return pChannel;
	}
	return NULL;
}

bool CCMatchChannelMap::Add(const char* pszChannelName, const char* pszRuleName, CCUID* pAllocUID, CCCHANNEL_TYPE nType, int nMaxPlayers, int nLevelMin, int nLevelMax,
						   const bool bIsTicketChannel, const DWORD dwTicketItemID, const bool bIsUseTicket, const char* pszChannelNameStrResId)
{
	CCUID uidChannel = UseUID();

	CCMatchChannel* pChannel = new MMatchChannel;
	pChannel->Create(uidChannel, pszChannelName, pszRuleName, nType, nMaxPlayers, nLevelMin, nLevelMax, bIsTicketChannel, dwTicketItemID, bIsUseTicket, pszChannelNameStrResId);
	Insert(uidChannel, pChannel);
	*pAllocUID = uidChannel;


	if ((nType >= 0) && (nType < CCCHANNEL_TYPE_MAX))
	{
		m_TypesChannelMap[nType].insert(map<CCUID, CCMatchChannel*>::value_type(uidChannel, pChannel));
	}
	else
	{
		CCMatchServer::GetInstance()->LOG(CCCommandCommunicator::LOG_FILE, "invalid channel type %d",nType);
		erase(uidChannel);

		delete pChannel;
		return false;
	}

	return true;
}

bool CCMatchChannelMap::Remove(const CCUID& uidChannel, CCMatchChannelMap::iterator* pNextItor){
	CCMatchChannelMap::iterator i = find(uidChannel);
	if(i == end()) return false;

	CCMatchChannel* pChannel = (*i).second;
	//LOG(LOG_DEBUG, "Closing Channel '%s'(UID:%d%d)", pChannel->GetName(), pChannel->GetUID().High, pChannel->GetUID().Low);

	CCCHANNEL_TYPE nType = pChannel->GetChannelType();
	if ((nType >= 0) && (nType < CCCHANNEL_TYPE_MAX))
	{
		map<CCUID, CCMatchChannel*>::iterator itorChannelTypeMap = m_TypesChannelMap[nType].find(uidChannel);
		if (itorChannelTypeMap != m_TypesChannelMap[nType].end())
		{
			m_TypesChannelMap[nType].erase(itorChannelTypeMap);
		}
	}


	delete pChannel;

	CCMatchChannelMap::iterator itorTemp = erase(i);
	if (pNextItor)
		*pNextItor = itorTemp;


	
	return true;
}


void CCMatchChannelMap::Destroy(){
	iterator itorChannel = begin();
	while(itorChannel != end()) {
		CCUID uid = (*itorChannel).first;
		Remove(uid, &itorChannel);
	}

	for (int i = 0; i < CCCHANNEL_TYPE_MAX; i++)
	{
		m_TypesChannelMap[i].clear();
	}
}

void CCMatchChannelMap::Update(unsigned long nClock){
	unsigned long nChannelListChecksum = 0;
	for(iterator itor=begin(); itor != end();)
	{
		CCMatchChannel* pChannel = (*itor).second;
		pChannel->Tick(nClock);
		if (pChannel->CheckLifePeriod() == false)
		{
			Remove(pChannel->GetUID(), &itor);
			continue;
		}
		else
		{
			itor++;
		}
		nChannelListChecksum += pChannel->GetChecksum();
	}

	m_nChecksum = nChannelListChecksum;
}

int CCMatchChannelMap::GetChannelCount(CCCHANNEL_TYPE nChannelType){
	if ((nChannelType < 0) || (nChannelType >= CCCHANNEL_TYPE_MAX)) return 0;

	return (int)m_TypesChannelMap[nChannelType].size();

}

map<CCUID, CCMatchChannel*>::iterator CCMatchChannelMap::GetTypesChannelMapBegin(CCCHANNEL_TYPE nType){
	if ((nType < 0) || (nType >= CCCHANNEL_TYPE_MAX)) return m_TypesChannelMap[CCCHANNEL_TYPE_PRESET].begin();
	return m_TypesChannelMap[nType].begin();
}

map<CCUID, CCMatchChannel*>::iterator CCMatchChannelMap::GetTypesChannelMapEnd(CCCHANNEL_TYPE nType){
	if ((nType < 0) || (nType >= CCCHANNEL_TYPE_MAX)) return m_TypesChannelMap[CCCHANNEL_TYPE_PRESET].end();
	return m_TypesChannelMap[nType].end();
}

int CCMatchChannel::GetPlayers(){
	int nPlayers = 0;

	for( CCUIDRefCache::iterator i = GetObjBegin();  i != GetObjEnd();  i++)
	{
		CCMatchObject* pObj = (CCMatchObject*)((*i).second);
		
//		if ( IsAdminGrade(pObj) && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide))		// 테스트 요함
//			continue;

		nPlayers++;
	}

	return nPlayers;
}
