#ifndef CCBGROUPLOOK_H
#define CCBGROUPLOOK_H

#include "CCGroup.h"

#define FRAME_BITMAP_COUNT	9
class CCBGroupLook : public CCGroupLook{
public:
	CCBitmap*	m_pFrameBitmaps[FRAME_BITMAP_COUNT];
	CCFont*		m_pFont;
	sColor		m_FontColor;
	sPoint		m_TitlePosition;
	bool		m_bStretch;

protected:
	virtual void OnDraw(CCGroup* pGroup, CCDrawContext* pDC);

public:
	CCBGroupLook(void);

	virtual sRect GetClientRect(CCGroup* pGroup, sRect& r);
};

#endif