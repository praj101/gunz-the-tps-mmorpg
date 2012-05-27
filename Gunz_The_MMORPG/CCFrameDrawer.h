#pragma once
#include "CCDrawContext.h"

enum CCFDTextStyle{
	CCFDTS_NORMAL,
	CCFDTS_ACTIVE,
	CCFDTS_DISABLE,
};

////////////////////////////////////////////////////
//Frame for drawing class
//This can be supported through the skin.
//Default Frame Drawer
class CCFrameDrawer{
public:
	virtual void DrawOuterBevel(CCDrawContext* pDC, sRect& r);		//Draw the outer contour
	virtual void DrawInnerBevel(CCDrawContext* pDC, sRect& r);		//Draw the inner contour
	virtual void DrawFlatBevel(CCDrawContext* pDC, sRect& r);		//flat contour drawing (usually)
	virtual void DrawOuterPlane(CCDrawContext* pDC, sRect& r);		//If the outer
	virtual void DrawInnerPlane(CCDrawContext* pDC, sRect& r);		//inner surface
	virtual void DrawFlatPlane(CCDrawContext* pDC, sRect& r);		//If the plane
	virtual void DrawOuterBorder(CCDrawContext* pDC, sRect& r);		//outer plates (Bevel + Plane) Drawing
	virtual void DrawInnerBorder(CCDrawContext* pDC, sRect& r);		//Inside Edition (Bevel + Plane) Drawing
	virtual void DrawFlatBorder(CCDrawContext* pDC, sRect& r);		//typical flat plate (Bevel + Plane) Drawing
	virtual void Text(CCDrawContext* pDC, sRect& r, const char* szText, CCAlignmentMode am=MAM_NOTALIGN, CCFDTextStyle nTextStyle=CCFDTS_NORMAL, bool bHighlight=false);
	virtual void Text(CCDrawContext* pDC, sPoint& p, const char* szText, CCFDTextStyle nTextStyle=CCFDTS_NORMAL, bool bHighlight=false, sRect* r=NULL);
	virtual void TextMC(CCDrawContext* pDC, sRect& r, const char* szText, CCAlignmentMode am=MAM_NOTALIGN, CCFDTextStyle nTextStyle=CCFDTS_NORMAL, bool bHighlight=false);
	virtual void TextMC(CCDrawContext* pDC, sPoint& p, const char* szText, CCFDTextStyle nTextStyle=CCFDTS_NORMAL, bool bHighlight=false, sRect* r=NULL);
};

#endif