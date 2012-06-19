#include "stdafx.h"
#include "CCXml.h"
#include "CCLocale.h"
#include "CCZFileSystem.h"
#include "CCDebug.h"
#include <string.h>
#include <string>
#include <algorithm>
using namespace std; 

#ifdef _CCSXML2
	#import "msxml4/msxml4.dll" named_guids implementation_only
//	using namespace MSXML2;

	typedef MSXML2::IXMLDOMDocumentPtr				CCXmlDomDocPtr;
	typedef MSXML2::IXMLDOMNodePtr					CCXmlDomNodePtr;
	typedef MSXML2::IXMLDOMNodeListPtr				CCXmlDomNodeListPtr;
	typedef MSXML2::IXMLDOMElementPtr				CCXmlDomElementPtr;
	typedef MSXML2::IXMLDOMProcessingInstructionPtr CCXmlDomPIPtr;
	typedef MSXML2::IXMLDOMNamedNodeMapPtr			CCXmlDomNamedNodeMapPtr;
	typedef MSXML2::IXMLDOMTextPtr					CCXmlDomTextPtr;
	typedef MSXML2::IXMLDOMParseErrorPtr			CCXmlDomParseErrorPtr;
#else
	#import "msxml.dll" named_guids implementation_only

	typedef MSXML::IXMLDOMDocumentPtr				CCXmlDomDocPtr;
	typedef MSXML::IXMLDOMNodePtr					CCXmlDomNodePtr;
	typedef MSXML::IXMLDOMNodeListPtr				CCXmlDomNodeListPtr;
	typedef MSXML::IXMLDOMElementPtr				CCXmlDomElementPtr;
	typedef MSXML::IXMLDOCCProcessingInstructionPtr	CCXmlDomPIPtr;
	typedef MSXML::IXMLDOMNamedNodeMapPtr			CCXmlDomNamedNodeMapPtr;
	typedef MSXML::IXMLDOCCTextPtr					CCXmlDomTextPtr;
	typedef MSXML::IXMLDOMParseErrorPtr				CCXmlDomParseErrorPtr;

//	using namespace MSXML;
#endif

BSTR _AsciiToBSTR(const char* ascii)
{
	WCHAR wide[1024];
	int ret=::MultiByteToWideChar(CP_ACP, 0, ascii, -1, wide, 1024);
	CCASSERT(ret!=0);
	return SysAllocString(wide);
}


//-----------------------------------------------------------------------------
void CCXmlNode::GetNodeName(char* sOutStr)
{
	if (m_pDomNode)
	{
		strcpy(sOutStr, _BSTRToAscii(m_pDomNode->nodeName));
	}
}

void CCXmlNode::GetText(char* sOutStr, int nMaxCharNum)
{
	if ( m_pDomNode)
	{
		if ( nMaxCharNum == -1)
		{
			strcpy(sOutStr, _BSTRToAscii(m_pDomNode->Gettext()));
			return;
		}
		else
		{
			int nLen = min( nMaxCharNum, (int)strlen( m_pDomNode->Gettext()));

			if (m_pDomNode)
			{
				strncpy(sOutStr, _BSTRToAscii(m_pDomNode->Gettext()), nLen);
				*(sOutStr + nLen) = 0;
			}
		}
	}
}

void CCXmlNode::SetText(const char* sText)
{
	if (m_pDomNode)
	{
		BSTR bszText;

		bszText = _AsciiToBSTR(sText);
		m_pDomNode->Puttext(bszText);
		SysFreeString(bszText);
	}
}


int	CCXmlNode::GetChildNodeCount()
{
	if (m_pDomNode) return m_pDomNode->childNodes->length;
	else return -1;
}


DOMNodeType CCXmlNode::GetNodeType()
{
	if (m_pDomNode) return (DOMNodeType)m_pDomNode->nodeType;
	return NODE_INVALID;
}


bool CCXmlNode::HasChildNodes()
{
	if ((m_pDomNode != NULL) && 
		(m_pDomNode->hasChildNodes() == VARIANT_TRUE)) return true;

	return false;
}

CCXmlNode CCXmlNode::GetChildNode(int iIndex)
{
	if (m_pDomNode)
	{
		return CCXmlNode(m_pDomNode->childNodes->item[iIndex]);
	}
	else
	{
		return CCXmlNode();
	}
}

void CCXmlNode::NextSibling()
{
	if (m_pDomNode)
	{
		m_pDomNode = m_pDomNode->nextSibling;
	}
}

void CCXmlNode::PreviousSibling()
{
	if (m_pDomNode)
	{
		m_pDomNode = m_pDomNode->previousSibling;
	}
}

bool CCXmlNode::FindChildNode(const char* sNodeName, CCXmlNode* pOutNode)
{
	int iCount, i;
	char szBuf[8192];
	CCXmlDomNodePtr		pNode;

	iCount = m_pDomNode->childNodes->length;

	for (i = 0; i < iCount; i++)
	{
		pNode = m_pDomNode->childNodes->item[i];
		strcpy(szBuf, _BSTRToAscii(pNode->nodeName));

		if (!stricmp(szBuf, sNodeName))
		{
			pOutNode->SetXmlDomNodePtr(pNode);
			return true;
		}
	}

	return false;
}


CCXmlNode CCXmlNode::SelectSingleNode(TCHAR* sQueryStr)
{
	BSTR bszQueryStr;

	bszQueryStr = _AsciiToBSTR(sQueryStr);
	CCXmlDomNodePtr pNode = m_pDomNode->selectSingleNode(bszQueryStr);
	SysFreeString(bszQueryStr);

	return CCXmlNode(pNode);
}

CCXmlDomNodeListPtr CCXmlNode::SelectNodes(TCHAR* sQueryStr)
{
	BSTR bszQueryStr;

	bszQueryStr = _AsciiToBSTR(sQueryStr);
	CCXmlDomNodeListPtr pNodeList = m_pDomNode->selectNodes(bszQueryStr);
	SysFreeString(bszQueryStr);

	return pNodeList;
}

bool CCXmlNode::AppendChild(CCXmlNode node)
{
	m_pDomNode->appendChild(node.GetXmlDomNodePtr());

	return true;
}


//-----------------------------------------------------------------------------
void CCXmlElement::SetContents(int iValue)
{
	char szTemp[20];
	itoa(iValue, szTemp, 10);

	SetContents(szTemp);
	
}

void CCXmlElement::SetContents(float fValue)
{
	char szTemp[32];
	sprintf(szTemp, "%12.4f", fValue);
	SetContents(szTemp);
}

void CCXmlElement::SetContents(bool bValue)
{
	if (bValue)
	{
		SetContents("true");
	}
	else
	{
		SetContents("false");
	}
}

bool CCXmlElement::AppendText(const char* sText)
{
	CCXmlDomTextPtr		pText;
	CCXmlDomDocPtr		pDom(MSXML2::CLSID_DOMDocument);

	BSTR pBSTRText = _AsciiToBSTR(sText);
	pText = pDom->createTextNode(pBSTRText);
	SysFreeString(pBSTRText);
	m_pDomNode->appendChild(pText);

	return true;
}

bool CCXmlElement::AppendChild(::CCXmlElement aChildElement)
{
	CCXmlDomElementPtr	pElement;

	pElement = aChildElement.GetXmlDomNodePtr();
	m_pDomNode->appendChild(pElement);

	return true;
}

bool CCXmlElement::AppendChild(const char* sTagName, const char* sTagText)
{
	CCXmlDomElementPtr	pElement;
	CCXmlDomDocPtr		pDom(MSXML2::CLSID_DOMDocument);

	pElement = pDom->createElement(sTagName);

	if (sTagText != NULL)
	{
		pElement->appendChild(pDom->createTextNode(sTagText));
	}

	m_pDomNode->appendChild(pElement);

	return true;
}

CCXmlElement	CCXmlElement::CreateChildElement(const char* sTagName)
{
	CCXmlDomElementPtr	pDomElement;
	CCXmlDomDocPtr		pDom(MSXML2::CLSID_DOMDocument);

	pDomElement = pDom->createElement(sTagName);

	m_pDomNode->appendChild(pDomElement);

	return CCXmlElement(pDomElement);
}

bool CCXmlElement::GetAttribute(char* sOutText, const char* sAttrName, char* sDefaultText)
{
	CCXmlDomNamedNodeMapPtr pAttributes = m_pDomNode->attributes;
	CCXmlDomNodePtr pNode;

	for(int i=0; i < pAttributes->length; i++)
	{
		 pNode = pAttributes->item[i];
		 
		 if(!stricmp(_BSTRToAscii(pNode->nodeName), sAttrName))
		 {
			 strcpy(sOutText, _BSTRToAscii(pNode->text));
			 return true;
		 }
	}

	strcpy(sOutText, sDefaultText);
	return false;
}

bool CCXmlElement::GetAttribute(int* ipOutValue, const char* sAttrName, int nDefaultValue)
{
	char szTemp[256];
	memset(szTemp, 0, 256);

	if (!GetAttribute(szTemp, sAttrName)) 
	{
		*ipOutValue = nDefaultValue;
		return false;
	}

	try
	{
		*ipOutValue = atoi(szTemp);
	}
	catch(...)
	{
		*ipOutValue = nDefaultValue;
		return false;
	}
	
	return true;
}

bool CCXmlElement::GetAttribute(float* fpOutValue, const char* sAttrName, float fDefaultValue)
{
	char szTemp[256];
	memset(szTemp, 0, 256);

	if (!GetAttribute(szTemp, sAttrName)) 
	{
		*fpOutValue = fDefaultValue;
		return false;
	}

	try
	{
		*fpOutValue = (float)atof(szTemp);
	}
	catch(...)
	{
		*fpOutValue = fDefaultValue;
		return false;
	}
	
	return true;
}
bool CCXmlElement::GetAttribute(bool* bOutValue, const char* sAttrName, bool bDefaultValue)
{
	char szTemp[1024];
	memset(szTemp, 0, 1024);

	if (!GetAttribute(szTemp, sAttrName))
	{
		*bOutValue = bDefaultValue;
		return false;
	}

	if (!stricmp(szTemp, "true"))
	{
		*bOutValue = true;
	}
	else if (!stricmp(szTemp, "false"))
	{
		*bOutValue = false;
	}
	else
	{
		*bOutValue = bDefaultValue;
	}
	return true;
}

bool CCXmlElement::GetAttribute(string* pstrOutValue, const char* sAttrName, char* sDefaultValue)
{
	char szTemp[256];
	memset(szTemp, 0, 256);

	if (!GetAttribute(szTemp, sAttrName)) 
	{
		*pstrOutValue = sDefaultValue;
		return false;
	}

	*pstrOutValue = szTemp;
	
	return true;

}

int CCXmlElement::GetAttributeCount()
{
	CCXmlDomNamedNodeMapPtr pAttributes = m_pDomNode->attributes;
	return pAttributes->length;
}

void CCXmlElement::GetAttribute(int index, char* szoutAttrName, char* szoutAttrValue)
{
	CCXmlDomNamedNodeMapPtr pAttributes = m_pDomNode->attributes;
	CCXmlDomNodePtr pNode = pAttributes->item[index];

	strcpy(szoutAttrName, (_BSTRToAscii(pNode->nodeName)));
	strcpy(szoutAttrValue, _BSTRToAscii(pNode->text));
}

bool CCXmlElement::AddAttribute(const char* sAttrName, const char* sAttrText)
{
	CCXmlDomDocPtr	pDom(MSXML2::CLSID_DOMDocument);
	CCXmlDomNodePtr pNode;
	CCXmlDomNamedNodeMapPtr pAttrs;

	BSTR pBSTRAttrName = _AsciiToBSTR(sAttrName);
	BSTR pBSTRAttrText = _AsciiToBSTR(sAttrText);

	pAttrs = m_pDomNode->Getattributes();
	pNode = pDom->createAttribute(pBSTRAttrName);
	pNode->Puttext(pBSTRAttrText);
	pAttrs->setNamedItem(pNode);

	SysFreeString(pBSTRAttrName);
	SysFreeString(pBSTRAttrText);

	return true;
}

bool CCXmlElement::AddAttribute(const char* sAttrName, int iAttrValue)
{
	char szTemp[20];
	itoa(iAttrValue, szTemp, 10);

	return AddAttribute(sAttrName, szTemp);
}

bool CCXmlElement::AddAttribute(const char* sAttrName, bool bAttrValue)
{
	if (bAttrValue)
	{
		return AddAttribute(sAttrName, "true");
	}
	else
	{
		return AddAttribute(sAttrName, "false");
	}
	return false;
}
bool CCXmlElement::SetAttribute(const char* sAttrName, char* sAttrText)
{
	CCXmlDomNodePtr pNode;
	CCXmlDomNamedNodeMapPtr pAttrs;
	char sTemp[8192];

	if (!GetAttribute(sTemp, sAttrName))
	{
		AddAttribute(sAttrName, sAttrText);
	}
	else
	{
		pAttrs = m_pDomNode->attributes;
		for (int i = 0; i < pAttrs->length; i++)
		{
			pNode = pAttrs->item[i];
			if (!stricmp(_BSTRToAscii(pNode->nodeName), sAttrName))
			{
				BSTR pBSTRAttrText = _AsciiToBSTR(sAttrText);
				pNode->Puttext(pBSTRAttrText);
				SysFreeString(pBSTRAttrText);
			}
		}
	}

	return true;
}

bool CCXmlElement::SetAttribute(const char* sAttrName, int iAttrValue)
{
	char szTemp[20];
	itoa(iAttrValue, szTemp, 10);

	return SetAttribute(sAttrName, szTemp);
}

bool CCXmlElement::GetChildContents(char* sOutStr, const char* sChildTagName, int nMaxCharNum)
{
	CCXmlNode		node;

	if (FindChildNode(sChildTagName, &node))
	{
		node.GetText(sOutStr, nMaxCharNum);
		return true;
	}
	else
	{
		return false;
	}
	
}

bool CCXmlElement::GetChildContents(int* iOutValue, const char* sChildTagName)
{
	char szBuf[256];
	if (GetChildContents(szBuf, sChildTagName))
	{
		try
		{
			*iOutValue = atoi(szBuf);
		}
		catch(...)
		{
			*iOutValue = 0;
			return false;
		}

		return true;
	}
	
	return false;
}

bool CCXmlElement::GetChildContents(float* fOutValue, const char* sChildTagName)
{
	char szBuf[256];
	if (GetChildContents(szBuf, sChildTagName))
	{
		try
		{
			*fOutValue = (float)atof(szBuf);
		}
		catch(...)
		{
			*fOutValue = 0;
			return false;
		}

		return true;
	}
	
	return false;
}

bool CCXmlElement::GetChildContents(bool* bOutValue, const char* sChildTagName)
{
	char szBuf[256];
	if (GetChildContents(szBuf, sChildTagName))
	{
		if (!stricmp(szBuf, "true"))
		{
			*bOutValue = true;
		}
		else
		{
			*bOutValue = false;
		}
		return true;
	}

	return false;
}

void CCXmlElement::GetContents(int* ipOutValue)
{
	char sTemp[256];
	memset(sTemp, 0, 256);

	CCXmlNode::GetText(sTemp);

	try
	{
		*ipOutValue = atoi(sTemp);
	}
	catch(...)
	{
		*ipOutValue = 0;
	}
}

void CCXmlElement::GetContents(bool* bpOutValue)
{
	char sTemp[64];
	memset(sTemp, 0, 64);
	CCXmlNode::GetText(sTemp);

	if (!stricmp(sTemp, "true"))
	{
		*bpOutValue = true;
	}
	else
	{
		*bpOutValue = false;
	}
}

void CCXmlElement::GetContents(float* fpOutValue)
{
	char sTemp[256];
	memset(sTemp, 0, 256);

	CCXmlNode::GetText(sTemp);

	try
	{
		*fpOutValue = (float)atof(sTemp);
	}
	catch(...)
	{
		*fpOutValue = 0.0f;
	}

}

void CCXmlElement::GetContents(string* pstrValue)
{
	char sTemp[256];
	memset(sTemp, 0, 256);
	CCXmlNode::GetText(sTemp);
	*pstrValue = sTemp;

}

bool CCXmlElement::RemoveAttribute(const char* sAttrName)
{
	BSTR pBSTRAttrName = _AsciiToBSTR(sAttrName);
	((CCXmlDomElementPtr)m_pDomNode)->removeAttribute(pBSTRAttrName);
	SysFreeString(pBSTRAttrName);

	return true;
}
//-----------------------------------------------------------------------------


CCXmlDocument::CCXmlDocument()
{
	m_bInitialized = false;

	m_ppDom = NULL;

	CoInitialize(NULL);
}

CCXmlDocument::~CCXmlDocument()
{
	if (m_bInitialized) Destroy();

	CoUninitialize();
}

bool CCXmlDocument::Create()
{
	// CoInitialize()를 먼저 수행하기 위해 동적으로 할당함.
	m_ppDom = new CCXmlDomDocPtr;


	CCXmlDomDocPtr pDom(MSXML2::CLSID_DOMDocument);


	(*m_ppDom) = pDom;

	(*m_ppDom)->async = false;
	(*m_ppDom)->preserveWhiteSpace = VARIANT_TRUE;

	m_bInitialized = true;

	return true;
}

bool CCXmlDocument::Destroy()
{
	if (!m_bInitialized) return false;

	delete m_ppDom;	m_ppDom = NULL;

	m_bInitialized = false;

	return true;
}

bool CCXmlDocument::LoadFromFile(const char* m_sFileName)
{
	if (!m_bInitialized) return false;

	_variant_t varOut((bool)TRUE);
	varOut = (*m_ppDom)->load((_variant_t)m_sFileName);
	if ((bool)varOut == FALSE)
	{
		CCXmlDomParseErrorPtr errPtr = (*m_ppDom)->GetparseError();
		_bstr_t bstrErr(errPtr->reason);

#ifdef _DEBUG
		char szBuf[8192];
		sprintf(szBuf, "-------------------------------\n");
		OutputDebugString(szBuf);
		sprintf(szBuf, "Error In Xml File(%s)\n", m_sFileName);		
		OutputDebugString(szBuf);
		sprintf(szBuf, "Code = 0x%x\n", errPtr->errorCode);
		OutputDebugString(szBuf);
		sprintf(szBuf, "Source = Line : %ld; Char : %ld\n", errPtr->line, errPtr->linepos);
		OutputDebugString(szBuf);
		sprintf(szBuf, "Error Description = %s\n", (char*)bstrErr);
		OutputDebugString(szBuf);
#endif

		_ASSERT(0);

		return false;
	}

	return true;
}

bool CCXmlDocument::LoadFromMemory(char* szBuffer, LANGID lanid)
{
	if (!m_bInitialized) return false;

	// UTF8인지 검사한다
	string s;
	if ( (szBuffer[0] == (char)0xEF) && (szBuffer[1] == (char)0xBB) && (szBuffer[2] == (char)0xBF))
		s = CCLocale::ConvUTF8ToAnsi( szBuffer, lanid);		// UTF8 -> ANSI
	else
		s = szBuffer;										// ANSI


	// XML 내용의 시작을 찾는다
	int pos = s.find( "<XML>");								// <XML>
	if ( pos == -1)
	{
		pos = s.find( "<xml>");								// <xml>
		
		if ( pos == -1)
		{
			pos = s.find( "?>");							// ?>
	
			if ( pos == -1)
			{
				s.clear();
				return false;
			}

			pos += 2;
		}
	}
	char* cp = &s[pos];


	// XML 내용의 끝을 찾는다
	pos = s.find( "</XML>");
	if ( pos == -1)
	{
		pos = s.find( "</xml>");
		
		if ( pos == -1)
		{
			s.clear();
			return false;
		}
	}
	s[pos+6] = '\0';


	_bstr_t bsXML(cp);
	if ((*m_ppDom)->loadXML(BSTR(bsXML))!= -1)
	{
		CCXmlDomParseErrorPtr errPtr = (*m_ppDom)->GetparseError();
		_bstr_t bstrErr(errPtr->reason);

#ifdef _DEBUG
		char szBuf[8192];

		sprintf(szBuf, "-------------------------------\n");
		OutputDebugString(szBuf);
		sprintf(szBuf, "Error In Load Xml Memory\n");		
		OutputDebugString(szBuf);
		sprintf(szBuf, "Code = 0x%x\n", errPtr->errorCode);
		OutputDebugString(szBuf);
		sprintf(szBuf, "Source = Line : %ld; Char : %ld\n", errPtr->line, errPtr->linepos);
		OutputDebugString(szBuf);
		sprintf(szBuf, "Error Description = %s\n", (char*)bstrErr);
		OutputDebugString(szBuf);
#endif

		_ASSERT(0);

		s.clear();
		return false;
	}

	s.clear();
	return true;
}


bool CCXmlDocument::SaveToFile(const char* m_sFileName)
{
	try
	{
		(*m_ppDom)->save((_variant_t)m_sFileName);
	}
	catch(_com_error& e)
    {
#ifdef _DEBUG
		OutputDebugString(e.ErrorMessage());
#endif
    }
 
	return true;
}


bool CCXmlDocument::CreateProcessingInstruction( const char* szHeader)
{
	CCXmlDomPIPtr	po;

	_bstr_t bsPI( "xml" );
    _bstr_t bsAttr( szHeader);

	try
	{
		po = (*m_ppDom)->createProcessingInstruction((BSTR)bsPI, (BSTR)bsAttr);
	}
	catch(_com_error& e)
	{
#ifdef _DEBUG
		OutputDebugString(e.ErrorMessage());
#endif
	}

	if (po != NULL) (*m_ppDom)->appendChild(po);

	return true;
}

bool CCXmlDocument::Delete(CCXmlNode* pNode)
{
	pNode->GetParent().GetXmlDomNodePtr()->removeChild(pNode->GetXmlDomNodePtr());

	if (pNode->GetXmlDomNodePtr() != NULL) 
	{
		return true;
	}
	else return false;
}


CCXmlNode CCXmlDocument::FindElement(TCHAR* sTagName)
{
	char sBuf[1023];
	sprintf(sBuf, "//%s", sTagName);

	BSTR bszQueryStr;
	bszQueryStr = _AsciiToBSTR(sBuf);
	CCXmlDomNodePtr pNode = (*m_ppDom)->selectSingleNode(bszQueryStr);
	SysFreeString(bszQueryStr);

	return CCXmlNode(pNode);
}

bool CCXmlDocument::AppendChild(CCXmlNode node)
{
	(*m_ppDom)->appendChild(node.GetXmlDomNodePtr());

	return true;
}

CCXmlElement	CCXmlDocument::CreateElement(const char* sName)
{
	CCXmlDomElementPtr pDomElement;
	pDomElement = (*m_ppDom)->createElement(sName);

	return CCXmlElement(pDomElement);
}