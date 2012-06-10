#include "stdafx.h"
#include "CCHttpSpooler.h"


void CCHttpSpooler::OnCreate()
{
}

void CCHttpSpooler::OnDestroy()
{
	Shutdown();
}

void CCHttpSpooler::Run()
{
	while(CheckShutdown() == false) {
		CCHttpSpoolerNode* pNode = m_RequestQueue.Pop();
		if (pNode) {
			if (m_AsyncHttp.Get(pNode->GetURL().c_str()))
				m_ResultQueue.Post(pNode);
			else
				delete pNode;
		}
		Sleep(100);
	}
}
