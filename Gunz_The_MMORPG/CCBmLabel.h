#pragma once

#include "CCWidget.h"
#include "CCDrawContext.h"
#include "CCLookNFeel.h"

#define CCBMLABEL	"BmLabel"

class CCBmLabel : public CCWidget{
protected:
	CCBitmap*		m_pLabelBitmap;
	sSize			m_CharSize;

	virtual void OnDraw(CCDrawContext* pDC);

public:
	CCBmLabel(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);

	void SetLabelBitmap(CCBitmap* pLabelBitmap);
	void SetCharSize(sSize& size);

	virtual const char* GetClassName(){ return CCBMLABEL; } ;
};