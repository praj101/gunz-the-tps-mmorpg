#pragma once
#include "CCMatchGlobal.h"


enum CCMATCH_MAP 
{
	CCMATCH_MAP_MANSION			= 0,
	CCMATCH_MAP_PRISON			= 1,
	CCMATCH_MAP_STATION			= 2,
	CCMATCH_MAP_PRISON_II		= 3,
	CCMATCH_MAP_BATTLE_ARENA		= 4,
	CCMATCH_MAP_TOWN				= 5,
	CCMATCH_MAP_DUNGEON			= 6,
	CCMATCH_MAP_RUIN				= 7,
	CCMATCH_MAP_ISLAND			= 8,
	CCMATCH_MAP_GARDEN			= 9,
	CCMATCH_MAP_CASTLE			= 10,
	CCMATCH_MAP_FACTORY			= 11,
	CCMATCH_MAP_PORT				= 12,
	CCMATCH_MAP_LOST_SHRINE		= 13,
	CCMATCH_MAP_STAIRWAY			= 14,
	CCMATCH_MAP_SNOWTOWN			= 15,
	CCMATCH_MAP_HALL				= 16,
	CCMATCH_MAP_CATACOMB			= 17,
	CCMATCH_MAP_JAIL				= 18,
	CCMATCH_MAP_SHOWERROOM		= 19,
	CCMATCH_MAP_HIGH_HAVEN		= 20,
	CCMATCH_MAP_CITADEL			= 21,

	// »õ¸Ê Ãß°¡´Â ÀÌ ¹Ø¿¡ ÇÏ¼¼¿ä.

	//	CCMATCH_MAP_EVENT,					// ÀÌº¥Æ® Àü¿ë¸Ê
	CCMATCH_MAP_RELAYMAP			= 22,

	CCMATCH_MAP_HALLOWEEN_TOWN	= 23,
	CCMATCH_MAP_WEAPON_SHOP		= 24,
	CCMATCH_MAP_MAX
};

#define CCMATCH_MAP_COUNT	CCMATCH_MAP_MAX			// ÀüÃ¼ ¸Ê °¹¼ö

#define CCMATCH_MAPNAME_RELAYMAP				"RelayMap"


class MMapDesc
{
private:
	const struct MapInfo
	{
		int			nMapID;							// map id
		char		szMapName[MAPNAME_LENGTH];		// ¸Ê ÀÌ¸§
		char		szMapImageName[MAPNAME_LENGTH];	// ¸Ê ÀÌ¹ÌÁö ÀÌ¸§
		char		szBannerName[MAPNAME_LENGTH];	// º£³Ê ÀÌ¸§
		float		fExpRatio;						// °æÇèÄ¡ ½Àµæ·ü
		int			nMaxPlayers;					// ÃÖ´ë ÀÎ¿ø
		bool		bOnlyDuelMap;					// µà¾ó¸Ê Àü¿ë
	};


	// data
	MapInfo	m_MapVectors[CCMATCH_MAP_COUNT];
	MMapDesc();
public:
	~MMapDesc() { }
	
	static MMapDesc* GetInstance();

	bool Initialize(const char* szFileName);
	bool Initialize(CCZFileSystem* pfs, const char* szFileName);
	bool MIsCorrectMap(const int nMapID);
	

 
	bool IsMapOnlyDuel( const int nMapID);
	int GetMapID( const int nMapID);
	const char* GetMapName(const int nMapID);
	const char* GetMapImageName(const char* szMapName);
	const char* GetBannerName(const char* szMapName);
	float GetExpRatio( const int nMapID); 
	int GetMaxPlayers( const int nMapID);

	int GetMapCount(){ return CCMATCH_MAP_COUNT; }
};


inline MMapDesc* CCGetMapDescMgr() 
{ 
	return MMapDesc::GetInstance();
}