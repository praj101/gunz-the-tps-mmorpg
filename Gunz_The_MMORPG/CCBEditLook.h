#ifndef CCBEDITLOOK_H
#define CCBEDITLOOK_H

#include "CCEdit.h"

class CCBEditLook : public CCEditLook{
public:
	CCBitmap*	m_pFrameBitmaps[9];
	CCFont*		m_pFont;
	

public:
	CCBEditLook(void);

	virtual void OnFrameDraw(CCEdit* pEdit, CCDrawContext* pDC);
	virtual sRect GetClientRect(CCEdit* pEdit, sRect& r);
	virtual void OnDraw(CCEdit* pEdit, CCDrawContext* pDC);
};

#endif