#pragma once

#include "CCButton.h"
#include "CCBCCButton.h"

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

class cBCCButton : public CCBCCButton{ 
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
	cBCCButton(const char* szName, CCWidget* pParent, CCListener* pListener);
	virtual ~cBCCButton();
};