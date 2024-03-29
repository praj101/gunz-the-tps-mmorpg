#pragma warning(disable : 4018)
#include "stdafx.h"
//#include <winsock2.h>
#include "CCMatchServer.h"
#include "CCMatchObject.h"
#include "CCMatchGlobal.h"
#include "CCMatchConfig.h"
#include "CCUtil.h"

#include "CCAsyncDBJob_InsertCharBRInfo.h"
#include "CCAsyncDBJob_UpdateCharBRInfo.h"
#include "CCAsyncDBJob_RewardCharBR.h"

#define CYCLE_MATCHSTAGELISTUPDATE			1000
#define CYCLE_MATCHCHANNELLISTUPDATE		1000
#define CYCLE_MATCHCHANNELPLAYERLISTUPDATE	1000
#define CYCLE_MATCHCHANNELCLANMEMBER		1000
#define CYCLE_MATCHBATTLETIMEREWARDUPDATE	500
#define CYCLE_CHAR_BUFF_CHECK				500


#define CYCLE_MATCH_STANDBY_CLANLIST_UPDATE	1000		// 클랜전 대기 클랜 리스트 업데이트 시간은 10초이다.



const DWORD CCMatchDisconnStatusInfo::CCINTERVAL_DISCONNECT_STATUS_MIN = (5 * 1000);

CCMatchObject::CCMatchObject(const CCUID& uid) : CCObject(uid) 
{ 
	m_pCharInfo = NULL;
	m_pFriendInfo = NULL;

	m_dwIP = 0;
	ZeroMemory(m_szIP, sizeof(char)*64);	
	m_nPort=0;
	
	m_uidStage = CCUID(0,0);
	m_uidChatRoom = CCUID(0,0);

	m_bBridgePeer = false;
	m_bRelayPeer = false;
	m_uidAgent = CCUID(0,0);

	m_nPlayerFlags = 0;
	m_nUserOptionFlags = 0;

	m_ChannelInfo.Clear();

	m_bStageListTransfer = false;
	m_nStageListChecksum = 0;
	m_nStageListLastChecksum = 0;
	m_nTimeLastStageListTrans = 0;
	m_nStageCursor = 0;

	m_RefreshClientChannelImpl.SetMatchObject(this);
	m_RefreshClientClanMemberImpl.SetMatchObject(this);

	m_nTeam=CCMT_ALL;
	SetLadderGroupID(0);
	m_nStageState = MOSS_NONREADY;
	m_bEnterBattle=false;
	m_bAlive=false;
	m_bForcedEntried = false;
	m_bLadderChallenging = false;
	m_nKillCount = 0;
	m_nDeathCount = 0;
	m_nPlace = MMP_OUTSIDE;
	m_bLaunchedGame = false;
	m_nAllRoundDeathCount = 0;
	m_nAllRoundKillCount = 0;
	m_bNewbie = false;
	m_nDeadTime = 0;

	m_GameInfo.bJoinedGame = false;

	m_bDBFriendListRequested = false;

	m_nTickLastPacketRecved = 0;
	m_nLastHShieldMsgRecved = 0;
	m_bHShieldMsgRecved = false;
	m_bHacker = false;

	//m_dwLastHackCheckedTime			= timeGetTime();
	//m_dwLastRecvNewHashValueTime	= timeGetTime();
	//m_bIsRequestNewHashValue		= false;

	m_dwLastSpawnTime				= timeGetTime();

	m_dwHShieldCheckCount = 0;

	m_nLastPingTime = m_nQuestLatency = 0;
	m_bQuestRecvPong = true;

	m_bIsLoginCompleted				= false;
	m_dwLastSendGambleItemTime		= 0;
	m_IsSendFirstGameguardRequest	= false;
	m_IsRecvFirstGameguardResponse	= false;

	m_DBJobQ.bIsRunningAsyncJob		= false;
	m_DBJobQ.nLastJobID				= 0;

	m_pDuelTournamentCharInfo = NULL;
	m_CharBuffInfo.SetObject(this);

	m_nLastCheckBattleTimeReward = 0;
}

CCMatchObject::~CCMatchObject()
{
	FreeCharInfo();
	FreeFriendInfo();
	FreeDuelTournamentInfo();

	LoginNotCompleted();
}

void CCMatchObject::FreeCharInfo()
{
	if (m_pCharInfo) {
		m_pCharInfo->Clear();
		delete m_pCharInfo;
		m_pCharInfo = NULL;
	}
}

void CCMatchObject::FreeFriendInfo()
{
	if (m_pFriendInfo) {
		delete m_pFriendInfo;
		m_pFriendInfo = NULL;
	}
	m_bDBFriendListRequested = false;
}

void CCMatchObject::FreeDuelTournamentInfo()
{
	if(m_pDuelTournamentCharInfo){
		delete m_pDuelTournamentCharInfo;
		m_pDuelTournamentCharInfo = NULL;
	}
}

void CCMatchObject::SetTeam(CCMatchTeam nTeam)
{ 
	m_nTeam = nTeam; 

	if (IsAdminGrade(this) && CheckPlayerFlags(CCTD_PlayerFlags_AdminHide))
		m_nTeam = CCMT_SPECTATOR;
}

void CCMatchObject::SetStageCursor(int nStageCursor)
{
	m_nStageCursor = nStageCursor;
}

void CCMatchObject::SetPlace(CCMatchPlace nPlace)
{
	m_nPlace = nPlace;

	switch(m_nPlace) {
	case MMP_OUTSIDE:
		{
			CCRefreshClientChannelImpl* pChannelImpl = GetRefreshClientChannelImplement();
			pChannelImpl->Enable(false);
		}
		break;
	case MMP_LOBBY:
		{
			CCRefreshClientChannelImpl* pChannelImpl = GetRefreshClientChannelImplement();
			pChannelImpl->Enable(true);
		}
		break;
	case MMP_STAGE:
		{
			CCRefreshClientChannelImpl* pChannelImpl = GetRefreshClientChannelImplement();
			pChannelImpl->Enable(false);
		}
		break;
	case MMP_BATTLE:
		{
			CCRefreshClientChannelImpl* pChannelImpl = GetRefreshClientChannelImplement();
			pChannelImpl->Enable(false);
		}
		break;
	default:
		{
		}break;
	};
}

void CCMatchObject::Tick(unsigned long int nTime)
{
	CCMatchServer* pServer = CCMatchServer::GetInstance();

	if (CheckStageListTransfer() == true) {
		// 로비에서 클랜채널에 있으면 클랜전 대기 클랜 리스트 업데이트해준다.
		CCMatchChannel* pChannel = pServer->FindChannel(GetChannelUID());
		if ((CCGetServerConfig()->GetServerMode() == CSM_CLAN) && (pChannel) && (pChannel->GetChannelType() == CCCHANNEL_TYPE_CLAN))
		{
			if ((unsigned int)(nTime - m_nTimeLastStageListTrans) > CYCLE_MATCH_STANDBY_CLANLIST_UPDATE) {
				unsigned long int nCurrStageListChecksum = pServer->GetLadderMgr()->GetChecksum(m_nStageCursor, 
																			TRANS_STANDBY_CLANLIST_NODE_COUNT);
				if (nCurrStageListChecksum != GetStageListChecksum()) {
					m_nTimeLastStageListTrans = nTime;

					pServer->StandbyClanList(GetUID(), m_nStageCursor, true);
					UpdateStageListChecksum(nCurrStageListChecksum);
				}
			}
		}
		else
		{
			// 일반적인 스테이지 리스트 업데이트
			if ((unsigned int)(nTime - m_nTimeLastStageListTrans) > CYCLE_MATCHSTAGELISTUPDATE) {
				unsigned long int nCurrStageListChecksum = pServer->GetStageListChecksum(m_ChannelInfo.uidChannel, 
																		m_nStageCursor, TRANS_STAGELIST_NODE_COUNT);
				if (nCurrStageListChecksum != GetStageListChecksum()) {
					m_nTimeLastStageListTrans = nTime;

					pServer->StageList(GetUID(), m_nStageCursor, true);
					UpdateStageListChecksum(nCurrStageListChecksum);
				}
			}
		}
	}

	if (CheckChannelListTransfer() == true) {
		if ((unsigned int)(nTime - m_ChannelInfo.nTimeLastChannelListTrans) > CYCLE_MATCHCHANNELLISTUPDATE) {
			if (pServer->GetChannelListChecksum() != GetChannelListChecksum()) {
				m_ChannelInfo.nTimeLastChannelListTrans = nTime;

				if ((m_ChannelInfo.nChannelListType != CCCHANNEL_TYPE_CLAN) || (GetChannelListChecksum() == 0))
				{
					pServer->ChannelList(GetUID(), m_ChannelInfo.nChannelListType);
					UpdateChannelListChecksum(pServer->GetChannelListChecksum());				
				}
			}
		}
	}

	if (GetRefreshClientChannelImplement()->IsEnable()) {
		if (nTime - GetRefreshClientChannelImplement()->GetLastUpdatedTime() > CYCLE_MATCHCHANNELPLAYERLISTUPDATE) {
			GetRefreshClientChannelImplement()->SetLastUpdatedTime(nTime);

			CCMatchChannel* pChannel = pServer->FindChannel(GetChannelUID());
			if (pChannel) {
				pChannel->SyncPlayerList(this, GetRefreshClientChannelImplement()->GetCategory());
			}
		}
	}

	if (GetRefreshClientClanMemberImplement()->IsEnable()) {
		if (nTime - GetRefreshClientClanMemberImplement()->GetLastUpdatedTime() > CYCLE_MATCHCHANNELCLANMEMBER) {
			GetRefreshClientClanMemberImplement()->SetLastUpdatedTime(nTime);

			CCMatchClan* pClan = pServer->FindClan(GetCharInfo()->m_ClanInfo.m_nClanID);
			if (pClan) {
				pClan->SyncPlayerList(this, GetRefreshClientClanMemberImplement()->GetCategory());
			}
		}
	}
	

	if( GetEnterBattle() && m_pCharInfo != NULL )
	{
		if( nTime - GetLastCheckBattleTimeReward() > CYCLE_MATCHBATTLETIMEREWARDUPDATE )
		{
			SetLastCheckBattleTimeReward(nTime);

			BattleTimeReward(nTime);
		}		
	}

	GetDisconnStatusInfo().Update( nTime );

	//버프정보임시주석 
	/*if( m_pCharInfo ) {		
		if( nTime - m_CharBuffInfo.GetLastCheckBuffInfoTime() > CYCLE_CHAR_BUFF_CHECK ) {			
			int nPlayTime = CCGetTimeDistance(m_pCharInfo->m_nConnTime, nTime) / 1000;
			m_pCharInfo->UpdatePlayTime(nPlayTime);

			bool bChanged = m_CharBuffInfo.Tick(nTime);
			if( bChanged ) {
				pServer->PostCmdCharacterBuffInfo(m_UID);
			}

			m_CharBuffInfo.SetLastCheckBuffInfoTime(nTime);
		}
	}*/
}

void CCMatchObject::SetBattleTimeReward(bool bVal)
{
	if( bVal )
	{
		unsigned long int nTime = CCMatchServer::GetInstance()->GetGlobalClockCount();

		CCMatchCharBattleTimeRewardInfoMap::iterator iter = m_pCharInfo->GetBRInfoMap().begin();
		for( ; iter !=  m_pCharInfo->GetBRInfoMap().end(); iter++ )
		{
			CCMatchCharBRInfo* pInfo = iter->second;
			pInfo->SetLastCheckTime(nTime);
			pInfo->SetLastUpdateDBTime(nTime);
		}
	}
}

void CCMatchObject::BattleTimeReward(unsigned int nTime)
{
	CCMatchServer* pServer = CCMatchServer::GetInstance();
	CCMatchBRDescriptionMap pMap = pServer->GetBattleTimeRewardMachine().GetBattleTimeRewardDescriptionMap();
	for( CCMatchBRDescriptionMap::iterator iter = pMap.begin(); iter != pMap.end(); iter++ )
	{
		CCMatchBRDescription* pDesc = iter->second;

		CCMatchCharBRInfo* pInfo = m_pCharInfo->GetBRInfoMap().Get(pDesc->GetBRID());

		// 만약 Description은 있지만, 정보가 없다면.. 새로 만들어서 삽입!
		// (DB에 정보가 삽입될 때까지는 체크하지 않는다)
		if( pInfo == NULL )
		{	
			pInfo = new CCMatchCharBRInfo(pDesc->GetBRID(), pDesc->GetBRTID(), 0, 0, 0);

			pInfo->SetLastCheckTime(nTime);
			pInfo->SetLastUpdateDBTime(nTime);

			if( m_pCharInfo->GetBRInfoMap().Insert(pInfo->GetBRID(), pInfo) )
			{
				CCAsyncDBJob_GetCharBRInfo* pJob = new CCAsyncDBJob_GetCharBRInfo(m_UID);
				pJob->Input(m_pCharInfo->m_nCID, pDesc->GetBRID(), pDesc->GetBRTID());
				m_DBJobQ.DBJobQ.push_back(pJob);
			}
		}
		else
		{
			if( !pInfo->IsCheckSkip() )
			{
				BRRESULT nResult = pInfo->CheckBattleTimeReward(nTime, pDesc);

				if( nResult == BRRESULT_DO_REWARD )
				{
					CCMatchBRItem* pBRItem = pDesc->GetRewardItem();
					if( pBRItem == NULL ) { _ASSERT(0); return; }

					pServer->OnAsyncRequest_RewardCharBP(m_UID, pInfo->GetBRID(), pInfo->GetBRTID(), 
						pInfo->GetRewardCount(), pInfo->GetBattleTime(), pInfo->GetKillCount(), 
						pBRItem->GetItemID((int)m_pCharInfo->m_nSex), pBRItem->GetItemCnt(), pBRItem->GetRentHourPeriod());

					m_pCharInfo->GetBRInfoMap().Remove(pInfo->GetBRID());
				}
				else if( nResult == BRRESULT_RESET_INFO )
				{
					pInfo->ResetInfo();

					pServer->RouteCmdBattleTimeReward(m_UID, GetStageUID(), pDesc->GetName().c_str(), pDesc->GetResetDesc().c_str(), 0, 0, 0, pDesc->GetRewardCount());
				}
				else if( nResult == BRRESULT_NO_REWARD )
				{
					if( pInfo->IsNeedUpdateDB(nTime) )
					{
						pServer->OnAsyncRequest_UpdateCharBRInfo(m_UID, pInfo->GetBRID(), pInfo->GetBRTID(), 
							pInfo->GetRewardCount(), pInfo->GetBattleTime(), pInfo->GetKillCount());
					}
					else if( pInfo->IsExpired(pDesc->GetBRTID()) )
					{						
#ifdef _DEBUG
						cclog("CID(%d), 의 BR Info(%d, %d, %d, %d)가 만료되었습니다.\n", m_pCharInfo->m_nCID, pInfo->GetBRID(), pInfo->GetBRTID(), 
							pInfo->GetBattleTime(), pInfo->GetKillCount());
#endif

						pServer->OnAsyncRequest_UpdateCharBRInfo(m_UID, pInfo->GetBRID(), pInfo->GetBRTID(), 
							pInfo->GetRewardCount(), pInfo->GetBattleTime(), pInfo->GetKillCount());

						m_pCharInfo->GetBRInfoMap().Remove(pInfo->GetBRID());
					}
				}								
			}
		}
	}
}

void CCMatchObject::OnStageJoin()
{
	SetAllRoundDeathCount(0);	
	SetAllRoundKillCount(0);
	SetStageListTransfer(false);	
	SetForcedEntry(false);
	SetPlace(MMP_STAGE);
	m_GameInfo.bJoinedGame = false;
	m_nDeadTime = 0;
}

void CCMatchObject::OnEnterBattle()
{	
	SetAlive(false);
	SetKillCount(0);
	SetDeathCount(0);
	SetAllRoundDeathCount(0);	
	SetAllRoundKillCount(0);

	SetEnterBattle(true);
	SetBattleTimeReward(true);

	ResetGamePlayInfo();
}

void CCMatchObject::OnLeaveBattle()
{	
	SetKillCount(0);
	SetDeathCount(0);
	SetAlive(false);
	SetStageState(MOSS_NONREADY);
	SetLaunchedGame(false);

	SetEnterBattle(false);
	SetBattleTimeReward(false);
}


void CCMatchObject::OnInitRound()
{
	SetAlive(true);
	SetKillCount(0);
	SetDeathCount(0);
	ResetCustomItemUseCount();

	m_GameInfo.bJoinedGame = true;
	m_nDeadTime = 0;
}

void CCMatchObject::SetChannelListTransfer(const bool bVal, const CCCHANNEL_TYPE nChannelType)
{ 
	if ((nChannelType < 0) || (nChannelType >= CCCHANNEL_TYPE_MAX)) 
	{
		_ASSERT(0);
		return;
	}

	m_ChannelInfo.bChannelListTransfer = bVal; 
	m_ChannelInfo.nChannelListType = nChannelType;
	UpdateChannelListChecksum(0); 
}

bool CCMatchObject::CheckEnableAction(CCMO_ACTION nAction)
{
	switch (nAction)
	{
	case CCMOA_STAGE_FOLLOW:		// 따라가기가 가능한 상태인지 여부
		{
			if (GetPlace() != MMP_LOBBY) return false;
			if (IsLadderChallenging()) return false;
			if (IsChallengeDuelTournament()) return false;

			return true;
		}
		break;
	default:
		_ASSERT(0);
	}

	return true;
}

void CCMatchObject::CheckNewbie(int nCharMaxLevel)
{
#define NEWBIE_LEVEL_CUTLINE		20		// 가지고 있는 캐릭터들의 최고레벨이 21레벨이상이면 뉴비가 아니다.

	if (nCharMaxLevel > NEWBIE_LEVEL_CUTLINE) m_bNewbie = false;
	else m_bNewbie = true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CCMatchCharInfo::EquipFromItemList()
{
	m_EquipedItem.Clear();
	
	for (CCMatchItemMap::iterator itor = m_ItemList.begin(); itor != m_ItemList.end(); ++itor)
	{
		CCMatchItem* pItem = (*itor).second;
		pItem->SetEquiped(false);
	}

	for (int i = 0; i < MMCIP_END; i++)
	{
		if (m_nEquipedItemCIID[i] == 0) continue;
		for (CCMatchItemMap::iterator itor = m_ItemList.begin(); itor != m_ItemList.end(); ++itor)
		{
			CCMatchItem* pItem = (*itor).second;
			if ( m_nEquipedItemCIID[i] == pItem->GetCIID() )
			{
				if( m_EquipedItem.SetItem(CCMatchCharItemParts(i), itor->first, pItem) ) {
					break;
				} else {
					return false;
				}				
			}
		}
	}

	return true;
}

void CCMatchCharInfo::ClearItems()
{
	m_EquipedItem.Clear();
	m_ItemList.Clear();
	m_QuestItemList.Clear();
}

void CCMatchCharInfo::Clear()
{
	m_nCID					= 0;
	m_nCharNum				= 0;
	m_nLevel				= 0;
	m_nSex					= MMS_MALE;
	m_nFace					= 0;
	m_nHair					= 0;
	m_nXP					= 0;
	m_nBP					= 0;
	m_fBonusRate			= DEFAULT_CHARINFO_BONUSRATE;
	m_nPrize				= DEFAULT_CHARINFO_PRIZE;
	m_nHP					= 0;
	m_nAP					= 0;
	m_nMaxWeight			= DEFAULT_CHARINFO_MAXWEIGHT;
	m_nSafeFalls			= DEFAULT_CHARINFO_SAFEFALLS;
	m_nFR					= 0;
	m_nCR					= 0;
	m_nER					= 0;
	m_nWR					= 0;
	m_nTotalPlayTimeSec		= 0;
	m_nConnTime				= 0;
	m_nBattleStartTime		= 0;
	m_nBattleStartXP		= 0;
	m_nTotalKillCount		= 0;
	m_nTotalDeathCount		= 0;
	m_nConnKillCount		= 0;
	m_nConnDeathCount		= 0;

	memset(m_szName, 0, MATCHOBJECT_NAME_LENGTH);
	memset(m_nEquipedItemCIID, 0, sizeof(m_nEquipedItemCIID));
	
	m_ClanInfo.Clear();
	
	ClearItems();

	m_IsSendMyItemListByRequestClient = false;
}

void CCMatchCharInfo::GetTotalWeight(int* poutWeight, int* poutMaxWeight)
{
	int nWeight, nMaxWeight;

	m_EquipedItem.GetTotalWeight(&nWeight, &nMaxWeight);
	nMaxWeight = nMaxWeight + m_nMaxWeight;

	*poutWeight = nWeight;
	*poutMaxWeight = nMaxWeight;
}


bool IsEquipableItem(unsigned long int nItemID, int nPlayerLevel, CCMatchSex nPlayerSex)
{
	CCMatchItemDesc* pItemDesc = CCGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pItemDesc == NULL) return false;

	// 성별 제한 조건
	if (pItemDesc->m_nResSex.Ref() != -1)
	{
		if (pItemDesc->m_nResSex.Ref() != int(nPlayerSex)) return false;
	}

	if (CCGetServerConfig()->GetServerMode() != CSM_EVENT) {	// EVENT때 레벨제한 없이 장착한다
		// 레벨 제한 조건
		if (pItemDesc->m_nResLevel.Ref() > nPlayerLevel) return false;
	}

	return true;
}



void CCMatchObject::SetFriendInfo(CCMatchFriendInfo* pFriendInfo)
{
	m_bDBFriendListRequested = true;
	m_pFriendInfo = pFriendInfo;
}


void CCMatchObject::SetCharInfo(CCMatchCharInfo* pCharInfo)
{ 
	m_pCharInfo = pCharInfo; 
#ifdef _QUEST_ITEM
	if( CSM_TEST == CCGetServerConfig()->GetServerMode() ) 
	{
		m_pCharInfo->m_DBQuestCachingData.SetCharObject( this );
	}
#endif
}

void CCMatchObject::SetDuelTournamentCharInfo(CCMatchObjectDuelTournamentCharInfo *pDTCharInfo)
{
	// 이미 있는 정보면 어쩌죠? -.,-;
	// 그럴 일은 없겠지만.. 지우고 다시 할당하자!
	if( m_pDuelTournamentCharInfo ) {
		delete m_pDuelTournamentCharInfo;
		m_pDuelTournamentCharInfo = NULL;
	}

	m_pDuelTournamentCharInfo = pDTCharInfo;
}

void CCMatchObject::OnDead()
{
	CCMatchServer* pServer = CCMatchServer::GetInstance();
	m_nDeadTime = pServer->GetTickTime();
	SetAlive(false);
	DeathCount();
}

void CCMatchObject::OnKill()
{
	KillCount();
}

bool CCMatchObject::IsEnabledRespawnDeathTime(unsigned int nNowTime)
{
	// 캐릭터가 죽은후 6.5초안에 다시 죽었다는 메세지가 넘어오면 처리안해준다.
	// 버그발생, 죽은후 6.5초안에 스테이지 나간 후 다시 배틀에들어와 죽는다면 true 리턴한다

	CCMatchBuffSummary* pBuffSummary = GetCharBuff()->GetBuffSummary();
	int nDelayAfterDying = pBuffSummary->GetRespawnTime(RESPAWN_DELAYTIME_AFTER_DYING);

	if ((nNowTime - m_nDeadTime) > (nDelayAfterDying - 500)) return true;
	return false;
}

void CCMatchObject::UpdateTickLastPacketRecved()
{
	CCMatchServer* pServer = CCMatchServer::GetInstance();
	m_nTickLastPacketRecved = pServer->GetTickTime();
}

void CCMatchObject::UpdateLastHShieldMsgRecved()
{
	CCMatchServer* pServer = CCMatchServer::GetInstance();
	m_nLastHShieldMsgRecved = pServer->GetTickTime();
	SetHShieldMsgRecved(true);		// 이 클라이언트는 최신 ReqMsg에 대한 응답을 보냈다.
}

void CCMatchObject::DisconnectHacker( CCMatchHackingType eType )
{
	GetDisconnStatusInfo().SetStatus( CCMDS_DISCONN_WAIT );

	const PUNISH_TABLE_ITEM& punish = CCPunishTable::GetPunish(eType);

	GetDisconnStatusInfo().SetMsgID( punish.dwMessageID ); 
	GetDisconnStatusInfo().SetHackingType( eType ); 
	GetDisconnStatusInfo().SetComment( punish.szComment );
	GetDisconnStatusInfo().SetEndDate( CCGetStrLocalTime(punish.nDay, punish.nHour, punish.nMin) ); 
	GetDisconnStatusInfo().SetBlockLevel( punish.eLevel );

	if( NULL != GetAccountInfo() ) {
		GetAccountInfo()->m_HackingType = eType;
	}
}

/*
void CCMatchObject::SetBadUserDisconnectWaitInfo( const CCMatchDisconnectStatus DisStatus )
{
	GetDisconnStatusInfo().SetStatus( DisStatus );

	GetDisconnStatusInfo().SetMsgID( 130001 ); 
	GetDisconnStatusInfo().SetHackingType( CCMHT_BADUSER ); // 이 기능은 X-Trap에서 사용함.
	GetDisconnStatusInfo().SetComment( "irregularity player" );
	GetDisconnStatusInfo().SetEndDate( CCGetStrLocalTime(0, 0, 0, 0, 10) ); // 10분후 접속 가능.
	GetDisconnStatusInfo().SetBlockLevel( MMBL_LOGONLY );

	if( NULL != GetAccountInfo() )
		GetAccountInfo()->m_HackingType = CCMHT_BADUSER;
}


void CCMatchObject::SetXTrapHackerDisconnectWaitInfo( const CCMatchDisconnectStatus DisStatus )
{
	GetDisconnStatusInfo().SetStatus( DisStatus );

	GetDisconnStatusInfo().SetMsgID( 130001 ); 
	GetDisconnStatusInfo().SetHackingType( CCMHT_XTRAP_HACKER ); // 이 기능은 X-Trap에서 사용함.
	GetDisconnStatusInfo().SetComment( "x-trap hacking detected." );
	GetDisconnStatusInfo().SetEndDate( CCGetStrLocalTime(0, 0, 0, 1, 0) ); // 1시간 후에 접속 가능.
	GetDisconnStatusInfo().SetBlockLevel( MMBL_LOGONLY );

	if( NULL != GetAccountInfo() )
		GetAccountInfo()->m_HackingType = CCMHT_XTRAP_HACKER;
}


void CCMatchObject::SetHShieldHackerDisconnectWaitInfo( const CCMatchDisconnectStatus DisStatus )
{
	GetDisconnStatusInfo().SetStatus( DisStatus );

	GetDisconnStatusInfo().SetMsgID( 130001 ); 
	GetDisconnStatusInfo().SetHackingType( CCMHT_HSHIELD_HACKER ); // 이 기능은 HShield에서 사용.
	GetDisconnStatusInfo().SetComment( "hackshield hacking detected." );
	GetDisconnStatusInfo().SetEndDate( CCGetStrLocalTime(0, 0, 0, 1, 0) ); // 1시간 후에 접속 가능.
	GetDisconnStatusInfo().SetBlockLevel( MMBL_LOGONLY );

	if( NULL != GetAccountInfo() )
		GetAccountInfo()->m_HackingType = CCMHT_HSHIELD_HACKER;
}


void CCMatchObject::SetBadFileCRCDisconnectWaitInfo( const CCMatchDisconnectStatus DisStatus )
{
	GetDisconnStatusInfo().SetStatus( DisStatus );

	GetDisconnStatusInfo().SetMsgID( 130004 ); 
	GetDisconnStatusInfo().SetHackingType( CCMHT_BADFILECRC ); // 이 기능은 X-Trap에서 사용함.
	GetDisconnStatusInfo().SetComment( "bad filecrc." );
	GetDisconnStatusInfo().SetEndDate( CCGetStrLocalTime(0, 0, 0, 0, 0) ); // 1시간 후에 접속 가능.
	GetDisconnStatusInfo().SetBlockLevel( MMBL_LOGONLY );

	if( NULL != GetAccountInfo() )
		GetAccountInfo()->m_HackingType = CCMHT_BADFILECRC;
}


void CCMatchObject::SetGameguardHackerDisconnectWaitInfo( const CCMatchDisconnectStatus DisStatus )
{
	GetDisconnStatusInfo().SetStatus( DisStatus );

	GetDisconnStatusInfo().SetMsgID( 130001 ); 
	GetDisconnStatusInfo().SetHackingType( CCMHT_GAMEGUARD_HACKER ); 
	GetDisconnStatusInfo().SetComment( "gameguard hacker" );
	GetDisconnStatusInfo().SetEndDate( CCGetStrLocalTime(0, 0, 0, 0, 0) ); 
	GetDisconnStatusInfo().SetBlockLevel( MMBL_LOGONLY );

	if( NULL != GetAccountInfo() )
		GetAccountInfo()->m_HackingType = CCMHT_GAMEGUARD_HACKER;
}


void CCMatchObject::SetDllInjectionDisconnectWaitInfo( const CCMatchDisconnectStatus DisStatus )
{
	GetDisconnStatusInfo().SetStatus( DisStatus );

	GetDisconnStatusInfo().SetMsgID( 130001 ); 
	GetDisconnStatusInfo().SetHackingType( CCMHT_GIVE_ONESELF_UP_DLLINJECTION ); 
	GetDisconnStatusInfo().SetComment( "dll injectoin" );
	GetDisconnStatusInfo().SetEndDate( CCGetStrLocalTime(0, 0, 0, 0, 0) ); 
	GetDisconnStatusInfo().SetBlockLevel( MMBL_LOGONLY );

	if( NULL != GetAccountInfo() )
	{
		GetAccountInfo()->m_HackingType = CCMHT_GIVE_ONESELF_UP_DLLINJECTION;
	}
}


void CCMatchObject::SetInvalidStageSettingDisconnectWaitInfo( const CCMatchDisconnectStatus DisStatus )
{
	GetDisconnStatusInfo().SetStatus( DisStatus );

	GetDisconnStatusInfo().SetMsgID( 130001 ); 
	GetDisconnStatusInfo().SetHackingType( CCMHT_INVALIDSTAGESETTING ); 
	GetDisconnStatusInfo().SetComment( "invalid stage setting." );
	GetDisconnStatusInfo().SetEndDate( CCGetStrLocalTime(0, 0, 0, 0, 0) ); 
	GetDisconnStatusInfo().SetBlockLevel( MMBL_LOGONLY );

	if( NULL != GetAccountInfo() )
	{
		GetAccountInfo()->m_HackingType = CCMHT_INVALIDSTAGESETTING;
	}
}
*/

void CCMatchObject::ResetCustomItemUseCount()
{
	for(int i = MMCIP_CUSTOM1; i < MMCIP_CUSTOM2 + 1; i++)
	{
		CCMatchItem* pCustomItem = GetCharInfo()->m_EquipedItem.GetItem( CCMatchCharItemParts(i) );
		if( pCustomItem ) 
		{
			if( pCustomItem->GetDesc()->IsSpendableItem() == false )
			{
				pCustomItem->ResetUseCountOfNonDestroyItem();
			}
		}
	}
}


const bool CCMatchObject::IsHaveCustomItem()
{
	// 단순히 커스텀 아이템을 착용하고 있는지 확인
	if( NULL != GetCharInfo()->m_EquipedItem.GetItem(MMCIP_CUSTOM1) ) return true;
	if( NULL != GetCharInfo()->m_EquipedItem.GetItem(MMCIP_CUSTOM2) ) return true;

	return false;
}

const bool CCMatchObject::IncreaseCustomItemUseCount()
{
	/*
	처음 착용한 곳을 검사해서 더이상 사용 할 수 없다면 다음 착용한 곳을 검사한다.
	만약 다음 착용한 부분마저 수량이 남지 않았다면 작업은 실패한다.
	*/

	for(int i = MMCIP_CUSTOM1; i < MMCIP_CUSTOM2 + 1; i++)
	{
		CCMatchItem* pCustomItem = GetCharInfo()->m_EquipedItem.GetItem( CCMatchCharItemParts(i) );
		if( pCustomItem ) 
		{
			if( pCustomItem->GetDesc()->IsSpendableItem() ) 
			{
				return true;
			}
			else if( pCustomItem->GetDesc()->m_nMagazine.Ref() > pCustomItem->GetUseCountOfNonDestroyItem() )
			{
				pCustomItem->IncreaseUseCountOfNonDestroyItem();
				return true;
			}
		}
	}

#ifdef _DEBUG
	static int nTestCnt = 0;
	if( 0 == nTestCnt++ ) _ASSERT( 0 && "비정상적인 커스텀 아이템 사용.\n" );
#endif

	return false;
}

bool CCMatchObject::IsEquipCustomItem(int nItemId)
{
	CCMatchItem* pItem;
	for (int i=MMCIP_CUSTOM1; i<=MMCIP_CUSTOM2; ++i)
	{
		pItem = GetCharInfo()->m_EquipedItem.GetItem((CCMatchCharItemParts)i);
		if (!pItem)
			continue;
		if (pItem->GetDescID() == nItemId)
			return true;
	}
	return false;
}

