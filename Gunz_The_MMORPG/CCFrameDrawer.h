#pragma once
#include "CCDrawContext.h"

enum CCFDTextStyle{
	CCFDTS_NORMAL,
	CCFDTS_ACTIVE,
	CCFDTS_DISABLE,
};

////////////////////////////////////////////////////
// Frame을 그리기 위한 클래스
// 이를 통해 스킨을 지원할 수 있다.
// Default Frame Drawer
class MFrameDrawer{
public:
	virtual void DrawOuterBevel(CCDrawContext* pDC, sRect& r);		// 바깥쪽 윤곽 그리기
	virtual void DrawInnerBevel(CCDrawContext* pDC, sRect& r);		// 안쪽 윤곽 그리기
	virtual void DrawFlatBevel(CCDrawContext* pDC, sRect& r);		// 평면 윤곽 그리기 ( 보통 눌렸을때... )
	virtual void DrawOuterPlane(CCDrawContext* pDC, sRect& r);		// 바깥쪽 면
	virtual void DrawInnerPlane(CCDrawContext* pDC, sRect& r);		// 안쪽 면
	virtual void DrawFlatPlane(CCDrawContext* pDC, sRect& r);		// 평면 면
	virtual void DrawOuterBorder(CCDrawContext* pDC, sRect& r);		// 바깥쪽 판(Bevel+Plane) 그리기
	virtual void DrawInnerBorder(CCDrawContext* pDC, sRect& r);		// 안쪽 판(Bevel+Plane) 그리기
	virtual void DrawFlatBorder(CCDrawContext* pDC, sRect& r);		// 일반적인 평면 판(Bevel+Plane) 그리기
	virtual void Text(CCDrawContext* pDC, sRect& r, const char* szText, CCAlignmentMode am=MAM_NOTALIGN, CCFDTextStyle nTextStyle=CCFDTS_NORMAL, bool bHighlight=false);
	virtual void Text(CCDrawContext* pDC, sPoint& p, const char* szText, CCFDTextStyle nTextStyle=CCFDTS_NORMAL, bool bHighlight=false, sRect* r=NULL);
	virtual void TextMC(CCDrawContext* pDC, sRect& r, const char* szText, CCAlignmentMode am=MAM_NOTALIGN, CCFDTextStyle nTextStyle=CCFDTS_NORMAL, bool bHighlight=false);
	virtual void TextMC(CCDrawContext* pDC, sPoint& p, const char* szText, CCFDTextStyle nTextStyle=CCFDTS_NORMAL, bool bHighlight=false, sRect* r=NULL);
};

#endif