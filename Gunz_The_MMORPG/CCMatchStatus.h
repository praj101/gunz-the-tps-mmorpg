#pragma once
class CCMatchServer;

#define MATCHSTATUS_DUMP_LEN		4096


struct CMD_HISTORY_INFO
{
	CMD_HISTORY_INFO() : nCmdID( 0 ), uidSender( CCUID(0, 0) ) {}

	int		nCmdID;
	CCUID	uidSender;	// 단지 같은 유저가 보낸 커맨드인지 구별만 하기 위해서.
};


class CCMatchStatus
{
private:
	bool			m_bCreated;
	CCMatchServer*	m_pMatchServer;
protected:
	unsigned long int	m_nStartTime;
	unsigned long int	m_nTotalCommandQueueCount;
	unsigned long int	m_nTickCommandQueueCount;

#define MSTATUS_MAX_CMD_COUNT		100000
#define MSTATUS_MAX_DBQUERY_COUNT	60
#define MSTATUS_MAX_CMD_HISTORY		100

	unsigned long int	m_nCmdCount[MSTATUS_MAX_CMD_COUNT][3];
	unsigned long int	m_nDBQueryCount[MSTATUS_MAX_DBQUERY_COUNT][3];
	// unsigned long int	m_nCmdHistory[MSTATUS_MAX_CMD_HISTORY];
	CMD_HISTORY_INFO	m_CmdHistory[MSTATUS_MAX_CMD_HISTORY];
	int					m_nHistoryCursor;
	int					m_nRunStatus;
	void AddCmdHistory(unsigned long int nCmdID, const CCUID& uidSender);

	char				m_szDump[MATCHSTATUS_DUMP_LEN];
public:
	CCMatchStatus();
	virtual ~CCMatchStatus();
	static CCMatchStatus* GetInstance();
	bool Create(CCMatchServer* pMatchServer);
public:
	void SaveToLogFile();
	void AddCmdCount(unsigned long int nCmdCount)
	{
		m_nTickCommandQueueCount = nCmdCount;
		m_nTotalCommandQueueCount += m_nTickCommandQueueCount;
	}
	void AddCmd(unsigned long int nCmdID, const CCUID& uidSender, int nCount = 1, unsigned long int nTime = 0)
	{
		AddCmdHistory(nCmdID, uidSender);

		if (nCmdID >= MSTATUS_MAX_CMD_COUNT) return;
		m_nCmdCount[nCmdID][0] += nCount;
		m_nCmdCount[nCmdID][1] += nTime;
		m_nCmdCount[nCmdID][2] = nTime;
	}
	void AddDBQuery(unsigned long int nDBQueryID, unsigned long int nTime)
	{
		if (nDBQueryID >= MSTATUS_MAX_DBQUERY_COUNT) return;

		m_nDBQueryCount[nDBQueryID][0]++;
		m_nDBQueryCount[nDBQueryID][1] += nTime;

		m_nDBQueryCount[nDBQueryID][2] = nTime;
	}
	inline void SaveCmdHistory();
	void SetRunStatus(int value) { m_nRunStatus = value; }
	void SetLog(const char* szDump);
	inline void Dump();
};

inline CCMatchStatus* CCGetServerStatusSingleton() 
{
	return CCMatchStatus::GetInstance();
}

inline void CCMatchStatus::SaveCmdHistory()
{
	int t=0;
	for (int i = m_nHistoryCursor; i < MSTATUS_MAX_CMD_HISTORY; i++)
	{
		// cclog("History(%d): %u\n", t++, m_nCmdHistory[i]);
		cclog( "History. sender(%u, %u), CmdID(%d)\n"
			, m_CmdHistory[i].uidSender.High, m_CmdHistory[i].uidSender.Low
			, m_CmdHistory[i].nCmdID );
	}
	for (int i = 0; i < m_nHistoryCursor; i++)
	{
		// cclog("History(%d): %u\n", t++, m_nCmdHistory[i]);
		cclog( "History. sender(%u, %u), CmdID(%d)\n"
			, m_CmdHistory[i].uidSender.High, m_CmdHistory[i].uidSender.Low
			, m_CmdHistory[i].nCmdID );
	}

	cclog("RunStatus : %d\n", m_nRunStatus);
}

inline void CCMatchStatus::Dump()
{
	SaveCmdHistory();

//	cclog("Dump: ");
//	cclog(m_szDump);
}