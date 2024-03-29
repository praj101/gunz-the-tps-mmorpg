#include "stdafx.h"
#include "CCMatchEventManager.h"

CCMatchEventManager::CCMatchEventManager()
{
}


CCMatchEventManager::~CCMatchEventManager()
{
	Clear();
}


// 이전의 리스트에 하나씩 추가.
void CCMatchEventManager::AddEvent( CCMatchEvent* pEvent )
{
	if( 0 != pEvent )
	{
		ASSERT( m_EventMap.end() == m_EventMap.find(pEvent->GetEventListID()) );

		m_EventVec.push_back( pEvent );
		m_EventMap.insert( pair<DWORD, CCMatchEvent*>(pEvent->GetEventListID(), pEvent) );
	}
}

// 기존에 있는 이벤트 리스트에 추가를 한다.
void CCMatchEventManager::AddEventList( EventPtrVec& EvnPtrVec )
{
	EventPtrVec::iterator it, end;
	end = EvnPtrVec.end();
	for( it = EvnPtrVec.begin(); it != end; ++it )
	{
		ASSERT( m_EventMap.end() == m_EventMap.find((*it)->GetEventListID()) );

		m_EventVec.push_back( (*it) );
		m_EventMap.insert( pair<DWORD, CCMatchEvent*>((*it)->GetEventListID(), (*it)) );
	}
}

// 기존에 있는 이벤트 리스트를 새것으로 교체한다. 이전의 내용은 지워짐.
void CCMatchEventManager::ChangeEventList( EventPtrVec& EvnPtrVec )
{
	Clear();
	m_EventVec.swap( EvnPtrVec );

	vector< CCMatchEvent* >::iterator it, end;
	end = m_EventVec.end();
	for( it = m_EventVec.begin(); it != end; ++it )
	{
		CCMatchEvent* pEvent = (*it);
		ASSERT( m_EventMap.end() == m_EventMap.find(pEvent->GetEventListID()) );
		m_EventMap.insert( pair<DWORD, CCMatchEvent*>(pEvent->GetEventListID(), pEvent) );
	}
}

// 한번에 하나씩 제거됨. 같은게 여러게 있을시는 처음거만 지워짐.
// 같은걸 전부 지우고 싶을시는 GetEventCount로 카운트를 확인한다음,
//  필요한 만큼 호출해야 된다.
void CCMatchEventManager::DeleteEvent( const DWORD dwEventID )
{
	EventPtrVec::iterator it, end;
	end = m_EventVec.end();
	for( it = m_EventVec.begin(); it != end; ++it )
	{
		if( (*it)->GetEventID() == dwEventID )
		{
			m_EventMap.erase( (*it)->GetEventID() );
			delete (*it);
			m_EventVec.erase( it );
			return;
		}
	}
}


void CCMatchEventManager::CheckEventObj( CCMatchObject* pObj, const DWORD dwCurTime  )
{
	if( 0 != pObj )
	{
		EventPtrVec::iterator it, end;
		end = m_EventVec.end();
		for( it = m_EventVec.begin(); it != end; ++it )
			(*it)->CheckEventObj( pObj, dwCurTime );
	}
}


void CCMatchEventManager::Run()
{
	EventPtrVec::iterator it, end;
	end = m_EventVec.end();
	for( it = m_EventVec.begin(); it != end; ++it )
		(*it)->Run();
}


void CCMatchEventManager::Clear()
{
	if( m_EventMap.empty() ) 
		return;

	EventPtrVec::iterator it, end;
	end = m_EventVec.end();
	for( it = m_EventVec.begin(); it != end; ++it )
		delete (*it);
	m_EventVec.clear();
	m_EventMap.clear();
}


size_t CCMatchEventManager::GetEventCount( const DWORD dwEventID )
{
	size_t cnt = 0;
	EventPtrVec::iterator it, end;
	end = m_EventVec.end();
	for( it = m_EventVec.begin(); it != end; ++it )
	{
		if( dwEventID == (*it)->GetEventID() )
			++cnt;
	}

	return cnt;
}


void CCMatchEventManager::StartNewEvent()
{
	size_t cnt = 0;
	EventPtrVec::iterator it, end;
	end = m_EventVec.end();
	for( it = m_EventVec.begin(); it != end; ++it )
		(*it)->StartNewEvent();
}


void CCMatchEventManager::CustomCheckEventObj( const DWORD dwEventID, CCMatchObject* pObj, void* pContext )
{
	EventPtrVec::iterator it, end;
	end = m_EventVec.end();
	for( it = m_EventVec.begin(); it != end; ++it )
	{
		if( dwEventID == (*it)->GetEventID() )
			(*it)->CustomCheckEventObj( pObj, pContext );
	}
}


void CCMatchEventManager::CustomCheckEventObj( const DWORD dwEventListID, const DWORD dwEventID, CCMatchObject* pObj, void* pContext )
{
	EventPtrMap::iterator it = m_EventMap.find( dwEventListID );
	ASSERT( (m_EventMap.end() != it) && (dwEventID == it->second->GetEventID()) );
	if( (m_EventMap.end() != it) && (dwEventID == it->second->GetEventID()) )
		it->second->CustomCheckEventObj( pObj, pContext );
}

void CCMatchEventManager::SetLastCheckTime( const DWORD dwCurTime )
{
	EventPtrVec::iterator it, end;
	end = m_EventVec.end();
	for( it = m_EventVec.begin(); it != end; ++it )
		(*it)->SetLastCheckTime( dwCurTime );
}