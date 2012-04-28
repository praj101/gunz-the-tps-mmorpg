#pragma once

#include <string>
#include "RTypes.h"
#include "RNameSpace.h"
using namespace std;
_NAMESPACE_REALSPACE2_BEGIN

class MXmlElement;

struct RLIGHT
{
	string	Name;
	rvector Color;
	rvector Position;
	float	fIntensity;
	float	fAttnStart,fAttnEnd;
	DWORD	dwFlags;
};

class RLightList : public list<RLIGHT*> {
public:
	virtual ~RLightList();
	bool Open(MXmlElement *pElement);
	bool Save(MXmlElement *pElement);
};

_NAMESPACE_REALSPACE2_END