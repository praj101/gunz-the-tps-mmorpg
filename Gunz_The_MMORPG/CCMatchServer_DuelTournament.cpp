//////////////////////////////////////////////////////////////////////////////////////////////
// 2009. 9. 23 - Added By Hong KiJu
// Notice By Hong KiJu - 홍기주에게 문의하세용

#include "stdafx.h"
#include "CCBlobArray.h"

#include "CCAsyncDBJob_DuelTournament.h"

#include "CCMatchConfig.h"
#include "CCMatchServer.h"
#include "CCMatchDuelTournamentMgr.h"

void CCMatchServer::ResponseDuelTournamentJoinChallenge(CCUID &uidPlayer, CCDUELTOURNAMENTTYPE nType)
{
	CCMatchObject *pDTObj = GetPlayerByComCCUID(uidPlayer);
	if(IsEnabledObject(pDTObj) == false) return;

	CCMatchChannel *pChannel = FindChannel(pDTObj->GetChannelUID());
	if( pChannel == NULL ) {
		LOG(LOG_PROG, "CCMatchServer::OnRequestDuelTournamentChallenge - Wrong Channel(ChannelUID - %d%d)",
			pDTObj->GetChannelUID().High, pDTObj->GetChannelUID().Low);
		PostCmdDuelTournamentChallenge(uidPlayer, MERR_DT_WRONG_CHANNEL);
		return;
	}

	if( pDTObj->IsChallengeDuelTournament() == true ) {
		LOG(LOG_PROG, "CCMatchServer::OnRequestDuelTournamentChallenge - Already Join Challenge(PlayerUID - %d%d)",
			pDTObj->GetUID().High, pDTObj->GetUID().Low);
		PostCmdDuelTournamentChallenge(uidPlayer, MERR_DT_ALREADY_JOIN);
		return;
	}

	if( GetDTMgr()->AddPlayer(nType, uidPlayer) == false ) {
		LOG(LOG_PROG, "CCMatchServer::OnRequestDuelTournamentChallenge - Can't Join Challenge the Game(PlayerUID - %d%d)",
			pDTObj->GetUID().High, pDTObj->GetUID().Low);
		PostCmdDuelTournamentChallenge(uidPlayer, MERR_DT_CANNOT_CHALLENGE);
		return;
	}

	pDTObj->SetChallengeDuelTournament(true);
	PostCmdDuelTournamentChallenge(uidPlayer, MOK);
}

void CCMatchServer::ResponseDuelTournamentCancelChallenge(CCUID &uidPlayer, CCDUELTOURNAMENTTYPE nType)
{
	CCMatchObject *pDTObj = GetPlayerByComCCUID(uidPlayer);
	if(IsEnabledObject(pDTObj) == false) return;

	CCMatchChannel *pChannel = FindChannel(pDTObj->GetChannelUID());
	if( pChannel == NULL ) {
		LOG(LOG_PROG, "CCMatchServer::OnRequestDuelTournamentCancelChallenge - Wrong Channel(ChannelUID - %d%d)",
			pDTObj->GetChannelUID().High, pDTObj->GetChannelUID().Low);
		return;
	}

	if( pDTObj->IsChallengeDuelTournament() == false ) {
		LOG(LOG_PROG, "CCMatchServer::OnRequestDuelTournamentChallenge - Not Join Challenge(PlayerUID - %d%d)",
			pDTObj->GetUID().High, pDTObj->GetUID().Low);
		return;
	}

	if( GetDTMgr()->RemovePlayer(nType, uidPlayer) == false ) {
		LOG(LOG_PROG, "CCMatchServer::OnRequestDuelTournamentCancelChallenge - Can't Cancel Challenge the Game(Type - %d, PlayerUID - %d%d)",
			(int)nType, pDTObj->GetUID().High, pDTObj->GetUID().Low);
		return;
	}

	pDTObj->SetChallengeDuelTournament(false);
}

void CCMatchServer::ResponseDuelTournamentCharSideRanking(CCUID &uidPlayer)
{
	CCMatchObject *pDTObj = GetPlayerByComCCUID(uidPlayer);
	if(IsEnabledObject(pDTObj) == false) return;

	CCMatchObjectDuelTournamentCharInfo *pInfo = pDTObj->GetDuelTournamentCharInfo();
	if( pInfo == NULL ) return;

	if( pInfo->GetSideRankingList()->size() == 0 ){
		OnAsyncRequest_GetDuelTournamentSideRankingInfo(pDTObj->GetUID(), pDTObj->GetCharInfo()->m_nCID);
	} else {
		if( GetDTMgr()->IsSameTimeStamp(pDTObj->GetDuelTournamentCharInfo()->GetTimeStamp()) ){
			PostCmdDuelTournamentCharSideRankingInfo(pDTObj->GetUID(), pInfo->GetSideRankingList());
		} else {
			OnAsyncRequest_GetDuelTournamentSideRankingInfo(pDTObj->GetUID(), pDTObj->GetCharInfo()->m_nCID);
		}
	}
}

void CCMatchServer::ResponseDuelTournamentCharStatusInfo(CCUID &uidPlayer, CCCommand *pCommand)
{
	CCMatchObject *pDTObj = GetPlayerByComCCUID(uidPlayer);
	if(IsEnabledObject(pDTObj) == false) return;

	CCMatchStage *pStage = FindStage(pDTObj->GetStageUID());
	if( pStage == NULL ) return;

	pStage->OnCommand(pCommand);
}

void CCMatchServer::SendDuelTournamentPreviousCharInfoToPlayer(CCUID uidPlayer)
{
	CCMatchObject *pDTObj = GetPlayerByComCCUID(uidPlayer);
	if(IsEnabledObject(pDTObj) == false) return;

	OnAsyncRequest_GetDuelTournamentPreviousCharacterInfo(uidPlayer, pDTObj->GetCharInfo()->m_nCID);
}

void CCMatchServer::SendDuelTournamentCharInfoToPlayer(CCUID uidPlayer)
{
	CCMatchObject *pDTObj = GetPlayerByComCCUID(uidPlayer);
	if(IsEnabledObject(pDTObj) == false) return;

	if( pDTObj->GetDuelTournamentCharInfo() == NULL ) {
		if( pDTObj->GetCharInfo() == NULL ) return;
		OnAsyncRequest_GetDuelTournamentCharacterInfo(uidPlayer, pDTObj->GetCharInfo()->m_nCID);
		return;
	} else {
		if( GetDTMgr()->IsSameTimeStamp(pDTObj->GetDuelTournamentCharInfo()->GetTimeStamp()) ){
			PostCmdDuelTournamentCharInfo(uidPlayer, pDTObj->GetDuelTournamentCharInfo());
		} else {
			LOG(LOG_PROG, "CCMatchServer::SendDuelTournamentCharInfoToPlayer - PlayerUID(%d%d) - TimeStamp Changed! %s -> %s",
				pDTObj->GetUID().High, pDTObj->GetUID().Low, pDTObj->GetDuelTournamentCharInfo()->GetTimeStamp(), GetDTMgr()->GetTimeStamp());
			
			OnAsyncRequest_GetDuelTournamentCharacterInfo(uidPlayer, pDTObj->GetCharInfo()->m_nCID);
		}
	}	
}

bool CCMatchServer::DuelTournamentJoin(const CCUID& uidPlayer, const CCUID& uidStage)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return false;

	if (pObj->GetStageUID() != CCUID(0,0))
		StageLeave(pObj->GetUID());//, pObj->GetStageUID());

	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return false;

	pObj->OnStageJoin();

	// Join
	pStage->AddObject(uidPlayer, pObj);
	pObj->SetStageUID(uidStage);
	pObj->SetStageState(MOSS_READY);
	pObj->SetChallengeDuelTournament(false);
	pStage->PlayerState(uidPlayer, MOSS_READY);

	return true;
}

void CCMatchServer::SendDuelTournamentServiceTimeClose(const CCUID& uidPlayer)
{
	if ( MGetServerConfig()->IsEnabledDuelTournament() == false ) return;

	// 듀얼토너먼트 참가신청 취소
	CCMatchObject *pDTObj = GetPlayerByComCCUID(uidPlayer);
	if(IsEnabledObject(pDTObj) == false) return;
	pDTObj->SetChallengeDuelTournament(false);
 
	CCCommand *pCmd = CreateCommand(MC_MATCH_DUELTOURNAMENT_NOT_SERVICE_TIME, uidPlayer);
	pCmd->AddParameter(new CCCommandParameterInt(MGetServerConfig()->GetDuelTournamentServiceStartTime()));
	pCmd->AddParameter(new CCCommandParameterInt(MGetServerConfig()->GetDuelTournamentServiceEndTime()));
	Post(pCmd);
}

void CCMatchServer::LaunchDuelTournamentMatch(CCDUELTOURNAMENTTYPE nType, CCDuelTournamentPickedGroup* pPickedGroup, CCDUELTOURNAMENTMATCHMAKINGFACTOR matchFactor)
{
	if ( MGetServerConfig()->IsEnabledDuelTournament() == false ) return;

	// 선수들이 뛸 Stage 생성!
	CCUID uidStage = CCUID(0,0);
	if (StageAdd(NULL, "DuelTournament_Stage", true, "", &uidStage, true) == false) {
		LOG(LOG_PROG, "CCMatchServer::LaunchDuelTournamentMatch - Can't Add Stage - StageUID(%d%d)", uidStage.High, uidStage.Low);
		RouteCmdDuelTournamentCancelMatch(pPickedGroup, MERR_DT_CANNOT_MAKE_STAGE);
		return;
	}

	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) {
		LOG(LOG_PROG, "CCMatchServer::LaunchDuelTournamentMatch - Can't Find Stage - StageUID(%d%d)", uidStage.High, uidStage.Low);
		RouteCmdDuelTournamentCancelMatch(pPickedGroup, MERR_DT_CANNOT_FIND_STAGE);
		return;
	}

	//////////////////////////////////////////////////////////////////////////////
	// 선수들이 뛸 Stage 설정!

	CCMATCH_GAMETYPE nGameType = CCMATCH_GAMETYPE_DUELTOURNAMENT;
	
	pStage->ChangeRule(nGameType);	
	pStage->SetStageType(MST_NORMAL);
	pStage->SetDuelTournamentMatchList(nType, pPickedGroup);

	int nRandomMapIndex = pStage->GetDuelTournamentRandomMapIndex();
	int nMaxRound = pStage->GetDuelTournamentTotalRound();
	int nLimitTimeForRound = 1;

	CCMatchStageSetting* pSetting = pStage->GetStageSetting();
	pSetting->SetMasterUID(CCUID(0,0));
	pSetting->SetMapIndex(nRandomMapIndex);
	pSetting->SetGameType(nGameType);
	pSetting->SetLimitTime(nLimitTimeForRound);	
	pSetting->SetRoundMax(nMaxRound);

#ifdef _DUELTOURNAMENT_LOG_ENABLE_	
	LOG(LOG_PROG, "CCMatchServer::LaunchDuelTournamentMatch - MapName=%s(id=%d), MaxRound=%d"
		, MGetMapDescMgr()->GetMapName(nRandomMapIndex), nRandomMapIndex, nMaxRound);
#endif

	// 디비에 로그를 남긴다.
	if ( (MGetMapDescMgr()->MIsCorrectMap(nRandomMapIndex)) && (MGetGameTypeMgr()->IsCorrectGameType(CCMATCH_GAMETYPE_DUELTOURNAMENT)) ) {

		// 선수들 입장!
		for (CCDuelTournamentPickedGroup::iterator i=pPickedGroup->begin(); i!= pPickedGroup->end(); i++)
		{
			CCUID uidPlayer = (*i);
			if( DuelTournamentJoin(uidPlayer, uidStage) == false ){
				LOG(LOG_PROG, "CCMatchServer::LaunchDuelTournamentMatch - Can't Join DT Stage - PlayerUID(%d%d)", uidPlayer.High, uidPlayer.Low);

				// 이거 다 취소시켜야되나? -_ㅠ 고민해보자...
				// Added By 홍기주
				return;
			}
		}

		RouteCmdDuelTournamentPrepareMatch(nType, uidStage, pPickedGroup);
		RouteCmdDuelTournamentStageSetting(uidStage);

		if (pStage->StartGame(MGetServerConfig()->IsUseResourceCRC32CacheCheck()) == true) {		// 게임시작
			ReserveAgent(pStage);

			CCMatchObjectCacheBuilder CacheBuilder;
			CacheBuilder.Reset();

			for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {
				CCUID uidObj = (CCUID)(*i).first;
				CCMatchObject* pScanObj = (CCMatchObject*)GetObject(uidObj);
				if (pScanObj) CacheBuilder.AddObject(pScanObj);
			}

			CCCommand* pCmdCacheAdd = CacheBuilder.GetResultCmd(MATCHCACHEMODE_UPDATE, this);
			RouteToStage(pStage->GetUID(), pCmdCacheAdd);

			int nOutNumber;
			char szOutTimeStamp[DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1] = {0, };

			if( OnSyncRequest_InsertDuelTournamentGameLog(nType, matchFactor, pPickedGroup, &nOutNumber, szOutTimeStamp) ){				
				pStage->SetDuelTournamentMatchNumber(nOutNumber);
				pStage->SetDuelTournamentMatchTimeStamp(szOutTimeStamp);

				RouteCmdDuelTournamentLaunchMatch(uidStage);

				SaveGameLog(uidStage);
			} else{
				// DB 적재 실패할 경우.. 어떻게 해야되는겁니까? -_ㅠ
				// NHN에서는 가끔 DB접속이 끊기기도 하던데... 가끔 DB 쿼리 실패도 있던데... 
				//
				// 이거 다 취소시켜야되나? -_ㅠ 고민해보자...
				// Added By 홍기주
				//RouteCmdDuelTournamentCancelMatch(pPickedGroup, MERR_DT_CANNOT_ACCESS_DB);
				LOG(LOG_PROG, "CCMatchServer::LaunchDuelTournamentMatch - Insert Data to Database Fail- Can't Start Stage");
				return;
			}
		} else {
			LOG(LOG_PROG, "CCMatchServer::LaunchDuelTournamentMatch - Can't Start Stage");
			RouteCmdDuelTournamentCancelMatch(pPickedGroup, MERR_DT_CANNOT_START_STAGE);			
		}
	}
	else{
		_ASSERT(0);//channelrule.xml 에 듀얼토너먼트용 맵을 추가
		LOG(LOG_PROG, "CCMatchServer::LaunchDuelTournamentMatch - Wrong Stage Setting");
		RouteCmdDuelTournamentCancelMatch(pPickedGroup, MERR_DT_CANNOT_WRONG_STAGE_SETTING);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sync Request
bool CCMatchServer::OnSyncRequest_InsertDuelTournamentGameLog(CCDUELTOURNAMENTTYPE nDTType, int nMatchFactor, CCDuelTournamentPickedGroup *pPickedGroup, int *nOutNumber, char *szOutTimeStamp)
{
	int nPlayerCID[8] = {0, };

	int nIndex = 0;
	for (CCDuelTournamentPickedGroup::iterator i=pPickedGroup->begin(); i!= pPickedGroup->end(); i++)
	{
		CCUID uidPlayer = (*i);
		CCMatchObject* pObj = GetObject(uidPlayer);
		if (pObj == NULL) return false;

		nPlayerCID[nIndex++] = pObj->GetCharInfo()->m_nCID;
	}

	return m_MatchDBMgr.InsertDuelTournamentGameLog(nDTType, nMatchFactor, nPlayerCID[0], nPlayerCID[1], nPlayerCID[2], nPlayerCID[3], 
		nPlayerCID[4], nPlayerCID[5], nPlayerCID[6], nPlayerCID[7], nOutNumber, szOutTimeStamp);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Async Request
bool CCMatchServer::OnAsyncRequest_GetDuelTournamentTimeStamp()
{
	CCAsyncDBJob_GetDuelTournamentTimeStamp *pAsyncDbJob = new CCAsyncDBJob_GetDuelTournamentTimeStamp;
	if( 0 == pAsyncDbJob ) return false;
	CCMatchServer::GetInstance()->PostAsyncJob( pAsyncDbJob );
	return true;
}

bool CCMatchServer::OnAsyncRequest_GetDuelTournamentCharacterInfo(CCUID uidPlayer, DWORD dwCID)
{
	CCAsyncDBJob_GetDuelTournamentCharInfo *pAsyncDbJob = new CCAsyncDBJob_GetDuelTournamentCharInfo;
	if( 0 == pAsyncDbJob ) return false;

	pAsyncDbJob->Input(uidPlayer, dwCID);
	CCMatchServer::GetInstance()->PostAsyncJob( pAsyncDbJob );
	return true;
}

bool CCMatchServer::OnAsyncRequest_GetDuelTournamentPreviousCharacterInfo(CCUID uidPlayer, DWORD dwCID)
{
	CCAsyncDBJob_GetDuelTournamentPreviousCharInfo *pAsyncDbJob = new CCAsyncDBJob_GetDuelTournamentPreviousCharInfo;
	if( 0 == pAsyncDbJob ) return false;

	pAsyncDbJob->Input(uidPlayer, dwCID);
	CCMatchServer::GetInstance()->PostAsyncJob( pAsyncDbJob );
	return true;
}

bool CCMatchServer::OnAsyncRequest_UpdateDuelTournamentCharacterInfo(CCUID uidPlayer, char *szTimeStamp)
{
	CCMatchObject *pObj = GetObject(uidPlayer);
	if( pObj == NULL ) return false;

	CCAsyncDBJob_UpdateDuelTournamentCharInfo *pAsyncDBJob = new CCAsyncDBJob_UpdateDuelTournamentCharInfo;
	if( 0 == pAsyncDBJob ) return false;

	pAsyncDBJob->Input(pObj->GetCharInfo()->m_nCID, szTimeStamp, pObj->GetDuelTournamentCharInfo());
	CCMatchServer::GetInstance()->PostAsyncJob( pAsyncDBJob );
	return true;
}

bool CCMatchServer::OnAsyncRequest_GetDuelTournamentSideRankingInfo(CCUID uidPlayer, DWORD dwCID)
{
	CCAsyncDBJob_GetDuelTournamentSideRankingInfo *pAsyncDbJob = new CCAsyncDBJob_GetDuelTournamentSideRankingInfo;
	if( 0 == pAsyncDbJob ) return false;

	pAsyncDbJob->Input(uidPlayer, dwCID);
	CCMatchServer::GetInstance()->PostAsyncJob( pAsyncDbJob );
	return true;
}

bool CCMatchServer::OnAsyncRequest_GetDuelTournamentGroupRankingInfo()
{
	CCAsyncDBJob_GetDuelTournamentGroupRankingInfo *pAsyncDbJob = new CCAsyncDBJob_GetDuelTournamentGroupRankingInfo;
	if( 0 == pAsyncDbJob ) return false;

	pAsyncDbJob->Input();
	CCMatchServer::GetInstance()->PostAsyncJob( pAsyncDbJob );
	return true;
}

bool CCMatchServer::OnAsyncRequest_UpdateDuelTournamentGameLog(char* szTimeStamp, int nGameNumber, CCUID uidChampion)
{
	CCAsyncDBJob_UpdateDuelTournamentGameLog *pAsyncDbJob = new CCAsyncDBJob_UpdateDuelTournamentGameLog;
	if( 0 == pAsyncDbJob ) return false;

	CCMatchObject *pObj = GetObject(uidChampion);
	if( pObj != NULL )	{ pAsyncDbJob->Input(szTimeStamp, nGameNumber, pObj->GetCharInfo()->m_nCID);}
	else				{ pAsyncDbJob->Input(szTimeStamp, nGameNumber, -1); }

	CCMatchServer::GetInstance()->PostAsyncJob( pAsyncDbJob );
	return true;
}

bool CCMatchServer::OnAsyncRequest_InsertDuelTournamentGameLogDetail(int nLogID, char* szTimeStamp, CCDUELTOURNAMENTROUNDSTATE nDTRoundState, int nWinnerCID, int nLoserCID, int nGainTP, int nLoseTp, int nPlayTime) 
{
	CCAsyncDBJob_InsertDuelTournamentGameLogDetail *pAsyncDbJob = new CCAsyncDBJob_InsertDuelTournamentGameLogDetail;
	if( 0 == pAsyncDbJob ) return false;

	pAsyncDbJob->Input(nLogID, szTimeStamp, nDTRoundState, nWinnerCID, nLoserCID, nGainTP, nLoseTp, nPlayTime);
	CCMatchServer::GetInstance()->PostAsyncJob( pAsyncDbJob );

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Async Response
void CCMatchServer::OnAsyncResponse_GetDuelTournamentTimeStamp(CCAsyncJob *pJobResult)
{
	CCAsyncDBJob_GetDuelTournamentTimeStamp* pJob = (CCAsyncDBJob_GetDuelTournamentTimeStamp*)pJobResult;

	if( CCASYNC_RESULT_SUCCEED != pJob->GetResult() ) {
		cclog("CCMatchServer::OnAsyncResponse_GetDuelTournamentTimeStamp - 실패\n");
		return;
	}

	if( GetDTMgr()->IsSameTimeStamp(pJob->GetTimeStamp()) == false ) {
		GetDTMgr()->SetTimeStamp(pJob->GetTimeStamp());
		GetDTMgr()->SetTimeStampChanged(true);

		LOG(LOG_PROG, "Time Stamp Changed! - %s", pJob->GetTimeStamp());
	}	
}

void CCMatchServer::OnAsyncResponse_GetDuelTournamentCharacterInfo(CCAsyncJob *pJobResult)
{
	CCAsyncDBJob_GetDuelTournamentCharInfo* pJob = (CCAsyncDBJob_GetDuelTournamentCharInfo*)pJobResult;

	if( CCASYNC_RESULT_SUCCEED != pJob->GetResult() ) {
		cclog("CCMatchServer::OnAsyncResponse_GetDuelTournamentCharacterInfo - 실패\n");
		return;
	}

	CCMatchObject* pObj = GetObject(pJob->GetPlayerUID());
	if( pObj == NULL ) return;
	if( pJob->GetDTCharInfo()->IsSettingData() == false ) {
		cclog("CCMatchServer::OnAsyncResponse_GetDuelTournamentCharacterInfo - 데이터가 이상합니다.\n");
		return;
	} else if( pJob->GetDTCharInfo()->GetTP() < 0 ) {
		cclog("CCMatchServer::OnAsyncResponse_GetDuelTournamentCharacterInfo - 데이터가 이상합니다(2)\n");
		return;
	}
	pObj->SetDuelTournamentCharInfo( new CCMatchObjectDuelTournamentCharInfo( pJob->GetDTCharInfo() ) );
	PostCmdDuelTournamentCharInfo(pObj->GetUID(), pObj->GetDuelTournamentCharInfo());
}

void CCMatchServer::OnAsyncResponse_GetDuelTournamentPreviousCharacterInfo(CCAsyncJob *pJobResult)
{
	CCAsyncDBJob_GetDuelTournamentPreviousCharInfo* pJob = (CCAsyncDBJob_GetDuelTournamentPreviousCharInfo*)pJobResult;

	if( CCASYNC_RESULT_SUCCEED != pJob->GetResult() ) {
		cclog("CCMatchServer::OnAsyncResponse_GetDuelTournamentPreviousCharacterInfo - 실패\n");
		return;
	}

	PostCmdDuelTournamentCharInfoPrevious(pJob->GetPlayerUID(), pJob->GetPrevTP(), pJob->GetPrevWins(), pJob->GetPrevLoses(), pJob->GetPrevRanking(), pJob->GetPrevFinalWins());
}

void CCMatchServer::OnAsyncResponse_GetDuelTournamentSideRanking(CCAsyncJob *pJobResult)
{
	CCAsyncDBJob_GetDuelTournamentSideRankingInfo* pJob = (CCAsyncDBJob_GetDuelTournamentSideRankingInfo*)pJobResult;

	if( CCASYNC_RESULT_SUCCEED != pJob->GetResult() ) {
		cclog("CCMatchServer::OnAsyncResponse_GetDuelTournamentSideRanking - 실패\n");
		return;
	}

	CCMatchObject* pObj = GetObject(pJob->GetPlayerUID());
	if( pObj == NULL ) return;


	CCMatchObjectDuelTournamentCharInfo *pCharInfo = pObj->GetDuelTournamentCharInfo();
	if( pCharInfo == NULL ) return;

	pCharInfo->RemoveSideRankingAll();
	list<DTRankingInfo*>::iterator iter = pJob->GetSideRankingList()->begin();
	for(; iter != pJob->GetSideRankingList()->end(); ++iter) {
		pCharInfo->AddSideRankingInfo((*iter));
	}

	PostCmdDuelTournamentCharSideRankingInfo(pObj->GetUID(), pCharInfo->GetSideRankingList());
}

void CCMatchServer::OnAsyncResponse_GetDuelTournamentGroupRanking(CCAsyncJob *pJobResult)
{
	CCAsyncDBJob_GetDuelTournamentGroupRankingInfo* pJob = (CCAsyncDBJob_GetDuelTournamentGroupRankingInfo*)pJobResult;

	if( CCASYNC_RESULT_SUCCEED != pJob->GetResult() ) {
		cclog("CCMatchServer::OnAsyncResponse_GetDuelTournamentGroupRanking - 실패\n");
		return;
	}

	GetDTMgr()->AddGroupRanking(pJob->GetGroupRankingList());
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Post Command To Client(or Route to Stage)
void CCMatchServer::PostCmdDuelTournamentChallenge(CCUID uidPlayer, int nResult)
{
	CCCommand *pCmd = CreateCommand(MC_MATCH_DUELTOURNAMENT_RESPONSE_JOINGAME, uidPlayer);
	pCmd->AddParameter(new CCCommandParameterInt(nResult));
	Post(pCmd);
}

void CCMatchServer::PostCmdDuelTournamentCharInfo(CCUID uidPlayer, CCMatchObjectDuelTournamentCharInfo *pDTCharInfo)
{
	if( pDTCharInfo == NULL ) return;

	CCCommand *pCmd = CreateCommand(MC_MATCH_DUELTOURNAMENT_CHAR_INFO, uidPlayer);
	pCmd->AddParameter(new CCCommandParameterInt(pDTCharInfo->GetTP()));
	pCmd->AddParameter(new CCCommandParameterInt(pDTCharInfo->GetWins()));
	pCmd->AddParameter(new CCCommandParameterInt(pDTCharInfo->GetLoses()));
	pCmd->AddParameter(new CCCommandParameterInt(pDTCharInfo->GetRanking()));
	pCmd->AddParameter(new CCCommandParameterInt(pDTCharInfo->GetRankingIncrease()));
	pCmd->AddParameter(new CCCommandParameterInt(pDTCharInfo->GetFinalWins()));
	pCmd->AddParameter(new CCCommandParameterInt(pDTCharInfo->GetLastWeekGrade()));

	Post(pCmd);
}

void CCMatchServer::PostCmdDuelTournamentCharInfoPrevious(CCUID uidPlayer, int nPrevTP, int nPrevWins, int nPrevLoses, int nPrevRanking, int nPrevFinalWins)
{
	CCCommand *pCmd = CreateCommand(MC_MATCH_DUELTOURNAMENT_CHAR_INFO_PREVIOUS, uidPlayer);

	pCmd->AddParameter(new CCCommandParameterInt(nPrevTP));
	pCmd->AddParameter(new CCCommandParameterInt(nPrevWins));
	pCmd->AddParameter(new CCCommandParameterInt(nPrevLoses));
	pCmd->AddParameter(new CCCommandParameterInt(nPrevRanking));
	pCmd->AddParameter(new CCCommandParameterInt(nPrevFinalWins));

	Post(pCmd);
}

void CCMatchServer::PostCmdDuelTournamentCharSideRankingInfo(CCUID uidPlayer, list<DTRankingInfo*>* pSideRankingList)
{
	CCMatchObject *pDTObj = GetPlayerByComCCUID(uidPlayer);
	if(IsEnabledObject(pDTObj) == false) return;

	void* pBlobRanking = CCMakeBlobArray(sizeof(DTRankingInfo), (int)pSideRankingList->size() );

	int index = 0;
	list<DTRankingInfo*>::iterator iter;
	for(iter = pSideRankingList->begin(); iter != pSideRankingList->end(); ++iter){
		DTRankingInfo *pInfoDest = reinterpret_cast<DTRankingInfo *>(CCGetBlobArrayElement(pBlobRanking, index++));	
		DTRankingInfo *pInfoSrc = (*iter);

		if( pInfoDest == NULL || pInfoSrc == NULL ) { CCEraseBlobArray( pBlobRanking ); return; }
		memcpy(pInfoDest, pInfoSrc, sizeof(DTRankingInfo));
	}

	CCCommand* pCmd = CreateCommand(MC_MATCH_DUELTOURNAMENT_RESPONSE_SIDERANKING_INFO, uidPlayer);
	if( NULL == pCmd ) {
		CCEraseBlobArray( pBlobRanking );
		return;
	}
	pCmd->AddParameter( new CCCommandParameterBlob(pBlobRanking, CCGetBlobArraySize(pBlobRanking)) );
	CCEraseBlobArray( pBlobRanking );

	Post(pCmd);
}

void CCMatchServer::PostCmdDuelTournamentCancelMatch(CCUID uidPlayer, int nErrorCode)
{
	CCCommand *pCmd = CreateCommand(MC_MATCH_DUELTOURNAMENT_CANCEL_MATCH, uidPlayer);
	pCmd->AddParameter(new CCCommandParameterInt(nErrorCode));
	Post(pCmd);
}

void CCMatchServer::RouteCmdDuelTournamentPrepareMatch(CCDUELTOURNAMENTTYPE nType, CCUID uidStage, CCDuelTournamentPickedGroup *pPickedGroup)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	void* pBlobPlayerInfo = CCMakeBlobArray(sizeof(DTPlayerInfo), (int)pPickedGroup->size() );

	int index = 0;
	CCDuelTournamentPickedGroup::iterator iter;
	for(iter = pPickedGroup->begin(); iter != pPickedGroup->end(); ++iter){
		DTPlayerInfo *pPlayerInfo = reinterpret_cast<DTPlayerInfo*>(CCGetBlobArrayElement(pBlobPlayerInfo, index++));
		CCMatchObject* pObj = GetObject(*iter);
		if (pObj == NULL){
			CCEraseBlobArray(pBlobPlayerInfo);
			return;
		}

        pPlayerInfo->m_nTP = pObj->GetDuelTournamentCharInfo()->GetTP();
		pPlayerInfo->uidPlayer = pObj->GetUID();
		strcpy(pPlayerInfo->m_szCharName, pObj->GetCharInfo()->m_szName);
	}

	CCCommand* pCmd = CreateCommand(MC_MATCH_DUELTOURNAMENT_PREPARE_MATCH, CCUID(0, 0));
	if( pCmd == NULL ) {
		CCEraseBlobArray(pBlobPlayerInfo);
		return;
	}

	pCmd->AddParameter(new CCCmdParaCCUID(uidStage));
	pCmd->AddParameter(new CCCmdParamInt(nType));
	pCmd->AddParameter(new CCCommandParameterBlob(pBlobPlayerInfo, CCGetBlobArraySize(pBlobPlayerInfo)));
	CCEraseBlobArray( pBlobPlayerInfo );

	RouteToStage(uidStage, pCmd);
}

void CCMatchServer::RouteCmdDuelTournamentStageSetting(CCUID uidStage)
{
	CCCommand* pCmd = CreateCmdResponseStageSetting(uidStage);
	RouteToStage(uidStage, pCmd);
}

void CCMatchServer::RouteCmdDuelTournamentLaunchMatch(CCUID uidStage)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	CCCommand* pCmd = CreateCommand(MC_MATCH_DUELTOURNAMENT_LAUNCH_MATCH, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParaCCUID(uidStage));
	pCmd->AddParameter(new CCCmdParamStr( const_cast<char*>(pStage->GetMapName()) ));
	RouteToStage(uidStage, pCmd);
}

void CCMatchServer::RouteCmdDuelTournamentCancelMatch(CCDuelTournamentPickedGroup *pPickedGroup, int nErrorCode)
{
	for (CCDuelTournamentPickedGroup::iterator i=pPickedGroup->begin(); i!= pPickedGroup->end(); i++)
	{
		CCUID uidPlayer = (*i);
		PostCmdDuelTournamentCancelMatch(uidPlayer, nErrorCode);
	}
}

void CCMatchServer::RouteCmdDuelTournamentMTDGameInfo(const CCUID& uidStage, CCTD_DuelTournamentGameInfo& GameInfo)
{
	CCCommand* pCmd = CreateCommand(MC_MATCH_DUELTOURNAMENT_GAME_INFO, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamBlob(&GameInfo, sizeof(CCTD_DuelTournamentGameInfo)));
	RouteToBattle(uidStage, pCmd);
}

void CCMatchServer::RouteCmdDuelTournamentMTDNextGamePlayerInfo(const CCUID& uidStage, CCTD_DuelTournamentNextMatchPlayerInfo& PlayerInfo)
{
	CCCommand* pCmd = CreateCommand(MC_MATCH_DUELTOURNAMENT_GAME_NEXT_MATCH_PLYAERINFO, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamBlob(&PlayerInfo, sizeof(CCTD_DuelTournamentNextMatchPlayerInfo)));
	RouteToBattle(uidStage, pCmd);
}


void CCMatchServer::RouteCmdDuelTournamentMTDRoundResultInfo(const CCUID& uidStage, CCTD_DuelTournamentRoundResultInfo* RoundResultInfo)
{
	CCCommand* pCmd = CreateCommand(MC_MATCH_DUELTOURNAMENT_GAME_ROUND_RESULT_INFO, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamBlob(RoundResultInfo, sizeof(CCTD_DuelTournamentRoundResultInfo)));
	RouteToBattle(uidStage, pCmd);
}

void CCMatchServer::RouteCmdDuelTournamentMTDMatchResultInfo(const CCUID& uidStage, CCTD_DuelTournamentMatchResultInfo* MatchResultInfo)
{
	CCCommand* pCmd = CreateCommand(MC_MATCH_DUELTOURNAMENT_GAME_MATCH_RESULT_INFO, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamBlob(MatchResultInfo, sizeof(CCTD_DuelTournamentMatchResultInfo)));
	RouteToBattle(uidStage, pCmd);
}

