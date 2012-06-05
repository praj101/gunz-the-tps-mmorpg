#ifndef CCBSliderLook_H
#define CCBSliderLook_H

#include "CCSlider.h"

class CCBSliderThumbLook : public CCSliderThumbLook
{
public:
	CCBitmap*	m_pBitmap;
	CCBitmap*	m_pPressedBitmap;
	sColor		m_ThumbColor;

public:
	virtual void OnDraw(CCSliderThumb* pThumb, CCDrawContext* pDC);

	CCBSliderThumbLook(void);

	virtual sSize GetDefaultSize(CCSliderThumb* pThumb);
};

#endif