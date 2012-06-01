#include "stdafx.h"
#include "CCMatchRuleDuel.h"
#include "CCMatchTransDataType.h"
#include "CCBlobArray.h"
#include "CCMatchServer.h"

#include <algorithm>
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
// CCMatchRuleDuel	   ///////////////////////////////////////////////////////////
CCMatchRuleDuel::CCMatchRuleDuel(CCMatchStage* pStage) : CCMatchRule(pStage)
{

}

void CCMatchRuleDuel::OnBegin()
{
	uidChampion = CCUID(0, 0);
	uidChallenger = CCUID(0, 0);

	CCMatchStage* pStage = GetStage();

	WaitQueue.clear();	// 대기 큐를 비우고

	if (pStage != NULL)
	{
		for(CCUIDRefCache::iterator itor=pStage->GetObjBegin(); itor!=pStage->GetObjEnd(); itor++) 
			WaitQueue.push_back((*itor).first);			// 플레이어들 그냥 몽땅 대기 큐에 넣는다.

//		SpawnPlayers();
	}

	nVictory = 0;

	return;
}

void CCMatchRuleDuel::OnEnd()
{
}


void CCMatchRuleDuel::OnRoundBegin()
{
	isRoundEnd = false;
	isTimeover = true;

	SpawnPlayers();
	SendQueueInfo(true);
	// 왜그런지 몰라도 옵저버 해야 할 놈이 스폰된걸 봐서 -_- 죽여버린다. 서버에선 스폰안됐으니 더이상의 처리는 필요없을듯.
	// 이거 한 후에 스폰되는거면 낭팬데;
	for (list<CCUID>::iterator i = WaitQueue.begin(); i!=WaitQueue.end();  ++i)
		CCMatchServer::GetInstance()->OnDuelSetObserver(*i);							

}


void CCMatchRuleDuel::OnRoundEnd()
{
	if (isTimeover)
	{	
		WaitQueue.push_back(uidChampion);
		WaitQueue.push_back(uidChallenger);
		uidChampion = uidChallenger = CCUID(0, 0);
		nVictory = 0;
	}
	else
	{
		if (isChangeChampion || uidChampion == CCUID(0, 0))				// 챔피온이 바뀌어야 하면 일단 챔피온과 도전자를 스왑
		{
			CCUID uidTemp;
			uidTemp = uidChampion;
			uidChampion = uidChallenger;
			uidChallenger = uidTemp;
		}

		if (uidChallenger != CCUID(0, 0))
		{
			WaitQueue.push_back(uidChallenger);	// 도전자는 큐의 맨 뒤로 밀어넣고
			uidChallenger = CCUID(0, 0);			// 도전자의 id를 무효화
		}
	}

//	SpawnPlayers();
	LogInfo();
}

bool CCMatchRuleDuel::RoundCount() 
{
	if (m_pStage == NULL) return false;

	int nTotalRound = m_pStage->GetStageSetting()->GetRoundMax();
	m_nRoundCount++;

	CCMatchStage* pStage = GetStage();
	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
	{
		CCMatchObject* pObj = (CCMatchObject*)(*i).second;
		if (pObj->GetEnterBattle() == false) continue;

		if (pObj->GetAllRoundKillCount() >= (unsigned int)pStage->GetStageSetting()->GetRoundMax())
		{
			return false;
		}
	}

	return true;
}

bool CCMatchRuleDuel::OnCheckRoundFinish()
{
	if (!isRoundEnd)
		return false;
	else
	{
		isRoundEnd = false;
		isTimeover = false;
		return true;	
	}
}

void CCMatchRuleDuel::OnRoundTimeOut()
{
	SetRoundArg(CCMATCH_ROUNDRESULT_DRAW);
}


void CCMatchRuleDuel::OnGameKill(const CCUID& uidAttacker, const CCUID& uidVictim)
{
	isRoundEnd = true;

	/// 크래쉬로 인한 방어 코드 추가(출력에서 0x00000000 이값으로 에러가 났기때문에 의심가는 부분을 걸러준다.)
	if( CCMatchServer::GetInstance()->GetChannelMap()->Find(m_pStage->GetOwnerChannel()) == NULL ) 
	{
		cclog( "error: can't find OwnerChannel [ CCMatchRuleDuel::OnGameKill() ]\n" );
		//LOG(LOG_FILE, "error: can't find OwnerChannel [ CCMatchRuleDuel::OnGameKill() ]\n" );
		return;
	}
	if( m_pStage == NULL)
	{
		cclog( "error: can't find m_pStage [ CCMatchRuleDuel::OnGameKill() ]\n" );
		//LOG(LOG_FILE, "error: can't find m_pStage [ CCMatchRuleDuel::OnGameKill() ]\n" );
		return;
	}

	CCUID chanID = CCMatchServer::GetInstance()->GetChannelMap()->Find(m_pStage->GetOwnerChannel())->GetUID();

	if (uidVictim == uidChallenger)		// 챔피온이 잡았으면 챔피온 유지
	{
		isChangeChampion = false;
		nVictory++;

		if (m_pStage == NULL) return;
		if (m_pStage->IsPrivate()) return;		// 비밀방이면 방송 패스

		if (nVictory % 10 != 0) return;			// 연승수가 10의 배수일때만

		CCMatchObject* pChamp;
		pChamp = m_pStage->GetObj(uidChampion);
		if (pChamp == NULL) return;


		CCMatchServer::GetInstance()->BroadCastDuelRenewVictories(
			chanID,
			pChamp->GetName(), 
			CCMatchServer::GetInstance()->GetChannelMap()->Find(m_pStage->GetOwnerChannel())->GetName(), 
			m_pStage->GetIndex()+1,
			nVictory
			);
	}
	else
	{
		isChangeChampion = true;

		int nowVictory = nVictory;

		nVictory = 1;

		if (nowVictory < 10) return;				// 10연승 이상을 저지했을때만
		if (m_pStage == NULL) return;
		if (m_pStage->IsPrivate()) return;		// 비밀방이면 방송 패스
	
		CCMatchObject* pChamp, *pChallenger;
		pChamp = m_pStage->GetObj(uidChampion);
		if (pChamp == NULL) return;
		pChallenger = m_pStage->GetObj(uidChallenger);
		if (pChallenger == NULL) return;

		if (strcmp(m_pStage->GetPassword(), "") != 0) return;

		CCMatchServer::GetInstance()->BroadCastDuelInterruptVictories(
			chanID,
			pChamp->GetName(),
			pChallenger->GetName(),
			nowVictory
			);
	}


	LogInfo();

}

void CCMatchRuleDuel::OnEnterBattle(CCUID& uidChar)
{
	if ((uidChar != uidChampion) && (uidChar != uidChallenger) && (find(WaitQueue.begin(), WaitQueue.end(), uidChar) == WaitQueue.end()))
	{
		WaitQueue.push_back(uidChar);
		SpawnPlayers();
	}
	SendQueueInfo();
	LogInfo();
}

void CCMatchRuleDuel::OnLeaveBattle(CCUID& uidChar)
{
	if (uidChar == uidChampion)
	{
		isChangeChampion = true;
		isRoundEnd = true;
		uidChampion = CCUID(0, 0);
		nVictory = 0;
	}
	else if (uidChar == uidChallenger)
	{
		isChangeChampion = false;
		isRoundEnd = true;
		uidChallenger = CCUID(0, 0);
	}
	else
	{
		WaitQueue.remove(uidChar);
		SendQueueInfo();
		LogInfo();
	}
}

void CCMatchRuleDuel::SpawnPlayers()
{
	if (uidChampion == CCUID(0, 0))
	{
		if (!WaitQueue.empty())
		{
			uidChampion = WaitQueue.front();
			WaitQueue.pop_front();
		}
	}
	if (uidChallenger == CCUID(0, 0))
	{
		if (!WaitQueue.empty())
		{
			uidChallenger = WaitQueue.front();
			WaitQueue.pop_front();
		}
	}
}

bool CCMatchRuleDuel::OnCheckEnableBattleCondition()
{
	if (uidChampion == CCUID(0, 0) || uidChallenger == CCUID(0, 0))
	{
		if (WaitQueue.empty())
			return false;
		else
			isRoundEnd = true;
	}

	return true;
}

void CCMatchRuleDuel::LogInfo()
{
#ifdef _DEBUG
	if (m_pStage == NULL) return;
	CCMatchObject* pObj;
	char buf[250];
	sprintf(buf, "Logging Que--------------------\n");
	OutputDebugString(buf);

	pObj = m_pStage->GetObj(uidChampion);
	if (pObj != NULL)
	{
		sprintf(buf, "Champion name : %s \n", pObj->GetName());
		OutputDebugString(buf);
	}

	pObj = m_pStage->GetObj(uidChallenger);
	if (pObj != NULL)
	{
		sprintf(buf, "Challenger name : %s \n", pObj->GetName());
		OutputDebugString(buf);
	}

	int x = 0;
	for (list<CCUID>::iterator i = WaitQueue.begin(); i!=WaitQueue.end();  ++i)
	{
		pObj = m_pStage->GetObj(*i);
		if (pObj != NULL)
		{
			sprintf(buf, "Wait Queue #%d : %s \n", x, pObj->GetName());
			OutputDebugString(buf);		
			x++;
		}
	}
#endif
}

void CCMatchRuleDuel::SendQueueInfo(bool isRoundEnd)
{
	if (m_pStage == NULL) return;
	CCTD_DuelQueueInfo QInfo;
	QInfo.m_uidChampion = uidChampion;
	QInfo.m_uidChallenger = uidChallenger;
	QInfo.m_nQueueLength = static_cast<char>(WaitQueue.size());
	QInfo.m_nVictory = nVictory;
	QInfo.m_bIsRoundEnd = isRoundEnd;

	int i=0;
	list<CCUID>::const_iterator itEnd = WaitQueue.end();
	for (list<CCUID>::iterator iter = WaitQueue.begin(); iter != itEnd; ++iter, ++i)
	{
		if( 14 > i )
		{
			QInfo.m_WaitQueue[i] = *iter;
		}
		else
		{
			cclog( "duel queue info index error. size : %u\n", WaitQueue.size() );
			break;
		}
	}

	CCMatchServer::GetInstance()->OnDuelQueueInfo(m_pStage->GetUID(), QInfo);
}