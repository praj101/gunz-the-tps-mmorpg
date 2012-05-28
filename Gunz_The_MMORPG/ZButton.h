#pragma once

#include "CCButton.h"
#include "CCBmButton.h"

class ZButton: public CCButton 
{
protected:
	unsigned char	m_nIllumination;
	DWORD			m_dwCurrentTime;
	DWORD			m_dwLastTime;
	DWORD			m_dwClickedTime;
	bool			m_bClicked;

	virtual void OnMouseIn();
	virtual void OnMouseOut();
	virtual void OnButtonDown();
	virtual void OnButtonUp();
	virtual void OnButtonClick();

	virtual bool OnShow();
	virtual void OnHide();

	virtual void OnDraw(CCDrawContext* pDC);

public:
	ZButton(const char* szName, CCWidget* pParent, CCListener* pListener);
	virtual ~ZButton();
};

class ZBmButton: public CCBmButton 
{
protected:
	unsigned char	m_nIllumination;
	DWORD			m_dwCurrentTime;
	DWORD			m_dwLastTime;
	DWORD			m_dwClickedTime;
	bool			m_bClicked;

	virtual void OnMouseIn(void);
	virtual void OnMouseOut(void);
	virtual void OnButtonDown(void);
	virtual void OnButtonUp(void);
	virtual void OnButtonClick(void);

	virtual bool OnShow(void);
	virtual void OnHide(void);

	virtual void OnDraw(CCDrawContext* pDC);

public:
	ZBmButton(const char* szName, CCWidget* pParent, CCListener* pListener);
	virtual ~ZBmButton(void);
};