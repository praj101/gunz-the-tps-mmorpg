#ifndef CCBLABELLOOK_H
#define CCBLABELLOOK_H

#include "CCLabel.h"

class CCBLabelLook : public CCLabelLook{
public:
	CCFont*		m_pFont;
	sColor		m_FontColor;

protected:
	virtual void OnDraw(CCLabel* pLabel, CCDrawContext* pDC);

public:
	CCBLabelLook(void);

	virtual sRect GetClientRect(CCLabel* pLabel, sRect& r);
};

#endif
