#pragma once


#include "CCUID.h"
#include <map>
using namespace std;


class MCommand;


#define CHATROOM_MAX_ROOMMEMBER	64


class MMatchChatRoom {
protected:
	CCUID			m_uidChatRoom;
	CCUID			m_uidMaster;
	char			m_szName[ MAX_CHATROOMNAME_STRING_LEN ];
    CCUIDRefCache	m_PlayerList;

public:
	MMatchChatRoom(const CCUID& uidRoom, const CCUID& uidMaster, const char* pszName);
	virtual ~MMatchChatRoom();

	const CCUID& GetUID()	{ return m_uidChatRoom; }
	const CCUID& GetMaster()	{ return m_uidMaster; }
	const char* GetName()	{ return m_szName; }
	size_t GetUserCount()	{ return m_PlayerList.size(); }

	bool AddPlayer(const CCUID& uidPlayer);
	void RemovePlayer(const CCUID& uidPlayer);
	bool IsFindPlayer(const CCUID& uidPlayer);							// 해당 플레이어가 있는지 확인한다

	void RouteChat(const CCUID& uidSender, char* pszMessage);
	void RouteInfo(const CCUID& uidReceiver);
	void RouteCommand(const MCommand* pCommand);
};


class MMatchChatRoomMap : public map<CCUID, MMatchChatRoom*> {
	CCUID	m_uidGenerate;
public:
	MMatchChatRoomMap()			{	m_uidGenerate = CCUID(0,10);	}
	virtual ~MMatchChatRoomMap(){}
	CCUID UseUID()				{	m_uidGenerate.Increase();	return m_uidGenerate;	}
	void Insert(const CCUID& uid, MMatchChatRoom* pStage)	{	insert(value_type(uid, pStage));	}
};

class MMatchChatRoomStringSubMap : public map<string, CCUID> {};


class MMatchChatRoomMgr {
protected:
	MMatchChatRoomMap			m_RoomMap;
	MMatchChatRoomStringSubMap	m_RoomStringSubMap;

public:
	MMatchChatRoomMgr();
	virtual ~MMatchChatRoomMgr();

	MMatchChatRoom* AddChatRoom(const CCUID& uidMaster, const char* pszName);
	void RemoveChatRoom(const CCUID& uidChatRoom);

	MMatchChatRoom* FindChatRoom(const CCUID& uidChatRoom);
	MMatchChatRoom* FindChatRoomByName(const char* pszName);

	void Update();
};
