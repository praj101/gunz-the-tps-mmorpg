#pragma once

#include "CCButton.h"

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