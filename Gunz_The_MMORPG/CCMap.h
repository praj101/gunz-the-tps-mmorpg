#pragma once
#include "CCGridMap.h"
#include <vector>
using namespace std;


class CCObject;
namespace RealSpace2{
	class RBspObject;
}
class CCZFileSystem;

#define MMAP_NAME_LENGTH	64

typedef CCGridMap<CCObject*>	CCObjectGridMap;

/// Map
class CCMap : public CCObjectGridMap{
protected:
	char	m_szName[MMAP_NAME_LENGTH];	///< Map Name
	int		m_nID;
	/*
	CCGridMap<CCObjectCharacter*>	m_CharacterGridMap;	///< Grid Map for Referencing
	CCGridMap<CCObjectItem*>		m_ItemGridMap;		///< Grid Map for Referencing
	*/
	RealSpace2::RBspObject*	m_pBspObj;

public:
	CCMap(int nID, const char* szName);
	virtual ~CCMap();

	int GetID(){ return m_nID; }
	const char* GetName(){ return m_szName; }

	int Create(const char* szFileName, CCZFileSystem* pZFS=NULL);
	void Destroy();

	/*
	void AddRef(CCObject* pObj);
	void DelRef(CCObject* pObj);
	*/

	void RandomLocate();

	RealSpace2::RBspObject*	GetBspObject(){ return m_pBspObj; }
};

/// ¸Ê ÄÁÅ×ÀÌ³Ê
class CCMapManager : public vector<CCMap*>{
public:
	CCMapManager();
	virtual ~CCMapManager();

	CCMap* Get(const char* szName);
	int GetID(const char* szName);
};