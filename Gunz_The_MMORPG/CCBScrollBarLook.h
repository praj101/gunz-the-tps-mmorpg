#ifndef CCBScrollBarLook_H
#define CCBScrollBarLook_H

#include "CCScrollBar.h"

class CCBArrowLook : public CCArrowLook{
public:
	CCBitmap*	m_pArrowBitmaps[8];
protected:
	virtual void OnDrawUpArrow(CCDrawContext* pDC, sRect& r, bool bPressed);
	virtual void OnDrawDownArrow(CCDrawContext* pDC, sRect& r, bool bPressed);
	virtual void OnDrawLeftArrow(CCDrawContext* pDC, sRect& r, bool bPressed);
	virtual void OnDrawRightArrow(CCDrawContext* pDC, sRect& r, bool bPressed);
public:
	CCBArrowLook(void);

	virtual sSize GetDefaultSize(CCArrow* pThumb);
};

class CCBThumbLook : public CCThumbLook{
public:
	CCBitmap*	m_pHBitmaps[3];
	CCBitmap*	m_pHPressedBitmaps[3];
	CCBitmap*	m_pVBitmaps[3];
	CCBitmap*	m_pVPressedBitmaps[3];
public:
	virtual void OnDraw(CCThumb* pThumb, CCDrawContext* pDC);

	CCBThumbLook(void);
};

class CCBScrollBarLook : public CCScrollBarLook{
public:
	CCBitmap*	m_pHFrameBitmaps[3];
	CCBitmap*	m_pVFrameBitmaps[3];

public:
	CCBScrollBarLook(void);

	virtual void OnDraw(CCScrollBar* pScrollBar, CCDrawContext* pDC);
	virtual sRect GetClientRect(CCScrollBar* pScrollBar, sRect& r);
};

#endif