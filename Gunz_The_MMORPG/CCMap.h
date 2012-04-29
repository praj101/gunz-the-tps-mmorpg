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
	virtual ~CCMap(void);

	int GetID(void){ return m_nID; }
	const char* GetName(void){ return m_szName; }

	int Create(const char* szFileName, CCZFileSystem* pZFS=NULL);
	void Destroy(void);

	/*
	void AddRef(CCObject* pObj);
	void DelRef(CCObject* pObj);
	*/

	void RandomLocate(void);

	RealSpace2::RBspObject*	GetBspObject(void){ return m_pBspObj; }
};

/// ¸Ê ÄÁÅ×ÀÌ³Ê
class CCMapManager : public vector<CCMap*>{
public:
	CCMapManager(void);
	virtual ~CCMapManager(void);

	CCMap* Get(const char* szName);
	int GetID(const char* szName);
};