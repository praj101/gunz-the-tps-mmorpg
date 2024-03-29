#pragma once

//////////////////////////////////////////////////////////////////////////
// CCMatchObjectCommandHistory
// 각 MatchObject 에 대해 커맨드를 일정시간, 일정개수 보관하고 있다.
// 필요한 시점에 덤프해보기 위한 자료이다

const int MAX_COMMAND_HISTORY_COUNT = 50;			///< 보관 개수 30개
const int MAX_HISTORY_HOLD_TIME		= 10*60*1000;	///< 최대 보관시간 10분

struct MOBJECTCOMMANDHISTORY {
	string			m_strName;	///< 캐릭터 이름
	unsigned int	m_nCID;		///< 캐릭터 ID
	list < pair < int, DWORD > >	m_commands;	///< command id 와 추가된 시간
	DWORD			m_dwLastTime;	///< 마지막 커맨드가 추가된 시간
};

class CCMatchObjectCommandHistory {
public:
	~CCMatchObjectCommandHistory();

	void SetCharacterInfo(CCUID uid, const char* szName, unsigned int nCID );
	
	void PushCommand(CCUID uid, int nCommandID, DWORD dwCurrentTime, bool* pbFloodingSuspect = NULL);
	void Update( DWORD dwCurrentTime );
	unsigned int GetObjectCount();

	MOBJECTCOMMANDHISTORY* GetCommandHistory(CCUID uid);
	bool Dump(CCUID uid);

private:
	typedef map< CCUID, MOBJECTCOMMANDHISTORY* > UIDHISTORYMAP;

	UIDHISTORYMAP m_mapHistories;

	MOBJECTCOMMANDHISTORY* AddNew(CCUID uid); 
};

inline unsigned int CCMatchObjectCommandHistory::GetObjectCount() { return (unsigned int)m_mapHistories.size(); }
