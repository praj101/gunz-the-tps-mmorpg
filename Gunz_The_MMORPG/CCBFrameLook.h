#ifndef CCBFrameLOOK_H
#define CCBFrameLOOK_H

#include "CCFrame.h"
#include "CCScalableLook.h"

#define FRAME_BITMAP_COUNT			9
#define FRAME_BUTTON_BITMAP_COUNT	3

class CCBFrameLook : public CCFrameLook , public CCScalableLook {
public:
	char		m_szDefaultTitle[256];
	CCBitmap*		m_pFrameBitmaps[FRAME_BITMAP_COUNT];
	CCFont*			m_pFont;
	sColor		m_FontColor;
	sColor		m_BGColor;
	sPoint		m_TitlePosition;
	bool				m_bStretch;
	CCBitmap*		m_pCloseButtonBitmaps[FRAME_BUTTON_BITMAP_COUNT];	// 0 - up, 1 - down, 2 - disable
	CCBitmap*		m_pMinimizeButtonBitmaps[FRAME_BUTTON_BITMAP_COUNT];
protected:
	virtual void	OnDraw(CCFrame* pFrame, CCDrawContext* pDC);
	int					m_iCustomLook;

public:
	CCBFrameLook(void);
	void SetCustomLook( int i) {
		m_iCustomLook = i;
	}
	int GetCustomLook() const{
		return m_iCustomLook;
	}

	virtual sRect GetClientRect(CCFrame* pFrame, sRect& r);
};

#endif