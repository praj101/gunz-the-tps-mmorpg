#pragma	once


#include <string>
#include "RTypes.h"
#include "RNameSpace.h"
_NAMESPACE_REALSPACE2_BEGIN
using namespace std;

class CCXmlElement;

class ROcclusion {
public:
	ROcclusion();
	~ROcclusion();

	inline void CalcPlane() { D3DXPlaneFromPoints(&plane,pVertices,pVertices+1,pVertices+2); }

	int nCount;				// 버텍스의 수
	rvector *pVertices;		// 버텍스
	rplane	*pPlanes;		// 카메라위치와 occlusion의 한변을 잇는 평면의 방정식들
	rplane plane;			// occlusion이 있는 평면
	string	Name;
};


class ROcclusionList : public list<ROcclusion*> {
public:
	virtual ~ROcclusionList();
	bool Open(CCXmlElement *pElement);
	bool Save(CCXmlElement *pElement);

	void UpdateCamera(rmatrix &matWorld,rvector &cameraPos);
	bool IsVisible(rboundingbox &bb);

};

_NAMESPACE_REALSPACE2_END