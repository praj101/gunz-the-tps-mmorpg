#pragma once

#include <list>
#include "CCWidget.h"
#include "CCLookNFeel.h"

using namespace std;

class CCTabCtrl;
class CCDrawContext;
class CCButton;

#define CCTABCTRL	"TabCtrl"

class CCTabCtrlLook{
public:
	virtual void	OnDraw(CCTabCtrl* pTabCtrl, CCDrawContext* pDC);
	virtual sRect	GetClientRect(CCTabCtrl* pTabCtrl, sRect& r);
};

class CCTabItem{
public:
	CCWidget*	pFrame;
	CCButton*	pButton;
	CCListener*	pButtonListener;

	CCTabItem(CCWidget* pFrame, CCButton* pButton, CCListener* pListener);
};

class CCTabCtrl : public CCWidget{
	protected:
	int			m_iCurrentSel;
	CCTabList	m_TabList;

public:
	CCTabCtrl(const char* szName, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	CCTabCtrl(CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~MTabCtrl();

	void	Add(MButton *pButton, CCWidget *pFrame);
	void	RemoveAll();

	int		GetCount();
	int		GetSelIndex();
	bool	SetSelIndex(int i);

	void	UpdateListeners();		

	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage);

public:
	DECLARE_LOOK(CCTabCtrlLook)
	DECLARE_LOOK_CLIENT()

	virtual const char* GetClassName(){ return CCTABCTRL; }

};