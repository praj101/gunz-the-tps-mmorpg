#include "stdafx.h"
#include "ZPlayerManager.h"



ZPlayerManager::ZPlayerManager()
{
}

ZPlayerManager::~ZPlayerManager()
{
	Clear();
}

ZPlayerManager* ZPlayerManager::GetInstance()
{
	static ZPlayerManager PlayerManager;
	return &PlayerManager;
}

void ZPlayerManager::AddPlayer( CCUID& uID, ZPlayerInfo* pInfo)
{
	m_PlayerList.insert( map<CCUID,ZPlayerInfo*>::value_type( uID, pInfo));
}

void ZPlayerManager::AddPlayer( CCUID& uID, const char* name, int rank, int kill, int death)
{
	ZPlayerInfo* pInfo = new ZPlayerInfo( name, rank, kill, death);

	AddPlayer( uID, pInfo);
}

void ZPlayerManager::RemovePlayer( CCUID& uID)
{
	map<CCUID,ZPlayerInfo*>::iterator itr = m_PlayerList.find( uID);

	if ( itr == m_PlayerList.end())
		return;

    
	delete (*itr).second;
	m_PlayerList.erase( itr);
}

void ZPlayerManager::Clear()
{
	while ( !m_PlayerList.empty())
	{
		delete ( *m_PlayerList.begin()).second;
		m_PlayerList.erase( m_PlayerList.begin());
	}
}

ZPlayerInfo* ZPlayerManager::Find( CCUID& uID)
{
	map<CCUID,ZPlayerInfo*>::iterator itr = m_PlayerList.find( uID);

	if ( itr == m_PlayerList.end())
		return NULL;

	return (*itr).second;
}
