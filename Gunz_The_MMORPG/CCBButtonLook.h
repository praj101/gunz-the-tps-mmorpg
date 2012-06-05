#ifndef CCBButtonLook_H
#define CCBButtonLook_H

#include "CCButton.h"
#include "CCScalableLook.h"

class CCBButtonLook : public CCButtonLook, public CCScalableLook {
public:
	CCBitmap*	m_pUpBitmaps[9];
	CCBitmap*	m_pDownBitmaps[9];
	CCBitmap*	m_pOverBitmaps[9];
	CCBitmap*	m_pFocusBitmaps[4];
	CCFont*		m_pFont;
	sColor		m_FontColor;
	sColor		m_FontDisableColor;
	sColor		m_FontDownColor;
	sColor		m_FontHighlightColor;
	bool		m_bStretch;
	bool		m_bCustomLook;
	sPoint		m_FontDownOffset;
	
protected:
	virtual void DrawText(CCButton* pButton, CCDrawContext* pDC, sRect& r, const char* szText, CCAlignmentMode a);
	virtual void DrawFocus(CCDrawContext* pDC, sRect& r);
	virtual void OnDownDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual void OnUpDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual void OnOverDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual void OnDisableDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual void OnCheckBoxDraw(CCButton* pButton, CCDrawContext* pDC, bool bPushed);
	virtual void OnDraw(	CCButton* pButton, CCDrawContext* pDC );
public:
	CCBButtonLook(void);
	void SetCustomLook(bool b) {
		m_bCustomLook = b;
	}
	bool GetCustomLook() const{
		return m_bCustomLook;
	}

	virtual sRect GetClientRect(CCButton* pButton, sRect& r);
};

#endif
