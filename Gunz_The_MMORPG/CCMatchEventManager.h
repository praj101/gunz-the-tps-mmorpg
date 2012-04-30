#pragma once

#include "CCMatchEvent.h"

#include <vector>
using std::vector;


class CCMatchObject;


typedef vector< CCMatchEvent* >		EventPtrVec;
typedef map< DWORD, CCMatchEvent* >	EventPtrMap;


class CCMatchEventManager
{
public :
	CCMatchEventManager();
	~CCMatchEventManager();

	void			AddEvent( CCMatchEvent* pEvent );
	void			AddEventList( EventPtrVec& EvnPtrVec );
	void			ChangeEventList( EventPtrVec& EvnPtrVec );
	void			DeleteEvent( const DWORD dwEventID );
	void			Clear();
	void			StartNewEvent();
	size_t			GetEventCount( const DWORD dwEventID );
	bool			Empty()											{ return m_EventVec.empty(); }
	CCMATCH_GAMETYPE GetLastSetGameType()							{ return m_GameType; }
	void			SetGameType( const MMATCH_GAMETYPE GameType )	{ m_GameType = GameType; }

	void CustomCheckEventObj( const DWORD dwEventID, CCMatchObject* pObj, void* pContext );
	void CustomCheckEventObj( const DWORD dwEventListID, const DWORD dwEventID, CCMatchObject* pObj, void* pContext );
	void CheckEventObj( CCMatchObject* pObj, const DWORD dwCurTime );
	void Run();

	void SetLastCheckTime( const DWORD dwCurTime );

	static void ClearEventPtrVec( EventPtrVec& EvnPtrVec )
	{
		EventPtrVec::iterator it, end;
		end = EvnPtrVec.end();
		for( it = EvnPtrVec.begin(); it != end; ++it )
			delete (*it);
		EvnPtrVec.clear();
	}

private :
	CCMATCH_GAMETYPE m_GameType;
	EventPtrVec		m_EventVec;
	EventPtrMap		m_EventMap;
};