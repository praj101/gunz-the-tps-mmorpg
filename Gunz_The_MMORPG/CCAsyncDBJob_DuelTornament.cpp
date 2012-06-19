#include "stdafx.h"
#include "CCAsyncDBJob_DuelTournament.h"

void CCAsyncDBJob_GetDuelTournamentTimeStamp::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->GetDuelTournamentTimeStamp(m_szTimeStamp) )
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}

void CCAsyncDBJob_GetDuelTournamentCharInfo::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->GetDuelTournamentCharInfo(m_dwPlayerCID, &m_pDTCharInfo) ) {
		if(!pDBMgr->InsertDuelTournamentCharInfo(m_dwPlayerCID) ) {
			SetResult(CCASYNC_RESULT_FAILED);
			return;
		}

		if (!pDBMgr->GetDuelTournamentCharInfo(m_dwPlayerCID, &m_pDTCharInfo) ) {
			SetResult(CCASYNC_RESULT_FAILED);
			return;
		};
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}

void CCAsyncDBJob_UpdateDuelTournamentCharInfo::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->UpdateDuelTournamentCharacterInfo(m_dwPlayerCID, m_szTimeStamp, m_pDTCharInfo))
	{
		SetResult(CCASYNC_RESULT_FAILED);;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}

void CCAsyncDBJob_GetDuelTournamentPreviousCharInfo::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->GetDuelTournamentPreviousCharacterInfo(m_dwPlayerCID, 
		&m_nPrevTP, &m_nPrevWins, &m_nPrevLoses, &m_nPrevRanking, &m_nPrevFinalWins) )
	{
		SetResult(CCASYNC_RESULT_FAILED);;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}

void CCAsyncDBJob_UpdateDuelTournamentGameLog::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->UpdateDuelTournamentGameLog(m_szTimeStamp, m_nLogID, m_nChampionCID) )
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}

void CCAsyncDBJob_InsertDuelTournamentGameLogDetail::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->InsertDuelTournamentGameLogDetail(m_nLogID, m_szTimeStamp, (int)m_nDTRoundState, m_nPlayTime
		, m_nWinnerCID, m_nGainTP, m_nLoserCID, m_nLoseTP) )
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}

void CCAsyncDBJob_GetDuelTournamentSideRankingInfo::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->GetDuelTournamentSideRankingInfo(m_dwPlayerCID, &m_SideRankingList) )
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}

void CCAsyncDBJob_GetDuelTournamentGroupRankingInfo::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->GetDuelTournamentGroupRankingInfo(&m_GroupRankingList)) 
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}