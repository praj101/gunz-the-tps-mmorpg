#pragma once

#include "winsock2.h"
#include "windows.h"
#include <comutil.h>
#include <stdio.h>
#include <string>
using namespace std;

#define _CCSXML2


#ifdef _CCSXML2
	#import "msxml4/msxml4.dll" named_guids no_implementation
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
	#import "msxml.dll" named_guids no_implementation

	typedef MSXML::IXMLDOMDocumentPtr				CCXmlDomDocPtr;
	typedef MSXML::IXMLDOMNodePtr					CCXmlDomNodePtr;
	typedef MSXML::IXMLDOMNodeListPtr				CCXmlDomNodeListPtr;
	typedef MSXML::IXMLDOMElementPtr				CCXmlDomElementPtr;
	typedef MSXML::IXMLDOMProcessingInstructionPtr	CCXmlDomPIPtr;
	typedef MSXML::IXMLDOMNamedNodeMapPtr			CCXmlDomNamedNodeMapPtr;
	typedef MSXML::IXMLDOCCTextPtr					CCXmlDomTextPtr;
	typedef MSXML::IXMLDOMParseErrorPtr				CCXmlDomParseErrorPtr;

//	using namespace MSXML;
#endif

class CCXmlDocument;

/// IXMLDOMNode 래핑 클래스
class CCXmlNode
{
private:

protected:
	CCXmlDomNodePtr		m_pDomNode;			///< IXMLDOMNode 스마트 포인터
public:
	/// Default constructor.
	CCXmlNode() { m_pDomNode = NULL; }
	CCXmlNode(CCXmlDomNodePtr a_pDomNode) { m_pDomNode = a_pDomNode; }
	/// Default destructor.
	virtual ~CCXmlNode() { m_pDomNode = NULL; }

	/// IXMLDOMNode인터페이스로 반환
	CCXmlDomNodePtr	GetXmlDomNodePtr() { return m_pDomNode; }
	/// IXMLDOMNode를 설정
	void			SetXmlDomNodePtr(CCXmlDomNodePtr pNode) { m_pDomNode = pNode; }

	/// IXMLDOMNode가 NULL인지 여부
	bool IsEmpty() { if (m_pDomNode == NULL) return true; else return false; }
	/// Node의 이름을 반환.
	/// @param sOutStr			[out] 반환값
	void GetNodeName(char* sOutStr);
	/// Node의 Text를 반환.
	/// @param sOutStr			[out] 반환값
	void GetText(char* sOutStr, int nMaxCharNum = -1);
	/// Node의 Text를 설정.
	/// @param sOutStr			[in] 설정할 Text
	void SetText(const char* sText);
	
	/// Child Node의 갯수를 반환.
	int	GetChildNodeCount();
	/// Node의 타입을 반환.
	DOMNodeType GetNodeType();
	/// Child Node가 있나?
	bool HasChildNodes();

	void NextSibling();
	void PreviousSibling();

	bool AppendChild(CCXmlNode node);

	/// 해당이름을 가진 Child Node를 찾는다.
	/// 없으면 NULL로 설정
	/// @param sNodeName		[in] 찾을 Node의 이름
	bool FindChildNode(const char* sNodeName, CCXmlNode* pOutNode);

	/// 부모 Node를 반환. 없으면 NULL로 반환한다.
	CCXmlNode GetParent() { if (m_pDomNode) return CCXmlNode(m_pDomNode->parentNode); else return CCXmlNode(); }
	/// 인덱스로 Child Node를 반환
	/// @param iIndex			[in] 인덱스
	CCXmlNode GetChildNode(int iIndex);

	/// 패턴식을 이용하여 해당 노드를 찾는다. 가장 먼저 찾는 첫노드만 반환
	/// @param sQueryStr		[in] 패턴식
	CCXmlNode SelectSingleNode(TCHAR* sQueryStr);
	/// 패턴식에 맞는 여러개의 노드를 찾는다.
	/// @param sQueryStr		[in] 패턴식
	CCXmlDomNodeListPtr	SelectNodes(TCHAR* sQueryStr);

	CCXmlNode& operator= (CCXmlNode aNode) { m_pDomNode = aNode.GetXmlDomNodePtr(); return *this; }
};

/// IXMLDOMElement 래핑 클래스
class CCXmlElement: public CCXmlNode
{
public:
	/// Default constructor.
	CCXmlElement() { }
	CCXmlElement(CCXmlDomElementPtr a_pDomElement)	{ m_pDomNode = a_pDomElement; }
	CCXmlElement(CCXmlDomNodePtr a_pDomNode)			{ m_pDomNode = a_pDomNode; }
	CCXmlElement(CCXmlNode aNode)						{ m_pDomNode = aNode.GetXmlDomNodePtr(); }
	/// Default destructor.
	virtual ~CCXmlElement() { }
	/// 태그 이름을 반환한다.
	/// @param sOutStr			[out] 태그 이름
	void GetTagName(char* sOutStr) { CCXmlNode::GetNodeName(sOutStr); }
	
	/// 해당태그로 둘러싸인 Contents를 반환
	/// @param sOutStr			[out] 반환값
	void GetContents(char* sOutStr) { CCXmlNode::GetText(sOutStr); }
	void GetContents(int* ipOutValue);
	void GetContents(bool* bpOutValue);
	void GetContents(float* fpOutValue);
	void GetContents(string* pstrValue);

	/// Contents를 설정
	void SetContents(const char* sStr) { CCXmlNode::SetText(sStr); }
	void SetContents(int iValue);
	void SetContents(bool bValue);
	void SetContents(float fValue);

	bool GetChildContents(char* sOutStr, const char* sChildTagName, int nMaxCharNum = -1);
	bool GetChildContents(int* iOutValue, const char* sChildTagName);
	bool GetChildContents(float* fOutValue, const char* sChildTagName);
	bool GetChildContents(bool* bOutValue, const char* sChildTagName);

	/// 속성값을 반환 - 선형검색이라 시간은 오래 걸린다.
	/// @param sOutText			[out] 반환될 속성값
	/// @param sAttrName		[in] 속성 이름
	bool GetAttribute(char* sOutText, const char* sAttrName, char* sDefaultText = "");
	bool GetAttribute(int* ipOutValue, const char* sAttrName, int nDefaultValue = 0);
	bool GetAttribute(bool* bOutValue, const char* sAttrName, bool bDefaultValue = false);
	bool GetAttribute(float* fpOutValue, const char* sAttrName, float fDefaultValue = 0.0f);
	bool GetAttribute(string* pstrOutValue, const char* sAttrName, char* sDefaultValue = "");
	/// 속성을 추가한다.
	/// @param sAttrName		[in] 속성 이름
	/// @param sAttrText		[in] 속성값
	bool AddAttribute(const char* sAttrName, const char* sAttrText);
	bool AddAttribute(const char* sAttrName, int iAttrValue);
	bool AddAttribute(const char* sAttrName, bool bAttrValue);
	/// 속성을 재설정. 해당속성이 존재하지 않으면 추가한다.
	/// @param sAttrName		[in] 속성 이름
	/// @param sAttrText		[in] 속성값
	bool SetAttribute(const char* sAttrName, char* sAttrText);
	bool SetAttribute(const char* sAttrName, int iAttrValue);
	/// 속성을 지운다.
	bool RemoveAttribute(const char* sAttrName);

	int GetAttributeCount();
	/// 속성값 반환 - 위의 GetAttribute함수보다 빠르다.
	void GetAttribute(int index, char* szoutAttrName, char* szoutAttrValue);

	/// 자식 Element를 추가한다.
	/// @param sTagName			[in] 태그 이름
	/// @param sTagText			[in] 태그 Contents
	bool AppendChild(const char* sTagName, const char* sTagText = NULL);
	bool AppendChild(::CCXmlElement aChildElement);

	/// 자식 Element를 추가한다.
	/// @param sTagName			[in] 태그 이름
	CCXmlElement	CreateChildElement(const char* sTagName);

	/// 텍스트를 추가한다.
	bool AppendText(const char* sText);

	CCXmlElement& operator= (::CCXmlElement aElement) { m_pDomNode = aElement.GetXmlDomNodePtr(); return *this; }
	CCXmlElement& operator= (CCXmlNode aNode) { m_pDomNode = aNode.GetXmlDomNodePtr(); return *this; }
};

/// XML Document 클래스.
class CCXmlDocument
{
private:
	bool							m_bInitialized;		///< 초기화되었는의 여부
	CCXmlDomDocPtr*					m_ppDom;			///< IXMLDOMDocument
protected:

public:
	/// Default constructor.
	CCXmlDocument();
	/// Default destructor.
	virtual ~CCXmlDocument();

	/// 초기화. 이 함수는 이 클래스를 사용하기 전에 꼭 수행되어야 한다
	bool				Create();
	/// 마무리.
	bool				Destroy();

	/// XML 파일을 읽는다.
	bool				LoadFromFile(const char* m_sFileName);
	/// XML 메모리버퍼로부터 읽는다.
	bool				LoadFromMemory(char* szBuffer, LANGID lanid = LANG_KOREAN);

	/// XML 파일로 저장.
	bool				SaveToFile(const char* m_sFileName);

	/// processing instruction node를 생성.
	bool				CreateProcessingInstruction( const char* szHeader = "version=\"1.0\"");
	/// Node를 삭제. 만약 Child Node가 있으면 함께 삭제한다.
	bool				Delete(CCXmlNode* pNode);

	CCXmlElement			CreateElement(const char* sName);

	bool				AppendChild(CCXmlNode node);

	/// XML DOM tree 최상위 Node를 반환.
	CCXmlDomDocPtr		GetDocument()	{ return (*m_ppDom); }
	/// 최상위 Element를 반환.
	CCXmlElement			GetDocumentElement()	{ return CCXmlElement((*m_ppDom)->documentElement); }

	/// 태그 이름으로 Element를 찾는다. 가장 먼저 찾는 첫노드만 반환.
	CCXmlNode			FindElement(TCHAR* sTagName);

	bool				IsInitialized() { return m_bInitialized; }
};

// Utils
#define _BSTRToAscii(s) (const char*)(_bstr_t)(s)
BSTR _AsciiToBSTR(const char* ascii);
