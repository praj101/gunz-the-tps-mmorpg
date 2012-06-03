#ifndef MBMULTICOLLISTBOXLOOK_H
#define MBMULTICOLLISTBOXLOOK_H

#include "CCMultiColListBox.h"

class CCBMultiColListBoxLook : public CCMultiColListBoxLook{
public:
	CCBitmap*	m_pFrameBitmaps[9];
	CCFont*		m_pFont;

protected:
	virtual void OnFrameDraw(CCMultiColListBox* pListBox, CCDrawContext* pDC);

public:
	CCBMultiColListBoxLook(void);

	virtual sRect GetClientRect(CCMultiColListBox* pListBox, sRect& r);
	virtual void OnDraw(CCMultiColListBox* pListBox, CCDrawContext* pDC);
};

#endif