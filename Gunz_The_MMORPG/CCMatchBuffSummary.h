#pragma once
#include "CCMatchBuff.h"

class CCMatchBuffSummary
{
protected:
	CCMatchBuffInfo m_BuffSummary[MMBET_END];

	bool m_bCleared;
public:
	CCMatchBuffSummary();
	~CCMatchBuffSummary();

	void Clear();
	void AddBuff(int nBuffID);

	CCMatchBuffInfo* GetBuffSummary(CCMatchBuffEffectType nType) { return &m_BuffSummary[(int)nType]; }

	int GetHP();
	int GetAP();

	float GetSpeedRatio(float fDefault);

	int	GetRespawnTime(int nDefault);

	int GetDoteHP();
	int GetDoteAP();
};
