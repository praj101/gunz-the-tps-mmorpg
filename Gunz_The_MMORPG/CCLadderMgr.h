#pragma once

#include "CCMatchGlobal.h"
#include "CCLadderGroup.h"
#include "CCLadderStatistics.h"
#include <vector>

class CCMatchObject;


class MLadderMgr {
protected:
	int					m_idGenerate;
	unsigned long		m_nLastTick;

	MLadderGroupMap		m_WaitingMaps[CCLADDERTYPE_MAX];
	list<MLadderGroup*>	m_GroupList;

	MLadderStatistics	m_Stat;
protected:
	unsigned long GetLastTick()	{ return m_nLastTick; }
	void SetLastTick(unsigned long nTick)	{ m_nLastTick = nTick; }

	inline MLadderGroupMap* GetWaitGroupContainer(CCLADDERTYPE nLadderType);

	void AddGroup(CCLADDERTYPE nLadderType, MLadderGroup* pGroup);
	int MakeMatch(CCLADDERTYPE nLadderType);
	void LaunchLadder(CCLADDERTYPE nLadderType, int nGroupA, int nGroupB);
	void RemoveFromGroupList(MLadderGroup* pGroup);
	void CleaningGarbages();
	unsigned long int GetTickInterval();
public:
	MLadderMgr() {
		m_idGenerate = 0;
	}
	bool Init();
	MLadderGroup* CreateLadderGroup();
	MLadderGroup* FindLadderGroup(int nGroupID);
	bool Challenge(MLadderGroup* pGroup);
	void CancelChallenge(int nGroupID, const char* pszCancelName);

	void Tick(unsigned long nTick);
	int GenerateID()	{ return ++m_idGenerate; }	
	int GetNeedMemberCount(CCLADDERTYPE nLadderType);
	int GetTotalGroupCount();
	unsigned long GetChecksum(int nFirstIndex, int nGroupCount);

	void DebugTest();
	
	list<MLadderGroup*>::iterator GetGroupListBegin()		{ return m_GroupList.begin(); }
	list<MLadderGroup*>::iterator GetGroupListEnd()			{ return m_GroupList.end(); }
	size_t GetGroupCount()									{ return m_GroupList.size(); }

	MLadderStatistics*	GetStatistics()						{ return &m_Stat; }
};




