#pragma once

#include <string>
#include <list>
#include "DirectX/d3dx9math.h"
#include "RNameSpace.h"

using namespace std;

_NAMESPACE_REALSPACE2_BEGIN

#define pi D3DX_PI
#define RM_FLAG_ADDITIVE		0x0001
#define RM_FLAG_USEOPACITY		0x0002
#define RM_FLAG_TWOSIDED		0x0004
#define RM_FLAG_NOTWALKABLE		0x0008		// can not go ahead and show area.
#define RM_FLAG_CASTSHADOW		0x0010
#define RM_FLAG_RECEIVESHADOW	0x0020
#define RM_FLAG_PASSTHROUGH		0x0040		// If this flag is not a collision.
#define RM_FLAG_HIDE			0x0080		// do not paint.
#define RM_FLAG_PASSBULLET		0x0100		// the bullet goes through
#define RM_FLAG_PASSROCKET		0x0200		// rockets / grenades etc. is passed.
#define RM_FLAG_USEALPHATEST	0x0400		// alpha test map (leaves, etc. ..
#define RM_FLAG_AI_NAVIGATION	0x1000		// quest for AI navigation


#define rvector D3DXVECTOR3
#define rmatrix D3DXMATRIX
#define rplane D3DXPLANE

#define rvector2 D3DXVECTOR2

#define FLOAT2RGB24(r, g, b) ( ( ((long)((r) * 255)) << 16) | (((long)((g) * 255)) << 8) | (long)((b) * 255))
#define VECTOR2RGB24(v)		FLOAT2RGB24((v).x,(v).y,(v).z)
#define BYTE2RGB24(r,g,b)	((DWORD) (((BYTE) (b)|((WORD) (g) << 8))|(((DWORD) (BYTE) (r)) << 16)))
#define BYTE2RGB32(a,r,g,b)	((DWORD) (((BYTE) (b)|((WORD) (g) << 8))|(((DWORD) (BYTE) (r)) << 16)|(((DWORD) (BYTE) (a)) << 24)))
#define DWORD2VECTOR(x)		rvector(float(((x)& 0xff0000) >> 16)/255.f, float(((x) & 0xff00) >> 8)/255.f,float(((x) & 0xff))/255.f)

#define RPIXELFORMAT D3DFORMAT

enum rsign { NEGATIVE= -1, ZERO= 0, POSITIVE= 1 };


//writes d3dformat
//Write format D3DFMT_X8R8G8B8, D3DFMT_R5G6B5, D3DFMT_X1R5G5B5
enum RRESULT {
	R_UNKNOWN = -1,
	R_OK = 0,
	R_NOTREADY = 1,
	R_RESTORED = 2,

	R_ERROR_LOADING = 1000,
	
	
};

//Initialize the parameters
struct RMODEPARAMS {
	int nWidth,nHeight;
	bool bFullScreen;
	RPIXELFORMAT PixelFormat;
};

struct rboundingbox
{
	union {
	struct {
		float minx,miny,minz,maxx,maxy,maxz;
	};
	struct {
		rvector vmin,vmax;
	};
	float m[2][3];
	};

	rvector Point(int i) const { return rvector( (i&1)?vmin.x:vmax.x, (i&2)?vmin.y:vmax.y, (i&4)?vmin.z:vmax.z );  }
	
	void Add(const rvector &kPoint)
	{
		if ( vmin.x > kPoint.x )	vmin.x = kPoint.x;
		if ( vmin.y > kPoint.y )	vmin.y = kPoint.y;
		if ( vmin.z > kPoint.z )	vmin.z = kPoint.z;
		if ( vmax.x < kPoint.x )	vmax.x = kPoint.x;
		if ( vmax.y < kPoint.y )	vmax.y = kPoint.y;
		if ( vmax.z < kPoint.z )	vmax.z = kPoint.z;
	}
};

//Vector

inline float Magnitude(const rvector &x) { return D3DXVec3Length(&x); }
inline float MagnitudeSq(const rvector &x)	{ return D3DXVec3LengthSq(&x); }
inline void Normalize(rvector &x) { D3DXVec3Normalize(&x,&x);}
inline float DotProduct(const rvector &a,const rvector &b) { return D3DXVec3Dot(&a,&b); }
inline void CrossProduct(rvector *result,const rvector &a,const rvector &b) { D3DXVec3Cross(result,&a,&b); }

//Matrix

void MakeWorldMatrix(rmatrix *pOut,rvector pos,rvector dir,rvector up);			// el 모델의 world matrix 를 만든다.

//RELEASE & DELETE macro (from dxutil.h)
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

//Help function of the ...
#ifndef TOLER
#define TOLER 0.001
#endif
#define IS_ZERO(a) ((fabs((double)(a)) < (double) TOLER))
#define IS_EQ(a,b) ((fabs((double)(a)-(b)) >= (double) TOLER) ? 0 : 1)
#define IS_EQ3(a,b) (IS_EQ((a).x,(b).x)&&IS_EQ((a).y,(b).y)&&IS_EQ((a).z,(b).z))
#define SIGNOF(a) ( (a)<-TOLER ? NEGATIVE : (a)>TOLER ? POSITIVE : ZERO )
#define RANDOMFLOAT ((float)rand()/(float)RAND_MAX)

// Point the distance from the straight line. line1, line2 two points of a straight line.
float GetDistance(const rvector &position,const rvector &line1,const rvector &line2);
// Point of the segment closest to the point of
rvector GetNearestPoint(const rvector &position,const rvector &a,const rvector &b);
// Point the distance from the segment
float GetDistanceLineSegment(const rvector &position,const rvector &a,const rvector &b);
// The distance between line segments and line segments.
float GetDistanceBetweenLineSegment(const rvector &a,const rvector &aa,const rvector &c,const rvector &cc,rvector *ap,rvector *cp);
// Point to the plane away from
float GetDistance(const rvector &position,const rplane &plane);
// Segment (a, aa) to the plane at the nearest point of the segment.
rvector GetNearestPoint(const rvector &a,const rvector &aa,const rplane &plane);
// Segment (a, aa) the distance from the flat
float GetDistance(const rvector &a,const rvector &aa,const rplane &plane);
// In the plane with a maximum distance boundingbox
float GetDistance(rboundingbox *bb,rplane *plane);
// In the plane with boundingbox minimum and maximum distance
void GetDistanceMinMax(rboundingbox &bb,rplane &plane,float *MinDist,float *MaxDist);
// Point and a minimum distance of boundingbox
float GetDistance(const rboundingbox &bb,const rvector &point);
// The area of ??a triangle
float GetArea(rvector &v1,rvector &v2,rvector &v3);

// Two vectors x, y, over the angle
float GetAngleOfVectors(rvector &ta,rvector &tb);

//  vector .. a, b the normalized
rvector InterpolatedVector(rvector &a,rvector &b,float x);

bool IsIntersect(rboundingbox *bb1,rboundingbox *bb2);
bool isInPlane(rboundingbox *bb,rplane *plane);
bool IsInSphere(const rboundingbox &bb,const rvector &point,float radius);
bool isInViewFrustum(const rvector &point,rplane *plane);
bool isInViewFrustum(const rvector &point,float radius,rplane *plane);		// bounding sphere
bool isInViewFrustum(rboundingbox *bb,rplane *plane);
bool isInViewFrustum(const rvector &point1,const rvector &point2,rplane *planes);	//segment
bool isInViewFrustumWithZ(rboundingbox *bb,rplane *plane);
bool isInViewFrustumwrtnPlanes(rboundingbox *bb,rplane *plane,int nplane);

bool IsIntersect( const rvector& orig, const rvector& dir, rvector& v0, rvector& v1, rvector& v2, float* t);
bool isLineIntersectBoundingBox(rvector &origin,rvector &dir,rboundingbox &bb);
bool IsIntersect( rvector& line_begin_, rvector& line_end_, rboundingbox& box_);
bool IsIntersect(rvector& line_begin_, rvector& line_dir_, rvector& center_, float radius_, float* dist = NULL, rvector* p = NULL );

// Function to obtain the intersection of circles and line segments. dir is a must normalize
bool IsIntersect(const rvector& orig, const rvector& dir, const rvector& center, const float radius, rvector* p = NULL);

// Two planes passing through the equation of the straight line is obtained
bool GetIntersectionOfTwoPlanes(rvector *pOutDir,rvector *pOutAPoint,rplane &plane1,rplane &plane2);

void MergeBoundingBox(rboundingbox *dest,rboundingbox *src);

// Aabb box will transform you. The greater
void TransformBox( rboundingbox* result, const rboundingbox& src, const rmatrix& matrix );


// Convert Macros


// Progress callback function type
typedef void (*RFPROGRESSCALLBACK)(void *pUserParams,float fProgress);

_NAMESPACE_REALSPACE2_END
