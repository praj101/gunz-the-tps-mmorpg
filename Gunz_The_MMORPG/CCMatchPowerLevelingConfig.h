#pragma once

class CCMatchPowerLevelingConfig
{
private :
	friend class CCMatchConfig;

	bool	m_IsUsePowerLevelingDBBlock;
	DWORD	m_dwPowerLevelingDBBlockTime;

private :
	void UsePowerLevelingDBBlock() { m_IsUsePowerLevelingDBBlock = true; }
	void SetPowerLevelingDBBlockTime( const DWORD dwTime ) { m_dwPowerLevelingDBBlockTime = dwTime * 60000; }


public :
	CCMatchPowerLevelingConfig()
	{
		m_IsUsePowerLevelingDBBlock		= false;
		m_dwPowerLevelingDBBlockTime	= 0;
	}

	bool IsUsePowerLevelingDBBlock() const		{ return m_IsUsePowerLevelingDBBlock; }
	DWORD GetPowerLevelingDBBlockTime() const	{ return m_dwPowerLevelingDBBlockTime; }
};
