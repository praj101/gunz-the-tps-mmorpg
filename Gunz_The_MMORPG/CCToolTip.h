#pragma once

#include "CCWidget.h"

#define CORE_CCTOOLTIP	"ToolTip"

class CCToolTip : public CCWidget{
protected:
	bool	m_bUseParentName;

public:
	CCToolTip(const char* szName, CCWidget* pParent);
	virtual ~CCToolTip();

	virtual void SetBounds(); //Was removed?

	void SetText(const char* szText);
	bool IsUseParentName();

	virtual void OnDraw(CCDrawContext* pDC);

	virtual const char* GetClassName(){ return CORE_CCTOOLTIP; };
};