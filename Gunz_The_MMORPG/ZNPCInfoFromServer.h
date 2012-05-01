#pragma once


#include <map>


using std::map;


struct CCTD_NPCINFO;



class ZNPCInfoFromServerManager : public map< BYTE, CCTD_NPCINFO*  >
{
public :
	ZNPCInfoFromServerManager();
	~ZNPCInfoFromServerManager();

	bool CreateNPCInfo( const CCTD_NPCINFO* pNPCInfo );

	const CCTD_NPCINFO* GetNPCInfo( const BYTE nNPCID );

	void Clear();
};