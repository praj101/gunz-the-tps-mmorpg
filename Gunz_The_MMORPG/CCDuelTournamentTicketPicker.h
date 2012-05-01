#pragma once

class CCDuelTournamentTicket;
class CCDuelTournamentTicketPicker
{
protected:
	list <CCDuelTournamentTicket *> m_TicketList;
public:
	void AddTicket(CCDuelTournamentTicket *pTicket);
	void Shuffle();

	bool PickMatch(list<CCDuelTournamentTicket*> *pTicketList, int nPlayerCount);
};