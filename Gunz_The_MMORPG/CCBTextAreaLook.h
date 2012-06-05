#ifndef MBTEXTAREALOOK_H
#define MBTEXTAREALOOK_H

#include "CCTextArea.h"

class CCBTextAreaLook : public CCTextAreaLook{
public:
	CCFont*		m_pFont;
	CCBitmap*	m_pFrameBitmaps[9];

public:
	CCBTextAreaLook(void);

	virtual void OnFrameDraw(CCTextArea* pTextArea, CCDrawContext* pDC);
	virtual sRect GetClientRect(CCTextArea* pTextArea, sRect& r);
};

#endif