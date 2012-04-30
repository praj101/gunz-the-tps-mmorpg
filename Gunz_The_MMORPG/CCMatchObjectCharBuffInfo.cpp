#include "stdafx.h"
#include "CCMatchGlobal.h"
#include "CCMatchObject.h"
#include "CCMatchObjectCharBuffInfo.h"

CCUID CCMatchObjectCharBuff::m_uidGenerate = CCUID(0,0);
CCCriticalSection CCMatchObjectCharBuff::m_csUIDGenerateLock;

CCMatchObjectCharBuff::CCMatchObjectCharBuff() : m_nLastCheckBuffInfoTime(0)
{
	FreeCharBuffInfo();
}

CCMatchObjectCharBuff::~CCMatchObjectCharBuff() 
{
}

void CCMatchObjectCharBuff::FreeCharBuffInfo()
{	
	m_ShortBuffInfoMap.Clear();
}


////////////////////////////////////////////////////////////////////////////////////////////
// Short Buff 관련
////////////////////////////////////////////////////////////////////////////////////////////
bool CCMatchObjectCharBuff::ApplyShortBuffInfo(int nBuffID, int nBuffSecondPeriod)
{
	if( MGetMatchBuffDescMgr()->GetBuffDesc(nBuffID) == NULL ) {
		_ASSERT(0);
		return false;
	}

	CCMatchCharInfo* pCharInfo = GetObject()->GetCharInfo();
	if( pCharInfo == NULL ) {
		_ASSERT(0);
		return false;
	}

	CCMatchShortBuff *pShortBuff = m_ShortBuffInfoMap.GetShortBuffByBuffID(nBuffID);
	if( pShortBuff != NULL ) {	
		m_ShortBuffInfoMap.Remove(pShortBuff->GetBuffUID());
	} 

	if( (int)m_ShortBuffInfoMap.size() >= MAX_CHARACTER_SHORT_BUFF_COUNT ) { 
		return false;
	}

	CCMatchShortBuff *pNewShortBuff = new CCMatchShortBuff;
	if( !pNewShortBuff->Set(CCMatchObjectCharBuff::UseUID(), nBuffID, timeGetTime(), nBuffSecondPeriod * 1000) ) {
		delete pNewShortBuff;
		_ASSERT(0);
		return false;
	}

	if( m_ShortBuffInfoMap.Insert(pNewShortBuff->GetBuffUID(), pNewShortBuff) == false ) {
		_ASSERT(0);
		return false;
	}

	return true;
}

void CCMatchObjectCharBuff::DeleteShortBuffInfo(CCUID& uidBuff)
{
	m_ShortBuffInfoMap.Remove(uidBuff);
}


////////////////////////////////////////////////////////////////////////////////////////////
// 본체 관련
////////////////////////////////////////////////////////////////////////////////////////////
bool CCMatchObjectCharBuff::Tick(int nGlobalTick)
{
	bool bResult = false;

	CCMatchShortBuffMap::iterator iter = m_ShortBuffInfoMap.begin();
	for( ; iter != m_ShortBuffInfoMap.end(); ) 
	{
		CCMatchShortBuff* pBuff = iter->second;
		if( pBuff->IsExpired(nGlobalTick) ) 
		{
			bResult = true;

			iter = m_ShortBuffInfoMap.erase(iter);
			delete pBuff;
			continue;
		}

		iter++;
	}

	if( bResult ) {
		MakeBuffSummary();
	}

	return bResult;
}

void CCMatchObjectCharBuff::MakeBuffSummary()
{
	CCMatchShortBuffMap::iterator iter = m_ShortBuffInfoMap.begin();
	for( ; iter != m_ShortBuffInfoMap.end(); iter++) {
		CCMatchShortBuff* pBuff = iter->second;
		if( pBuff->GetBuffID() != 0 ) {
			m_BuffSummary.AddBuff(pBuff->GetBuffID());
		}		
	}
}