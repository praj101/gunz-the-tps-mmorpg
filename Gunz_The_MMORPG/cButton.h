#pragma once

#include "CCButton.h"
#include "CCBmButton.h"

class cButton : public CCButton{
protected:
	uchar			m_iIllumination;
	DWORD			m_dwCurrentTime;
	DWORD			m_dwLastTime;
	DWORD			m_dwClickedTime;
	bool			m_bClicked;

	virtual void	OnMouseIn();
	virtual void	OnMouseOut();
	virtual void	OnButtonDown();
	virtual void	OnButtonUp();
	virtual void	OnButtonClick();

	virtual bool	OnShow();
	virtual void	OnHide();

	virtual void	OnDraw(CCDrawContext* pDC);

public:
	cButton(const char* szName, CCWidget* pParent, CCListener* pListener);
	virtual ~cButton();
};

class cBmButton : public CCBmButton{ 
protected:
	uchar			m_iIllumination;
	DWORD			m_dwCurrentTime;
	DWORD			m_dwLastTime;
	DWORD			m_dwClickedTime;
	bool			m_bClicked;

	virtual void	OnMouseIn();
	virtual void	OnMouseOut();
	virtual void	OnButtonDown();
	virtual void	OnButtonUp();
	virtual void	OnButtonClick();

	virtual bool	OnShow();
	virtual void	OnHide();

	virtual void	OnDraw(CCDrawContext* pDC);
public:
	cBmButton(const char* szName, CCWidget* pParent, CCListener* pListener);
	virtual ~cBmButton();
};