#pragma once
/***********************************************************************
  ZEmblemInterface.h
  
  Usage: emblem stored on the hard drive to load a bitmap into the memory cache
           Request passes the pointer to the bitmap.
  Date: 18, Nov, 2004
  By: imdonghwan
************************************************************************/


//#pragma once
#include <string>							// Include string
#include <map>								// Include map
#include "CCMatchGlobal.h"					// Include CCMatchGlobal.h
#include "CCBitmap.h"						// Include CCBitmap.h
#include "RTypes.h"
using namespace std;



// Class : ZEmblemMemNode
// Information and clan emblems in the form of information to store the node map list
struct ZEmblemInfoNode
{
	UINT		m_iClanID;								// Clan ID
	UINT		m_iNumOfClan;							// Number of people in clan
	CCBitmap*	m_pBitmapEmblem;						// Clan bitmap emblem

	// Initialize
	ZEmblemInfoNode() {									// Constructor
		m_iClanID			= 0;
		m_iNumOfClan		= 0;
		m_pBitmapEmblem		= NULL;
	}

	virtual ~ZEmblemInfoNode() {
		SAFE_DELETE(m_pBitmapEmblem);
	}
};

typedef  map<int,ZEmblemInfoNode>  EmblemInfoMapList;


// Class : ZEmblemInterface
// Emblems class for saving and loading of information
class ZEmblemInterface
{
public:		// public variables


public:		// public functions
	ZEmblemInterface( void);									// Constructor
	~ZEmblemInterface( void);									// Destructor

	void Create();
	void Destroy();

	bool AddClanInfo( UINT nClanID);							// Add clan infomation
	bool DeleteClanInfo( UINT nClanID);							// Delete clan infomation
	bool ClearClanInfo( void);									// Clear clan infomation
	bool ReloadClanInfo( UINT nClanID);							// 실제로 bitmap을 로드한다

	bool ZEmblemInterface::FindClanInfo( UINT nClanID, EmblemInfoMapList::iterator* pIterator);		// Find clan infomation

	CCBitmap* GetClanEmblem( UINT nClanID);						// Get pointer of bitmap emblem
	CCBitmap* GetClanEmblem2( UINT nClanID);						// Get pointer of bitmap emblem

protected:	// protected varialbes
	EmblemInfoMapList	m_EmblemInfoMap;						// Map list of emblem infomation
	CCBitmap*			m_pBitmapNoEmblem;						// Bitmap pointer of "no emblem"

protected:	// protected functions
};

