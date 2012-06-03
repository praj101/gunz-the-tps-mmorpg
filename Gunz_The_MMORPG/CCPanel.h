#pragma once
#include "CCWidget.h"
#include "CCDrawContext.h"
#include "CCLookNFeel.h"

#define CORE_CCPANEL	"Panel"

class CCPanel;

class CCPanelLook{
protected:
	virtual void OnFrameDraw(CCPanel* pPanel, CCDrawContext* pDC);

public:
	virtual void OnDraw(CCPanel* pLabel, CCDrawContext* pDC);
	virtual sRect GetClientRect(CCPanel* pLabel, sRect& r);
};

enum CCBorderStyle {
	CCBS_NONE = 0,
	CCBS_SINGLE,
};

/// Panel
class CCPanel : public CCWidget{
protected:
	DECLARE_LOOK(CCPanelLook)
	DECLARE_LOOK_CLIENT()
protected:
	CCBorderStyle	m_iBorderStyle;
	sColor			m_BorderColor;
	sColor			m_BackgroundColor;
public:
	CCPanel(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);

	void	SetBackgroundColor(sColor color);
	sColor	GetBackgroundColor();


	void	SetBorderColor(sColor color);
	sColor	GetBorderColor();
	void	SetBorderStyle(CCBorderStyle style);
	CCBorderStyle GetBorderStyle();
	virtual const char* GetClassName(){ return CORE_CCPANEL; }
};
