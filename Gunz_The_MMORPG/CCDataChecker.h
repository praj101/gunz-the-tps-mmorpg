#pragma once

#include <map>
using namespace std;


#ifndef BYTE
typedef unsigned char BYTE;
#endif

enum MEMORYFUGITIVE_TYPE {
	MT_MEMORYFUGITIVE_NONE,		//CCMemoryFugitive If you are not (native type)
	MT_MEMORYFUGITIVE_INT,
	MT_MEMORYFUGITIVE_FLOAT,
	//.. If you need to add more types
};

class CCDataChecker;
class CCDataCheckNode {
protected:
	unsigned int	m_iID;	//debugging identifier
	BYTE*			m_pData;
	unsigned int	m_iLen;
	unsigned int	m_iChecksum;
	unsigned int	m_iLastChecksum;
	MEMORYFUGITIVE_TYPE	m_memFugitiveType;

public:
	CCDataCheckNode(BYTE* pData, unsigned int nLen, MEMORYFUGITIVE_TYPE memFugitiveType);
	virtual ~CCDataCheckNode();

	unsigned int GetID()			{ return m_iID; }
	unsigned int GetChecksum()		{ return m_iChecksum; }
	unsigned int GetLastChecksum()	{ return m_iLastChecksum; }
	bool UpdateChecksum();	//is equal before and after updating true, different false
	void Validate()	{ m_iLastChecksum = m_iChecksum; }

friend CCDataChecker;
};
class CCDataCheckMap : public map<BYTE*, CCDataCheckNode*>{};


class CCDataChecker {
protected:
	unsigned int	m_iTotalChecksum;
	unsigned int	m_iLastTotalChecksum;

	CCDataCheckMap	m_DataCheckMap;

public:
	CCDataChecker();
	virtual ~CCDataChecker();

	void Clear();
	unsigned int GetChecksum()	{ return m_iTotalChecksum; }
	CCDataCheckNode* FindCheck(BYTE* pData);
	CCDataCheckNode* AddCheck(BYTE* pData, unsigned int nLen, MEMORYFUGITIVE_TYPE memFugitiveType=MT_MEMORYFUGITIVE_NONE);
	void RenewCheck(BYTE* pData, unsigned int nLen);
	bool UpdateChecksum();
	void BringError();
};