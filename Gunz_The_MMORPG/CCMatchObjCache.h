#pragma once
#include "CCMatchGlobal.h"
#include <list>
#include <map>
using namespace std;

#include "CCMatchObject.h"
#include "CCUID.h"


class MCommand;
class MCommandCommunicator;
class CCMatchObject;


struct MMatchObjCacheCostume
{
	MMatchSex			nSex;
	unsigned char		nHair;
	unsigned char		nFace;
	unsigned long int	nEquipedItemID[MMCIP_END];
};

class MMatchObjCache {
protected:
	CCUID					m_uidObject;
	char					m_szName[ MATCHOBJECT_NAME_LENGTH ];
	char					m_szClanName[CLAN_NAME_LENGTH];
	char					m_nLevel;
	MMatchUserGradeID		m_nUGrade;
	MMatchPremiumGradeID	m_nPGrade;
	unsigned char			m_nPlayerFlags;		// 플레이어 속성(운영자숨김등) - MTD_PlayerFlags 사용
	unsigned int			m_nCLID;			// ClanID
	unsigned int			m_nEmblemChecksum;	// Emblem Checksum
	MMatchObjCacheCostume	m_Costume;
	unsigned int			m_nRank;
	int						m_nKillCount;
	int						m_nDeathCount;
	int						m_nDTGrade;			// 듀얼토너먼트 등급
	
public:
	MMatchObjCache()				{ 
		m_szName[0] = NULL;
		m_nLevel = 0;
		m_nUGrade = MMUG_FREE;
		m_nPGrade = MMPG_FREE;
		memset(&m_Costume, 0, sizeof(MMatchObjCacheCostume));
		ResetFlag();
		m_nDTGrade = 0;
	}
	virtual ~MMatchObjCache()		{}

	CCUID GetUID()					{ return m_uidObject; }

	char* GetName()			{ return m_szName; }
	char* GetClanName()		{ return m_szClanName; }
	int GetLevel()					{ return m_nLevel; }
	MMatchUserGradeID		GetUGrade()	{ return m_nUGrade; }
	MMatchPremiumGradeID	GetPGrade()	{ return m_nPGrade; }

	void SetInfo(const CCUID& uid, const char* szName, const char* szClanName, int nLevel, 
				 MMatchUserGradeID nUGrade, MMatchPremiumGradeID nPGrade
				 , const unsigned int nRank, const int nKillCount, const int nDeathCount, int nDTGrade )
	{
		m_uidObject = uid;
		strcpy(m_szName, szName);
		strcpy(m_szClanName, szClanName);
		m_nLevel		= (char)nLevel;
		m_nUGrade		= nUGrade;
		m_nPGrade		= nPGrade;
		m_nPlayerFlags	= 0;
		m_nRank			= nRank;
		m_nKillCount	= nKillCount;
		m_nDeathCount	= nDeathCount;
		m_nDTGrade		= nDTGrade;
	}

	unsigned char GetFlags()				{ return m_nPlayerFlags; }
	void SetFlags(unsigned char nFlags)		{ m_nPlayerFlags = nFlags; }
	bool CheckFlag(unsigned char nFlagIdx)	{ return (m_nPlayerFlags & nFlagIdx) ? true:false; }
	void ResetFlag()						{ m_nPlayerFlags = 0; }
	void SetFlag(unsigned char nFlagIdx, bool bSet)	
	{ 
		if (bSet) m_nPlayerFlags |= nFlagIdx; 
		else m_nPlayerFlags &= (0xff ^ nFlagIdx);
	}
	unsigned int GetCLID()					{ return m_nCLID; }
	void SetCLID(unsigned int nCLID)		{ m_nCLID = nCLID; }
	unsigned int GetEmblemChecksum()		{ return m_nEmblemChecksum; }
	void SetEmblemChecksum(unsigned int nChecksum)	{ m_nEmblemChecksum = nChecksum; }

	const unsigned int GetRank()			{ return m_nRank; }
	const int GetKillCount()				{ return m_nKillCount; }
	const int GetDeathCount()				{ return m_nDeathCount; }

	int GetDTGrade()						{ return m_nDTGrade; }

	void AssignCostume(MMatchObjCacheCostume* pCostume) { memcpy(&m_Costume, pCostume, sizeof(MMatchObjCacheCostume)); }
	MMatchObjCacheCostume* GetCostume() { return &m_Costume; }
};

class MMatchObjCacheList : public list<MMatchObjCache*>{};
class MMatchObjCacheMap : public map<CCUID, MMatchObjCache*>{
public:
	void Insert(const CCUID& uid, MMatchObjCache* pCache)	{	
		insert(value_type(uid, pCache));	
	}
	MMatchObjCache* Find(const CCUID& uid) {
		iterator it = find(uid);
		if (it!=end())
			return it->second;
		return NULL;
	}
};


enum MATCHCACHEMODE {
	MATCHCACHEMODE_ADD = 0,
	MATCHCACHEMODE_REMOVE,
	MATCHCACHEMODE_UPDATE,		// 모두 업데이트
	MATCHCACHEMODE_REPLACE		// 새로운 내용으로 교체
};

class CCMatchObjectCacheBuilder {
	MMatchObjCacheList	m_ObjectCacheList;

public:
	CCMatchObjectCacheBuilder();
	virtual ~CCMatchObjectCacheBuilder();

	void AddObject(CCMatchObject* pObj);
	void Reset();
	MCommand* GetResultCmd(MATCHCACHEMODE nType, MCommandCommunicator* pCmdComm);
};
