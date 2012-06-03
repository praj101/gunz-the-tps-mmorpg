#pragma once

#include <string>
#include "RTypes.h"
#include "RNameSpace.h"
#include "CCXml.h"
using namespace std;
_NAMESPACE_REALSPACE2_BEGIN

//class CCXmlElement;

struct RLIGHT
{
	string	Name;
	rvector Color;
	rvector sPosition;
	float	fIntensity;
	float	fAttnStart,fAttnEnd;
	DWORD	dwFlags;
};

class RLightList : public list<RLIGHT*> {
public:
	virtual ~RLightList();
	bool Open(::CCXmlElement *pElement);
	bool Save(::CCXmlElement *pElement);
};

_NAMESPACE_REALSPACE2_END