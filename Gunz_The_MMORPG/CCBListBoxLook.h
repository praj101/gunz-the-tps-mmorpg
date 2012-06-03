<<<<<<< .mine
#ifndef CCBListBoxLook_H
#define CCBListBoxLook_H
=======
#pragma once
>>>>>>> .r362

#include "CCListBox.h"

class CCBListBoxLook : public CCListBoxLook{
public:
	CCBitmap*	m_pFrameBitmaps[9];
	CCFont*		m_pFont;

protected:
	virtual void OnFrameDraw(CCListBox* pListBox, CCDrawContext* pDC);

public:
	CCBListBoxLook(void);

	virtual sRect GetClientRect(CCListBox* pListBox, sRect& r);
	virtual void OnDraw(CCListBox* pListBox, CCDrawContext* pDC);
};

#endif