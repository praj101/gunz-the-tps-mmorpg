#pragma once
#pragma warning(disable:4786)

#include <list>
#include <string>
#include "RTypes.h"
#include "RToken.h"

using namespace std;

class CCXmlElement;

#include "RNameSpace.h"
_NAMESPACE_REALSPACE2_BEGIN

struct RMATERIAL {
	string Name;
	rvector Diffuse;
	rvector Ambient;
	rvector Specular;
	float Power;
	string DiffuseMap;
	DWORD dwFlags;
};

class RMaterialList : public list<RMATERIAL*> {
public:
	virtual ~RMaterialList();
	
	/*
	bool Save(const char *szFileName);
	bool Open(const char *szFileName);
	bool Open(void* pMemory,int nSize);
	*/

	bool Open(CCXmlElement *pElement);
	bool Save(CCXmlElement *pElement);

private:
	bool Open_Material(CCXmlElement *pElement);
};

_NAMESPACE_REALSPACE2_END