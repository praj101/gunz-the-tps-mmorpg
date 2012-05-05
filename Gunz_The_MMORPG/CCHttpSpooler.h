#pragma once

#include<list>
using namespace std;

#include "CCThread.h"
#include "CCSync.h"
#include "CCAsyncHttp.h"


class CCHttpSpoolerNode {
protected:
	unsigned int	m_nID;
	unsigned int	m_nChecksum;
	string			m_strURL;
public:
	CCHttpSpoolerNode(unsigned int nID, unsigned int nChecksum, const string& strURL) {
		m_nID = nID;
		m_nChecksum = nChecksum;
		m_strURL = strURL;
	}
	virtual ~CCHttpSpoolerNode()	{}
	unsigned int GetID()		{ return m_nID; }
	string GetURL()				{ return m_strURL; }
	unsigned int GetChecksum()	{ return m_nChecksum; }
};

class CCHttpSpoolerQueue {
protected:
	list<CCHttpSpoolerNode*>	m_SpoolQueue;
	CCCriticalSection		m_csLock;
public:
	bool CheckExist(unsigned int nID)
	{
		bool bFound = false;
		m_csLock.Lock();
		for (list<CCHttpSpoolerNode*>::iterator i=m_SpoolQueue.begin(); i!=m_SpoolQueue.end(); i++) {
			CCHttpSpoolerNode* pNode = (*i);
			if (pNode->GetID() == nID)
				bFound = true;
		}
		m_csLock.Unlock();
		return bFound;
	}
	void Post(CCHttpSpoolerNode* pSpoolNode) 
	{
		m_csLock.Lock();
		m_SpoolQueue.push_back(pSpoolNode);
		m_csLock.Unlock();
	}
	CCHttpSpoolerNode* Pop()
	{
		CCHttpSpoolerNode* pSpoolNode = NULL;
		m_csLock.Lock();
		if (!m_SpoolQueue.empty()) {
			pSpoolNode = (*m_SpoolQueue.begin());
			m_SpoolQueue.pop_front();
		}            
		m_csLock.Unlock();
		return pSpoolNode;
	}
};

class CCHttpSpooler : public CCThread {
protected:
	bool				m_bShutdown;

	CCAsyncHttp			m_AsyncHttp;

	CCHttpSpoolerQueue	m_RequestQueue;
	CCHttpSpoolerQueue	m_ResultQueue;

protected:
	bool CheckShutdown()	{ return m_bShutdown; }
	void Shutdown()			{ m_bShutdown = true; }

	virtual void OnCreate();
	virtual void OnDestroy();
	virtual void Run();

public:
	CCHttpSpooler()			{ m_bShutdown = false; }
	virtual ~CCHttpSpooler()	{}
	
	const char* GetBasePath()				{ return m_AsyncHttp.GetBasePath(); }
	void SetBasePath(const char* pszPath)	{ m_AsyncHttp.SetBasePath(pszPath); }

	void Post(unsigned int nID, unsigned int nChecksum, const string strURL)
	{
		if (m_RequestQueue.CheckExist(nID) || m_ResultQueue.CheckExist(nID))
			return;

		CCHttpSpoolerNode* pNode = new CCHttpSpoolerNode(nID, nChecksum, strURL);
		m_RequestQueue.Post(pNode);
	}
	bool Pop(unsigned int* poutID, unsigned int* poutChecksum, string* poutstrURL)
	{
		CCHttpSpoolerNode* pNode = m_ResultQueue.Pop();
		if (pNode == NULL) return false;

		*poutID = pNode->GetID();
		*poutChecksum = pNode->GetChecksum();
		*poutstrURL = pNode->GetURL();

		delete pNode;
		return true;
	}
};