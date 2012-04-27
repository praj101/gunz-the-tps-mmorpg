#pragma once

#include "CCButton.h"
#include "CCEdit.h"

#define CCBMBUTTON	"BmButton"

class CCBmButton : public CCButton{
protected:
	CCBitmap*		m_pUpBitmap;
	CCBitmap*		m_pDownBitmap;
	CCBitmap*		m_pOverBitmap;
	CCBitmap*		m_pDisableBitmap;

public:
	bool			m_bTextColor;
	cColor			m_BmTextColor;

protected:
	virtual void	OnDownDraw(CCDrawContext* pDC);
	virtual void	OnUpDraw(CCDrawContext* pDC);
	virtual void	OnOverDraw(CCDrawContext* pDC);
	virtual void	OnDiableDraw(CCDrawContext* pDC);
	virtual void	OnDraw(CCDrawContext* pDC);

public:
	CCBmButton(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);

	void SetUpBitmap(CCBitmap* pBitmap);
	void SetDownBitmap(CCBitmap* pBitmap);
	void SetOverBitmap(CCBitmap* pBitmap);
	void SetDisableBitmap(CCBitmap* pBitmap);

	void SetStretch(bool b) { m_bStretch=b; };
	virtual sRect GetClientRect() { return CCWidget::GetClientRect(); };
	virtual const char* GetClassName(){ return CCBMBUTTON; };
};