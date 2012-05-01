#include "stdafx.h"

#include "CCDuelTournamentTicket.h"
#include "CCDuelTournamentTicketPicker.h"

void CCDuelTournamentTicketPicker::AddTicket(CCDuelTournamentTicket *pTicket)
{
	m_TicketList.push_back(pTicket);
}

static bool CompareTicket(CCDuelTournamentTicket* left, CCDuelTournamentTicket* right) 
{
	return left->GetTP() < right->GetTP();
}

void CCDuelTournamentTicketPicker::Shuffle()
{
	if (m_TicketList.empty()) return;
/*
#define TEMP_TICKET_LIST_COUNT	5
	list <CCDuelTournamentTicket *> TempTicketList[TEMP_TICKET_LIST_COUNT];

	for(list <CCDuelTournamentTicket *>::iterator iter = m_DuelTournamentTicketList.begin(), int i = 0;
		iter != m_DuelTournamentTicketList.end() ; ++iter, ++i ){
		TempTicketList[i % TEMP_TICKET_LIST_COUNT] = (*iter);
	}

	for(int i = 0; i < TEMP_TICKET_LIST_COUNT; i++)
	{
		TempTicketList[i].sort(CompareTicket);
	}
*/
	//m_TicketList.sort(CompareTicket);
}

bool CCDuelTournamentTicketPicker::PickMatch(list<CCDuelTournamentTicket*> *pTicketList, int nPlayerCount)
{
	if( nPlayerCount == 0 ) return false;

	if( (int)m_TicketList.size() < nPlayerCount ) {
		return false;
	}

	for(int i = 0; i < nPlayerCount; i++){
		pTicketList->push_back(m_TicketList.front());
		m_TicketList.pop_front();
	}

	return true;
}