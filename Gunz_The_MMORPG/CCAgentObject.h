#pragma once

#include "CCObject.h"
#include <windows.h>


class CCAgentObject : public CCObject {
protected:
	char 			m_szIP[64];
	unsigned int	m_nTCPPort;
	unsigned int	m_nUDPPort;

public:
	CCAgentObject() {}
	CCAgentObject(const CCUID& uid) : CCObject(uid) { 
		ZeroMemory(m_szIP, sizeof(char)*64);	
		m_nTCPPort=0;
		m_nUDPPort=0;
	}
	virtual ~CCAgentObject() {};

	void SetAddr(char* szIP, unsigned short nTCPPort, unsigned short nUDPPort)	{ 
		strcpy(m_szIP, szIP); m_nTCPPort = nTCPPort; m_nUDPPort = nUDPPort;
	}
	char* GetIP()					{ return m_szIP; }
	unsigned short GetTCPPort()		{ return m_nTCPPort; }
	unsigned short GetUDPPort()		{ return m_nUDPPort; }

	int GetStageCount()			{ return 0; }
	int GetAssignCount()		{ return 0; }
};


class CCAgentObjectMap : public map<CCUID, CCAgentObject*>{};
