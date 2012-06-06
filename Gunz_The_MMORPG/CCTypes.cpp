#include "stdafx.h"
#include "CCTypes.h"
#include "CCWidget.h"
#include "Core.h"

void sPoint::Scale(float x, float y)
{
	sPoint::x = int(sPoint::x*x);
	sPoint::y = int(sPoint::y*y);
}

void sPoint::ScaleRes(void)
{
	x = sPoint::x*CCGetWorkspaceWidth()/640;
	y = sPoint::y*CCGetWorkspaceHeight()/480;
}

void sPoint::TranslateRes(void)
{
	int nDiffX = x - 320;
	int nDiffY = y - 240;

	x = nDiffX + CCGetWorkspaceWidth()/2;
	y = nDiffY + CCGetWorkspaceHeight()/2;
}

void sRect::ScalePos(float x, float y)
{
	sRect::x = int(sRect::x*x);
	sRect::y = int(sRect::y*y);
}

void sRect::ScaleArea(float x, float y)
{
	sRect::x = int(sRect::x*x);
	sRect::y = int(sRect::y*y);
	sRect::w = int(sRect::w*x);
	sRect::h = int(sRect::h*y);
}

void sRect::ScalePosRes(void)
{
	float x = CCGetWorkspaceWidth()/(float)640;
	float y = CCGetWorkspaceHeight()/(float)480;
	ScalePos(x, y);
}

void sRect::ScaleAreaRes(void)
{
	float x = CCGetWorkspaceWidth()/(float)640;
	float y = CCGetWorkspaceHeight()/(float)480;
	ScaleArea(x, y);
}

void sRect::TranslateRes(void)
{
	int nDiffX = x - 320;
	int nDiffY = y - 240;

	x = nDiffX + CCGetWorkspaceWidth()/2;
	y = nDiffY + CCGetWorkspaceHeight()/2;
}

void sRect::Enlarge(int w)
{
	x -= w;
	y -= w;
	w += (w*2);
	h += (w*2);
}

void sRect::Offset(int x, int y)
{
	sRect::x += x;
	sRect::y += y;
}

bool sRect::Intersect(sRect* pIntersect, const sRect& r)
{
	_ASSERT(pIntersect!=NULL);

	if(x>r.x){
		pIntersect->x = x;
		pIntersect->w = min(r.x+r.w, x+w) - x;
	}
	else{
		pIntersect->x = r.x;
		pIntersect->w = min(r.x+r.w, x+w) - r.x;
	}
	if(y>r.y){
		pIntersect->y = y;
		pIntersect->h = min(r.y+r.h, y+h) - y;
	}
	else{
		pIntersect->y = r.y;
		pIntersect->h = min(r.y+r.h, y+h) - r.y;
	}

	if(pIntersect->w<0) return false;
	if(pIntersect->h<0) return false;

	return true;
}
