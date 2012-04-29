#include "stdafx.h"
#include "CCDataChecker.h"
#include <crtdbg.h>
#include "CCDebug.h"
#include "CCMemoryProxy.h"


//// CCDataCheckNode ////
CCDataCheckNode::CCDataCheckNode(BYTE* pData, unsigned int nLen, MEMORYFUGITIVE_TYPE memFugitiveType)
{
	static unsigned int s_nID = 0;

	m_memFugitiveType = memFugitiveType;

	m_iID = s_nID++;

	m_pData = pData;
	m_iLen = nLen;

	UpdateChecksum();
	
	m_iLastChecksum = m_iChecksum;
}

CCDataCheckNode::~CCDataCheckNode()
{
	m_pData = NULL;
	m_iLen = 0;
	m_iChecksum = 0;
	m_iLastChecksum = 0;
}

bool CCDataCheckNode::UpdateChecksum()
{
	m_iLastChecksum = m_iChecksum;

	m_iChecksum = 0;

	BYTE* data_ptr = NULL;

	switch (m_memFugitiveType)
	{
	case MT_MEMORYFUGITIVE_NONE :
		data_ptr = m_pData;
		break;
	case MT_MEMORYFUGITIVE_INT :
		//data_ptr = (BYTE*) &((CCMemoryFugitive<int>*)m_pData)->GetData();
		break;
	case MT_MEMORYFUGITIVE_FLOAT :
		//data_ptr = (BYTE*) &((CCMemoryFugitive<float>*)m_pData)->GetData();
		break;
	default:
		_ASSERT(0);
		data_ptr = m_pData;
	}

	//Checksum is too simple. Tests compared to the cost effectiveness; (int value 5 05 00 00 00 -> 00 00 00 05, simply the end. In need of improvement)
	//However, because items figure Latitude as fugitive
	for (unsigned int i=0; i<m_iLen; i++) {
		m_iChecksum += data_ptr[i];
	}

	if (m_iChecksum == m_iLastChecksum)
		return true;
	else
		return false;
}

//// CCDataChecker ////
CCDataChecker::CCDataChecker()
{
	m_iTotalChecksum = 0;
	m_iLastTotalChecksum = 0;
}

CCDataChecker::~CCDataChecker()
{
	Clear();
}

void CCDataChecker::Clear()
{
	while(m_DataCheckMap.size()) {
		CCDataCheckMap::iterator i = m_DataCheckMap.begin();
		CCDataCheckNode* pNode = (*i).second;
		m_DataCheckMap.erase(i);
		delete pNode;
	}
}

CCDataCheckNode* CCDataChecker::FindCheck(BYTE* pData)
{
	CCDataCheckMap::iterator i = m_DataCheckMap.find(pData);
	if(i==m_DataCheckMap.end())
		return NULL;
	else
		return (*i).second;
}

CCDataCheckNode* CCDataChecker::AddCheck(BYTE* pData, unsigned int nLen, MEMORYFUGITIVE_TYPE memFugitiveType)
{
	CCDataCheckNode* pNode = new CCDataCheckNode(pData, nLen, memFugitiveType);
	m_DataCheckMap.insert(CCDataCheckMap::value_type(pData, pNode));
	return pNode;
}

void CCDataChecker::RenewCheck(BYTE* pData, unsigned int nLen)
{
	CCDataCheckMap::iterator i = m_DataCheckMap.find(pData);
	if(i==m_DataCheckMap.end()) {
		_ASSERT("CCDataChecker::RenewCheck() - Not existing CheckNode \n");
		return;
	}

	CCDataCheckNode* pNode = (*i).second;

	if (pNode->m_pData == pData) {
		pNode->m_iLen = nLen;
		pNode->UpdateChecksum();
		pNode->Validate();
		return;
	}
}

bool CCDataChecker::UpdateChecksum()
{
	m_iLastTotalChecksum = m_iTotalChecksum;

	bool bResult = true;
	m_iTotalChecksum = 0;
	for (CCDataCheckMap::iterator i=m_DataCheckMap.begin(); i!=m_DataCheckMap.end(); i++) {
		CCDataCheckNode* pNode = (*i).second;
		bool bResultCurrent = pNode->UpdateChecksum();
		bResult &= bResultCurrent;
		m_iTotalChecksum += pNode->GetChecksum();
		#ifdef _DEBUG
			if (bResultCurrent == false)
				cclog("MEMORYHACK: ID=%u, CurrChecksum=%u, LastChecksum=%u \n", 
					pNode->GetID(), pNode->GetChecksum(), pNode->GetLastChecksum());
		#endif
	}

	return bResult;
}

void CCDataChecker::BringError()
{
	int* pError = new int;
	AddCheck((BYTE*)(pError), sizeof(int));	
	*pError = 1742;	//Leak Checksum making a mes
}
