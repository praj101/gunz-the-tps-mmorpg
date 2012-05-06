#pragma once

class CCDuelTournamentFormula
{
public:
	CCDuelTournamentFormula(){}
	~CCDuelTournamentFormula(){}

	int Calc_WinnerTP(int nWinnerTP, int nLoserTP, bool isFinal=false);
	int Calc_LoserTP(int nWinnerGainTP, bool isFinal=false);
};
