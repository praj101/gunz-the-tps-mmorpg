#include "stdafx.h"
#include "CCMatchAccountPenaltyInfo.h"

CCMatchAccountPenaltyInfo::CCMatchAccountPenaltyInfo()
{
	for(int i = 0; i < MPC_MAX; i++) {
		ClearPenaltyInfo((CCPenaltyCode)i);
	}	
}

CCMatchAccountPenaltyInfo::~CCMatchAccountPenaltyInfo()
{
}

void CCMatchAccountPenaltyInfo::ClearPenaltyInfo(CCPenaltyCode nCode)
{
	memset(&m_PenaltyTable[(int)nCode], 0, sizeof(CCPenaltyInfo));
}

void CCMatchAccountPenaltyInfo::SetPenaltyInfo(CCPenaltyCode nCode, int nPenaltyHour)
{
	if( nCode <= MPC_NONE || nCode >= MPC_MAX ) {
		_ASSERT(0);
		return;
	}

	SetPenaltyInfo(nCode, GetPenaltyEndDate(nPenaltyHour));
}

void CCMatchAccountPenaltyInfo::SetPenaltyInfo(CCPenaltyCode nCode, SYSTEMTIME sysPenaltyEndDate)
{
	if( nCode <= MPC_NONE || nCode >= MPC_MAX ) {
		_ASSERT(0);
		return;
	}

	m_PenaltyTable[nCode].nPenaltyCode		= nCode;
	m_PenaltyTable[nCode].sysPenaltyEndDate	= sysPenaltyEndDate;
}

SYSTEMTIME CCMatchAccountPenaltyInfo::GetPenaltyEndDate(int nPenaltyHour)
{ 
#define IN_HOUR		(__int64)10000000*60*60
#define IN_MINUTE	(__int64)10000000*60
#define IN_SECOND	(__int64)10000000

	SYSTEMTIME sysTime;
	FILETIME ft;
	LARGE_INTEGER lgInt;

	GetLocalTime(&sysTime);						// 현재 시간 구하기
	SystemTimeToFileTime(&sysTime, &ft);			// SYSTEMTIME -> FILETIME 변환
	memcpy(&lgInt, &ft, sizeof(FILETIME));		// FILETIME -> LARGE_INTEGER 변환
	lgInt.QuadPart += IN_HOUR * nPenaltyHour;	// 시간 더하기
	memcpy(&ft, &lgInt, sizeof(FILETIME));		// LARGE_INTEGER -> FILETIME 변환
	FileTimeToSystemTime(&ft, &sysTime);			// FILETIME -> SYSTEMMTIME 변환

	return sysTime;
}

bool CCMatchAccountPenaltyInfo::IsBlock(CCPenaltyCode nCode)
{
	if( nCode <= MPC_NONE || nCode >= MPC_MAX ) {
		_ASSERT(0);
		return false;
	}

	if( m_PenaltyTable[nCode].nPenaltyCode == MPC_NONE ) {
		return false;
	}

	SYSTEMTIME stLocal;
	GetLocalTime( &stLocal );


	if( m_PenaltyTable[nCode].sysPenaltyEndDate.wYear < stLocal.wYear )			return false;
	else if( m_PenaltyTable[nCode].sysPenaltyEndDate.wYear > stLocal.wYear )	return true;

	if( m_PenaltyTable[nCode].sysPenaltyEndDate.wMonth < stLocal.wMonth )		return false;
	else if( m_PenaltyTable[nCode].sysPenaltyEndDate.wMonth > stLocal.wMonth )	return true;

	if( m_PenaltyTable[nCode].sysPenaltyEndDate.wDay < stLocal.wDay )			return false;
	else if( m_PenaltyTable[nCode].sysPenaltyEndDate.wDay > stLocal.wDay )		return true;

	if( m_PenaltyTable[nCode].sysPenaltyEndDate.wHour < stLocal.wHour )			return false;
	else if( m_PenaltyTable[nCode].sysPenaltyEndDate.wHour > stLocal.wHour )	return true;

	if( m_PenaltyTable[nCode].sysPenaltyEndDate.wMinute < stLocal.wMinute )		return false;

	return true;
}
