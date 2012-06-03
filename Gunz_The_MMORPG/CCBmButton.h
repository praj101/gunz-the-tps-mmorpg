#pragma once

#include "CCButton.h"
#include "CCEdit.h"

#define CORE_CCBMBUTTON	"BmButton"

class CCBmButton : public CCButton{
protected:
	CCBitmap*	m_pUpBitmap;
	CCBitmap*	m_pDownBitmap;
	CCBitmap*	m_pOverBitmap;
	CCBitmap*	m_pDisableBitmap;
	bool			m_bStretch;
public:
	bool			m_bTextColor;
	sColor	m_BmTextColor;

protected:
	virtual void OnDownDraw(CCDrawContext* pDC);
	virtual void OnUpDraw(CCDrawContext* pDC);
	virtual void OnOverDraw(CCDrawContext* pDC);
	virtual void OnDisableDraw(CCDrawContext* pDC);
	virtual void OnDraw(CCDrawContext* pDC);

public:
	CCBmButton(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);

	void SetUpBitmap(CCBitmap* pBitmap);
	void SetDownBitmap(CCBitmap* pBitmap);
	void SetDisableBitmap(CCBitmap* pBitmap);
	void SetOverBitmap(CCBitmap* pBitmap);
	virtual const char* GetClassName(void){ return CORE_CCBMBUTTON; }

	void SetStretch( bool b ){		m_bStretch = b;	}

	virtual sRect GetClientRect(void){ return CCWidget::GetClientRect(); }
};

