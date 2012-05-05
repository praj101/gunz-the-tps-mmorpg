#pragma once

#include "CCButton.h"
#include "CCBCCButton.h"

class ZButton : public CCButton{
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
	ZButton(const char* szName, CCWidget* pParent, CCListener* pListener);
	virtual ~ZButton(void);
};

class ZBCCButton: public CCBCCButton 
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
	ZBCCButton(const char* szName, CCWidget* pParent, CCListener* pListener);
	virtual ~ZBCCButton(void);
};