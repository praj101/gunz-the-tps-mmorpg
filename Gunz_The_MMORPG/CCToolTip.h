#pragma once

#include "CCWidget.h"

#define CCTOOLTIP	"ToolTip"

class CCToolTip : public CCWidget{
protected:
	bool	m_bUseParentName;

public:
	CCToolTip(const char* szName, CCWidget* pParent);
	virtual ~CCToolTip();

	void SetText(const char* szText);
	bool IsUseParentName();

	virtual void OnDraw(CCDrawContext* pDC);

	virtual const char* GetClassName(){ return CCTOOLTIP; };
};