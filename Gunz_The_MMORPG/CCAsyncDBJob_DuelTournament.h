//////////////////////////////////////////////////////////////////////////////////////////////
// Added By Hong KiJu
#pragma once

#ifndef _MASYNCDBJOB_DUELTOURNAMENT
#define _MASYNCDBJOB_DUELTOURNAMENT

#include "CCAsyncDBJob.h"


class CCAsyncDBJob_GetDuelTournamentTimeStamp : public CCAsyncJob
{
protected:
	char m_szTimeStamp[DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1];

public:
	CCAsyncDBJob_GetDuelTournamentTimeStamp() : CCAsyncJob(CCASYNCJOB_GET_DUELTOURNAMENT_TIMESTAMP, CCUID(0, 0)) {}
	~CCAsyncDBJob_GetDuelTournamentTimeStamp(){}


	void Input() {}
	virtual void Run(void* pContext);
	char* GetTimeStamp(){ return m_szTimeStamp; }
};

class CCAsyncDBJob_GetDuelTournamentCharInfo : public CCAsyncJob
{
protected:
	DWORD m_dwPlayerCID;

	CCUID m_uidPlayer;
	CCMatchObjectDuelTournamentCharInfo m_pDTCharInfo;
public:
	CCAsyncDBJob_GetDuelTournamentCharInfo() : CCAsyncJob(CCASYNCJOB_GET_DUELTOURNAMENT_CHARINFO, CCUID(0, 0)) {
		m_dwPlayerCID = 0;
		m_uidPlayer = CCUID(0, 0);
	}
	~CCAsyncDBJob_GetDuelTournamentCharInfo(){}

	void Input(CCUID uidPlayer, DWORD dwPlayerCID) {
		m_uidPlayer = uidPlayer;
		m_dwPlayerCID = dwPlayerCID;		
	}

	virtual void Run(void* pContext);

	CCUID GetPlayerUID()									{ return m_uidPlayer; }
	CCMatchObjectDuelTournamentCharInfo* GetDTCharInfo()	{ return &m_pDTCharInfo; }	
};

class CCAsyncDBJob_GetDuelTournamentPreviousCharInfo : public CCAsyncJob
{
protected:
	CCUID m_uidPlayer;
	DWORD m_dwPlayerCID;	

	int m_nPrevTP;
	int m_nPrevWins;
	int m_nPrevLoses;
	int m_nPrevRanking;
	int m_nPrevFinalWins;
public:
	CCAsyncDBJob_GetDuelTournamentPreviousCharInfo() : CCAsyncJob(CCASYNCJOB_GET_DUELTOURNAMENT_PREVIOUS_CHARINFO, CCUID(0, 0)) {
		m_dwPlayerCID = 0;
		m_uidPlayer = CCUID(0, 0);

		m_nPrevTP = 0;
		m_nPrevWins = 0;
		m_nPrevLoses = 0;
		m_nPrevRanking = 0;
		m_nPrevFinalWins = 0;
	}
	~CCAsyncDBJob_GetDuelTournamentPreviousCharInfo(){}


	void Input(CCUID uidPlayer, DWORD dwPlayerCID) {
		m_uidPlayer = uidPlayer;
		m_dwPlayerCID = dwPlayerCID;		
	}

	virtual void Run(void* pContext);
	CCUID GetPlayerUID()					{ return m_uidPlayer; }

	int GetPrevTP()						{ return m_nPrevTP; }
	int GetPrevWins()					{ return m_nPrevWins; }
	int GetPrevLoses()					{ return m_nPrevLoses; }
	int GetPrevRanking()				{ return m_nPrevRanking; }
	int GetPrevFinalWins()				{ return m_nPrevFinalWins; }


};

class CCAsyncDBJob_UpdateDuelTournamentCharInfo : public CCAsyncJob
{
protected:
	DWORD m_dwPlayerCID;
	char m_szTimeStamp[DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1];
	CCMatchObjectDuelTournamentCharInfo *m_pDTCharInfo;

public:
	CCAsyncDBJob_UpdateDuelTournamentCharInfo() : CCAsyncJob(CCASYNCJOB_UPDATE_DUELTOURNAMENT_CHARINFO, CCUID(0, 0)) {
		m_dwPlayerCID = 0;
		m_pDTCharInfo = NULL;
		ZeroMemory(m_szTimeStamp, DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1);
	}
	~CCAsyncDBJob_UpdateDuelTournamentCharInfo(){
		if( m_pDTCharInfo != NULL ) {
			delete m_pDTCharInfo;
		}
	}

	void Input(DWORD dwPlayerCID, char* szTimeStamp, CCMatchObjectDuelTournamentCharInfo *pDTCharInfo) {
		m_dwPlayerCID = dwPlayerCID;
		m_pDTCharInfo = pDTCharInfo;

		strcpy(m_szTimeStamp, szTimeStamp);
		m_pDTCharInfo = new CCMatchObjectDuelTournamentCharInfo(pDTCharInfo);
	}

	virtual void Run(void* pContext);
};


class CCAsyncDBJob_UpdateDuelTournamentGameLog : public CCAsyncJob
{
protected:
	int m_nLogID;
	int m_nChampionCID;
	char m_szTimeStamp[DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1];

public:
	CCAsyncDBJob_UpdateDuelTournamentGameLog() : CCAsyncJob(CCASYNCJOB_UPDATE_DUELTOURNAMENT_GAMELOG, CCUID(0, 0)) {
		memset(m_szTimeStamp, 0, DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1);
	}
	~CCAsyncDBJob_UpdateDuelTournamentGameLog(){}


	void Input(char* szTimeStamp, int nLogID, int nChampionCID) {
		m_nLogID = nLogID;
		m_nChampionCID = nChampionCID;
		strcpy(m_szTimeStamp, szTimeStamp);
	}

	virtual void Run(void* pContext);
};


class CCAsyncDBJob_InsertDuelTournamentGameLogDetail : public CCAsyncJob
{
protected:
	int m_nLogID;
	char m_szTimeStamp[DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1];

	CCDUELTOURNAMENTROUNDSTATE m_nDTRoundState;
	int m_nPlayTime;
	
	int m_nWinnerCID;
	int m_nLoserCID;
	int m_nGainTP;
	int m_nLoseTP;

public:
	CCAsyncDBJob_InsertDuelTournamentGameLogDetail() : CCAsyncJob(CCASYNCJOB_INSERT_DUELTOURNAMENT_GAMELOGDETAIL, CCUID(0, 0)) {
		memset(m_szTimeStamp, 0, DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1);
	}
	~CCAsyncDBJob_InsertDuelTournamentGameLogDetail(){}

	void Input(int nLogID, char* szTimeStamp, CCDUELTOURNAMENTROUNDSTATE nDTRoundState, int nWinnerCID, int nLoserCID, int nGainTP, int nLoseTp, int nPlayTime) {
		m_nLogID = nLogID;
		strcpy(m_szTimeStamp, szTimeStamp);

		m_nDTRoundState = nDTRoundState;
		m_nPlayTime = nPlayTime;

		m_nWinnerCID = nWinnerCID;
		m_nLoserCID = nLoserCID;
		m_nGainTP = nGainTP;
		m_nLoseTP = nLoseTp;
	}

	virtual void Run(void* pContext);
};

class CCAsyncDBJob_GetDuelTournamentSideRankingInfo : public CCAsyncJob
{
protected:
	DWORD m_dwPlayerCID;

	CCUID m_uidPlayer;
	list<DTRankingInfo*> m_SideRankingList;
public:
	CCAsyncDBJob_GetDuelTournamentSideRankingInfo() : CCAsyncJob(CCASYNCJOLB_GET_DUELTOURNAMENT_SIDERANKING, CCUID(0, 0)) {
		m_dwPlayerCID = 0;
		m_uidPlayer = CCUID(0, 0);
	}

	~CCAsyncDBJob_GetDuelTournamentSideRankingInfo()
	{
		RemoveListAll();
	}


	void Input(CCUID uidPlayer, DWORD dwPlayerCID) {
		m_uidPlayer = uidPlayer;
		m_dwPlayerCID = dwPlayerCID;		
	}

	virtual void Run(void* pContext);

	CCUID GetPlayerUID()							{ return m_uidPlayer; }
	list<DTRankingInfo*>* GetSideRankingList()	{ return &m_SideRankingList; }	

	void RemoveListAll()
	{
		list<DTRankingInfo*>::iterator iter = m_SideRankingList.begin();
		for(; iter != m_SideRankingList.end(); ){
			DTRankingInfo *pInfo = (*iter);
			delete pInfo;

			iter = m_SideRankingList.erase(iter);			
		}
	}
};


class CCAsyncDBJob_GetDuelTournamentGroupRankingInfo : public CCAsyncJob
{
protected:
	list<DTRankingInfo*> m_GroupRankingList;

public:
	CCAsyncDBJob_GetDuelTournamentGroupRankingInfo() : CCAsyncJob(CCASYNCJOB_GET_DUELTOURNAMENT_GROUPRANKING, CCUID(0, 0)) 
	{}

	~CCAsyncDBJob_GetDuelTournamentGroupRankingInfo()
	{
		RemoveListAll();
	}


	void Input() {}

	virtual void Run(void* pContext);

	list<DTRankingInfo*>* GetGroupRankingList()	{ return &m_GroupRankingList; }	

	void RemoveListAll()
	{
		list<DTRankingInfo*>::iterator iter = m_GroupRankingList.begin();
		for(; iter != m_GroupRankingList.end(); ){
			DTRankingInfo *pInfo = (*iter);
			iter = m_GroupRankingList.erase(iter);

			delete pInfo;
		}
	}
};
#endif