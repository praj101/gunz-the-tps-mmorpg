/////////////////////////////////////////////////////////////
#ifndef _CCMatchQuestMonsterGroupDesc_h
#define _CCMatchQuestMonsterGroupDesc_h

#include <list>
#include <map>
#include <string>
#include <algorithm>

using namespace std;

class CCNPCList : public list<string> {};

class CCNPCGroup {
public:
	CCNPCList	m_NpcList;
protected:
	int			m_nID;
	string		m_Name;
public:
	CCNPCGroup() { }
	virtual ~CCNPCGroup() { }
	void SetID(int nID)	{ m_nID = nID; }
	int  GetID() { return m_nID; }

	const char* GetName()	{ return m_Name.c_str(); }
	void SetName(const char* Name) { m_Name = Name; }

	void AddNpc(string NPCName) {
		m_NpcList.push_back(NPCName);
	}

	bool CheckNpcName(string NPCName) {
		for (CCNPCList::iterator i=m_NpcList.begin(); i!=m_NpcList.end(); i++) {
			if (stricmp((*i).c_str(), NPCName.c_str()) == 0)
				return true;
		}
		return false;
	}
};

class CCXmlElement;
class CCZFileSystem;

class CCNPCGroupMgr : public map<string, CCNPCGroup*>
{
public:
	CCNPCGroupMgr();
	virtual ~CCNPCGroupMgr();

	static CCNPCGroupMgr* GetInstance();
	void Clear();	

	CCNPCGroup* GetGroup(const string& strName);
	CCNPCGroup* GetGroup(int nGroupID);

	bool ReadXml(const char* szFileName);
	bool ReadXml(CCZFileSystem* pFileSystem, const char* szFileName);

protected:
	void ParseRule(::CCXmlElement* element);
};

inline CCNPCGroupMgr* CCGetNPCGroupMgr() { return CCNPCGroupMgr::GetInstance(); }

#endif//_CCMatchQuestMonsterGroupDesc_h