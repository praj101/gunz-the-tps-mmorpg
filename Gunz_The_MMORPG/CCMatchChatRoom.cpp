#include "stdafx.h"
#include "CCMatchChatRoom.h"
#include "CCMatchServer.h"
#include "CCSharedCommandTable.h"



CCMatchChatRoom::CCMatchChatRoom(const CCUID& uidRoom, const CCUID& uidMaster, const char* pszName)
{
	m_uidChatRoom = uidRoom;
	m_uidMaster = uidMaster;

	const size_t NameLen = strlen( pszName) + 1;

	strncpy(m_szName, pszName
		, NameLen > MAX_CHATROOMNAME_STRING_LEN ? MAX_CHATROOMNAME_STRING_LEN : NameLen );
}

CCMatchChatRoom::~CCMatchChatRoom() 
{
}

bool CCMatchChatRoom::AddPlayer(const CCUID& uidPlayer)
{
	CCUIDRefCache::iterator i = m_PlayerList.find(uidPlayer);
	if (i != m_PlayerList.end())
		return false;

	CCMatchServer* pServer = CCMatchServer::GetInstance();
	CCMatchObject* pPlayer = pServer->GetObject(uidPlayer);
	if( !IsEnabledObject(pPlayer) )
		return false;

	pPlayer->SetChatRooCCUID(GetUID());

	m_PlayerList.Insert(uidPlayer, pPlayer);
	return true;
}

bool CCMatchChatRoom::IsFindPlayer(const CCUID& uidPlayer)	// 해당 플레이어가 있는지 확인한다
{
	CCUIDRefCache::iterator i = m_PlayerList.find(uidPlayer);
	if (i == m_PlayerList.end())
		return false;
	return true;										// 플레이어가 있으면 참 반환
}

void CCMatchChatRoom::RemovePlayer(const CCUID& uidPlayer)
{
	CCUIDRefCache::iterator i = m_PlayerList.find(uidPlayer);
	if (i != m_PlayerList.end()) {
		m_PlayerList.erase(i);
	
		// 2008.08.28 채팅방에 탈퇴후 /채팅 할말을 하면 채팅방에 참가되어있는 사람들에게 채팅말 보임현상 처리
		// 채팅방 탈퇴시 플레이어 채팅룸 ID초기화 http://dev:8181/projects/gunz/ticket/158
		CCMatchServer* pServer = CCMatchServer::GetInstance();
		CCMatchObject* pPlayer = pServer->GetObject(uidPlayer);
		if( !IsEnabledObject(pPlayer) )
			return;
		pPlayer->SetChatRooCCUID(CCUID(0,0));
	}
}

void CCMatchChatRoom::RouteChat(const CCUID& uidSender, char* pszMessage)
{
	if( (0 == pszMessage) || (256 < strlen(pszMessage)) )
		return;

	CCMatchServer* pServer = CCMatchServer::GetInstance();
	CCMatchObject* pSenderObj = pServer->GetObject(uidSender);
	if (pSenderObj == NULL)
		return;

	for (CCUIDRefCache::iterator i=m_PlayerList.begin(); i!=m_PlayerList.end(); i++) {
		CCUID uidTarget = (*i).first;
		CCMatchObject* pTargetObj = pServer->GetObject(uidTarget);
		if (pTargetObj) {
			CCCommand* pCmd = pServer->CreateCommand(MC_MATCH_CHATROOM_CHAT, CCUID(0,0));
			pCmd->AddParameter(new CCCmdParamStr( const_cast<char*>(GetName()) ));
			pCmd->AddParameter(new CCCmdParamStr(pSenderObj->GetName()));
			pCmd->AddParameter(new CCCmdParamStr(pszMessage));
			pServer->RouteToListener(pTargetObj, pCmd);
		}
	}
}

void CCMatchChatRoom::RouteInfo(const CCUID& uidReceiver)
{
	
}

void CCMatchChatRoom::RouteCommand(const CCCommand* pCommand)
{
	if( 0 == pCommand )
		return;

	CCMatchServer* pServer = CCMatchServer::GetInstance();
	for (CCUIDRefCache::iterator i=m_PlayerList.begin(); i!=m_PlayerList.end(); i++) {
		CCUID uidTarget = (*i).first;
		CCMatchObject* pTargetObj = pServer->GetObject(uidTarget);
		if (pTargetObj) {
			CCCommand* pRouteCmd = pCommand->Clone();
			pServer->RouteToListener(pTargetObj, pRouteCmd);
		}
	}
	delete pCommand;
}


CCMatchChatRoomMgr::CCMatchChatRoomMgr()
{
}

CCMatchChatRoomMgr::~CCMatchChatRoomMgr()
{
}

CCMatchChatRoom* CCMatchChatRoomMgr::AddChatRoom(const CCUID& uidMaster, const char* pszName)
{
	if( (0 == pszName) || (128 < strlen(pszName)) )
		return 0;

	if (FindChatRoomByName(pszName) != NULL)
		return NULL;

	CCMatchObject* pMaster = CCMatchServer::GetInstance()->GetObject( uidMaster );
	if( !IsEnabledObject(pMaster) )
		return 0;

	CCMatchChatRoom* pRoom = new CCMatchChatRoom(m_RoomMap.UseUID(), uidMaster, pszName);
	if( 0 == pRoom )
		return 0;

	m_RoomMap.Insert(pRoom->GetUID(), pRoom);

	string strName = pszName;
	m_RoomStringSubMap.insert( 
		CCMatchChatRoomStringSubMap::value_type(strName, pRoom->GetUID()) 
	);

	return pRoom;
}

void CCMatchChatRoomMgr::RemoveChatRoom(const CCUID& uidChatRoom)
{
	CCMatchChatRoomMap::iterator i = m_RoomMap.find(uidChatRoom);
	if (i!=m_RoomMap.end()) {
		// Remove Sub Map
		CCMatchChatRoom* pRoom = (*i).second;
		CCMatchChatRoomStringSubMap::iterator itorSub = 
			m_RoomStringSubMap.find(pRoom->GetName());
		if (itorSub != m_RoomStringSubMap.end()) {
			m_RoomStringSubMap.erase(itorSub);
		}

		// Remove from Map
		m_RoomMap.erase(i);
	}
}

CCMatchChatRoom* CCMatchChatRoomMgr::FindChatRoom(const CCUID& uidChatRoom)
{
	CCMatchChatRoomMap::iterator i = m_RoomMap.find(uidChatRoom);
	if (i!=m_RoomMap.end())
		return (*i).second;
	return NULL;
}

CCMatchChatRoom* CCMatchChatRoomMgr::FindChatRoomByName(const char* pszName)
{
	if( (0 == pszName) || (128 < strlen(pszName)) )
		return 0;

	CCMatchChatRoomStringSubMap::iterator itorSub = 
		m_RoomStringSubMap.find(pszName);
	if (itorSub != m_RoomStringSubMap.end()) {
		CCUID uidRoom = (*itorSub).second;
		return FindChatRoom(uidRoom);
	}
	return NULL;
}

void CCMatchChatRoomMgr::Update()
{
}
