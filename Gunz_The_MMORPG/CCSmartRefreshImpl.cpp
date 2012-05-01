#include "stdafx.h"
#include "CCSmartRefreshImpl.h"
#include "CCMatchServer.h"
#include "CCMatchObject.h"
#include "CCMatchChannel.h"


//// CCRefreshCategoryChannel ////
bool CCRefreshCategoryChannelImpl::OnUpdateChecksum(unsigned long nTick)
{
	unsigned long nChecksum = 0;
	int nPage = GetCategory();
	CCChannelUserArray* pArray = GetMatchChannel()->GetUserArray();
	for (int i=0; i<pArray->GetPageSize(); i++) {
		CCMatchObject* pObj = pArray->Get(nPage,i);
		if (pObj == NULL) continue;

		nChecksum += pObj->GetUID().High;
		nChecksum += pObj->GetUID().Low;
		nChecksum += pObj->GetPlace();

		if (pObj->GetCharInfo())
		{
			nChecksum += pObj->GetCharInfo()->m_ClanInfo.m_nClanID;
			nChecksum += pObj->GetCharInfo()->m_ClanInfo.m_nGrade;
		}
	}
	SetChecksum(nChecksum);
	return true;
}

//// CCRefreshClientChannel ////
bool CCRefreshClientChannelImpl::OnSync(unsigned long nChecksum)
{
	CCMatchObject* pObj = GetMatchObject();
	CCMatchServer::GetInstance()->ChannelResponsePlayerList(pObj->GetUID(), pObj->GetChannelUID(), GetCategory());

	return true;
}


//// CCRefreshCategoryClanMember ////
bool CCRefreshCategoryClanMemberImpl::OnUpdateChecksum(unsigned long nTick)
{
	CCMatchClan* pClan = GetMatchClan();
	unsigned long nChecksum = 0;
	for (CCUIDRefCache::iterator itor= pClan->GetMemberBegin(); itor != pClan->GetMemberEnd(); ++itor) 
	{
		CCMatchObject* pObj = (CCMatchObject*)(*itor).second;

		nChecksum += pObj->GetUID().High;
		nChecksum += pObj->GetUID().Low;
		nChecksum += pObj->GetPlace();

		if (pObj->GetCharInfo())
		{
			nChecksum += pObj->GetCharInfo()->m_ClanInfo.m_nClanID;
			nChecksum += pObj->GetCharInfo()->m_ClanInfo.m_nGrade;
		}
	}
	SetChecksum(nChecksum);
	return true;
}


//// CCRefreshClientClanMemberImpl ////
bool CCRefreshClientClanMemberImpl::OnSync(unsigned long nChecksum)
{
	CCMatchObject* pObj = GetMatchObject();
	CCMatchServer::GetInstance()->ResponseClanMemberList(pObj->GetUID());
	return true;
}
