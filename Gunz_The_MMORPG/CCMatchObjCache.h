#pragma once
#include "CCMatchGlobal.h"
#include <list>
#include <map>
using namespace std;

#include "CCMatchObject.h"
#include "CCUID.h"


class CCCommand;
class CCCommandCommunicator;
class CCMatchObject;


struct CCMatchObjCacheCostume
{
	CCMatchSex			nSex;
	unsigned char		nHair;
	unsigned char		nFace;
	unsigned long int	nEquipedItemID[MMCIP_END];
};

class CCMatchObjCache {
protected:
	CCUID					m_uidObject;
	char					m_szName[ MATCHOBJECT_NAME_LENGTH ];
	char					m_szClanName[CLAN_NAME_LENGTH];
	char					m_nLevel;
	CCMatchUserGradeID		m_nUGrade;
	CCMatchPremiumGradeID	m_nPGrade;
	unsigned char			m_nPlayerFlags;		// 플레이어 속성(운영자숨김등) - CCTD_PlayerFlags 사용
	unsigned int			m_nCLID;			// ClanID
	unsigned int			m_nEmblemChecksum;	// Emblem Checksum
	CCMatchObjCacheCostume	m_Costume;
	unsigned int			m_nRank;
	int						m_nKillCount;
	int						m_nDeathCount;
	int						m_nDTGrade;			// 듀얼토너먼트 등급
	
public:
	CCMatchObjCache()				{ 
		m_szName[0] = NULL;
		m_nLevel = 0;
		m_nUGrade = CCMUG_FREE;
		m_nPGrade = MMPG_FREE;
		memset(&m_Costume, 0, sizeof(CCMatchObjCacheCostume));
		ResetFlag();
		m_nDTGrade = 0;
	}
	virtual ~CCMatchObjCache()		{}

	CCUID GetUID()					{ return m_uidObject; }

	char* GetName()			{ return m_szName; }
	char* GetClanName()		{ return m_szClanName; }
	int GetLevel()					{ return m_nLevel; }
	CCMatchUserGradeID		GetUGrade()	{ return m_nUGrade; }
	CCMatchPremiumGradeID	GetPGrade()	{ return m_nPGrade; }

	void SetInfo(const CCUID& uid, const char* szName, const char* szClanName, int nLevel, 
				 CCMatchUserGradeID nUGrade, CCMatchPremiumGradeID nPGrade
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

	void AssignCostume(CCMatchObjCacheCostume* pCostume) { memcpy(&m_Costume, pCostume, sizeof(CCMatchObjCacheCostume)); }
	CCMatchObjCacheCostume* GetCostume() { return &m_Costume; }
};

class CCMatchObjCacheList : public list<CCMatchObjCache*>{};
class CCMatchObjCacheMap : public map<CCUID, CCMatchObjCache*>{
public:
	void Insert(const CCUID& uid, CCMatchObjCache* pCache)	{	
		insert(value_type(uid, pCache));	
	}
	CCMatchObjCache* Find(const CCUID& uid) {
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
	CCMatchObjCacheList	m_ObjectCacheList;

public:
	CCMatchObjectCacheBuilder();
	virtual ~CCMatchObjectCacheBuilder();

	void AddObject(CCMatchObject* pObj);
	void Reset();
	CCCommand* GetResultCmd(MATCHCACHEMODE nType, CCCommandCommunicator* pCmdComm);
};
