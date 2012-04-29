/***********************************************************************
  ZEmblemInterface.cpp
  
  Usage: emblem stored on the hard drive to load a bitmap into the memory cache
           Request passes the pointer to the bitmap.
  Date: 18, Nov, 2004
  By: imdonghwan
************************************************************************/


#include "stdafx.h"							// Include stdafx.h
#include "ZEmblemInterface.h"				// Include ZEmblemInterface.h
#include "MResourceManager.h"				// Include MResourceManager.h
#include "FileInfo.h"
#include "ZGameClient.h"



/***********************************************************************
  ZEmblemInterface : public
  
  desc : Constructors
************************************************************************/
ZEmblemInterface::ZEmblemInterface( void)
{
}


/***********************************************************************
  ~ZEmblemInterface : public
  
  desc : destructor
************************************************************************/
ZEmblemInterface::~ZEmblemInterface( void)
{
	Destroy();
}

void ZEmblemInterface::Create(void)
{
	// "No emblem" written as a bitmap pointer is obtained in advance.
	m_pBitmapNoEmblem = CCBitmapManager::Get( "no_emblem.png");
}

void ZEmblemInterface::Destroy(void)
{
	ClearClanInfo();
}

/***********************************************************************
  AddClanInfo : public
  
  desc: Clan information is added. To make a call every time a player is logged on.
  arg: nClanID = Clan ID value
  ret: true (add) or false (do not add)
************************************************************************/
bool ZEmblemInterface::AddClanInfo( UINT nClanID)
{
	if(nClanID==0) return false;

	// Emblem emblem information is available from the list of information that is checked.
	EmblemInfoMapList::iterator Iterator;
	if ( FindClanInfo( nClanID, &Iterator))
	{
		// If there is already one of them to increase the numbers of those emblems.
		(*Iterator).second.m_iNumOfClan++;

		return ( false);					// return : false
	}
	
	// Create a new node is not on the list.
	ZEmblemInfoNode EmblemNode;
	EmblemNode.m_iClanID		= nClanID;
	EmblemNode.m_iNumOfClan		= 1;

	// Add a node to the list
	m_EmblemInfoMap.insert( EmblemInfoMapList::value_type( nClanID, EmblemNode));

	// Bitmap loaded
	ReloadClanInfo(nClanID);

	// Bitmap, old and unused shall be deleted from the cache memory (not remember the two do not seem to).
	if ( 0)
	{
		for ( EmblemInfoMapList::iterator i = m_EmblemInfoMap.begin();  i != m_EmblemInfoMap.end();  i++)
		{
			if ( (*i).second.m_iNumOfClan <= 1)		// now to use the emblem, if more than one person person.
				m_EmblemInfoMap.erase( i);			// is removed from the list.
		}
	}

	return ( true);							// return : true
}

bool ZEmblemInterface::ReloadClanInfo(UINT nClanID)
{
	EmblemInfoMapList::iterator Iterator;
	if ( !FindClanInfo( nClanID, &Iterator)) return false;

	ZEmblemInfoNode &EmblemNode = Iterator->second;

	char szFilePath[256];
	if(!ZGetGameClient()->GetEmblemManager()->GetEmblemPathByCLID(nClanID,szFilePath)) 
		return false;

	SAFE_DELETE(EmblemNode.m_pBitmapEmblem);

#ifdef _PUBLISH
	CCZFile::SetReadMode( CCZIPREADFLAG_ZIP | CCZIPREADFLAG_MRS | CCZIPREADFLAG_MRS2 | CCZIPREADFLAG_FILE );
#endif

	// The bitmap using the clan ID value is loaded into the cache memory.
	CCBitmapR2 *pBitmap = new CCBitmapR2;
	pBitmap->Create("clanEmblem",RGetDevice(),szFilePath,false);
	EmblemNode.m_pBitmapEmblem = pBitmap;

#ifdef _PUBLISH
	CCZFile::SetReadMode( CCZIPREADFLAG_MRS2 );
#endif

	return true;
}

/***********************************************************************
  DeleteClanInfo : public
  
  desc: the number of personnel to use its ClanID Clan information is deleted to 0 people.
         If a player is logged off every time you make a call.
  arg: nClanID = Clan ID value
  ret: true (delete) or false (do not delete)
************************************************************************/
bool ZEmblemInterface::DeleteClanInfo( UINT nClanID)
{
	if(nClanID==0) return false;

	// The list is checked for the presence of the corresponding clan ID.
	EmblemInfoMapList::iterator Iterator;
	if ( !FindClanInfo( nClanID, &Iterator))
		return ( false);					// return : false


	// The clan emblem to use one person to reduce the number of people.
	(*Iterator).second.m_iNumOfClan--;

	// The clan emblem Adults to use a zero if people are removed from the list.
	if ( (*Iterator).second.m_iNumOfClan == 0)
		m_EmblemInfoMap.erase( Iterator);

	return ( true);							// return : true
}


/***********************************************************************
  ClearClanInfo : public
  
  desc: Clan information is fully initialized.
  arg: none
  ret: true (success) or false (fail)
************************************************************************/
bool ZEmblemInterface::ClearClanInfo( void)
{
	m_EmblemInfoMap.clear();

	return ( true);							// return : true
}


/***********************************************************************
  GetClanEmblem : public
  
  desc: get pointer to the appropriate clan emblem of the bitmap. Writing should be discarded immediately
  arg: nClanID = Clan ID value
  ret: NULL (fail) or
         Pointer of bitmap (success)
************************************************************************/
CCBitmap* ZEmblemInterface::GetClanEmblem( UINT nClanID)
{
	if(nClanID==0) return NULL;

	// The list is checked for the presence of the corresponding clan ID.
	EmblemInfoMapList::iterator Iterator;
	if ( !FindClanInfo( nClanID, &Iterator))
	{
		// If there is a list of "no emblem" returns a pointer to a bitmap.
		return m_pBitmapNoEmblem;
	}

	// If the bitmap to the list of those clan emblems seek pointer.
	return Iterator->second.m_pBitmapEmblem;
}


/***********************************************************************
  GetClanEmblem2 : public
  
  desc: get pointer to the appropriate clan emblem of the bitmap. Writing should be discarded immediately
  arg: nClanID = Clan ID value
  ret: NULL (fail) or
         Pointer of bitmap (success)
************************************************************************/
CCBitmap* ZEmblemInterface::GetClanEmblem2( UINT nClanID)
{
	if(nClanID==0) return NULL;

	// The list is checked for the presence of the corresponding clan ID.
	EmblemInfoMapList::iterator Iterator;
	if ( !FindClanInfo( nClanID, &Iterator))
	{
		// If there is a list of "no emblem" returns a pointer to a bitmap.
		return m_pBitmapNoEmblem;
	}

	if ( Iterator->second.m_pBitmapEmblem == NULL)
		// If the bitmap emblem "no emblem" returns a pointer to a bitmap.
		return m_pBitmapNoEmblem;

	// If the bitmap to the list of those clan emblems seek pointer.
	return Iterator->second.m_pBitmapEmblem;
}


/***********************************************************************
  GetClanName : public
  
  desc: get the name of that clan.
  arg: nClanID = Clan ID value
  ret: NULL (fail) or
         String of clan name
************************************************************************/
//const char* ZEmblemInterface::GetClanName( UINT nClanID)
//{
//	// The list is checked for the presence of the corresponding clan ID.
//	EmblemInfoMapList::iterator Iterator;
//	if ( !FindClanInfo( nClanID, &Iterator))
//		return ( NULL);						// return : NULL
//
//	// If there is a list of name value is obtained corresponding to the clan.
//	char* szClanName = NULL;
//	strcpy( szClanName, (*Iterator).second.m_szClanName);
//
//	return ( szClanName);					// return : clan name
//}


/***********************************************************************
  FindClanInfo : public
  
  desc: check to see if there is information that the clan. If you are a list is returned.
  arg: nClanID = Clan ID value
         pIterator = iterator pointer to be returned
  ret: true (found) or false (not found)
************************************************************************/
bool ZEmblemInterface::FindClanInfo( UINT nClanID, EmblemInfoMapList::iterator* pIterator)
{
	EmblemInfoMapList::iterator Iterator;
	Iterator = m_EmblemInfoMap.find( nClanID);

	if ( Iterator == m_EmblemInfoMap.end())
		return ( false);					// return : not found

	*pIterator = Iterator;					// Set list iterator

	return ( true);							// return : found
}
