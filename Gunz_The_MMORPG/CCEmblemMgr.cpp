#include "stdafx.h"
#include "CCXml.h"
#include "CCEmblemMgr.h"
#include "FileInfo.h"
#include <shlwapi.h>
#include "ZFilePath.h"
#include "ZPost.h"


#define MTOK_EMBLEM_CLID			"CLID"
#define MTOK_EMBLEM					"EMBLEM"
#define MTOK_EMBLEM_URL				"URL"
#define MTOK_EMBLEM_CHECKSUM		"CHECKSUM"
#define MTOK_EMBLEM_TIMELASTUSED	"TIMELASTUSED"

#define MTICK_EMBLEM_SAVE_THRESHOLD		5000

#pragma comment(lib, "Wininet.lib")

bool CCEmblemMgr::InitDefaut()
{
	if(GetMyDocumentsPath(m_szEmblemBaseDir)) {
		// EmblemBaseFolder
		strcat(m_szEmblemBaseDir, GUNZ_FOLDER);
		strcat(m_szEmblemBaseDir, CCPATH_EMBLEMFOLDER);
		
		// EmblemDataFile
		strcpy(m_szEmblemDataFile, m_szEmblemBaseDir);
		strcat(m_szEmblemDataFile, CCPATH_EMBLEMFILE);
		return true;
	} else {
		return false;
	}
}

void CCEmblemMgr::Create()
{
	InitDefaut();

	m_nTotalRequest = 0;
	m_nCachedRequest = 0;

	m_HttpSpooler.SetBasePath(GetEmblemBaseDir());
	m_HttpSpooler.Create();
}

void CCEmblemMgr::Destroy()
{
	if (CheckSaveFlag())
		SaveCache();

	ClearCache();
	m_HttpSpooler.Destroy();
}

bool CCEmblemMgr::CreateCache()
{
	TCHAR szEmblemPath[MAX_PATH]="";
	TCHAR szPath[MAX_PATH]="";

	if(GetMyDocumentsPath(szPath)) {
		strcpy(szEmblemPath, szPath);
		strcat(szEmblemPath, GUNZ_FOLDER);
		CreatePath(szEmblemPath);

		strcat(szEmblemPath, CCPATH_EMBLEMFOLDER);
		CreatePath(szEmblemPath);

		strcat(szEmblemPath, CCPATH_EMBLEMFILE);
	} else {
		return false;
	}

	CCXmlDocument	xmlDoc;

	xmlDoc.Create();
	bool bResult = xmlDoc.SaveToFile(szEmblemPath);
	xmlDoc.Destroy();

	return bResult;
}

bool CCEmblemMgr::LoadCache()
{
	CCXmlDocument	xmlDoc;
	xmlDoc.Create();

	if (!xmlDoc.LoadFromFile(GetEmblemDataFile()))
	{
		xmlDoc.Destroy();
		return false;
	}

	::CCXmlElement rootElement,emblemElement,childElement;
	char szTagName[256];

	rootElement = xmlDoc.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		emblemElement = rootElement.GetChildNode(i);
		emblemElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, MTOK_EMBLEM))
		{
			int nCLID = -1;
			char szURL[256]="";
			int nChecksum = -1;
			time_t tmLastUsed = 0;

			int nEmblemChildCount = emblemElement.GetChildNodeCount();

			::CCXmlElement chrElement;
			for (int j = 0; j < nEmblemChildCount; j++)
			{
				chrElement = emblemElement.GetChildNode(j);
				chrElement.GetTagName(szTagName);
				if (szTagName[0] == '#') continue;

				if (!strcmp(szTagName, MTOK_EMBLEM_CLID))
				{
					chrElement.GetContents(&nCLID);
				}
				else if (!strcmp(szTagName, MTOK_EMBLEM_URL))
				{
					chrElement.GetContents(szURL);
				}
				else if (!strcmp(szTagName, MTOK_EMBLEM_CHECKSUM))
				{
					chrElement.GetContents(&nChecksum);
				}
			}

			if ((nCLID != -1) && (szURL[0] != 0) && (nChecksum != -1))
			{
				emblemElement.GetChildContents((int*)&tmLastUsed, MTOK_EMBLEM_TIMELASTUSED); // 없어도 로드되게
				RegisterEmblem(nCLID, szURL, nChecksum, tmLastUsed);
			}
		}
	}

	xmlDoc.Destroy();
	return true;
}

bool CCEmblemMgr::PrepareCache()
{
	if (LoadCache()) {
		return true;
	} else {
		return CreateCache();
	}
}


static bool CompareEmblem(CCEmblemNode* left, CCEmblemNode* right)
{ 
	double fDiff = difftime(left->GetTimeLastUsed(), right->GetTimeLastUsed());

	return 0.0 < fDiff;
}

bool CCEmblemMgr::SaveCache()
{
	list<CCEmblemNode*> sortedQueue;
	for (CCEmblemMap::iterator i=m_EmblemMap.begin(); i!=m_EmblemMap.end(); i++) {
		CCEmblemNode* pNode = (*i).second;
		sortedQueue.push_back(pNode);
	}
	sortedQueue.sort(CompareEmblem);

	CCXmlDocument	xmlDoc;
	char szBuf[256]="";

	xmlDoc.Create();
	xmlDoc.CreateProcessingInstruction();

	::CCXmlElement	rootElement;

	rootElement=xmlDoc.CreateElement("XML");

	xmlDoc.AppendChild(rootElement);

	int nCount = 0;
	for (list<CCEmblemNode*>::iterator i=sortedQueue.begin(); i!=sortedQueue.end(); i++) {
		CCEmblemNode* pNode = (*i);

		rootElement.AppendText("\n\t");

		::CCXmlElement	emblemElement = rootElement.CreateChildElement(MTOK_EMBLEM);
		emblemElement.AppendText("\n\t\t");

		::CCXmlElement	childElement;

		sprintf(szBuf,"%u", pNode->GetCLID());
		childElement = emblemElement.CreateChildElement(MTOK_EMBLEM_CLID);
		childElement.SetContents(szBuf);

		emblemElement.AppendText("\n\t\t");

		childElement = emblemElement.CreateChildElement(MTOK_EMBLEM_URL);
		childElement.SetContents(pNode->GetURL());

		emblemElement.AppendText("\n\t\t");

		sprintf(szBuf,"%u", pNode->GetChecksum());
		childElement = emblemElement.CreateChildElement(MTOK_EMBLEM_CHECKSUM);
		childElement.SetContents(szBuf);

		emblemElement.AppendText("\n\t\t");

		sprintf(szBuf,"%u", pNode->GetTimeLastUsed());
		childElement = emblemElement.CreateChildElement(MTOK_EMBLEM_TIMELASTUSED);
		childElement.SetContents(szBuf);

		emblemElement.AppendText("\n\t");

		if (++nCount >= 1000)
			break;
	}

	rootElement.AppendText("\n");

	bool bResult = xmlDoc.SaveToFile(GetEmblemDataFile());
	xmlDoc.Destroy();

	SetSaveFlag(false);

	sortedQueue.clear();

	return bResult;
}

void CCEmblemMgr::ClearCache()
{
	while(!m_EmblemMap.empty()) {
		CCEmblemMap::iterator itor = m_EmblemMap.begin();
		delete (*itor).second;
		m_EmblemMap.erase(itor);
	}
}

bool CCEmblemMgr::GetEmblemPath(char* pszFilePath, const char* pszURL)
{
	//// Parse URL //////////////////
	#define URLPATH_LEN	256
	char szFileName[URLPATH_LEN] = "";

	URL_COMPONENTS uc;
	ZeroMemory(&uc, sizeof uc);
	uc.dwStructSize = sizeof uc;
	uc.lpszUrlPath = szFileName;
	uc.dwUrlPathLength = URLPATH_LEN;

	if (!InternetCrackUrl(pszURL, lstrlen(pszURL), ICU_DECODE, &uc)) {
		// GetLastError()
		return false;
	}
	PathStripPath(szFileName);

	char szFullPath[_MAX_DIR];
	strcpy(szFullPath, GetEmblemBaseDir());
	strcat(szFullPath, "/");
	strcat(szFullPath, szFileName);

	// out
	strcpy(pszFilePath, szFullPath);

	return true;
}

bool CCEmblemMgr::GetEmblemPathByCLID(unsigned int nCLID, char* poutFilePath)
{
	CCEmblemMap::iterator i = m_EmblemMap.find(nCLID);
	if (i==m_EmblemMap.end())
		return false;

	CCEmblemNode* pEmblem = (*i).second;
	return GetEmblemPath(poutFilePath, pEmblem->GetURL());
}

bool CCEmblemMgr::CheckEmblem(unsigned int nCLID, unsigned long nChecksum)
{
	CCEmblemMap::iterator i = m_EmblemMap.find(nCLID);
	if (i==m_EmblemMap.end()) {
		return false;
	} else {
		CCEmblemNode* pEmblem = (*i).second;
		if (pEmblem->GetChecksum() == nChecksum) {
			pEmblem->UpdateTimeLastUsed();
			return true;
		} else {
			return false;
		}
	}
}

void CCEmblemMgr::PostDownload(unsigned int nCLID, unsigned int nChecksum, const char* pszURL)
{
	m_HttpSpooler.Post(nCLID, nChecksum, pszURL);
}

bool CCEmblemMgr::RegisterEmblem(unsigned int nCLID, const char* pszURL, unsigned long nChecksum, time_t tmLastUsed)
{
	CCEmblemMap::iterator i = m_EmblemMap.find(nCLID);
	if (i!=m_EmblemMap.end()) {
		delete (*i).second;
		m_EmblemMap.erase(i);
	}

	char szFilePath[_MAX_DIR]="";
	if (!GetEmblemPath(szFilePath, pszURL))
		return false;

	CCEmblemNode* pEmblem = new CCEmblemNode();
	pEmblem->SetCLID(nCLID);
	pEmblem->SetURL(pszURL);
	pEmblem->SetChecksum(nChecksum);
	pEmblem->SetTimeLastUsed(tmLastUsed);

	m_EmblemMap.insert(CCEmblemMap::value_type(nCLID, pEmblem));

	return true;
}

void CCEmblemMgr::NotifyDownloadDone(unsigned int nCLID, const char* pszURL)
{
	char szPath[_MAX_DIR]="";
	GetEmblemPathByCLID(nCLID, szPath);

	ZPostClanEmblemReady(nCLID, const_cast<char*>(pszURL));
}

bool CCEmblemMgr::ProcessEmblem(unsigned int nCLID, const char* pszURL, unsigned long nChecksum)
{
	m_nTotalRequest++;

	if (CheckEmblem(nCLID, nChecksum)) {
		m_nCachedRequest++;
		return true;
	} else {
		PostDownload(nCLID, nChecksum, pszURL);
		return false;
	}
}

void CCEmblemMgr::Tick(unsigned long nTick)
{
	int nRegisterCount = 0;

	unsigned int nCLID = 0;
	unsigned int nChecksum = 0;
	string strURL;

	while (m_HttpSpooler.Pop(&nCLID, &nChecksum, &strURL)) {
		char szFilePath[_MAX_DIR] = "";
		if (GetEmblemPath(szFilePath, strURL.c_str()) == false)
			return;

		//unsigned long nChecksum = GetFileCheckSum(szFilePath);
		if (RegisterEmblem(nCLID, strURL.c_str(), nChecksum)) {
			nRegisterCount++;
			CheckEmblem(nCLID, nChecksum);	// LastUsedTime 업데이트위해 공체크
			NotifyDownloadDone(nCLID, strURL.c_str());
		}
	}
	if (nRegisterCount > 0) {
		SetSaveFlag(true);
		SetLastSavedTick(nTick);
	}
/*	AUTOSAVE 봉인
	if (CheckSaveFlag() && (nTick - GetLastSavedTick() > MTICK_EMBLEM_SAVE_THRESHOLD))
	{
		SaveCache();
		SetLastSavedTick(nTick);

		// Logs
		char szLog[128];
		sprintf(szLog, "EmblemCache> Cached Emblem : %d/%d \n", 
				GetCachedRequest(), GetTotalRequest());
		OutputDebugString(szLog);
	} */
}
