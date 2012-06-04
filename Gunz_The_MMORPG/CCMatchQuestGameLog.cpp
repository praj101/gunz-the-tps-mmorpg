#include "stdafx.h"
#include "CCMatchQuestGameLog.h"
#include "CCQuestConst.h"
#include "CCAsyncDBJob.h"


void CCQuestPlayerLogInfo::AddUniqueItem( const unsigned long int nItemID, int nCount )
{
	if (IsQuestItemID(nItemID))
	{
		// 유니크 아이템들은 다른 테이블에 추가적인 정보를 저장하기위해서 따로 저장해 놓아야 함.
		CCQuestItemDesc* pQItemDesc = GetQuestItemDescMgr().FindQItemDesc( nItemID );
		if( 0 == pQItemDesc )
			return;

		if( !pQItemDesc->m_bUnique )
			return;
	}

	m_UniqueItemList.insert( map<unsigned long int, int>::value_type(nItemID, nCount) );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

CCMatchQuestGameLogInfoManager::CCMatchQuestGameLogInfoManager() : m_nMasterCID( 0 ), m_nScenarioID( 0 ), m_dwStartTime( 0 ), m_dwEndTime( 0 ), m_nTotalRewardQItemCount( 0 )
{
	memset( m_szStageName, 0, 64 );
}


CCMatchQuestGameLogInfoManager::~CCMatchQuestGameLogInfoManager()
{
	Clear();
}


void CCMatchQuestGameLogInfoManager::AddQuestPlayer( const CCUID& uidPlayer, CCMatchObject* pPlayer )
{
	if( (0 == pPlayer) || (uidPlayer != pPlayer->GetUID()) )
		return;

	CCQuestPlayerLogInfo* pQuestPlayerLogInfo = new CCQuestPlayerLogInfo;
	if( 0 == pQuestPlayerLogInfo )
		return;

	pQuestPlayerLogInfo->SetCID( pPlayer->GetCharInfo()->m_nCID );

	insert( value_type(uidPlayer, pQuestPlayerLogInfo) );
}

///
// 퀘스트를 클리어하고서 보상받은 아이템을 등록함.
// 끝나고 로그에 저장됨.
///
bool CCMatchQuestGameLogInfoManager::AddRewardQuestItemInfo( const CCUID& uidPlayer, CCQuestItemMap* pObtainQuestItemList )
{
	CCQuestPlayerLogInfo* pQuestPlayerLogInfo = Find( uidPlayer );
	if( 0 == pQuestPlayerLogInfo )
		return false;

	if( 0 == pObtainQuestItemList )
		return false;

	// 만약을 위해서 퀘스트 아이템에 관련된 모든 정보를 지운다.
	pQuestPlayerLogInfo->ClearQItemInfo();

	CCQuestItemMap::iterator itQItem, endQItem;
	endQItem = pObtainQuestItemList->end();
	for( itQItem = pObtainQuestItemList->begin(); itQItem != endQItem; ++itQItem )
	{
		if( GetQuestItemDescMgr().FindQItemDesc(itQItem->second->GetItemID())->m_bUnique )
			pQuestPlayerLogInfo->AddUniqueItem( itQItem->second->GetItemID(), itQItem->second->GetCount() );

		m_nTotalRewardQItemCount += itQItem->second->GetCount();
	}

	return true;
}

bool CCMatchQuestGameLogInfoManager::AddRewardZItemInfo( const CCUID& uidPlayer, CCQuestRewardZItemList* pObtainZItemList )
{
	CCQuestPlayerLogInfo* pQuestPlayerLogInfo = Find( uidPlayer );
	if (( 0 == pQuestPlayerLogInfo ) || ( 0 == pObtainZItemList )) return false;

	for(CCQuestRewardZItemList::iterator itor = pObtainZItemList->begin(); itor != pObtainZItemList->end(); ++itor )
	{
		RewardZItemInfo iteminfo = (*itor);
		CCMatchItemDesc* pItemDesc = CCGetMatchItemDescMgr()->GetItemDesc(iteminfo.nItemID);
		if (pItemDesc == NULL) continue;

		// 유니크 로그 생성
		pQuestPlayerLogInfo->AddUniqueItem( iteminfo.nItemID, 1);
	}

	return true;
}

void CCMatchQuestGameLogInfoManager::Clear()
{
	if( empty() )
		return;

	// Player제거.
	CCMatchQuestGameLogInfoManager::iterator It, End;
	for( It = begin(), End = end(); It != End; ++It )
		delete It->second;
	
	clear();

	m_nTotalRewardQItemCount = 0;
}

CCQuestPlayerLogInfo* CCMatchQuestGameLogInfoManager::Find( const CCUID& uidPlayer )
{
	CCMatchQuestGameLogInfoManager::iterator It = find( uidPlayer );
	if( end() == It )
		return 0;

	CCQuestPlayerLogInfo* pQuestPlayerLogInfo = It->second;
	if( 0 == pQuestPlayerLogInfo )
		return 0;

	return pQuestPlayerLogInfo;
}


///
// First : 2005.04.18 추교성.
// Last  : 2005.04.18 추교성.
//
// 퀘스트가 완료되면 저장되있던 정보를 가지고 디비에 로그를 남기는 작업을 함.
///
bool CCMatchQuestGameLogInfoManager::PostInsertQuestGameLog()
{
	const int nElapsedPlayTime = (m_dwEndTime - m_dwStartTime) / 60000; // 분단위로 계산을 함.

	CCAsyncDBJob_InsertQuestGameLog* pAsyncDbJob_InsertGameLog = new CCAsyncDBJob_InsertQuestGameLog;
	if( 0 == pAsyncDbJob_InsertGameLog )
		return false;

	pAsyncDbJob_InsertGameLog->Input( m_szStageName, 
									  m_nScenarioID, 
									  m_nMasterCID, 
									  this,
                                      m_nTotalRewardQItemCount, 
									  nElapsedPlayTime );

	CCMatchServer::GetInstance()->PostAsyncJob( pAsyncDbJob_InsertGameLog );

	return true;
}


void CCMatchQuestGameLogInfoManager::SetStageName( const char* pszStageName )
{
	if( (0 == pszStageName) || (64 < strlen(pszStageName)) )
		return;

	strcpy( m_szStageName, pszStageName );
}