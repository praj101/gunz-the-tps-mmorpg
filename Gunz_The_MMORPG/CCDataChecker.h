#pragma once

#include <map>
using namespace std;


#ifndef BYTE
typedef unsigned char BYTE;
#endif

enum MEMORYFUGITIVE_TYPE {
	MT_MEMORYFUGITIVE_NONE,		// CCMemoryFugitive 가 아닌 경우 (native type)
	MT_MEMORYFUGITIVE_INT,
	MT_MEMORYFUGITIVE_FLOAT,
	//.. 필요하면 타입을 더 추가
};

class CCDataChecker;
class CCDataCheckNode {
protected:
	unsigned int	m_nID;	// 디버깅용 식별자
	BYTE*			m_pData;
	unsigned int	m_nLen;
	unsigned int	m_nChecksum;
	unsigned int	m_nLastChecksum;
	MEMORYFUGITIVE_TYPE	m_memFugitiveType;

public:
	CCDataCheckNode(BYTE* pData, unsigned int nLen, MEMORYFUGITIVE_TYPE memFugitiveType);
	virtual ~CCDataCheckNode();

	unsigned int GetID()			{ return m_nID; }
	unsigned int GetChecksum()		{ return m_nChecksum; }
	unsigned int GetLastChecksum()	{ return m_nLastChecksum; }
	bool UpdateChecksum();	// 업데이트후 전과 같으면 true, 다르면 false
	void Validate()	{ m_nLastChecksum = m_nChecksum; }

friend CCDataChecker;
};
class CCDataCheckMap : public map<BYTE*, CCDataCheckNode*>{};


class CCDataChecker {
protected:
	unsigned int	m_nTotalChecksum;
	unsigned int	m_nLastTotalChecksum;

	CCDataCheckMap	m_DataCheckMap;

public:
	CCDataChecker();
	virtual ~CCDataChecker();

	void Clear();
	unsigned int GetChecksum()	{ return m_nTotalChecksum; }
	CCDataCheckNode* FindCheck(BYTE* pData);
	CCDataCheckNode* AddCheck(BYTE* pData, unsigned int nLen, MEMORYFUGITIVE_TYPE memFugitiveType=MT_MEMORYFUGITIVE_NONE);
	void RenewCheck(BYTE* pData, unsigned int nLen);
	bool UpdateChecksum();
	void BringError();
};

#endif