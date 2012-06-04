#pragma once

#include "CCStringRes.h"

const char FILENAME_ERROR_TABLE[]		= "cserror.xml";
const char FILENAME_STRING_TABLE[]		= "strings.xml";
const char FILENAME_MESSAGES[]			= "messages.xml";



class CCBaseStringResManager
{
protected:
	static CCBaseStringResManager*	m_pInstance;
	string							m_strPath;
	CCZFileSystem*					m_pFS;
	CCStringRes<int>					m_ErrorTable;
	CCStringRes<string>				m_StringTable;

	virtual bool OnInit() { return true; }
public:
	CCBaseStringResManager();
	virtual ~CCBaseStringResManager();
	bool Init(const char* szPath, const int nLangID, CCZFileSystem* pfs=NULL );
	static CCBaseStringResManager* GetInstance();
	static void FreeInstance();

	const char* GetErrorStr(int nID);
	const char* GetString(string& key);
	const char* GetStringFromXml(const char* str);
};


inline CCBaseStringResManager* CCGetStringResManager()
{
	return CCBaseStringResManager::GetInstance();
}
