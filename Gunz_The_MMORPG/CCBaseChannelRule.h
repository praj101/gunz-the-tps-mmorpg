#pragma once
#include "CCXmlParser.h"
#include <list>
#include <map>
#include <set>
#include <string>
#include <algorithm>
using namespace std;



enum CCCHANNEL_RULE {
	CCCHANNEL_RULE_NOVICE=0,
	CCCHANNEL_RULE_NEWBIE,
	CCCHANNEL_RULE_ROOKIE,
	CCCHANNEL_RULE_MASTERY,
	CCCHANNEL_RULE_ELITE,
	CCCHANNEL_RULE_CHAMPION,
	CCCHANNEL_RULE_QUEST,
	CCCHANNEL_RULE_DUELTOURNAMENT,
	CCCHANNEL_RULE_SET1,									// 예비용 채널 1
	CCCHANNEL_RULE_SET2,									// 예비용 채널 2
	CCCHANNEL_RULE_SET3,									// 예비용 채널 3

	CCCHANNEL_RULE_MAX
};

#define CCCHANNEL_RULE_NOVICE_STR		"novice"
#define CCCHANNEL_RULE_NEWBIE_STR		"newbie"
#define CCCHANNEL_RULE_ROOKIE_STR		"rookie"
#define CCCHANNEL_RULE_MASTERY_STR		"mastery"
#define CCCHANNEL_RULE_ELITE_STR			"elite"
#define CCCHANNEL_RULE_CHAMPION_STR		"champion"
#define CCCHANNEL_RULE_QUEST_STR			"quest"
#define CCCHANNEL_RULE_DUELTOURNAMENT_STR "dueltournament"
#define CCCHANNEL_RULE_SET1_STR			"set1"			// 예비용 채널 1
#define CCCHANNEL_RULE_SET2_STR			"set2"			// 예비용 채널 2
#define CCCHANNEL_RULE_SET3_STR			"set3"			// 예비용 채널 3



class CCChannelRuleMapList : public list<int>
{
private:
	set<int>		m_Set;
public:
	void Add(int nMapID)
	{
		m_Set.insert(nMapID);
		push_back(nMapID);
	}
	void Add(string strMapName);
	bool Exist(int nMapID, bool bOnlyDuel);
	bool Exist(const char* pszMapName, bool bOnlyDuel);
};

class CCChannelRuleGameTypeList : public list<int>
{
private:
	set<int>		m_Set;
public:
	void Add(int nGameTypeID)
	{
		m_Set.insert(nGameTypeID);
		push_back(nGameTypeID);
	}
	bool Exist(int nGameTypeID)
	{
#ifdef _DEBUG
		set<int>::iterator i = m_Set.begin();
		set<int>::iterator e = m_Set.end();

		for(;i != e; ++i )
		{
			// cclog( "game type : %d.\n", (*i) );
		}

#endif
		if (m_Set.find(nGameTypeID) != m_Set.end()) return true;
		return false;
	}

	const int GetFirstGameType()
	{
		if( empty() )
		{
			return -1;
		}

		return *(begin());
	}
};

class CCChannelRule {
protected:
	int							m_nID;
	string						m_Name;
	CCChannelRuleMapList			m_MapList;
	CCChannelRuleGameTypeList	m_GameTypeList;
	int							m_nDefault;

public:
	CCChannelRule()						{}
	virtual ~CCChannelRule()				{}
	void Init(int nID, const char* pszName)
	{
		m_nDefault = 0;
		m_nID = nID;
		m_Name = pszName;
	}

	int GetID()							{ return m_nID; }
	const char* GetName()				{ return m_Name.c_str(); }

	void AddMap(string strMapName)		{ m_MapList.Add(strMapName); }
	void AddGameType(int nGameTypeID)	{ m_GameTypeList.Add(nGameTypeID); }
	bool CheckMap(int nMapID, bool bOnlyDuel)
	{
		return m_MapList.Exist(nMapID, bOnlyDuel);
	}
	bool CheckMap(const char* pszMapName, bool bOnlyDuel)
	{
		return m_MapList.Exist(pszMapName, bOnlyDuel);
	}
	bool CheckGameType(int nGameTypeID)
	{
#ifdef _DEBUG
		//cclog( "Rule name : %s.\n", m_Name.c_str() );
#endif
		return m_GameTypeList.Exist(nGameTypeID);
	}
	CCChannelRuleMapList* GetMapList()					{ return &m_MapList; }
	CCChannelRuleGameTypeList* GetGameTypeList()			{ return &m_GameTypeList; }
	void SetDefault( int nDefault)						{ m_nDefault = nDefault; }
	int GetDefault()									{ return m_nDefault; }
};



class CCChannelRuleMgr : public map<string, CCChannelRule*>, public CCXmlParser
{
private:
	map<CCCHANNEL_RULE, CCChannelRule*>		m_RuleTypeMap;
	void AddRule(CCChannelRule* pRule);
public:
	CCChannelRuleMgr();
	virtual ~CCChannelRuleMgr();
	void Clear();	
	CCChannelRule* GetRule(const string& strName);
	CCChannelRule* GetRule(CCCHANNEL_RULE nChannelRule);

protected:
	void ParseRule(::CCXmlElement* element);
	virtual void ParseRoot(const char* szTagName, CCXmlElement* pElement);
};