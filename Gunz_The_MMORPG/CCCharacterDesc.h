#pragma once
#include "winsock2.h"
#include "CCObjectTypes.h"
#include "CCXml.h"

#include <map>
using namespace std;

#define FILENAME_CHARACTER_DESC		"CharInitialData.xml"

class CCCharacterDesc
{
private:
protected:
	char					m_szName[64];
	int						m_nDescID;
	CCCharacterBasicAttr		m_BasicAttr;
public:
	CCCharacterDesc(int id, char* name);
	virtual ~CCCharacterDesc();

	CCCharacterBasicAttr*	GetDesc() { return &m_BasicAttr; }
	int						GetID()   { return m_nDescID; }
	const char*				GetName() { return m_szName; }

	void SetDesc(CCCharacterBasicAttr* pAttr) { m_BasicAttr = *pAttr; }
};

class CCCharacterDescManager : public map<int, CCCharacterDesc*>
{
private:
protected:

public:
	CCCharacterDescManager();
	virtual ~CCCharacterDescManager();
	bool Read(const char* szFileName);
	void Clear();
	CCCharacterDesc* Find(int nDescID);
};

extern CCCharacterDescManager	g_CharacterDescManager;