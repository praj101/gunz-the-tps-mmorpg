#include "stdafx.h"
#include "CCMatchBuffSummary.h"

CCMatchBuffSummary::CCMatchBuffSummary(void)
{
	Clear();
}

CCMatchBuffSummary::~CCMatchBuffSummary(void)
{
}

void CCMatchBuffSummary::Clear()
{	
	memset(&m_BuffSummary, 0, sizeof(CCMatchBuffInfo) * MMBET_END);
	m_bCleared = true;
}

void CCMatchBuffSummary::AddBuff(int nBuffID)
{
	CCMatchBuffDesc* pDesc = MGetMatchBuffDescMgr()->GetBuffDesc(nBuffID);
	if( pDesc == NULL ) {
		_ASSERT(0);
		return;
	}

	int nIndex = pDesc->m_nBuffEffectType.Ref();
	if( nIndex >= MMBET_END ) {
		_ASSERT(0);
		return;
	}

	if( m_bCleared ) {
		m_BuffSummary[nIndex] = pDesc->m_pBuffInfo->Ref();
		m_bCleared = false;
	} else {
		m_BuffSummary[nIndex] = m_BuffSummary[nIndex] + pDesc->m_pBuffInfo->Ref();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Normal Buff
/////////////////////////////////////////////////////////////////////////////////////////////
int CCMatchBuffSummary::GetHP()
{
	return m_BuffSummary[MMBET_NORMAL].nHP;
}

int CCMatchBuffSummary::GetAP()
{
	return m_BuffSummary[MMBET_NORMAL].nAP;
}

float CCMatchBuffSummary::GetSpeedRatio(float fDefault)
{
	float fRatio = 1.0f;

	if( m_BuffSummary[MMBET_NORMAL].fSpeed_Ratio > 0.0f )
		fRatio = m_BuffSummary[MMBET_NORMAL].fSpeed_Ratio;

	return fDefault * fRatio;
}

int	CCMatchBuffSummary::GetRespawnTime(int nDefault)
{
	int nResult = nDefault - m_BuffSummary[MMBET_NORMAL].nRespawnDecTime;

	if( nResult < RESPAWN_DELAYTIME_AFTER_DYING_MIN )		return RESPAWN_DELAYTIME_AFTER_DYING_MIN;
	else if( nResult > RESPAWN_DELAYTIME_AFTER_DYING_MAX )	return RESPAWN_DELAYTIME_AFTER_DYING_MAX;
	else {
		return nResult;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Dote Buff
/////////////////////////////////////////////////////////////////////////////////////////////
int CCMatchBuffSummary::GetDoteHP()
{
	return m_BuffSummary[MMBET_DOTE].nHP;
}

int CCMatchBuffSummary::GetDoteAP()
{
	return m_BuffSummary[MMBET_DOTE].nAP;
}