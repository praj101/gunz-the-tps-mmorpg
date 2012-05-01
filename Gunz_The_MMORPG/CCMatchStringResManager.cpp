#include "stdafx.h"
#include "CCMatchStringResManager.h"

CCMatchStringResManager::CCMatchStringResManager() : CCBaseStringResManager()
{

}

CCMatchStringResManager::~CCMatchStringResManager()
{

}

void CCMatchStringResManager::MakeInstance()
{
	_ASSERT(m_pInstance == NULL);
	m_pInstance = new CCMatchStringResManager();
}
