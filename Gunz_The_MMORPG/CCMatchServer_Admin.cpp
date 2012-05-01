#include "stdafx.h"
#include "CCMatchServer.h"
#include "CCSharedCommandTable.h"
#include "CCErrorTable.h"
#include "CCBlobArray.h"
#include "CCObject.h"
#include "CCMatchObject.h"
#include "CCMatchItem.h"
#include "CCAgentObject.h"
#include "CCMatchNotify.h"
#include "Msg.h"
#include "CCMatchObjCache.h"
#include "CCMatchStage.h"
#include "CCMatchTransDataType.h"
#include "CCMatchFormula.h"
#include "CCMatchConfig.h"
#include "CCCommandCommunicator.h"
#include "CCMatchShop.h"
#include "CCMatchTransDataType.h"
#include "CCDebug.h"
#include "CCMatchAuth.h"
#include "CCMatchStatus.h"
#include "CCAsyncDBJob.h"
#include "CCMatchTransDataType.h"
#include "CCMatchAntiHack.h"


void CCMatchServer::OnAdminTerminal(const CCUID& uidAdmin, const char* szText)
{
	CCMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	char szOut[32768]; szOut[0] = 0;

	if (m_Admin.Execute(uidAdmin, szText))
	{
		MCommand* pNew = CreateCommand(MC_ADMIN_TERMINAL, CCUID(0,0));
		pNew->AddParameter(new MCmdParamUID(CCUID(0,0)));
		pNew->AddParameter(new MCmdParamStr(szOut));
		RouteToListener(pObj, pNew);
	}
}

void CCMatchServer::OnAdminAnnounce(const CCUID& uidAdmin, const char* szChat, unsigned long int nType)
{
	CCMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	char szMsg[256];
	strcpy(szMsg, szChat);
	MCommand* pCmd = CreateCommand(MC_ADMIN_ANNOUNCE, CCUID(0,0));
	pCmd->AddParameter(new MCmdParamUID(uidAdmin));
	pCmd->AddParameter(new MCmdParamStr(szMsg));
	pCmd->AddParameter(new MCmdParamUInt(nType));

	RouteToAllClient(pCmd);
}



void CCMatchServer::OnAdminRequestServerInfo(const CCUID& uidAdmin)
{
	CCMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	// 서버 정보 보여주는것 아직 안넣었음
/*
	MCommand* pNew = CreateCommand(MC_MATCH_ANNOUNCE, CCUID(0,0));
	pNew->AddParameter(new MCmdParamUInt(0));

	RouteToListener(pObj, pNew);
*/
}
void CCMatchServer::OnAdminServerHalt(const CCUID& uidAdmin)
{
	LOG(LOG_PROG, "OnAdminServerHalt(...) Called");

	CCMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	CCMatchUserGradeID nGrade = pObj->GetAccountInfo()->m_nUGrade;

	// 관리자 권한을 가진 사람이 아니면 무시.
	if ((nGrade != CCMUGADMIN) && (nGrade != CCMUGDEVELOPER)) return;

	// Shutdown 시작	
	m_MatchShutdown.Start(GetGlobalClockCount());	
}

// 서버에서 메뉴로만 쓰는 명령어..
void CCMatchServer::OnAdminServerHalt()
{
	LOG(LOG_PROG, "OnAdminServerHalt() Called");

	// Shutdown 시작	
	m_MatchShutdown.Start(GetGlobalClockCount());	
}

void CCMatchServer::OnAdminRequestUpdateAccountUGrade(const CCUID& uidAdmin, const char* szPlayer)
{
	CCMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	int nRet = MOK;

	if ((strlen(szPlayer)) < 2) return;
	CCMatchObject* pTargetObj = GetPlayerByName(szPlayer);
	if (pTargetObj == NULL) return;



/*
	MCommand* pNew = CreateCommand(MC_ADMIN_REQUEST_UPDATE_ACCOUNT_UGRADE, CCUID(0,0));
	pNew->AddParameter(new MCmdParamUInt(nRet));
	RouteToListener(pObj, pNew);
*/
}

void CCMatchServer::OnAdminPingToAll(const CCUID& uidAdmin)
{
	CCMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	MCommand* pNew = CreateCommand(MC_NET_PING, CCUID(0,0));
	pNew->AddParameter(new MCmdParamUInt(GetGlobalClockCount()));
	RouteToAllConnection(pNew);
}


void CCMatchServer::OnAdminRequestSwitchLadderGame(const CCUID& uidAdmin, const bool bEnabled)
{
	CCMatchObject* pObj = GetObject(uidAdmin);
	if (!IsEnabledObject(pObj)) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	
	MGetServerConfig()->SetEnabledCreateLadderGame(bEnabled);


	char szMsg[256] = "설정되었습니다.";
	Announce(pObj, szMsg);
}

void CCMatchServer::OnAdminHide(const CCUID& uidAdmin)
{
	CCMatchObject* pObj = GetObject(uidAdmin);
	if (!IsEnabledObject(pObj)) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

#if defined(LOCALE_NHNUSA) || defined(_DEBUG)
	m_HackingChatList.Init();
	cclog( "reload hacking chat list.\n" );
#endif

	if (pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) {
		pObj->SetPlayerFlag(MTD_PlayerFlags_AdminHide, false);
		Announce(pObj, "Now Revealing...");
	} else {
		pObj->SetPlayerFlag(MTD_PlayerFlags_AdminHide, true);
		Announce(pObj, "Now Hiding...");
	}
}

void CCMatchServer::OnAdminResetAllHackingBlock( const CCUID& uidAdmin )
{
	CCMatchObject* pObj = GetObject( uidAdmin );
	if( (0 != pObj) && IsAdminGrade(pObj) )
	{
		GetDBMgr()->AdminResetAllHackingBlock();
	}
}

void CCMatchServer::OnAdminRequestKickPlayer(const CCUID& uidAdmin, const char* szPlayer)
{
	CCMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL)			return;
	if (!IsAdminGrade(pObj))	return;
	if ((strlen(szPlayer)) < 2) return;

	int nRet = MOK;
	CCMatchObject* pTargetObj = GetPlayerByName(szPlayer);
	if (pTargetObj != NULL) 
	{
#ifdef LOCALE_KOREA
		pTargetObj->DisconnectHacker( CCMHT_COMMAND_BLOCK_BY_ADMIN );
#else
		// Notify Message 필요 -> 관리자 전용 - 해결(특별한 메세지 필요 없음)
		Disconnect(pTargetObj->GetUID());
#endif
	} else {
		nRet = MERR_ADMIN_NO_TARGET;
	}

	MCommand* pNew = CreateCommand(MC_ADMIN_RESPONSE_KICK_PLAYER, CCUID(0,0));
	pNew->AddParameter(new MCmdParamInt(nRet));
	RouteToListener(pObj, pNew);
}

void CCMatchServer::OnAdminRequestMutePlayer(const CCUID& uidAdmin, const char* szPlayer, const int nPenaltyHour)
{
	CCMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL)			return;	
	if (!IsAdminGrade(pObj))	return;
	if ((strlen(szPlayer)) < 2) return;

	int nRet = MOK;
	CCMatchObject* pTargetObj = GetPlayerByName(szPlayer);	
	if (pTargetObj != NULL) 
	{
		pTargetObj->GetAccountPenaltyInfo()->SetPenaltyInfo(MPC_CHAT_BLOCK, nPenaltyHour);
		
		const CCPenaltyInfo* pPenaltyInfo = pTargetObj->GetAccountPenaltyInfo()->GetPenaltyInfo(MPC_CHAT_BLOCK);
		if( m_MatchDBMgr.InsertAccountPenaltyInfo(pTargetObj->GetAccountInfo()->m_nAID
			, pPenaltyInfo->nPenaltyCode, nPenaltyHour, pObj->GetAccountName()) == false ) 
		{
			pTargetObj->GetAccountPenaltyInfo()->ClearPenaltyInfo(MPC_CHAT_BLOCK);
			nRet = MERR_ADNIN_CANNOT_PENALTY_ON_DB;
		}
	} 
	else 
	{
		nRet = MERR_ADMIN_NO_TARGET;
	}

	MCommand* pNew = CreateCommand(MC_ADMIN_RESPONSE_MUTE_PLAYER, CCUID(0,0));
	pNew->AddParameter(new MCmdParamInt(nRet));
	
	if( nRet == MOK ) {
		RouteToListener(pTargetObj, pNew->Clone());
	}

	RouteToListener(pObj, pNew);
}

void CCMatchServer::OnAdminRequestBlockPlayer(const CCUID& uidAdmin, const char* szPlayer, const int nPenaltyHour)
{
	CCMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL)			return;	
	if (!IsAdminGrade(pObj))	return;
	if ((strlen(szPlayer)) < 2) return;

	int nRet = MOK;
	CCMatchObject* pTargetObj = GetPlayerByName(szPlayer);	
	if (pTargetObj != NULL) 
	{
		pTargetObj->GetAccountPenaltyInfo()->SetPenaltyInfo(MPC_CONNECT_BLOCK, nPenaltyHour);

		const CCPenaltyInfo* pPenaltyInfo = pTargetObj->GetAccountPenaltyInfo()->GetPenaltyInfo(MPC_CONNECT_BLOCK);
		if( m_MatchDBMgr.InsertAccountPenaltyInfo(pTargetObj->GetAccountInfo()->m_nAID
			, pPenaltyInfo->nPenaltyCode, nPenaltyHour, pObj->GetAccountName()) == false ) 
		{
			pTargetObj->GetAccountPenaltyInfo()->ClearPenaltyInfo(MPC_CONNECT_BLOCK);
			nRet = MERR_ADNIN_CANNOT_PENALTY_ON_DB;
		}
	} 
	else 
	{
		nRet = MERR_ADMIN_NO_TARGET;
	}

	MCommand* pNew = CreateCommand(MC_ADMIN_RESPONSE_BLOCK_PLAYER, CCUID(0,0));
	pNew->AddParameter(new MCmdParamInt(nRet));

	if( nRet == MOK ) {
		Disconnect(pTargetObj->GetUID());
	}

	RouteToListener(pObj, pNew);
}