#include "stdafx.h"
#include "CCSmartRefresh.h"


#define TIME_CATEGORY_UPDATE	500		// 0.5 sec


//// CCRefreshCategory ////
CCRefreshCategory::CCRefreshCategory(int nCategory)
{
	m_nCategory = nCategory;
	SetChecksum(0);
	SetLastUpdateTick(0);
}

CCRefreshCategory::~CCRefreshCategory()
{
}

bool CCRefreshCategory::UpdateChecksum(unsigned long nTick)
{
	if (nTick > GetLastUpdateTick() + TIME_CATEGORY_UPDATE) {
		SetLastUpdateTick(nTick);
		return OnUpdateChecksum(nTick);
	}
	return false;
}

//// CCRefreshClient ////
CCRefreshClient::CCRefreshClient()
{
	SetCategory(0);
	Enable(false);
	SetChecksum(0);
	SetLastUpdatedTime(0);
}

CCRefreshClient::~CCRefreshClient()
{
}

bool CCRefreshClient::Sync(unsigned long nChecksum)
{
	if (OnSync(nChecksum) == true) {
		SetChecksum(nChecksum);
		return true;
	} else {
		return false;
	}
}

//// CCSmartRefresh ////
CCSmartRefresh::CCSmartRefresh()
{
}

CCSmartRefresh::~CCSmartRefresh()
{
	Clear();
}

void CCSmartRefresh::Clear()
{
	while(m_CategoryMap.size() > 0) {
		CCRefreshCategory* pCategory = (*m_CategoryMap.begin()).second;
		delete pCategory;
		m_CategoryMap.erase(m_CategoryMap.begin());
	}
}

CCRefreshCategory* CCSmartRefresh::GetCategory(int nCategory)
{
	CCRefreshCategoryMap::iterator i = m_CategoryMap.find(nCategory);
	if(i==m_CategoryMap.end())
		return NULL;
	else
		return (*i).second;
	return NULL;
}

void CCSmartRefresh::AddCategory(CCRefreshCategory* pCategory)
{
	m_CategoryMap.insert(CCRefreshCategoryMap::value_type(pCategory->GetCategory(), pCategory));
}

void CCSmartRefresh::UpdateCategory(unsigned int nTick)
{
	for (CCRefreshCategoryMap::iterator i=m_CategoryMap.begin(); i!=m_CategoryMap.end(); i++) {
		CCRefreshCategory* pCategory = (*i).second;
		pCategory->UpdateChecksum(nTick);
	}
}

bool CCSmartRefresh::SyncClient(CCRefreshClient* pClient)
{
	CCRefreshCategory* pCategory = GetCategory(pClient->GetCategory());
	if (pCategory == NULL) 
		return false;

	if (pClient->IsEnable() == false)
		return false;

	if (pCategory->GetChecksum() == pClient->GetChecksum())
		return false;

	return pClient->Sync(pCategory->GetChecksum());
}
