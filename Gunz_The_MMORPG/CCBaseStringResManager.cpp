#include "stdafx.h"
#include "CCBaseStringResManager.h"

#define TOK_XML_STRING_HEADER		"STR:"


CCBaseStringResManager* CCBaseStringResManager::m_pInstance = NULL;

CCBaseStringResManager* CCBaseStringResManager::GetInstance()
{
	_ASSERT(m_pInstance != NULL);

	return m_pInstance;
}

void CCBaseStringResManager::FreeInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CCBaseStringResManager::CCBaseStringResManager()
{

}

CCBaseStringResManager::~CCBaseStringResManager()
{
	
}

bool CCBaseStringResManager::Init(const char* szPath, const int nLangID, CCZFileSystem* pfs )
{
	m_strPath = szPath;	
	m_pFS = pfs;

	string strFileName;

	// string table
	strFileName = m_strPath + FILENAME_STRING_TABLE;
	if (!m_StringTable.Initialize(strFileName.c_str(), nLangID, pfs))
	{
		cclog("Error!! - StringTable Initalize Failed\n");
		_ASSERT(0);
		return false;
	}

	// error table
	strFileName = m_strPath + FILENAME_ERROR_TABLE;
	if (!m_ErrorTable.Initialize(strFileName.c_str(), nLangID, pfs))
	{
		cclog("Error!! - ErrorTable Initalize Failed\n");
		_ASSERT(0);
		return false;
	}

	bool ret = OnInit();
	return ret;
}


const char* CCBaseStringResManager::GetErrorStr(int nID)
{
	return m_ErrorTable.GetStr(nID);
}

const char* CCBaseStringResManager::GetString(string& key)
{
	return m_StringTable.GetStr(key);
}

const char* CCBaseStringResManager::GetStringFromXml(const char* str)
{
	int nHeaderLen = (int)strlen(TOK_XML_STRING_HEADER);
	int nStrLen = (int)strlen(str);

	if (nStrLen <= nHeaderLen) return str;
	if (strnicmp(str, TOK_XML_STRING_HEADER, nHeaderLen)) return str;

	string key;
	key = &str[nHeaderLen];

	return GetString(key);
}