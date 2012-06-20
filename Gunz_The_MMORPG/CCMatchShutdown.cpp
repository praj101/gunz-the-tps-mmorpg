#include "stdafx.h"
#include "CCMatchShutdown.h"
#include "CCMatchServer.h"
#include "CCSharedCommandTable.h"


CCMatchShutdown::~CCMatchShutdown()
{
	for (vector<MShutdownNotify*>::iterator i = m_ShutdownNotifyArray.begin(); i != m_ShutdownNotifyArray.end(); ++i)
	{
		delete (*i);
	}

	m_ShutdownNotifyArray.clear();
}

bool CCMatchShutdown::LoadXML_ShutdownNotify(const char* pszFileName)
{
/*	m_ShutdownNotifyArray.push_back(new MShutdownNotify(0, 1000, "Shutdown Started"));
	m_ShutdownNotifyArray.push_back(new MShutdownNotify(1, 1000, "5"));
	m_ShutdownNotifyArray.push_back(new MShutdownNotify(2, 1000, "4"));
	m_ShutdownNotifyArray.push_back(new MShutdownNotify(3, 1000, "3"));
	m_ShutdownNotifyArray.push_back(new MShutdownNotify(4, 1000, "2"));
	m_ShutdownNotifyArray.push_back(new MShutdownNotify(5, 1000, "1"));
	m_ShutdownNotifyArray.push_back(new MShutdownNotify(5, 1000, "Shutdown Complete"));
*/

	#define MTOK_SHUTDOWNNOTIFY		"SHUTDOWNNOTIFY"
	#define MTOK_ATTR_DELAY			"delay"

	CCXmlDocument	xmlIniData;
	xmlIniData.Create();

	if (!xmlIniData.LoadFromFile(pszFileName))
	{
		xmlIniData.Destroy();
		return false;
	}

	CCXmlElement rootElement, childElement;
	char szTagName[256];
	char szBuf[256];

	rootElement = xmlIniData.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		childElement = rootElement.GetChildNode(i);
		childElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, MTOK_SHUTDOWNNOTIFY))
		{
			childElement.GetAttribute(szBuf, MTOK_ATTR_DELAY);
			int nDelay = atoi(szBuf);
			childElement.GetContents(szBuf);

			//m_ShutdownNotifyArray.push_back(new MShutdownNotify(nDelay, CCGetStringResManager()->GetStringFromXml(szBuf)));
			m_ShutdownNotifyArray.push_back(new MShutdownNotify(nDelay, szBuf));	// 그냥 문자열리소스ID를 저장하게 함, 클라에서 ID를 현재 언어로 변환하도록.
		}
	}

	xmlIniData.Destroy();
	return true;
}

void CCMatchShutdown::Start(unsigned long nClock)
{
	m_bShutdown = true;
	m_nProgressIndex = 0;
	m_nTimeLastProgress = nClock;
}

void CCMatchShutdown::SetProgress(int nIndex, unsigned long nClock)
{
	m_nProgressIndex = nIndex;
	m_nTimeLastProgress = nClock;
}

void CCMatchShutdown::Notify(int nIndex)
{
	char* pszMsg = m_ShutdownNotifyArray[nIndex]->GetString();
	CCMatchServer* pServer = CCMatchServer::GetInstance();
	
	CCCommand* pCmd = pServer->CreateCommand(MC_ADMIN_ANNOUNCE, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamCCUID(CCUID(0,0)));
	pCmd->AddParameter(new CCCmdParamStr(pszMsg));
	pCmd->AddParameter(new CCCmdParamUInt(ZAAT_CHAT));
	pServer->RouteToAllClient(pCmd);

#ifdef _DEBUG
	cclog( "CCMatchShutdown::Notify - Notify : %s\n", pszMsg );
#endif
}

void CCMatchShutdown::Terminate()
{
	CCMatchServer* pServer = CCMatchServer::GetInstance();
	pServer->Shutdown();
}

void CCMatchShutdown::OnRun(unsigned long nClock)
{
	if (IsShutdown() == false) return;

	int nIndex = GetProgressIndex();
	if (nIndex < (int)m_ShutdownNotifyArray.size())
	{
		MShutdownNotify* pNotify = m_ShutdownNotifyArray[nIndex];
		if (nClock - GetTimeLastProgress() < pNotify->GetDelay())
			return;
		Notify(nIndex);
		SetProgress(nIndex+1, nClock);
	}
	else if (nIndex == m_ShutdownNotifyArray.size())
	{
		Terminate();
		SetProgress(nIndex+1, nClock);
	}
}
