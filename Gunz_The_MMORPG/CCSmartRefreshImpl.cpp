#include "stdafx.h"
#include "CCSmartRefreshImpl.h"
#include "CCMatchServer.h"
#include "CCMatchObject.h"
#include "CCMatchChannel.h"


//// MRefreshCategoryChannel ////
bool CCRefreshCategoryChannelImpl::OnUpdateChecksum(unsigned long nTick)
{
	unsigned long nChecksum = 0;
	int nPage = GetCategory();
	MChannelUserArray* pArray = GetMatchChannel()->GetUserArray();
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

//// MRefreshClientChannel ////
bool CCRefreshClientChannelImpl::OnSync(unsigned long nChecksum)
{
	CCMatchObject* pObj = GetMatchObject();
	CCMatchServer::GetInstance()->ChannelResponsePlayerList(pObj->GetUID(), pObj->GetChannelUID(), GetCategory());

	return true;
}


//// MRefreshCategoryClanMember ////
bool MRefreshCategoryClanMemberImpl::OnUpdateChecksum(unsigned long nTick)
{
	MMatchClan* pClan = GetMatchClan();
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


//// MRefreshClientClanMemberImpl ////
bool MRefreshClientClanMemberImpl::OnSync(unsigned long nChecksum)
{
	CCMatchObject* pObj = GetMatchObject();
	CCMatchServer::GetInstance()->ResponseClanMemberList(pObj->GetUID());
	return true;
}
