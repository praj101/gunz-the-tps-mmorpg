#pragma once

class MDuelTournamentTicket;
class MDuelTournamentTicketPicker
{
protected:
	list <MDuelTournamentTicket *> m_TicketList;
public:
	void AddTicket(MDuelTournamentTicket *pTicket);
	void Shuffle();

	bool PickMatch(list<MDuelTournamentTicket*> *pTicketList, int nPlayerCount);
};