#pragma once
#include "CCBaseLocale.h"
#include "CCNJ_DBAgentClient.h"
#include "CCUID.h"

class CCMatchLocale : public CCBaseLocale
{
protected:
	virtual bool			OnInit();
	CCNJ_DBAgentClient*		m_pDBAgentClient;		///< 일본넷마블에서만 사용하는 클라이언트 소켓
	bool					m_bCheckAntiHackCrack;
public:
							CCMatchLocale();
	virtual					~CCMatchLocale();
	static CCMatchLocale*	GetInstance();

	bool					ConnectToDBAgent();
	bool					PostLoginInfoToDBAgent(const CCUID&		uidComm, 
												   const char*		szCN, 
												   const char*		szPW, 
												   bool				bFreeLoginIP, 
												   unsigned long	nChecksumPack, 
												   int				nTotalUserCount);
//	bool					SkipCheckAntiHackCrack();	///< XTrap Crack을 체크할지 여부
};

inline CCMatchLocale* CCGetLocale()
{
	return CCMatchLocale::GetInstance();
}