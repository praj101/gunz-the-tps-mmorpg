#pragma once

#include "CCWidget.h"
#include "CCDrawContext.h"
#include "CCLookNFeel.h"

#define CCLABEL	"Label"

class CCLabel;

class CCLabelLook{
public:
	virtual void	OnDraw(CCLabel* pLabel, CCDrawContext* pDC);
	virtual sRect	GetClientRect(CCLabel* pLabel, mRect& r);
};

class CCLabel: public CCWidget{
protected:
	sColor			m_TextColor;
	CCAlignemntMode	m_AlignmentMode;

	DECLARE_LOOK(CCLabelLook);
	DECLARE_LOOK_CLIENT();

public:
	CCLabel(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);

	void	SetTextColor(sColor color);
	sColor	GetTextColor();

	CCAlignmentMode	GetAlignment();
	CCAlignmentMode	SetAlignment(CCAlignmentMode am);

	virtual const char* GetClassName() { return CCLABEL; };
};