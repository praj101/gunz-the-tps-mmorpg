#include "stdafx.h"
#include "CCDuelTournamentTimeChecker.h"

CCDuelTournamentTimeChecker::CCDuelTournamentTimeChecker()
{
	memset(m_szTimeStamp, 0, DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1);

	m_CurrTime = 0;
	m_nCurrMonth = 0;
	m_nCurrDay = 0;

	m_bTimeStampChanged = false;
}

void CCDuelTournamentTimeChecker::Tick(unsigned int nTick)
{
	CTime nCurrTime = CTime::GetCurrentTime();

	if( nCurrTime > m_CurrTime){
		int nMonth  = nCurrTime.GetMonth();
		int nDay    = nCurrTime.GetDay();
		int nHour   = nCurrTime.GetHour();
		int nMinute = nCurrTime.GetMinute();

		if( nMonth > m_nCurrMonth ){	///< ÇÏ·ç°¡ Èê·¶À»²¨´Ù..
			if( nMinute == 10 ) {
				CCMatchServer::GetInstance()->LOG(CCMatchServer::LOG_PROG, 
					"CCDuelTournamentTimeChecker::Current Month - %d, Day - %d", nMonth, nDay);

				CCMatchServer::GetInstance()->OnAsyncRequest_GetDuelTournamentTimeStamp();

				m_CurrTime = nCurrTime;
				m_nCurrMonth = nMonth;
				m_nCurrDay = nDay;
			}
		} else if( nDay > m_nCurrDay ){	///< ÇÏ·ç°¡ Èê·¶´Ù..
			if( nMinute == 10 ) {
				CCMatchServer::GetInstance()->LOG(CCMatchServer::LOG_PROG, 
					"CCDuelTournamentTimeChecker::Current Month - %d, Day - %d", nMonth, nDay);

				CCMatchServer::GetInstance()->OnAsyncRequest_GetDuelTournamentTimeStamp();

				m_CurrTime = nCurrTime;
				m_nCurrMonth = nMonth;
				m_nCurrDay = nDay;
			}
		}
	}
}

void CCDuelTournamentTimeChecker::SetTimeStamp(const char* szTimeStamp)
{
	memcpy(m_szTimeStamp, szTimeStamp, DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1);
}

bool CCDuelTournamentTimeChecker::IsSameTimeStamp(const char* szTimeStamp)
{
	if( strcmp(m_szTimeStamp, szTimeStamp) == 0 ) return true;
	return false;
}