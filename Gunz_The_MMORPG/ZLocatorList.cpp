#include "StdAfx.h"
#include ".\zlocatorlist.h"
#include "CCZFileSystem.h"

ZLocatorList::ZLocatorList()
{
}

ZLocatorList::~ZLocatorList()
{
}

/*
bool ZLocatorList::Init( CCZFileSystem* pFileSystem, const char* pszListFile )
{
	if( (0 == pszListFile) || (0 == strlen(pszListFile)) ) 
		return false;

	m_LocatorIPList.clear();

	if( !LoadXML(pFileSystem, pszListFile) )
		return false;

	return true;
}


bool ZLocatorList::LoadXML( CCZFileSystem* pFileSystem, const char* pszListFile )
{
	if( (0 == pszListFile) || (0 == strlen(pszListFile)) )
		return false;

	CCXmlDocument	xmlIniData;
	xmlIniData.Create();

	char *buffer;
	CCZFile mzf;

	if( 0 != pFileSystem )
	{
		if(!mzf.Open(pszListFile,pFileSystem)) 
		{
			if(!mzf.Open(pszListFile)) 
			{
				xmlIniData.Destroy();
				return false;
			}
		}
	}
	else
	{
		if(!mzf.Open(pszListFile))
		{
			xmlIniData.Destroy();
			return false;
		}
	}

	buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer,mzf.GetLength());

	if(!xmlIniData.LoadFromMemory(buffer))
	{
		xmlIniData.Destroy();
		return false;
	}
	delete[] buffer;
	mzf.Close();
	//	<------------------

	CCXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, "LOCATOR"))
		{
			ParseLocatorList(chrElement);
		}
	}

	xmlIniData.Destroy();
	
	return true;
}
*/

bool ZLocatorList::ParseLocatorList( CCXmlElement& element )
{
	int iCount = element.GetChildNodeCount();
	CCXmlElement chrElement;
	char szTagName[256];

	for (int i = 0; i < iCount; i++)
	{
		chrElement = element.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, "LOCATOR"))
		{
			ParseLocator(chrElement);
		}
	}
	return true;
}

bool ZLocatorList::ParseLocator( CCXmlElement& element )
{
#define MAX_IP_LEN 64

	int n;
	char ip[ MAX_IP_LEN ] = {0,};
	element.GetAttribute( &n, "id" );
	element.GetAttribute( ip, "IP" );

#ifdef _DEBUG
	char szDbgInfo[ 1024 ];

	_snprintf( szDbgInfo, 1023, "Locator id:%d ip:%s\n", n, ip );
	OutputDebugString( szDbgInfo );
#endif	

	m_LocatorIPList.push_back( ip );

	return true;
}

void ZLocatorList::AddIP( const char* szIP)
{
	m_LocatorIPList.push_back( szIP);
}
