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
#include "CCLadderMgr.h"
#include "CCTeamGameStrategy.h"




/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int CCMatchServer::ValidateChallengeLadderGame(CCMatchObject** ppMemberObject, int nMemberCount)
{
	CCBaseTeamGameStrategy* pTeamGameStrategy = CCBaseTeamGameStrategy::GetInstance(CCGetServerConfig()->GetServerMode());
	if (pTeamGameStrategy)
	{
		int nRet = pTeamGameStrategy->ValidateChallenge(ppMemberObject, nMemberCount);
		return nRet;
	};

	return MOK;
}

///////////////////////////////////////////////////////////////////////////
// LadderStage
bool CCMatchServer::LadderJoin(const CCUID& uidPlayer, const CCUID& uidStage, CCMatchTeam nTeam)
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
	pObj->SetLadderChallenging(false);
	pStage->PlayerTeam(uidPlayer, nTeam);
	pStage->PlayerState(uidPlayer, MOSS_READY);
	

	CCCommand* pCmd = CreateCommand(MC_MATCH_LADDER_PREPARE, uidPlayer);
	pCmd->AddParameter(new CCCmdParamCCUID(uidStage));
	pCmd->AddParameter(new CCCmdParamInt(nTeam));
	Post(pCmd);

	return true;
}

void CCMatchServer::LadderGameLaunch(MLadderGroup* pGroupA, MLadderGroup* pGroupB)
{
	if ((CCGetServerConfig()->GetServerMode() != CSM_LADDER) && 
		(CCGetServerConfig()->GetServerMode() != CSM_CLAN)) return;

	CCUID uidStage = CCUID(0,0);
	if (StageAdd(NULL, "LADDER_GAME", true, "", &uidStage) == false) {
		// Group 해체
		GetLadderMgr()->CancelChallenge(pGroupA->GetID(), "");
		GetLadderMgr()->CancelChallenge(pGroupB->GetID(), "");
		return;
	}
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) {
		// Group 해체
		GetLadderMgr()->CancelChallenge(pGroupA->GetID(), "");
		GetLadderMgr()->CancelChallenge(pGroupB->GetID(), "");
		return;
	}

	// A 그룹 입장
	for (list<CCUID>::iterator i=pGroupA->GetPlayerListBegin(); i!= pGroupA->GetPlayerListEnd(); i++)
	{
		CCUID uidPlayer = (*i);
		LadderJoin(uidPlayer, uidStage, CCMT_RED);
	}
	// B 그룹 입장
	for (list<CCUID>::iterator i=pGroupB->GetPlayerListBegin(); i!= pGroupB->GetPlayerListEnd(); i++)
	{
		CCUID uidPlayer = (*i);
		LadderJoin(uidPlayer, uidStage, CCMT_BLUE);
	}

	// Agent 준비
//	ReserveAgent(pStage);

	//////////////////////////////////////////////////////////////////////////////
	int nRandomMap = 0;
	// 클랜전은 Stage의 팀정보에 CLID까지 설정해야한다.
	CCBaseTeamGameStrategy* pTeamGameStrategy = CCBaseTeamGameStrategy::GetInstance(CCGetServerConfig()->GetServerMode());
	if (pTeamGameStrategy)
	{
		nRandomMap = pTeamGameStrategy->GetRandomMap((int)pGroupA->GetPlayerCount());
	};


	CCMATCH_GAMETYPE nGameType = CCMATCH_GAMETYPE_DEATHMATCH_TEAM;

	// Game 설정
	pStage->SetStageType(CCST_LADDER);
	pStage->ChangeRule(nGameType);

	// 클랜전은 Stage의 팀정보에 CLID까지 설정해야한다.
	if (pTeamGameStrategy)
	{
		CCMatchLadderTeamInfo a_RedLadderTeamInfo, a_BlueLadderTeamInfo;
		pTeamGameStrategy->SetStageLadderInfo(&a_RedLadderTeamInfo, &a_BlueLadderTeamInfo, pGroupA, pGroupB);

		pStage->SetLadderTeam(&a_RedLadderTeamInfo, &a_BlueLadderTeamInfo);
	};

	CCMatchStageSetting* pSetting = pStage->GetStageSetting();
	pSetting->SetMasterUID(CCUID(0,0));
	pSetting->SetMapIndex(nRandomMap);
	pSetting->SetGameType(nGameType);

	pSetting->SetLimitTime(3);	
	pSetting->SetRoundMax(99);		// 최대 99라운드까지 진행할 수 있다.
	

	CCCommand* pCmd = CreateCmdResponseStageSetting(uidStage);
	RouteToStage(uidStage, pCmd);	// Stage Setting 전송


	// 디비에 로그를 남긴다.
	// test 맵등은 로그 남기지 않는다.
	if ( (CCGetMapDescMgr()->MIsCorrectMap(nRandomMap)) && (CCGetGameTypeMgr()->IsCorrectGameType(nGameType)) )
	{
		if (pStage->StartGame(CCGetServerConfig()->IsUseResourceCRC32CacheCheck()) == true) {		// 게임시작
			// Send Launch Command
			ReserveAgent(pStage);

			/////////////////////////////////////////////////////////////////////////////////////////////
			// 클랜전은 ObjectCache를 따로 전송하지 않기 때문에 Stage가 완성되면 그때 전송해 준다.
			// 이 정보는 클라이언트들끼리 Peer의 정보를 받을 수 있지만 서버가 용청할 시점과
			//  클라이언트의 리스트 구성시점이 다를 수 있기 때문에 이때 전송을 해준다.
			// - by SungE.
			CCMatchObjectCacheBuilder CacheBuilder;
			CacheBuilder.Reset();
			for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {
				CCUID uidObj = (CCUID)(*i).first;
				CCMatchObject* pScanObj = (CCMatchObject*)GetObject(uidObj);
				if (pScanObj) {
					CacheBuilder.AddObject(pScanObj);
				}
			}
			CCCommand* pCmdCacheAdd = CacheBuilder.GetResultCmd(MATCHCACHEMODE_UPDATE, this);
			RouteToStage(pStage->GetUID(), pCmdCacheAdd);
			/////////////////////////////////////////////////////////////////////////////////////////////

			CCCommand* pCmd = CreateCommand(MC_MATCH_LADDER_LAUNCH, CCUID(0,0));
			pCmd->AddParameter(new CCCmdParamCCUID(uidStage));
			pCmd->AddParameter(new CCCmdParamStr( const_cast<char*>(pStage->GetMapName()) ));
			RouteToStage(uidStage, pCmd);

			// Ladder Log 남긴다.
		} else {
			// Group 해체
			GetLadderMgr()->CancelChallenge(pGroupA->GetID(), "");
			GetLadderMgr()->CancelChallenge(pGroupB->GetID(), "");
		}
	}
}


bool CCMatchServer::IsLadderRequestUserInRequestClanMember( const CCUID& uidRequestMember
														  , const CCTD_LadderTeamMemberNode* pRequestMemberNode )
{
	// - by SungE 2007-10-11 
	// 해커가 MemberNamesBlob를 조작할 수 있기 때문에 요청자 자신의 이름을 저장하는 0번째 인덱스 노드를 
	//  커맨드 요청자와 같은지 검사해 줘야 한다. 클라이언트와 서버의 구조를 변경하지 않기 위해 추가 검사코드만 작성.
	// 이 코드는 클랜전요청과 관련된 클라이언트 코드가 변경되지 않는다는 전제조건에서 작성된 것임.
	// 0번 인덱스의 노드가 요청자이다.

	if( NULL == pRequestMemberNode )
		return false;

	CCMatchObject* pRequestMemberObj = GetPlayerByName( pRequestMemberNode->szName );
	if( NULL == pRequestMemberObj )
		return false;

	// 요청자의 캐릭터와 MemberNameNode의 0번째 인덱스 유저의 UID가 같은지 검사한다.

	if( uidRequestMember != pRequestMemberObj->GetUID() )
		return false; ///< 같지 않으면 비정상 유저의 요청으로 판단.s

	return true;
}

void CCMatchServer::OnLadderRequestChallenge(const CCUID& uidRequestMember, void* pMemberNamesBlob, unsigned long int nOptions)
{
	if ((CCGetServerConfig()->GetServerMode() != CSM_LADDER) && 
		(CCGetServerConfig()->GetServerMode() != CSM_CLAN)) return;

	CCMatchObject* pLeaderObject = GetPlayerByComCCUID(uidRequestMember);
	if (! IsEnabledObject(pLeaderObject)) return;

	if (!CCGetServerConfig()->IsEnabledCreateLadderGame())
	{
		RouteResponseToListener(pLeaderObject, MC_MATCH_LADDER_RESPONSE_CHALLENGE, MERR_LADDER_NOT_SERVICE_TIME);
		return;
	}

	int nBlobCount = CCGetBlobArrayCount(pMemberNamesBlob);
	int nMemberCount = nBlobCount;
	if (nMemberCount <= 0) return;

	if( !IsLadderRequestUserInRequestClanMember(uidRequestMember
		, (CCTD_LadderTeamMemberNode*)CCGetBlobArrayElement(pMemberNamesBlob, 0)) )
		return;
	
	CCMatchObject* pMemberObjects[MAX_CLANBATTLE_TEAM_MEMBER];
	for (int i = 0; i < nMemberCount; i++)
	{
		CCTD_LadderTeamMemberNode* pNode = (CCTD_LadderTeamMemberNode*)CCGetBlobArrayElement(pMemberNamesBlob, i);
		if (pNode == NULL) break;
		if ((strlen(pNode->szName) <= 0) || (strlen(pNode->szName) >= MATCHOBJECT_NAME_LENGTH)) return;

		pMemberObjects[i] = GetPlayerByName(pNode->szName);

		// 한명이라도 존재하지 않으면 안된다
		if (! IsEnabledObject(pMemberObjects[i]))
		{
			// 메세지 보내주고 끝.
			RouteResponseToListener(pLeaderObject, MC_MATCH_LADDER_RESPONSE_CHALLENGE, MERR_LADDER_CANNOT_CHALLENGE);
			return;
		}
	}


	int nRet = ValidateChallengeLadderGame(pMemberObjects, nMemberCount);
	if (nRet != MOK)
	{
		RouteResponseToListener(pLeaderObject, MC_MATCH_LADDER_RESPONSE_CHALLENGE, nRet);
		return;
	}

	int nTeamID = 0;

	CCBaseTeamGameStrategy* pTeamGameStrategy = NULL;

	pTeamGameStrategy = CCBaseTeamGameStrategy::GetInstance(CCGetServerConfig()->GetServerMode());
	if (pTeamGameStrategy)
	{
        nTeamID = pTeamGameStrategy->GetNewGroupID(pLeaderObject, pMemberObjects, nMemberCount);
	}
	if (nTeamID == 0) return;

	// 실제로 Challenge한다.
	// Ensure All Player Not in LadderGroup
	MLadderGroup* pGroup = GetLadderMgr()->CreateLadderGroup();
	pGroup->SetID(nTeamID);

	// balancedMatching 설정
	if (nOptions == 1)
	{
		pGroup->SetBalancedMatching(true);
	}
	else
	{
		pGroup->SetBalancedMatching(false);	
	}

	if (pTeamGameStrategy)
	{
		pTeamGameStrategy->SetLadderGroup(pGroup, pMemberObjects, nMemberCount);
	}

	for (int i=0; i<nMemberCount; i++) {
		pGroup->AddPlayer( pMemberObjects[i] );
	}

	GetLadderMgr()->Challenge(pGroup);
}

void CCMatchServer::OnLadderRequestCancelChallenge(const CCUID& uidPlayer)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (!IsEnabledObject(pObj)) return;
	if (pObj->GetLadderGroupID() == 0) return;

	GetLadderMgr()->CancelChallenge(pObj->GetLadderGroupID(), pObj->GetCharInfo()->m_szName);
}

void CCMatchServer::OnRequestProposal(const CCUID& uidProposer, const int nProposalMode, const int nRequestID, 
		                const int nReplierCount, void* pReplierNamesBlob)
{
	CCMatchObject* pProposerObject = GetObject(uidProposer);
	if (! IsEnabledObject(pProposerObject)) return;


	if ((nReplierCount > MAX_REPLIER) || (nReplierCount < 0))
	{
		_ASSERT(0);	// 16명이상 동의할 수 없음
		return;
	}


	if (!CCGetServerConfig()->IsEnabledCreateLadderGame())
	{
		// 메세지 보내주고 끝.
		CCCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_PROPOSAL, CCUID(0,0));
		pNewCmd->AddParameter(new CCCommandParameterInt(MERR_LADDER_NOT_SERVICE_TIME));
		pNewCmd->AddParameter(new CCCommandParameterInt(nProposalMode));
		pNewCmd->AddParameter(new CCCommandParameterInt(nRequestID));
		RouteToListener(pProposerObject, pNewCmd);
		return;
	}


	int nBlobCount = CCGetBlobArrayCount(pReplierNamesBlob);
	if (nBlobCount != nReplierCount) return;

	CCMatchObject* ppReplierObjects[MAX_REPLIER];

	for (int i = 0; i < nReplierCount; i++)
	{
		CCTD_ReplierNode* pNode = (CCTD_ReplierNode*)CCGetBlobArrayElement(pReplierNamesBlob, i);
		if (pNode == NULL) return;
		if ((strlen(pNode->szName) <= 0) || (strlen(pNode->szName) >= MATCHOBJECT_NAME_LENGTH)) return;

		ppReplierObjects[i] = GetPlayerByName(pNode->szName);

		// 답변자가 한명이라도 존재하지 않으면 안된다
		if (!IsEnabledObject(ppReplierObjects[i]))
		{
			// 메세지 보내주고 끝.
			CCCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_PROPOSAL, CCUID(0,0));
			pNewCmd->AddParameter(new CCCommandParameterInt(MERR_NO_TARGET));
			pNewCmd->AddParameter(new CCCommandParameterInt(nProposalMode));
			pNewCmd->AddParameter(new CCCommandParameterInt(nRequestID));
			RouteToListener(pProposerObject, pNewCmd);

			return;
		}
	}

	int nRet = MERR_UNKNOWN;
	// 상황에 맞게 validate 한다.

	switch (nProposalMode)
	{
	case MPROPOSAL_LADDER_INVITE:
		{
			MLadderGameStrategy* pLadderGameStrategy = MLadderGameStrategy::GetInstance();
			nRet = pLadderGameStrategy->ValidateRequestInviteProposal(pProposerObject, ppReplierObjects, nReplierCount);
		}
		break;
	case MPROPOSAL_CLAN_INVITE:
		{
			MClanGameStrategy* pClanGameStrategy = MClanGameStrategy::GetInstance();
			nRet = pClanGameStrategy->ValidateRequestInviteProposal(pProposerObject, ppReplierObjects, nReplierCount);
		}
		break;
	};

	if (nRet != MOK)
	{
		CCCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_PROPOSAL, CCUID(0,0));
		pNewCmd->AddParameter(new CCCommandParameterInt(nRet));
		pNewCmd->AddParameter(new CCCommandParameterInt(nProposalMode));
		pNewCmd->AddParameter(new CCCommandParameterInt(nRequestID));
		RouteToListener(pProposerObject, pNewCmd);
		return;
	}


	int nMemberCount = nReplierCount+1;		// 제안자까지 
	void* pBlobMembersNameArray = CCMakeBlobArray(sizeof(CCTD_ReplierNode), nMemberCount);

	CCTD_ReplierNode* pProposerNode = (CCTD_ReplierNode*)CCGetBlobArrayElement(pBlobMembersNameArray, 0);
	strcpy(pProposerNode->szName, pProposerObject->GetCharInfo()->m_szName);

	for (int k = 0; k < nReplierCount; k++)
	{
		CCTD_ReplierNode* pMemberNode = (CCTD_ReplierNode*)CCGetBlobArrayElement(pBlobMembersNameArray, k+1);
		strcpy(pMemberNode->szName, ppReplierObjects[k]->GetCharInfo()->m_szName);
	}

	// 답변자에게 동의를 물어본다.
	for (int i = 0; i < nReplierCount; i++)
	{
		CCCommand* pNewCmd = CreateCommand(MC_MATCH_ASK_AGREEMENT, CCUID(0,0));
		pNewCmd->AddParameter(new CCCommandParameterUID(uidProposer));
//		pNewCmd->AddParameter(new CCCommandParameterString(pProposerObject->GetCharInfo()->m_szName));
		pNewCmd->AddParameter(new CCCommandParameterBlob(pBlobMembersNameArray, CCGetBlobArraySize(pBlobMembersNameArray)));

		pNewCmd->AddParameter(new CCCommandParameterInt(nProposalMode));
		pNewCmd->AddParameter(new CCCommandParameterInt(nRequestID));
		RouteToListener(ppReplierObjects[i], pNewCmd);


	}
	CCEraseBlobArray(pBlobMembersNameArray);


	// 제안자에게 응답 보내줌
	CCCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_PROPOSAL, CCUID(0,0));
	pNewCmd->AddParameter(new CCCommandParameterInt(nRet));
	pNewCmd->AddParameter(new CCCommandParameterInt(nProposalMode));
	pNewCmd->AddParameter(new CCCommandParameterInt(nRequestID));
	RouteToListener(pProposerObject, pNewCmd);

}

void CCMatchServer::OnReplyAgreement(CCUID& uidProposer, CCUID& uidReplier, const char* szReplierName, 
		                const int nProposalMode, const int nRequestID, const bool bAgreement)
{
	CCMatchObject* pProposerObject = GetObject(uidProposer);
	if (! IsEnabledObject(pProposerObject)) return;

	
	CCCommand* pNewCmd = CreateCommand(MC_MATCH_REPLY_AGREEMENT, CCUID(0,0));
	pNewCmd->AddParameter(new CCCommandParameterUID(uidProposer));
	pNewCmd->AddParameter(new CCCommandParameterUID(uidReplier));
	pNewCmd->AddParameter(new CCCommandParameterString(szReplierName));
	pNewCmd->AddParameter(new CCCommandParameterInt(nProposalMode));
	pNewCmd->AddParameter(new CCCommandParameterInt(nRequestID));
	pNewCmd->AddParameter(new CCCommandParameterBool(bAgreement));

	RouteToListener(pProposerObject, pNewCmd);	
}
