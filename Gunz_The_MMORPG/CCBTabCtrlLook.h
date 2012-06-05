#ifndef MBTABCTRLLOOK_H
#define MBTABCTRLLOOK_H

#include "CCTabCtrl.h"

class CCBTabCtrlLook : public CCTabCtrlLook {
public:
	CCBitmap*	m_pFrameBitmaps[9];

public:
	CCBTabCtrlLook(void);

//	virtual void OnFrameDraw(CCTabCtrl* pTabCtrl, CCDrawContext* pDC);
	virtual sRect GetClientRect(CCTabCtrl* pTabCtrl, sRect& r);

protected:
	virtual void	OnDraw(CCTabCtrl* pTabCtrl, CCDrawContext* pDC);
};

#endif