#pragma once
#include "CCDuelTournamentTicket.h"

#include <list>
#include <map>
using namespace std;

#include "CCUID.h"
#include "CCMatchGlobal.h"

class MDuelTournamentWaitGroup
{
protected:
	CCDUELTOURNAMENTTYPE		m_nType;
	map<CCUID, MDuelTournamentTicket*> m_PlayerTicketMap;

	unsigned int m_nRegTime;
	int m_nTickCount;

public:
	MDuelTournamentWaitGroup(CCDUELTOURNAMENTTYPE nType, unsigned int nRegTime)
	{ 
		m_nType = nType;
		m_nRegTime = nRegTime;

		m_nTickCount = 0;
	}

	CCDUELTOURNAMENTTYPE GetDuelTournamentType()				{ return m_nType; }
	void SetDuelTournamentType(CCDUELTOURNAMENTTYPE nType)		{ m_nType = nType; }

	unsigned int GetRegTime()		{ return m_nRegTime; }	
	int GetTickCount()				{ return m_nTickCount; }

	size_t GetPlayerTicketCount()							{ return m_PlayerTicketMap.size(); }
	map<CCUID, MDuelTournamentTicket*>* GetPlayerTicketMap()	{ return &m_PlayerTicketMap; }

	void AddPlayerTicket(CCUID uidPlayer, MDuelTournamentTicket *pTicket) 
	{
		m_PlayerTicketMap.insert(pair<CCUID, MDuelTournamentTicket*>(uidPlayer, pTicket));
	}

	MDuelTournamentTicket* FindPlayerTicket(CCUID uidPlayer) 
	{
		map<CCUID, MDuelTournamentTicket*>::iterator iter = m_PlayerTicketMap.find(uidPlayer);
		if( iter == m_PlayerTicketMap.end() )
			return NULL;

		return (*iter).second;
	}

	void RemovePlayerTicket(CCUID uidPlayer) 
	{
		map<CCUID, MDuelTournamentTicket*>::iterator iter = m_PlayerTicketMap.find(uidPlayer);
		if( iter == m_PlayerTicketMap.end() ) 
			return;

		m_PlayerTicketMap.erase(iter);
	}

	void UpdateTick() { 
		m_nTickCount++; 
	}
};


class MDuelTournamentPickedGroup : public vector<CCUID>
{
public:
	void Shuffle() {
		int numElem = (int)size();
		int k = 0;
		CCUID temp;
		for (int i=0; i<numElem; ++i) {
			k = rand() % numElem;
			temp = (*this)[i];
			(*this)[i] = (*this)[k];
			(*this)[k] = temp;
		}
	}
};
