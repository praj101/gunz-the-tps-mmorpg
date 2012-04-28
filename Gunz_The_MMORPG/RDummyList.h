#pragma once
#include <list>
#include <string>
#include "RTypes.h"
#include "RToken.h"
#include "RNameSpace.h"
_NAMESPACE_REALSPACE2_BEGIN
using namespace std;

class CCXmlElement;





// 더미, 더미 리스트
struct RDummy {
	string	szName;
	rvector Position;
	rvector Direction;
};

class RDummyList : public list<RDummy*> 
{
private:
	void Clear();
public:
	virtual ~RDummyList();

	bool Open(CCXmlElement *pElement);
	bool Save(CCXmlElement *pElement);
};

_NAMESPACE_REALSPACE2_END