#include "stdafx.h"
#include "CCMatchFriendInfo.h"
#include "CCMatchServer.h"
#include "CCMatchChannel.h"

CCMatchFriendInfo::CCMatchFriendInfo(){
}

CCMatchFriendInfo::~CCMatchFriendInfo(){
	while (m_FriendList.size() > 0) {
		CCMatchFriendList::iterator i = m_FriendList.begin();
		CCMatchFriendNode* pNode = (CCMatchFriendNode*)(*i);
		delete pNode;
		m_FriendList.pop_front();
	}
}

bool CCMatchFriendInfo::Add(unsigned long nFriendCID, unsigned short nFavorite, const char* pszName){
	if (Find(nFriendCID) != NULL)
		return false;

	CCMatchFriendNode* pNode = new CCMatchFriendNode;
	pNode->nFriendCID = nFriendCID;
	pNode->nFavorite = nFavorite;
	strcpy(pNode->szName, pszName);
	strcpy(pNode->szDescription, "");
	m_FriendList.push_back(pNode);

	return true;
}

void CCMatchFriendInfo::Remove(const char* pszName){
	for (CCMatchFriendList::iterator i=m_FriendList.begin(); i!= m_FriendList.end(); i++) {
		CCMatchFriendNode* pNode = (*i);
		if (stricmp(pNode->szName, pszName)==0) {
			m_FriendList.erase(i);
			delete pNode;
			return;
		}
	}
}

CCMatchFriendNode* CCMatchFriendInfo::Find(unsigned long nFriendCID){
	for (CCMatchFriendList::iterator i=m_FriendList.begin(); i!= m_FriendList.end(); i++) {
		CCMatchFriendNode* pNode = (*i);
		if (pNode->nFriendCID == nFriendCID)
			return pNode;
	}
	return NULL;
}

CCMatchFriendNode* CCMatchFriendInfo::Find(const char* pszName){
	for (CCMatchFriendList::iterator i=m_FriendList.begin(); i!= m_FriendList.end(); i++) {
		CCMatchFriendNode* pNode = (*i);
		if (stricmp(pNode->szName, pszName)==0)
			return pNode;
	}
	return NULL;
}

void CCMatchFriendInfo::UpdateDesc(){
	CCMatchServer* pServer = CCMatchServer::GetInstance();
	for (CCMatchFriendList::iterator i=m_FriendList.begin(); i!= m_FriendList.end(); i++) {
		CCMatchFriendNode* pNode = (*i);
		pNode->szDescription[0] = NULL;

		CCMatchObject* pObj = pServer->GetPlayerByName(pNode->szName);
		if (pObj) {
			char szDesc[CHANNELNAME_LEN*2]="";

			pNode->nState = pObj->GetPlace();
			CCMatchChannel* pChannel = pServer->FindChannel(pObj->GetChannelUID());
			if (pChannel) {
				sprintf(szDesc, "Channel '%s'", pChannel->GetName());
				strncpy(pNode->szDescription, szDesc, MATCH_SIMPLE_DESC_LENGTH);
				pNode->szDescription[MATCH_SIMPLE_DESC_LENGTH-1] = NULL;
			} else {
				strcpy(pNode->szDescription, "Unknown Channel");
			}
		} else {
			pNode->nState = MMP_OUTSIDE;
			strcpy(pNode->szDescription, "Not Logged on");
		}
	}
}
