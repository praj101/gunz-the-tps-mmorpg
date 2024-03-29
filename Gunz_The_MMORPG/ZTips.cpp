#include "stdafx.h"
#include "ZTips.h"
#include "CCXml.h"
#include "Physics.h"
#include "ZFilePath.h"

#define ZTOK_TIPS			"TIPS"
#define ZTOK_TIPS_MSG		"MSG"

#define ZTOK_TIPS_ATTR_CATEGORY		"category"
#define ZTOK_TIPS_MSG_ATTR_ID		"id"


ZTips::ZTips() : m_bIsShowedNewbieTips(false)
{

}


bool ZTips::Initialize(CCZFileSystem* pfs, const CCLanguage LangID )
{
	CCZFile mzf;
	if(!mzf.Open(FILENAME_TIPS,pfs)) return false;

	char *buffer;
	buffer=new char[mzf.GetLength()+1];
	mzf.Read(buffer,mzf.GetLength());
	buffer[mzf.GetLength()]=0;

//	LANGID LangID = LANG_KOREAN;			/* Korean : 이거 정말 하드코딩 박기 싫었는디... 쩝... -_-;;; */
//#ifdef LOCALE_JAPAN
//	LangID = LANG_JAPANESE;					/* Japanese */
//#elif  LOCALE_US
//	LangID = LANG_ENGLISH;					/* International */
//#elif  LOCALE_BRAZIL
//	LangID = LANG_PORTUGUESE;				/* Brazil */
//#elif  LOCALE_INDIA
//	LangID = LANG_ENGLISH;					/* India */
//#endif

	cclog( "Load XML from memory : %s(0x%04X)", FILENAME_TIPS, LangID);

	CCXmlDocument aXml;
	aXml.Create();
	if(!aXml.LoadFromMemory(buffer, LangID))
	{
		cclog( "- FAIL\n");
		delete buffer;
		return false;
	}
	delete buffer;
	cclog( "- SUCCESS\n");

	int iCount, i;
	CCXmlElement		aParent, aChild;
	aParent = aXml.GetDocumentElement();
	iCount = aParent.GetChildNodeCount();

	char szTagName[256]="";
	
	for (i = 0; i < iCount; i++)
	{
		aChild = aParent.GetChildNode(i);
		aChild.GetTagName(szTagName);
		if(stricmp(szTagName,ZTOK_TIPS)==0)
		{
			if (szTagName[0] == '#') continue;
			ParseTips(&aChild);
		}
	}

	return true;
}

void ZTips::ParseTips(::CCXmlElement* pElement)
{
	int nCategory=0;
	pElement->GetAttribute(&nCategory, ZTOK_TIPS_ATTR_CATEGORY, 0);	// default는 0

	CCXmlElement childElement;
	char szTagName[256]=""; char szAttr[256]="";
	char szContents[1024]="";

	int nCount = pElement->GetChildNodeCount();
	for (int i = 0; i < nCount; i++) 
	{
		childElement = pElement->GetChildNode(i);

		childElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, ZTOK_TIPS_MSG))
		{
			int nMsgID = -1;
			childElement.GetAttribute(&nMsgID, ZTOK_TIPS_MSG_ATTR_ID, -1);
			childElement.GetContents(szContents);

			// data 입력
			m_StringVector.push_back(string(szContents));

			ZTIPS_MSG msg;
			msg.nMSGID = nMsgID;
			msg.nVectorIndex = (int)m_StringVector.size() - 1;

			m_MsgsVectors[nCategory].push_back(msg);
		}
	}
}

void ZTips::Finalize()
{
	m_StringVector.clear();
}

const char* ZTips::GetRandomTips()
{
	int nSize = (int)m_StringVector.size();
	if (nSize == 0) return "";


	int nIndex = rand() % nSize;
	if ((nIndex < 0) || (nIndex >= nSize)) return "";
	return m_StringVector[nIndex].c_str();
}

const char* ZTips::GetTips(ZTIPS_CATEGORY nCategory, int nID)
{
	vector<int>		nIndices;
	for (int i = 0; i < (int)m_MsgsVectors[(int)nCategory].size(); i++)
	{
		if (nID == m_MsgsVectors[(int)nCategory][i].nMSGID)
		{
			nIndices.push_back(m_MsgsVectors[(int)nCategory][i].nVectorIndex);
		}
	}

	int nStrIndex = 0;
	if (nIndices.empty())
	{
		_ASSERT(0);	// 보여줄 메세지가 없다.
		return "";
	}
	else
	{
		int nRandIndex = rand() % (int)nIndices.size();
		nStrIndex = nIndices[nRandIndex];
	}

	return m_StringVector[nStrIndex].c_str();
}

