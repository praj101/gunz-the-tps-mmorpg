#ifndef CCMatchBuff_H
#define CCMatchBuff_H

//#include "CCMatchBuffDesc.h"

#include <map>
using namespace std;

enum CCMatchBuffEffectType
{
	MMBET_NORMAL = 0,
	MMBET_DOTE,
	MMBET_END,
};

enum CCMatchBuffPeriodType
{
	MMBPT_NONE = 0,
	MMBPT_LONG,
	MMBPT_SHORT,
	MMBPT_END,
};

struct CCMatchBuffName
{
	char szBuffName[128];
	char szBuffIconName[128];
};

struct CCMatchBuffInfo
{
	int nHP;
	int nAP;

	float fSpeed_Ratio;

	int nRespawnDecTime;

	inline CCMatchBuffInfo& operator=(const CCMatchBuffInfo& b)
	{
		nHP = b.nHP;
		nAP = b.nAP;

		fSpeed_Ratio = b.fSpeed_Ratio;

		nRespawnDecTime = b.nRespawnDecTime;

		return *this;
	}

	inline CCMatchBuffInfo& operator+(const CCMatchBuffInfo& b)
	{
		nHP = nHP + b.nHP;
		nAP = nAP + b.nAP;

		fSpeed_Ratio = fSpeed_Ratio * b.fSpeed_Ratio;

		nRespawnDecTime = nRespawnDecTime + b.nRespawnDecTime;

		return *this;
	}
};

struct CCMatchBuffDesc
{
	unsigned long int		m_nBuffID;

	CCProtectValue<CCMatchBuffEffectType> m_nBuffEffectType;
	CCProtectValue<CCMatchBuffPeriodType>	m_nBuffPeriodType;
	CCProtectValue<int>					m_nBuffPeriod;

	CCProtectValue<CCMatchBuffInfo>*		m_pBuffInfo;

	char				m_szBuffName[128];
	char				m_szBuffDesc[8192];
	char				m_szBuffIconName[128];

	CCMatchBuffDesc();
	~CCMatchBuffDesc();	

	void CacheCRC32( CCMatchCRC32XORCache& crc );
	void ShiftFugitiveValues();
};

class CCMatchBuffDescMgr : public map<int, CCMatchBuffDesc*>
{
protected:
	unsigned long m_nChecksum;

	CCMatchBuffDescMgr();
	virtual ~CCMatchBuffDescMgr();

	bool ParseItem(::CCXmlElement& element);

public:
	bool ReadXml(const char* szFileName);
	bool ReadXml(CCZFileSystem* pFileSystem, const char* szFileName);

	void Clear();

	CCMatchBuffDesc* GetBuffDesc(unsigned long int nBuffID);

	unsigned long GetChecksum() { return m_nChecksum; }

	static CCMatchBuffDescMgr* GetInstance() 
	{
		static CCMatchBuffDescMgr m_BuffDescMgr;
		return &m_BuffDescMgr;
	}


	bool SetBuffName(CCMatchItemDescMgr* pItemDescMgr);
};

inline CCMatchBuffDescMgr* CCGetMatchBuffDescMgr() { return CCMatchBuffDescMgr::GetInstance(); }

#include "cxr_CCMatchBuff.h"


class CCMatchBuff
{
protected:	
	CCUID	m_uidBuff;

	int		m_nBuffID;
	int		m_nRegTime;	
	int		m_nBuffPeriodRemainder;

	CCMatchBuffDesc* m_pBuffDesc;

public:
	CCMatchBuff(){}
	~CCMatchBuff(){}

	virtual void Reset();
	virtual bool Set(CCUID& uidBuff, int nBuffID, int nRegTime, int nBuffPeriodRemainder);

	bool IsExpired(int nGlobalTick);

	CCUID GetBuffUID()				{ return m_uidBuff; }
	int  GetBuffID()				{ return m_pBuffDesc == NULL ? 0 : m_pBuffDesc->m_nBuffID; }
	int  GetBuffPeriod()			{ return m_pBuffDesc == NULL ? 0 : m_pBuffDesc->m_nBuffPeriod.Ref(); }	
	
	int  GetBuffPeriodRemainder(int nGlobalTick);
};



/////////////////////////////////////////////////////////////////////////////
// CCMatchShortBuff
class CCMatchShortBuff : public CCMatchBuff
{
protected:
public:
	CCMatchShortBuff() : CCMatchBuff() {}
	~CCMatchShortBuff() {}
};


/////////////////////////////////////////////////////////////////////////////
// CCMatchShortBuffMap
class CCMatchShortBuffMap : public map<CCUID, CCMatchShortBuff*>
{
protected:
public:
	void Clear();
	void Remove(CCUID& uidBuff);
	bool Insert(CCUID& uidBuff, CCMatchShortBuff* pBuff);
	CCMatchShortBuff* GetShortBuffByBuffID(int nBuffID);
};

#endif