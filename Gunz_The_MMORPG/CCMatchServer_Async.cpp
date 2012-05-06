#include "stdafx.h"
#include "CCMatchServer.h"
#include "CCAsyncDBJob.h"
#include "CCAsyncDBJob_FriendList.h"
#include "CCAsyncDBJob_GetLoginInfo.h"
#include "CCAsyncDBJob_InsertConnLog.h"
#include "CCBlobArray.h"
#include "CCMatchFormula.h"
#include "CCAsyncDBJob_Event.h"
#include "CCAsyncDBJob_GetAccountItemList.h"
#include "CCAsyncDBJob_BuyQuestItem.h"
#include "CCAsyncDBJob_SurvivalMode.h"				// 2009. 6. 3 - Added By Hong KiJu

void CCMatchServer::PostAsyncJob(CCAsyncJob* pJob )
{
	m_AsyncProxy.PostJob(pJob);
}

void CCMatchServer::ProcessAsyncJob()
{
	while(CCAsyncJob* pJob = m_AsyncProxy.GetJobResult()) 
	{
		//////////////////////////////////////////////////////////////////////
		{
			CCMatchObject* pPlayer = GetObject( pJob->GetOwnerUID() );
			if( NULL != pPlayer )
			{
				pPlayer->m_DBJobQ.bIsRunningAsyncJob = false;
			}
		}
		//////////////////////////////////////////////////////////////////////

		switch(pJob->GetJobID()) {
		case CCASYNCJOB_GETACCOUNTCHARLIST:
			{
				OnAsyncGetAccountCharList(pJob);
			}
			break;				
		//case CCASYNCJOB_GETACCOUNTCHARINFO:
		//	{
		//		OnAsyncGetAccountCharInfo(pJob);
		//	}
		//	break;				
		//case CCASYNCJOB_GETCHARINFO:
		//	{
		//		OnAsyncGetCharInfo(pJob);
		//	}
		//	break;
		case CCASYNCJOB_FRIENDLIST:
			{
				OnAsyncGetFriendList(pJob);
			}
			break;			
		case CCASYNCJOB_CREATECHAR:
			{
				OnAsyncCreateChar(pJob);
			}
			break;
		case CCASYNCJOB_GETLOGININFO:
			{
				OnAsyncGetLoginInfo(pJob);
			}
			break;
		case CCASYNCJOB_DELETECHAR:
			{
				OnAsyncDeleteChar(pJob);
			}
			break;
		case CCASYNCJOB_WINTHECLANGAME:
			{
				OnAsyncWinTheClanGame(pJob);
			}
			break;
		case CCASYNCJOB_UPDATECHARINFODATA:
			{
				OnAsyncUpdateCharInfoData(pJob);
			}
			break;
		case CCASYNCJOB_CHARFINALIZE:
			{
				OnAsyncCharFinalize(pJob);
			}
			break;
		//case CCASYNCJOB_BRINGACCOUNTITEM:
		//	{
		//		OnAsyncBringAccountItem(pJob);
		//	}
		//	break;
		case CCASYNCJOB_INSERTCONNLOG:
			{
				OnAsyncInsertConnLog(pJob);
			}
			break;
		case CCASYNCJOB_INSERTGAMELOG:
			{
				OnAsyncInsertGameLog(pJob);
			}
			break;
		case CCASYNCJOB_INSERTGAMEPLAYERLOG:
			{

			}
			break;
		case CCASYNCJOB_CREATECLAN:
			{
				OnAsyncCreateClan(pJob);
			}
			break;
		case CCASYNCJOB_EXPELCLANMEMBER:
			{
				OnAsyncExpelClanMember(pJob);
			}
			break;
			
		case CCASYNCJOB_INSERTQUESTGAMELOG :
			{

			}
			break;
		case CCASYNCJOB_UPDATEQUESTITEMINFO :
			{
			}
			break;
			

		case CCASYNCJOB_PROBABILITYEVENTPERTIME :
			{
				OnAsyncInsertEvent( pJob );
			}
			break;

		case CCASYNCJOB_UPDATEIPTOCOUNTRYLIST :
			{
				OnAsyncUpdateIPtoCoutryList( pJob );
			};
			break;

		case CCASYNCJOB_UPDATEBLOCKCOUNTRYCODELIST :
			{
				OnAsyncUpdateBlockCountryCodeList( pJob );
			}
			break;

		case CCASYNCJOB_UPDATECUSTOMIPLIST :
			{
				OnAsyncUpdateCustomIPList( pJob );
			}
			break;

		case CCASYNCJOB_GETACCOUNTITEMLIST :
			{
				OnAsyncGetAccountItemList( pJob );
			}
			break;

		case CCASYNCJOB_BUYQUESTITEM :
			{
				OnAsyncBuyQuestItem( pJob );
			}
			break;


		//////////////////////////////////////////////////////////////////////////////////////////////
		// 2009. 6. 3 - Added By Hong KiJu
		case CCASYNCJOB_INSERT_SURVIVALMODE_GAME_LOG :
			{
				OnAsyncSurvivalModeGameLog(pJob);				
			}
			break;
		case CCASYNCJOB_GET_SURVIVALMODE_GROUP_RANKING :
			{
				OnAsyncSurvivalModeGroupRanking(pJob);
			}
			break;

		case CCASYNCJOB_GET_SURVIVALMODE_PRIVATE_RANKING :
			{
				OnAsyncSurvivalModePrivateRanking(pJob);
			}
			break;

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Added By Hong KiJu(2009-09-25)
		case CCASYNCJOB_GET_DUELTOURNAMENT_CHARINFO :
			{
				OnAsyncResponse_GetDuelTournamentCharacterInfo(pJob);
			}
			break;

		case CCASYNCJOB_GET_DUELTOURNAMENT_PREVIOUS_CHARINFO :
			{
				OnAsyncResponse_GetDuelTournamentPreviousCharacterInfo(pJob);

			}
			break;

		case CCASYNCJOB_GET_DUELTOURNAMENT_SIDERANKING :
			{
				OnAsyncResponse_GetDuelTournamentSideRanking(pJob);
			}
			break;
		
		case CCASYNCJOB_GET_DUELTOURNAMENT_GROUPRANKING :
			{
				OnAsyncResponse_GetDuelTournamentGroupRanking(pJob);
			}
			break;

		case CCASYNCJOB_GET_DUELTOURNAMENT_TIMESTAMP :
			{
				OnAsyncResponse_GetDuelTournamentTimeStamp(pJob);
			}
			break;

		case CCASYNCJOB_UPDATE_DUELTOURNAMENT_CHARINFO :
		case CCASYNCJOB_UPDATE_DUELTOURNAMENT_GAMELOG :
		case CCASYNCJOB_INSERT_DUELTOURNAMENT_GAMELOGDETAIL :
			{

			}
			break;

		case CCASYNCJOB_UPDATE_CHARITEM_COUNT :
			{

			}
			break;

		case CCASYNCJOB_GET_BR_DESCRIPTION:
			{
				OnAsyncResponse_GetBR_Description(pJob);
			}
			break;

		case CCASYNCJOB_GET_CHAR_BR_INFO:
			{
				OnAsyncResponse_GetCharBRInfo(pJob);
			}
			break;

		case CCASYNCJOB_UPDATE_CHAR_BR_INFO:
			{
				OnAsyncResponse_UpdateCharBRInfo(pJob);
			}
			break;

		case CCASYNCJOB_REWARD_CHAR_BR:
			{
				OnAsyncResponse_RewardCharBR(pJob);
			}
			break;

		default :
			{
				OnProcessAsyncJob( pJob );
			}
			break;
		};

		delete pJob;
	}
}



void CCMatchServer::OnAsyncGetLoginInfo(CCAsyncJob* pJobInput)
{
	CCAsyncDBJob_GetLoginInfo* pJob = (CCAsyncDBJob_GetLoginInfo*)pJobInput;


	if (pJob->GetResult() != CCASYNC_RESULT_SUCCEED) 
	{		
		// Notify Message 필요 -> 로그인 관련 - 해결(Login Fail 메세지 이용)
		// Disconnect(pJob->GetComCCUID());
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(pJob->GetComCCUID(), MERR_FAILED_GETACCOUNTINFO);
		Post(pCmd);

		pJob->DeleteMemory();
		return;
	}

	CCMatchAccountInfo* pAccountInfo = pJob->GetAccountInfo();
	if( pAccountInfo == 0 ) return;

	CCMatchAccountPenaltyInfo* pAccountPenaltyInfo = pJob->GetAccountPenaltyInfo();
	if( pAccountPenaltyInfo == 0 ) return;


#ifndef _DEBUG
	// 중복 로그인이면 이전에 있던 사람을 끊어버린다.
	CCMatchObject* pCopyObj = GetPlayerByAID(pAccountInfo->m_nAID);
	if (pCopyObj != NULL) 
	{
		// Notify Message 필요 -> 로그인 관련 - 해결(특별한 메세지 필요 없음)
		// 중복 접속에 관한 것은 이전 접속자의 접속을 해지하는 것이므로,
		// 특별한 오류 패킷을 만들지 않는다.
		Disconnect(pCopyObj->GetUID());
	}
#endif

	// 사용정지 계정인지 확인한다.
	if ((pAccountInfo->m_nUGrade == CCMUGBLOCKED) || (pAccountInfo->m_nUGrade == CCMUGPENALTY))
	{
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(pJob->GetComCCUID(), MERR_CLIENT_CCMUGBLOCKED);
		Post(pCmd);

		pJob->DeleteMemory();
		return;
	}

	AddObjectOnMatchLogin(pJob->GetComCCUID(), pJob->GetAccountInfo(), pJob->GetAccountPenaltyInfo(), 
		pJob->IsFreeLoginIP(), pJob->GetCountryCode3(), pJob->GetChecksumPack());

/*
	// 할당...
	CCUID AllocUID = ComCCUID;
	int nErrCode = ObjectAdd(ComCCUID);
	if(nErrCode!=MOK) {
		LOG(LOG_DEBUG, MErrStr(nErrCode) );
	}

	CCMatchObject* pObj = GetObject(AllocUID);
	if (pObj == NULL)
	{
		Disconnect(ComCCUID);
		delete pJob->GetAccountInfo();
		return;
	}

	pObj->AddCommListener(ComCCUID);
	pObj->SetObjectType(MOT_PC);
	memcpy(pObj->GetAccountInfo(), pAccountInfo, sizeof(CCMatchAccountInfo));
	pObj->SetFreeLoginIP(pJob->IsFreeLoginIP());
	pObj->SetCountryCode3( pJob->GetCountryCode3() );


	CCCommObject* pCommObj = (CCCommObject*)m_CommRefCache.GetRef(ComCCUID);
	if (pCommObj != NULL)
	{
		pObj->SetPeerAddr(pCommObj->GetIP(), pCommObj->GetIPString(), pCommObj->GetPort());
	}
	
	SetClientClockSynchronize(ComCCUID);

	CCCommand* pCmd = CreateCmdMatchResponseLoginOK(ComCCUID, AllocUID, pAccountInfo->m_szUserID, pAccountInfo->m_nUGrade, pAccountInfo->m_nPGrade);
	Post(pCmd);	


	// 접속 로그
	CCAsyncDBJob_InsertConnLog* pNewJob = new CCAsyncDBJob_InsertConnLog();
	pNewJob->Input(pObj->GetAccountInfo()->m_nAID, pObj->GetIPString(), pObj->GetCountryCode3() );
	PostAsyncJob(pNewJob);

#ifndef _DEBUG
	// Client DataFile Checksum을 검사한다.
	unsigned long nChecksum = pJob->GetChecksumPack() ^ ComCCUID.High ^ ComCCUID.Low;
	if (nChecksum != GetItemFileChecksum()) {
		LOG(LOG_PROG, "Invalid ZItemChecksum(%u) , UserID(%s) ", nChecksum, pObj->GetAccountInfo()->m_szUserID);
		Disconnect(ComCCUID);
	}
#endif

	delete pJob->GetAccountInfo();
*/

}

void CCMatchServer::OnAsyncGetAccountCharList(CCAsyncJob* pJobResult)
{
	CCAsyncDBJob_GetAccountCharList* pJob = (CCAsyncDBJob_GetAccountCharList*)pJobResult;

	if (pJob->GetResult() != CCASYNC_RESULT_SUCCEED) {
		char szTime[128]="";
		_strtime(szTime);

		cclog("[%s] Async DB Query(ResponseAccountCharList) Failed\n", szTime);
		return;
	}		

	CCMatchObject* pObj = GetObject(pJob->GetUID());
	if (pObj == NULL) 
		return;

	const int					nCharCount		= pJob->GetCharCount();
	const CCTD_AccountCharInfo * pCharList		= pJob->GetCharList();
	CCTD_AccountCharInfo*		pTransCharInfo	= NULL;
	int							nCharMaxLevel	= 0;

	CCCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_ACCOUNT_CHARLIST, CCUID(0,0));
	void* pCharArray = CCMakeBlobArray(sizeof(CCTD_AccountCharInfo), nCharCount);

	for (int i = 0; i < nCharCount; i++)
	{
		pTransCharInfo = (CCTD_AccountCharInfo*)CCGetBlobArrayElement(pCharArray, i);
		memcpy(pTransCharInfo, &pCharList[i], sizeof(CCTD_AccountCharInfo));

		nCharMaxLevel = max(nCharMaxLevel, pTransCharInfo->nLevel);
	}

	pObj->CheckNewbie( nCharMaxLevel );

	pNewCmd->AddParameter(new CCCommandParameterBlob(pCharArray, CCGetBlobArraySize(pCharArray)));
	CCEraseBlobArray(pCharArray);
    
	RouteToListener( pObj, pNewCmd );
}

//void CCMatchServer::OnAsyncGetAccountCharInfo(CCAsyncJob* pJobResult)
//{
//	CCAsyncDBJob_GetAccountCharInfo* pJob = (CCAsyncDBJob_GetAccountCharInfo*)pJobResult;
//
//	if (pJob->GetResult() != CCASYNC_RESULT_SUCCEED) {
//		char szTime[128]="";
//		_strtime(szTime);
//
//		cclog("[%s] Async DB Query(ResponseAccountCharInfo) Failed\n", szTime);
//		return;
//	}		
//
//	CCMatchObject* pObj = GetObject(pJob->GetUID());
//	if (pObj == NULL) return;
//	if (pJob->GetResultCommand() == NULL) return;
//
//	RouteToListener(pObj, pJob->GetResultCommand());
//}


//void CCMatchServer::OnAsyncGetCharInfo(CCAsyncJob* pJobResult)
//{
//	CCAsyncDBJob_GetCharInfo* pJob = (CCAsyncDBJob_GetCharInfo*)pJobResult;
//
//	if (pJob->GetResult() != CCASYNC_RESULT_SUCCEED) {
//		cclog("DB Query(OnAsyncGetCharInfo > GetCharInfoByAID) Failed\n");
//		return;
//	}
//
//	CCMatchObject* pObj = GetObject(pJob->GetUID());
//	if (pObj == NULL) return;
//
//	if (pObj->GetCharInfo())
//	{
//		// 이전에 캐릭이 선택되어 있었다면 캐릭끝날때 로그 남긴다
//		if (pObj->GetCharInfo()->m_nCID != 0)
//		{
//			CharFinalize(pObj->GetUID());		// 캐릭끝날때 디비 로그 등 처리
//		}
//
//		pObj->FreeCharInfo();
//		pObj->FreeFriendInfo();
//	}
//
//	if (pJob->GetCharInfo() == NULL)
//	{
//		cclog("pJob->GetCharInfo() IS NULL\n");
//		return;
//	}
//	pObj->SetCharInfo(pJob->GetCharInfo());		// Save Async Result
////	pObj->SetFriendInfo(pJob->GetFriendInfo());	// Save Async Result
//
//	if (CharInitialize(pJob->GetUID()) == false)
//	{
//		cclog("OnAsyncGetCharInfo > CharInitialize failed");
//		return;
//	}
//
//	// Client에 선택한 캐릭터 정보 전송
//	CCTD_CharInfo trans_charinfo;
//	CopyCharInfoForTrans(&trans_charinfo, pJob->GetCharInfo(), pObj);
//	
//	CCCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_SELECT_CHAR, CCUID(0,0));
//	pNewCmd->AddParameter(new CCCommandParameterInt(MOK));		// result
//
//	void* pCharArray = CCMakeBlobArray(sizeof(CCTD_CharInfo), 1);
//	CCTD_CharInfo* pTransCharInfo = (CCTD_CharInfo*)CCGetBlobArrayElement(pCharArray, 0);
//	memcpy(pTransCharInfo, &trans_charinfo, sizeof(CCTD_CharInfo));
//	pNewCmd->AddParameter(new CCCommandParameterBlob(pCharArray, CCGetBlobArraySize(pCharArray)));
//	CCEraseBlobArray(pCharArray);
//
//
//	// 내 캐릭터의 추가 정보
//	void* pMyExtraInfoArray = CCMakeBlobArray(sizeof(CCTD_MyExtraCharInfo), 1);
//	CCTD_MyExtraCharInfo* pMyExtraInfo = (CCTD_MyExtraCharInfo*)CCGetBlobArrayElement(pMyExtraInfoArray, 0);
//	int nPercent = CCMatchFormula::GetLevelPercent(trans_charinfo.nXP, (int)trans_charinfo.nLevel);
//	pMyExtraInfo->nLevelPercent = (char)nPercent;
//	pNewCmd->AddParameter(new CCCommandParameterBlob(pMyExtraInfoArray, CCGetBlobArraySize(pMyExtraInfoArray)));
//	CCEraseBlobArray(pMyExtraInfoArray);
//
//	RouteToListener(pObj, pNewCmd);
//
//#ifdef _DELETE_CLAN
//	if( CCMCDS_NORMAL != pJob->GetDeleteState() )
//	{
//		if( MMCDS_WAIT )
//		{
//			// 글랜 폐쇄 날짜를 알려줌.
//			
//			CCCommand* pCmdDelClan = CreateCommand( MC_MATCH_CLAN_ACCOUNCE_DELETE, pObj->GetUID() );
//			pCmdDelClan->AddParameter( new CCCmdParamStr(pObj->GetCharInfo()->m_ClanInfo.m_strDeleteDate.c_str()) );
//			Post( pCmdDelClan );
//		}
//		else if( MMCDS_DELETE )
//		{
//			// 클랜 폐쇄 시킴.
//		}
//	}
//#endif
//}

void CCMatchServer::OnAsyncGetFriendList(CCAsyncJob* pJobInput)
{
	CCAsyncDBJob_FriendList* pJob = (CCAsyncDBJob_FriendList*)pJobInput;

	if (pJob->GetResult() != CCASYNC_RESULT_SUCCEED) 
	{
		return;
	}

	CCMatchObject* pObj = GetObject(pJob->GetUID());
	if (!IsEnabledObject(pObj)) return;

	pObj->SetFriendInfo(pJob->GetFriendInfo());	// Save Async Result

	FriendList(pObj->GetUID());
}

void CCMatchServer::OnAsyncCreateChar(CCAsyncJob* pJobResult)
{
	CCAsyncDBJob_CreateChar* pJob = (CCAsyncDBJob_CreateChar*)pJobResult;

	if (pJob->GetResult() != CCASYNC_RESULT_SUCCEED) {
		char szTime[128]="";
		_strtime(szTime);

		cclog("[%s] Async DB Query(CreateChar) Failed\n", szTime);
		return;
	}		

	CCMatchObject* pObj = GetObject(pJob->GetUID());
	if (pObj == NULL) return;

	// Make Result
	CCCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_CREATE_CHAR, CCUID(0,0));
	pNewCmd->AddParameter(new CCCommandParameterInt(pJob->GetDBResult()));			// result
	pNewCmd->AddParameter(new CCCommandParameterString(pJob->GetCharName()));	// 만들어진 캐릭터 이름

	RouteToListener( pObj, pNewCmd );
}

void CCMatchServer::OnAsyncDeleteChar(CCAsyncJob* pJobResult)
{
	CCAsyncDBJob_DeleteChar* pJob = (CCAsyncDBJob_DeleteChar*)pJobResult;

	if (pJob->GetResult() != CCASYNC_RESULT_SUCCEED) {
		char szTime[128]="";
		_strtime(szTime);

		cclog("[%s] Async DB Query(DeleteChar) Failed\n", szTime);
	}		

	CCMatchObject* pObj = GetObject(pJob->GetUID());
	if (pObj == NULL) return;

	RouteResponseToListener(pObj, MC_MATCH_RESPONSE_DELETE_CHAR, pJob->GetDeleteResult());
}

void CCMatchServer::OnAsyncWinTheClanGame(CCAsyncJob* pJobInput)
{
	if (pJobInput->GetResult() != CCASYNC_RESULT_SUCCEED) {
		char szTime[128]="";
		_strtime(szTime);

		cclog("[%s] Async DB Query(OnAsyncWinTheClanGame) Failed\n", szTime);
		return;
	}		

}


void CCMatchServer::OnAsyncUpdateCharInfoData(CCAsyncJob* pJobInput)
{
	if (pJobInput->GetResult() != CCASYNC_RESULT_SUCCEED) {
		char szTime[128]="";
		_strtime(szTime);

		cclog("[%s] Async DB Query(OnAsyncUpdateCharInfoData) Failed\n", szTime);
		return;
	}		

}

void CCMatchServer::OnAsyncCharFinalize(CCAsyncJob* pJobInput)
{
	if (pJobInput->GetResult() != CCASYNC_RESULT_SUCCEED) {
		char szTime[128]="";
		_strtime(szTime);

		cclog("[%s] Async DB Query(OnAsyncCharFinalize) Failed\n", szTime);
		return;
	}

}

//void CCMatchServer::OnAsyncBringAccountItem(CCAsyncJob* pJobResult)
//{
//	CCAsyncDBJob_BringAccountItem* pJob = (CCAsyncDBJob_BringAccountItem*)pJobResult;
//
//	CCMatchObject* pObj = GetObject(pJob->GetUID());
//	if (!IsEnabledObject(pObj)) return;
//
//	int nRet = MERR_UNKNOWN;
//
//	if (pJob->GetResult() == CCASYNC_RESULT_SUCCEED) 
//	{
//		unsigned long int nNewCIID =	pJob->GetNewCIID();
//		unsigned long int nNewItemID =	pJob->GetNewItemID();
//		bool bIsRentItem =				pJob->GetRentItem();
//		int nRentMinutePeriodRemainder = pJob->GetRentMinutePeriodRemainder();
//
//
//
//
//		// 오브젝트에 아이템 추가
//		CCUID uidNew = CCMatchItemMap::UseUID();
//		pObj->GetCharInfo()->m_ItemList.CreateItem(uidNew, nNewCIID, nNewItemID, bIsRentItem, nRentMinutePeriodRemainder);
//
//		nRet = MOK;
//	}		
//
//	ResponseCharacterItemList(pJob->GetUID());	// 새로 바뀐 아이템 리스트도 다시 뿌려준다.
//
//
//	CCCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BRING_ACCOUNTITEM, CCUID(0,0));
//	pNew->AddParameter(new CCCmdParamInt(nRet));
//	RouteToListener(pObj, pNew);
//
//
//}

void CCMatchServer::OnAsyncInsertConnLog(CCAsyncJob* pJobResult)
{
	if (pJobResult->GetResult() != CCASYNC_RESULT_SUCCEED) {
		char szTime[128]="";
		_strtime(szTime);

		cclog("[%s] Async DB Query(OnAsyncInsertConnLog) Failed\n", szTime);
		return;
	}		

}

void CCMatchServer::OnAsyncInsertGameLog(CCAsyncJob* pJobResult)
{
	CCAsyncDBJob_InsertGameLog* pJob = (CCAsyncDBJob_InsertGameLog*)pJobResult;

	if (pJob->GetResult() != CCASYNC_RESULT_SUCCEED) {
		char szTime[128]=""; _strtime(szTime);
		cclog("[%s] Async DB Query(OnAsyncInsertGameLog) Failed\n", szTime);
		return;
	}

	CCMatchStage* pStage = FindStage(pJob->GetOwnerUID());
	if( pStage == NULL ) return;

	pStage->SetGameLogID(pJob->GetID());
}

void CCMatchServer::OnAsyncCreateClan(CCAsyncJob* pJobResult)
{
	CCAsyncDBJob_CreateClan* pJob = (CCAsyncDBJob_CreateClan*)pJobResult;

	CCUID uidMaster = pJob->GetMasterUID();
	CCMatchObject* pMasterObject = GetObject(uidMaster);
	

	if (pJob->GetResult() != CCASYNC_RESULT_SUCCEED) {
		if (IsEnabledObject(pMasterObject))
		{
			RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_AGREED_CREATE_CLAN, MERR_CLAN_CANNOT_CREATE);
		}
		return;
	}		

	int nNewCLID = pJob->GetNewCLID();

	if ( (pJob->GetDBResult() == false) || (nNewCLID ==0) )
	{
		if (IsEnabledObject(pMasterObject))
		{
			RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_AGREED_CREATE_CLAN, MERR_CLAN_CANNOT_CREATE);
		}
		return;
	}


	// 마스터의 바운티를 깎는다.
	if (IsEnabledObject(pMasterObject))
	{
		pMasterObject->GetCharInfo()->IncBP(-CLAN_CREATING_NEED_BOUNTY);
		ResponseMySimpleCharInfo(pMasterObject->GetUID());
	
		UpdateCharClanInfo(pMasterObject, nNewCLID, pJob->GetClanName(), CCG_MASTER);
	
		// 임시코드... 잘못된 CCMatchObject*가 온다면 체크하여 잡기위함...20090224 by kammir
		if(pMasterObject->GetCharInfo()->m_ClanInfo.GetClanID() >= 9000000)
			LOG(LOG_FILE, "[OnAsyncCreateClan()] %s's ClanID:%d.", pMasterObject->GetAccountName(), pMasterObject->GetCharInfo()->m_ClanInfo.GetClanID());

	}


	CCMatchObject* pSponsorObjects[CLAN_SPONSORS_COUNT];
	_ASSERT(CLAN_SPONSORS_COUNT == 4);

	pSponsorObjects[0] = GetObject(pJob->GetMember1UID());
	pSponsorObjects[1] = GetObject(pJob->GetMember2UID());
	pSponsorObjects[2] = GetObject(pJob->GetMember3UID());
	pSponsorObjects[3] = GetObject(pJob->GetMember4UID());

	for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	{
		if (IsEnabledObject(pSponsorObjects[i]))
		{
			UpdateCharClanInfo(pSponsorObjects[i], nNewCLID, pJob->GetClanName(), CCG_MEMBER);
			// 임시코드... 잘못된 CCMatchObject*가 온다면 체크하여 잡기위함...20090224 by kammir
			if(pSponsorObjects[i]->GetCharInfo()->m_ClanInfo.GetClanID() >= 9000000)
				LOG(LOG_FILE, "[OnAsyncCreateClan()] %s's ClanID:%d.", pSponsorObjects[i]->GetAccountName(), pSponsorObjects[i]->GetCharInfo()->m_ClanInfo.GetClanID());

			RouteResponseToListener(pSponsorObjects[i], MC_MATCH_RESPONSE_RESULT, MRESULT_CLAN_CREATED);
		}
	}

	if (IsEnabledObject(pMasterObject))
	{
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_AGREED_CREATE_CLAN, MOK);
	}
}

void CCMatchServer::OnAsyncExpelClanMember(CCAsyncJob* pJobResult)
{
	CCAsyncDBJob_ExpelClanMember* pJob = (CCAsyncDBJob_ExpelClanMember*)pJobResult;

	CCMatchObject* pAdminObject = GetObject(pJob->GetAdminUID());

	if (pJobResult->GetResult() != CCASYNC_RESULT_SUCCEED) 
	{
		if (IsEnabledObject(pAdminObject))
		{
			RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MERR_CLAN_CANNOT_EXPEL_FOR_NO_MEMBER);
		}
		return;
	}		

	int nDBRet = pJob->GetDBResult();
	switch (nDBRet)
	{
	case CCMatchDBMgr::ER_NO_MEMBER:
		{
			if (IsEnabledObject(pAdminObject))
			{
				RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MERR_CLAN_CANNOT_EXPEL_FOR_NO_MEMBER);
			}
			return;
		}
		break;
	case CCMatchDBMgr::ER_WRONG_GRADE:
		{
			if (IsEnabledObject(pAdminObject))
			{
				RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MERR_CLAN_CANNOT_CHANGE_GRADE);
			}
			return;
		}
		break;
	}


	// 만약 당사자가 접속해있으면 클랜탈퇴되었다고 알려줘야한다.
	CCMatchObject* pMemberObject = GetPlayerByName(pJob->GetTarMember());
	if (IsEnabledObject(pMemberObject))
	{
		UpdateCharClanInfo(pMemberObject, 0, "", CCG_NONE);
		// 임시코드... 잘못된 CCMatchObject*가 온다면 체크하여 잡기위함...20090224 by kammir
		if(pMemberObject->GetCharInfo()->m_ClanInfo.GetClanID() >= 9000000)
			LOG(LOG_FILE, "[OnAsyncExpelClanMember()] %s's ClanID:%d.", pMemberObject->GetAccountName(), pMemberObject->GetCharInfo()->m_ClanInfo.GetClanID());

	}

	if (IsEnabledObject(pAdminObject))
	{
		RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MOK);
	}
}



void CCMatchServer::OnAsyncInsertEvent( CCAsyncJob* pJobResult )
{
	if( 0 == pJobResult )
		return;

	CCAsyncDBJob_EventLog* pEventJob = 
		reinterpret_cast< CCAsyncDBJob_EventLog* >( pJobResult );

	if( pEventJob->GetAnnounce().empty() )
		return;

	if( CCASYNC_RESULT_SUCCEED == pJobResult->GetResult() )
	{
		CCCommand* pCmd;
		AsyncEventObjVec::const_iterator it, end;
		const AsyncEventObjVec& EventObjList = pEventJob->GetEventObjList();

		end = EventObjList.end();
		for( it = EventObjList.begin(); it != end; ++it )
		{
			if( CCUID(0, 0) != it->uidUser )
			{
				pCmd = CreateCommand( MC_MATCH_ANNOUNCE, it->uidUser );
				if( 0 != pCmd )
				{
					pCmd->AddParameter( new CCCmdParamUInt(0) );
					pCmd->AddParameter( new CCCmdParamStr(pEventJob->GetAnnounce().c_str()) );
					Post( pCmd );
				}
			}
		}
	}
}


void CCMatchServer::OnAsyncUpdateIPtoCoutryList( CCAsyncJob* pJobResult )
{
	CCCommand* pCmd = CreateCommand( MC_LOCAL_UPDATE_IP_TO_COUNTRY, GetUID() );
	if( 0 != pCmd )
		Post( pCmd );
}


void CCMatchServer::OnAsyncUpdateBlockCountryCodeList( CCAsyncJob* pJobResult )
{
	CCCommand* pCmd = CreateCommand( MC_LOCAL_UPDATE_BLOCK_COUTRYCODE, GetUID() );
	if( 0 != pCmd )
		Post( pCmd );
}


void CCMatchServer::OnAsyncUpdateCustomIPList( CCAsyncJob* pJobResult )
{
	CCCommand* pCmd = CreateCommand( MC_LOCAL_UPDATE_CUSTOM_IP, GetUID() );
	if( 0 != pCmd )
		Post( pCmd );
}


void CCMatchServer::OnAsyncGetAccountItemList( CCAsyncJob* pJobResult )
{
	CCAsyncDBJob_GetAccountItemList* pJob = (CCAsyncDBJob_GetAccountItemList*)pJobResult;

	if( CCASYNC_RESULT_SUCCEED != pJob->GetResult() ) {
		cclog("GetAccountItemList Failed\n");
		return;
	}

	CCMatchObject* pObj = GetObject( pJob->GetPlayerUID() );
	if( NULL == pObj ) return;

	if( !pJob->GetExpiredAccountItems().empty() ) {
		ResponseExpiredItemIDList(pObj, pJob->GetExpiredAccountItems());
	}

	const int nAccountItemCount = pJob->GetAccountItemCount();

	if (nAccountItemCount > 0) {
		CCAccountItemNode* accountItems = pJob->GetAccountItemList();
		if( NULL == accountItems ) return;

		CCCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_ACCOUNT_ITEMLIST, pObj->GetUID());

		// 갖고 있는 아이템 리스트 전송
		int nCountableAccountItemCount = 0;
		for(int i = 0; i < nAccountItemCount; i++ ) {
			if( accountItems[i].nCount > 0 && accountItems[i].nItemID > 0 ) { 
				nCountableAccountItemCount++; 
			}
		}

		void* pItemArray = CCMakeBlobArray(sizeof(CCTD_AccountItemNode), nCountableAccountItemCount);		

		int nIndex = 0;
		for (int i = 0; i < nAccountItemCount; i++)
		{
			if( accountItems[i].nItemID == 0 ) continue;
			CCTD_AccountItemNode* pItemNode = (CCTD_AccountItemNode*)CCGetBlobArrayElement(pItemArray, nIndex);			

			_ASSERTE( ((NULL != MGetMatchItemDescMgr()->GetItemDesc(accountItems[i].nItemID)) 
				|| (NULL != m_GambleMachine.GetGambleItemByGambleItemID(accountItems[i].nItemID)))
				&& "zitem.xml or GambleItem에 기술되어 있지 않는 아이템입니다." );

			if( accountItems[i].nCount > 0 ) {
				Make_MTDAccountItemNode(pItemNode, accountItems[i].nAIID, accountItems[i].nItemID
					, accountItems[i].nRentMinutePeriodRemainder, accountItems[i].nCount);

				nIndex++;

				if( nIndex == nCountableAccountItemCount ) { break;	}
			}			
		}

		pNew->AddParameter(new CCCommandParameterBlob(pItemArray, CCGetBlobArraySize(pItemArray)));
		CCEraseBlobArray(pItemArray);

		PostSafeQueue( pNew );
	}
}


void CCMatchServer::OnAsyncBuyQuestItem( CCAsyncJob* pJobReslt )
{
	CCAsyncDBJob_BuyQuestItem* pJob = (CCAsyncDBJob_BuyQuestItem*)pJobReslt;
	if( CCASYNC_RESULT_SUCCEED != pJob->GetResult() ){ return; }

	CCMatchObject* pPlayer = GetObject( pJob->GetPlayerUID() );
	if( NULL == pPlayer ) {	return; }

	CCMatchCharInfo* pCharInfo = pPlayer->GetCharInfo();
	if( NULL == pCharInfo ) { return; }

	// 아이템 거래 카운트 증가. 내부에서 디비 업데이트 결정.
	pCharInfo->GetDBQuestCachingData().IncreaseShopTradeCount(pJob->GetItemCount());
	pCharInfo->m_nBP -= pJob->GetPrice();

	
	CCCommand* pNewCmd = CreateCommand( MC_MATCH_RESPONSE_BUY_QUEST_ITEM, pJob->GetPlayerUID() );
	if( 0 == pNewCmd ) {
		cclog( "CCMatchServer::OnResponseBuyQuestItem - new Command실패.\n" );
		return;
	}
	
	pNewCmd->AddParameter( new CCCmdParamInt(MOK) );
	pNewCmd->AddParameter( new CCCmdParamInt(pCharInfo->m_nBP) );
	PostSafeQueue( pNewCmd );

	// 퀘스트 아이템 리스트를 다시 전송함.
	OnRequestCharQuestItemList( pJob->GetPlayerUID() );
}

//////////////////////////////////////////////////////////////////////////////////////////////
// 2009. 6. 3 - Added By Hong KiJu

void CCMatchServer::OnAsyncSurvivalModeGameLog( CCAsyncJob* pJobResult )
{
	CCAsyncDBJob_InsertSurvivalModeGameLog *pJob = (CCAsyncDBJob_InsertSurvivalModeGameLog *)pJobResult;

	if( CCASYNC_RESULT_SUCCEED != pJob->GetResult() )
	{
		return;
	}
}

void CCMatchServer::OnAsyncSurvivalModeGroupRanking(CCAsyncJob* pJobResult)
{
	CCAsyncDBJob_GetSurvivalModeGroupRanking* pJob = (CCAsyncDBJob_GetSurvivalModeGroupRanking*)pJobResult;
	
	if( CCASYNC_RESULT_SUCCEED != pJob->GetResult() )
	{
		return;
	}
/*#ifdef _DEBUG
	// 다음은 CCAsyncDBJob_GetSurvivalModeGroupRanking Class의 사용 요령입니다.
	// Class에서 더 필요한 부분이 있으면 수정하시면 됩니다.
	for(DWORD dwScenarioID = 1; dwScenarioID <= MAX_SURVIVAL_SCENARIO_COUNT; dwScenarioID++)
	{
		cclog("-------- Scenario ID = %d, Top 100 --------\n", dwScenarioID);

		for(int i = 1; i <= 100; i++)
		{
			RANKINGINFO *pRankingInfo = pJob->GetRankingInfo(dwScenarioID, i);
			
			if( NULL != pRankingInfo )
			{
				cclog("CID = %d, Ranking = %d, Ranking Point = %d\n", 
					pRankingInfo->dwCID, pRankingInfo->dwRanking, pRankingInfo->dwRankingPoint);
			}

		}
		
	}
#endif*/

	//가짜 목록 생성
	/*char sz[256];
	for (DWORD s=0; s<MAX_SURVIVAL_SCENARIO_COUNT; ++s)
	{
		for (int i=0; i<MAX_SURVIVAL_RANKING_LIST; ++i)
		{
			RANKINGINFO *pRankingInfo = pJob->GetRankingInfo(s, i);
			if (pRankingInfo)
			{
				pSurvivalRankInfo->SetRanking(s, i, 
					pRankingInfo->dwRanking, pRankingInfo->szCharName, pRankingInfo->dwRankingPoint);
			}
		}
	}*/

	CCSurvivalRankInfo* pSurvivalRankInfo = CCMatchServer::GetInstance()->GetQuest()->GetSurvivalRankInfo();
	pSurvivalRankInfo->ClearRanking();

	for (DWORD s=0; s<MAX_SURVIVAL_SCENARIO_COUNT; ++s)
	{
		for (int i=0; i<MAX_SURVIVAL_RANKING_LIST; ++i)
		{
			RANKINGINFO *pRankingInfo = pJob->GetRankingInfo(s, i);
			if (pRankingInfo)
			{
				pSurvivalRankInfo->SetRanking(s, i, 
					pRankingInfo->dwRanking, pRankingInfo->szCharName, pRankingInfo->dwRankingPoint);
			}
		}
	}
}

void CCMatchServer::OnAsyncSurvivalModePrivateRanking(CCAsyncJob* pJobResult)
{
	CCAsyncDBJob_GetSurvivalModePrivateRanking* pJob = (CCAsyncDBJob_GetSurvivalModePrivateRanking*)pJobResult;

	if( CCASYNC_RESULT_SUCCEED != pJob->GetResult() )
	{
		cclog("CCMatchServer::OnAsyncSurvivalModePrivateRanking - 실패! stageUID[%d] playerCID[%d]\n", pJob->GetStageUID(), pJob->GetCID());
		return;
	}
#ifdef _DEBUG
	cclog("CCMatchServer::OnAsyncSurvivalModePrivateRanking - Test Log입니다. 성공!\n");

	// 다음은 CCAsyncDBJob_GetSurvivalModePrivateRanking Class의 사용 요령입니다.
	// Class에서 더 필요한 부분이 있으면 수정하시면 됩니다.
	cclog("-------- User Ranking Info --------\n");
	cclog("User CID = %d\n", pJob->GetCID());

	for(DWORD dwScenarioID = 1; dwScenarioID < MAX_SURVIVAL_SCENARIO_COUNT + 1; dwScenarioID++)
	{
		RANKINGINFO* pRankingInfo = pJob->GetPrivateRankingInfo(dwScenarioID);

		cclog("Scenario ID = %01d, Ranking = %d, Ranking Point = %d\n", 
			dwScenarioID, pRankingInfo->dwRanking, pRankingInfo->dwRankingPoint);
	}
#endif

	_ASSERT( pJob->GetScenarioID()-1 < MAX_SURVIVAL_SCENARIO_COUNT );
	RANKINGINFO* pRankingInfo = pJob->GetPrivateRankingInfo( pJob->GetScenarioID() );
	if (pRankingInfo)
	{
		// 플레이어에게 랭킹 정보를 보낸다		
		CCCommand* pCmdPrivateRanking = MGetMatchServer()->CreateCommand( MC_SURVIVAL_PRIVATERANKING, CCUID(0, 0) );
		if( NULL == pCmdPrivateRanking )
			return;

		pCmdPrivateRanking->AddParameter( new CCCommandParameterUInt(pRankingInfo->dwRanking) );
		pCmdPrivateRanking->AddParameter( new CCCommandParameterUInt(pRankingInfo->dwRankingPoint) );
		
		RouteToObjInStage(pJob->GetStageUID(), pJob->GetPlayerUID(), pCmdPrivateRanking);
	}
	else _ASSERT(0);
	
}