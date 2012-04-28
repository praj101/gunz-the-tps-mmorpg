#pragma once

#include "CCWidget.h"
#include "CCDrawContext.h"
#include "CCLookNFeel.h"

class CCPopupMenu;
class CCMenuItem;

#define CCPOPUPMENU	"PopupMenu"
#define CCMENUITEM	"MenuItem"

class CCPopupMenuLook {
public: 
	sColor		m_SelectedPlaneColor;
	sColor		m_SelectedTextColor;
	sColor		m_UnfocusedSelectedPlaneColor;

protected:
	virtual void OnFrameDraw(CCPopupMenu* pPopupMenu, CCDrawContext* pDC);
public:
	CCPopupMenuLook();
	virtual void OnDraw(CCPopupMenu* pPopupMenu, CCDrawContext* pDC);
	virtual sRect GetClientRect(CCPopupMenu* pPopupMenu, sRect& r);
};

class CCMenuItem : public CCWidget{
private:
	bool			m_bSelected;

protected:
	virtual void OnDraw(CCDrawContext* pDC);
	virtual void OnDrawMenuItem(CCDrawContext* pDC, bool bSelected);
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);
public:
	CCMenuItem(const char* szName=NULL);
	virtual ~CCMenuItem();

	bool	IsSelected();
	
	int		GetWidth();
	int		GetHeight();

	virtual	CCPopupMenu* CreateSubMenu();
	CCPopupMenu* GetSubMenu();
	void	Select(bool bSelect);

	virtual const char* GetClassName(){ return CCMENUITEM; }
};

enum CCPopupMenuTypes{
	CCPMT_VERTICAL = 0,
	CCPMT_HORIZONTAL = 1
};

class CCPopupMenu : public CCWidget{
protected:
	CCPopupMenuTypes m_iPopupMenuType;
	int				m_iSelectedMenu;

protected:
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);

	virtual bool OnCommand(CCWidget* pWindow, const char* szMessage);

public:
	CCPopupMenu(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL, CCPopupMenuTypes t=CCPMT_VERTICAL);
	virtual ~CCPopupMenu();

	CCPopupMenuTypes GetPopupMenuType() { return m_iPopupMenuType; }
	virtual	CCMenuItem* AddMenuItem(const char* szMenuName);
	void	AddMenuItem(CCMenuItem* pMenuItem);
	void	RemoveMenuItem(CCMenuItem* pMenuItem);
	void	RemoveAllMenuItem();

	virtual void Show(bool bVisible=true);
	virtual void Show(int x, int y, bool bVisible=true);

	void	SetType(CCPopupMenuTypes t);
	CCPopupMenuTypes GetType();

	void	Select(int idx);
	void	Select(CCMenuItem* pMenuItem);

	virtual const char* GetClassName(){ return CCPOPUPMENU; }

	DECLARE_LOOK(CCPopupMenuLook)
	DECLARE_LOOK_CLIENT()
};
