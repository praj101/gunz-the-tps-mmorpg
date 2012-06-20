#include "stdafx.h"
#include "CCMatchServer.h"
#include "CCSharedCommandTable.h"
#include "CCErrorTable.h"
#include "CCBlobArray.h"
#include "CCAgentObject.h"
#include "CCDebug.h"
#include "CCCommandCommunicator.h"
#include "CCCommandBuilder.h"

int CCMatchServer::AgentAdd(const CCUID& uidComm)
{
	CCAgentObject* pAgent = new CCAgentObject(uidComm);

	m_AgentMap.insert(CCAgentObjectMap::value_type(pAgent->GetUID(), pAgent));

	LOG(LOG_PROG, "Agent Added (UID:%d%d)", pAgent->GetUID().High, pAgent->GetUID().Low);

	return MOK;
}

int CCMatchServer::AgentRemove(const CCUID& uidAgent, CCAgentObjectMap::iterator* pNextItor)
{
	CCAgentObjectMap::iterator i = m_AgentMap.find(uidAgent);
	if(i==m_AgentMap.end()) return MERR_OBJECT_INVALID;

	CCAgentObject* pAgent = (*i).second;

	LOG(LOG_PROG, "Agent Removed (UID:%d%d)", pAgent->GetUID().High, pAgent->GetUID().Low);

	// Clear up the Agent
	delete pAgent;

	CCAgentObjectMap::iterator itorTemp = m_AgentMap.erase(i);
	if (pNextItor)
		*pNextItor = itorTemp;

	return MOK;
}

void CCMatchServer::AgentClear()
{
	CCAgentObjectMap::iterator i = m_AgentMap.begin();
	for(;i!=m_AgentMap.end(); i++)
	{
		AgentRemove( i->first, &i);
	}
}

CCAgentObject* CCMatchServer::GetAgent(const CCUID& uidAgent)
{
	CCAgentObjectMap::iterator i = m_AgentMap.find(uidAgent);
	if(i==m_AgentMap.end()) return NULL;
	return (*i).second;
}


CCAgentObject* CCMatchServer::GetAgentByComCCUID(const CCUID& uidComm)
{
	for(CCAgentObjectMap::iterator i=m_AgentMap.begin(); i!=m_AgentMap.end(); i++){
		CCAgentObject* pAgent = ((*i).second);
		for (list<CCUID>::iterator j=pAgent->m_CommListener.begin();j!=pAgent->m_CommListener.end();j++){
			CCUID TargetUID = *j;
			if (TargetUID == uidComm)
				return pAgent;
		}
	}
	return NULL;
}


CCAgentObject* CCMatchServer::FindFreeAgent()
{
	CCAgentObject* pFreeAgent = NULL;
	for (CCAgentObjectMap::iterator i=m_AgentMap.begin(); i!=m_AgentMap.end(); i++) {
		CCAgentObject* pAgent = (*i).second;
		if ( (pFreeAgent == NULL) || (pFreeAgent->GetAssignCount() > pAgent->GetStageCount()) )
			pFreeAgent = pAgent;
	}
	return pFreeAgent;
}

void CCMatchServer::ReserveAgent(CCMatchStage* pStage)
{
	CCAgentObject* pFreeAgent = FindFreeAgent();
	if (pFreeAgent == NULL) {
		LOG(LOG_DEBUG, "No available Agent (Stage %d%d)", pStage->GetUID().High, pStage->GetUID().Low);
		return;
	}
	pStage->SetAgentUID(pFreeAgent->GetUID());

	CCCommand* pCmd = CreateCommand(MC_AGENT_STAGE_RESERVE, pFreeAgent->GetCommListener());
	pCmd->AddParameter(new CCCmdParamCCUID(pStage->GetUID()));
	Post(pCmd);
}

void CCMatchServer::LocateAgentToClient(const CCUID& uidPlayer, const CCUID& uidAgent)
{
	CCAgentObject* pAgent = GetAgent(uidAgent);
	if (pAgent == NULL) 
		return;

	char szCharName[64];
	CCMatchObject* pChar = GetObject(uidPlayer);
	sprintf(szCharName, "%s(%d%d)", (pChar?pChar->GetAccountName():"?"), uidPlayer.High, uidPlayer.Low);
	LOG(LOG_DEBUG, "Locate Agent : Locate Agent(%d%d) to Player %s ", uidAgent.High, uidAgent.Low, szCharName);

	static int nCount = 0;
	if( 10 > nCount++ )
	{
		cclog( "Locate Agent : Locate Agent(%d%d) to Player %s. IP(%s), Port(%d)\n"
			, uidAgent.High
			, uidAgent.Low
			, szCharName
			, pAgent->GetIP()
			, pAgent->GetUDPPort() );
	}

	CCCommand* pCmd = CreateCommand(MC_AGENT_LOCATETO_CLIENT, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamCCUID(uidAgent));
	pCmd->AddParameter(new CCCmdParamStr(pAgent->GetIP()));
	pCmd->AddParameter(new CCCmdParamInt(pAgent->GetTCPPort()));
	pCmd->AddParameter(new CCCmdParamInt(pAgent->GetUDPPort()));
	RouteToListener(pChar, pCmd);
}


void CCMatchServer::OnRegisterAgent(const CCUID& uidComm, char* szIP, int nTCPPort, int nUDPPort)
{
	LOG(LOG_PROG, "Start agent Register (ComCCUID %u:%u) IP:%s, TCPPort:%d, UDPPort:%d\n ", 
		uidComm.High, uidComm.Low, szIP, nTCPPort, nUDPPort);

	// Remove Old Connections ////////////////////////
	CCAgentObject* pOldAgent = NULL;
	while(pOldAgent=FindFreeAgent())
	{
		AgentRemove(pOldAgent->GetUID(), NULL);
	}
	//////////////////////////////////////////////////

	int nErrCode = AgentAdd(uidComm);
	if(nErrCode!=MOK)
	{
		LOG(LOG_DEBUG, MErrStr(nErrCode) );
	}

	// 패킷 시리얼 체크를 하지 않도록 한다.
	LockCommList();
	CCCommObject* pCommObj = (CCCommObject*)m_CommRefCache.GetRef(uidComm);
	if (pCommObj)
	{
		pCommObj->GetCommandBuilder()->SetCheckCommandSN(false);
	}
	UnlockCommList();

	CCAgentObject* pAgent = GetAgent(uidComm);
	pAgent->AddCommListener(uidComm);
	pAgent->SetAddr(szIP, nTCPPort, nUDPPort);

//	SetClientClockSynchronize(uidComm);
	LOG(LOG_PROG, "Agent Registered (ComCCUID %u:%u) IP:%s, TCPPort:%d, UDPPort:%d\n ", 
		uidComm.High, uidComm.Low, szIP, nTCPPort, nUDPPort);

/*	//// IOCP DEBUG RAONHAJE ///////////////////////////////////
	char* pszAlpha = "abcdefghijklmnopqrstuvwxyz";
	int nAlphaLen = strlen(pszAlpha);
	char szBulk[200];	ZeroMemory(szBulk, 200);
	for (int i=0; i<200; i++)
		szBulk[i] = pszAlpha[i%nAlphaLen];
	for (i=0; i<1000; i++) {
		char szMsg[512];
		sprintf(szMsg, "TEST_STRING=%d  (%s)", i, szBulk);

		CCCommand* pCmd = CreateCommand(MC_AGENT_DEBUGTEST, pAgent->GetCommListener());
		pCmd->AddParameter(new CCCmdParamStr(szMsg));
		Post(pCmd);
	}
	//////////////////////////////////////////////////////////// */
}

void CCMatchServer::OnUnRegisterAgent(const CCUID& uidComm)
{
	CCAgentObject* pAgent = GetAgentByComCCUID(uidComm);
	if (pAgent)
		AgentRemove(pAgent->GetUID(), NULL);

	LOG(LOG_DEBUG, "Agent Unregistered (ComCCUID %u:%u) Cleared", uidComm.High, uidComm.Low);
}
void CCMatchServer::OnAgentStageReady(const CCUID& uidCommAgent, const CCUID& uidStage)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	CCAgentObject* pAgent = GetAgentByComCCUID(uidCommAgent);
	if (pAgent == NULL) return;
	
	pStage->SetAgentReady(true);

	LOG(LOG_DEBUG, "Agent Ready to Handle Stage(%d%d)", uidStage.High, uidStage.Low);

/*	for (CCMatchObjectList::iterator i=m_Objects.begin(); i!=m_Objects.end(); i++) {
		CCMatchObject* pObj = (*i).second;
		if (pObj->GetBridgePeer() == false)
			LocateAgentToClient(pObj->GetUID(), pAgent->GetUID());
	}*/
}

void CCMatchServer::OnRequestLiveCheck(const CCUID& uidComm, unsigned long nTimeStamp, unsigned long nStageCount, unsigned long nUserCount)
{
	CCCommand* pCmd = CreateCommand(MC_MATCH_AGENT_RESPONSE_LIVECHECK, uidComm);
	pCmd->AddParameter(new CCCmdParamUInt(nTimeStamp));
	PostSafeQueue(pCmd);
}

void CCMatchServer::OnPeerReady(const CCUID& uidChar, const CCUID& uidPeer)
{
	CCMatchObject* pChar = GetObject(uidChar);
	if (pChar == NULL) return;

	CCMatchStage* pStage = FindStage(pChar->GetStageUID());
	if (pStage == NULL) return;

	LocateAgentToClient(uidChar, pStage->GetAgentUID());

	CCCommand* pCmd = CreateCommand(MC_MATCH_RESPONSE_PEER_RELAY, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamCCUID(uidPeer));
	RouteToListener(pChar, pCmd);

	static int nCount = 0;
	if( 10 > nCount++ )
	{
		cclog( "MC_AGENT_PEER_READY OK!!" );
	}
}


void CCMatchServer::OnRequestRelayPeer(const CCUID& uidChar, const CCUID& uidPeer)
{
	CCMatchObject* pChar = GetObject(uidChar);
	if (pChar == NULL) return;

	CCMatchObject* pPeer = GetObject(uidPeer);
	if (pPeer == NULL) return;

	pChar->SetRelayPeer(true);
	LOG(LOG_DEBUG, "%s Request relay peer on %s", pChar->GetName(), pPeer->GetName());

	CCMatchStage* pStage = FindStage(pChar->GetStageUID());
	if (pStage == NULL) return;

	CCAgentObject* pAgent = GetAgent(pStage->GetAgentUID());
	if (pAgent == NULL) {
		pAgent = FindFreeAgent();
		if (pAgent == NULL) {
			// Notify Agent not ready
			CCCommand* pCmd = CreateCommand(MC_AGENT_ERROR, CCUID(0,0));
			pCmd->AddParameter(new CCCmdParamInt(0));
			RouteToListener(pChar, pCmd);
			return;
		}
	}

	static int nCount = 0;
	if( 10 > nCount++ )
	{
		cclog( "%s Request relay peer on %s\n", pChar->GetName(), pPeer->GetName());
	}

	pChar->SetAgentUID(pAgent->GetUID());
	pStage->SetAgentUID(pAgent->GetUID());
	pStage->SetAgentReady(true);

	// Send Relay order to Agent
	CCCommand* pCmd = CreateCommand(MC_AGENT_RELAY_PEER, pAgent->GetCommListener());
	pCmd->AddParameter(new CCCmdParamCCUID(uidChar));
	pCmd->AddParameter(new CCCmdParamCCUID(uidPeer));
	pCmd->AddParameter(new CCCmdParamCCUID(pStage->GetUID()));
	Post(pCmd);

/*	CCCommand* pCmd2 = CreateCommand(MC_AGENT_RELAY_PEER, pAgent->GetCommListener());
	pCmd2->AddParameter(new CCCmdParamCCUID(uidPeer));
	pCmd2->AddParameter(new CCCmdParamCCUID(uidChar));
	pCmd2->AddParameter(new CCCmdParamCCUID(pStage->GetUID()));
	Post(pCmd2);*/

//	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {
//		CCMatchObject* pObj = (CCMatchObject*)(*i).second;
//		LocateAgentToClient(pObj->GetUID(), pStage->GetAgentUID());
//	}
}

