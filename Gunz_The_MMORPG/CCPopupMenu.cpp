#include "stdafx.h"
#include "CCPopupMenu.h"
#include "CCColorTable.h"

#define MENUITEM_MARGIN_X	20
#define MENUITEM_MARGIN_Y	4

IMPLEMENT_LOOK(CCPopupMenu, CCPopupMenuLook)

CCPopupMenuLook::CCPopupMenuLook(void){//MPopupMenuLook
	m_SelectedPlaneColor = DEFCOLOR_CCLIST_SELECTEDPLANE;
	m_SelectedTextColor = DEFCOLOR_CCLIST_SELECTEDTEXT;
	m_UnfocusedSelectedPlaneColor = DEFCOLOR_DARK;
}

void CCPopupMenuLook::OnFrameDraw(CCPopupMenu* pPopupMenu, CCDrawContext* pDC){
	sRect r = pPopupMenu->GetClientRect();
	pDC->SetColor(sColor(DEFCOLOR_CCPOPUP_PLANE));
	pDC->FillRectangle(r);
	pDC->SetColor(sColor(DEFCOLOR_FRAME_OUTLINE));
	pDC->Rectangle(r);
}

void CCPopupMenuLook::OnDraw(CCPopupMenu* pPopupMenu, CCDrawContext* pDC){
	OnFrameDraw(pPopupMenu, pDC);

	if(pPopupMenu->GetPopupMenuType()==CCPMT_VERTICAL){
		pDC->SetColor(sColor(DEFCOLOR_FRAME_OUTLINE));
		pDC->Rectangle(pPopupMenu->GetInitialClientRect());
	}
}

sRect CCPopupMenuLook::GetClientRect(CCPopupMenu* pPopupMenu, sRect& r){
	if(pPopupMenu->GetPopupMenuType()==CCPMT_VERTICAL)
		return sRect(1, 1, r.w-2, r.h-2);
	else
		return pPopupMenu->GetInitialClientRect();
}

void CCMenuItem::OnDrawMenuItem(CCDrawContext* pDC, bool bSelected){//MMenuItem
	sRect r = GetClientRect();	
	pDC->SetColor(bSelected==true?sColor(DEFCOLOR_CCPOPUP_SELECTEDPLANE):sColor(DEFCOLOR_CCPOPUP_PLANE));
	pDC->FillRectangle(r);
	pDC->SetColor(bSelected==true?sColor(DEFCOLOR_CCPOPUP_SELECTEDTEXT):sColor(DEFCOLOR_CCPOPUP_TEXT));
	pDC->Text(r, GetText(), CCD_LEFT);
}

void CCMenuItem::OnDraw(CCDrawContext* pDC){
	OnDrawMenuItem(pDC, IsSelected());
}

bool CCMenuItem::OnEvent(CCEvent* pEvent, CCListener* pListener){
	sRect r = GetClientRect();
	switch(pEvent->iMessage){
	case CCWM_MOUSEMOVE:
	case CCWM_LBUTTONDOWN:
	case CCWM_RBUTTONDOWN:
	case CCWM_LBUTTONDBLCLK:
		if(r.InPoint(pEvent->sPos)==true){
			CCPopupMenu* pPopupMenu = (CCPopupMenu *)GetParent();
			pPopupMenu->Select(this);
			return true;
		}
		//else Select(false);
		break;
	case CCWM_LBUTTONUP:
		if(r.InPoint(pEvent->sPos)==true){
			CCPopupMenu* pPopupMenu = (CCPopupMenu *)GetParent();
			if(GetSubMenu()!=NULL) pPopupMenu->Select(this);
			else{
				if(pListener!=NULL) pListener->OnCommand(this, GetText());
			}
			return true;
		}
		break;
	case CCWM_RBUTTONUP:
		break;
	}
	return false;
}

CCMenuItem::CCMenuItem(const char* szName) : CCWidget(szName, NULL, NULL){
	m_bSelected = false;
}

CCMenuItem::~CCMenuItem(void){
	while(m_Children.GetCount()>0){
		CCWidget* pWidget = m_Children.Get(0);
		delete pWidget;
	}
}

bool CCMenuItem::IsSelected(void){
	return m_bSelected;
}

int CCMenuItem::GetWidth(void){
	CCFont* pFont = GetFont();
	const char* szText = GetText();

	return pFont->GetWidth(szText) + MENUITEM_MARGIN_X;
}

int CCMenuItem::GetHeight(void){
	CCFont* pFont = GetFont();
	return pFont->GetHeight() + MENUITEM_MARGIN_Y;
	return GetClientRect().h;
}

CCPopupMenu* CCMenuItem::CreateSubMenu(void){
	if(m_Children.GetCount()>0) return (CCPopupMenu *)m_Children.Get(0);
	CCPopupMenu* pSubMenu = new CCPopupMenu("SubMenu", this, GetParent());
	return pSubMenu;
}

CCPopupMenu* CCMenuItem::GetSubMenu(void){
	if(m_Children.GetCount()>0) return (CCPopupMenu *)m_Children.Get(0);
	return NULL;
}

void CCMenuItem::Select(bool bSelect)
{
	m_bSelected = bSelect;

	sRect r = GetClientRect();

	if(m_bSelected==true){
		CCPopupMenu* pSubMenu = GetSubMenu();
		if(pSubMenu!=NULL && pSubMenu->IsVisible()==false){
			CCPopupMenu* pPopupMenu = (CCPopupMenu *)GetParent();
			if(pPopupMenu->GetType()==CCPMT_VERTICAL) pSubMenu->Show(r.x+r.w, r.y, true);
			else pSubMenu->Show(r.x, r.y+r.h, true);
		}
	}
	else{
		CCPopupMenu* pSubMenu = GetSubMenu();
		if(pSubMenu!=NULL) pSubMenu->Show(false);
	}
}

bool CCPopupMenu::OnEvent(CCEvent* pEvent, CCListener* pListener){//MPopupMenu
	sRect r = GetClientRect();
	switch(pEvent->iMessage){
	case CCWM_LBUTTONDOWN:
	case CCWM_LBUTTONUP:
	case CCWM_LBUTTONDBLCLK:
	case CCWM_RBUTTONDOWN:
	//case MWM_RBUTTONUP:
	case CCWM_RBUTTONDBLCLK:
	case CCWM_CCButtonDOWN:
	case CCWM_CCButtonUP:
	case CCWM_CCButtonDBLCLK:
		if(r.InPoint(pEvent->sPos)==false){
			if(m_iPopupMenuType==CCPMT_VERTICAL) Show(false);
			else Select((CCMenuItem *)NULL);
		}
		break;
	}
	return false;
}

CCPopupMenu::CCPopupMenu(const char* szName, CCWidget* pParent, CCListener* pListener, CCPopupMenuTypes t) : CCWidget(szName, pParent, pListener){
	m_iPopupMenuType = t;

	LOOK_IN_CONSTRUCTOR()
}

CCPopupMenu::~CCPopupMenu(void){
	while(m_Children.GetCount()>0){
		CCWidget* pWidget = m_Children.Get(0);
		delete pWidget;
	}
}

CCMenuItem* CCPopupMenu::AddMenuItem(const char* szMenuName){
	CCMenuItem* pNewItem = new CCMenuItem(szMenuName);
	AddMenuItem(pNewItem);
	return pNewItem;
}

void CCPopupMenu::AddMenuItem(CCMenuItem* pMenuItem){
	AddChild(pMenuItem);
	pMenuItem->SetListener(this);

	sRect cr = GetClientRect();
	sRect ir = GetInitialClientRect();
	if(m_iPopupMenuType==CCPMT_VERTICAL){
		int y = 0;
		int nWidth = 0;
		for(int i=0; i<m_Children.GetCount(); i++){
			CCMenuItem* pMenuItem = (CCMenuItem *)m_Children.Get(i);
			pMenuItem->SetPosition(cr.x, cr.y+y);
			nWidth = max(nWidth, pMenuItem->GetWidth());
			y += pMenuItem->GetHeight();
		}
		for(int i=0; i<m_Children.GetCount(); i++){
			CCMenuItem* pMenuItem = (CCMenuItem *)m_Children.Get(i);
			pMenuItem->SetSize(nWidth, pMenuItem->GetHeight());
		}
		SetSize(nWidth+ir.w-cr.w-1, y+ir.h-cr.h-1);
	}
	else{
		int x = 0;
		int nHeight = 0;
		for(int i=0; i<m_Children.GetCount(); i++){
			CCMenuItem* pMenuItem = (CCMenuItem *)m_Children.Get(i);
			pMenuItem->SetPosition(cr.x+x, cr.y);
			nHeight = max(nHeight, pMenuItem->GetHeight());
			x += pMenuItem->GetWidth();
		}
		for(int i=0; i<m_Children.GetCount(); i++){
			CCMenuItem* pMenuItem = (CCMenuItem *)m_Children.Get(i);
			pMenuItem->SetSize(pMenuItem->GetWidth(), nHeight);
		}
		SetSize(x+ir.w-cr.w-1, nHeight+ir.h-cr.h-1);
	}
}

void CCPopupMenu::RemoveMenuItem(CCMenuItem* pMenuItem){
	delete pMenuItem;
}

void CCPopupMenu::RemoveAllMenuItem(void){
	while(GetChildCount()>0){
		CCWidget* pChild = m_Children.Get(0);
		delete pChild;
	}
}

void CCPopupMenu::Show(int x, int y, bool bVisible){
	CCWidget::Show(bVisible);
	if(bVisible==true) SetPosition(x, y);
}

void CCPopupMenu::Show(bool bVisible){
	Select((CCMenuItem *)NULL);
	CCWidget::Show(bVisible);;
}

void CCPopupMenu::SetType(CCPopupMenuTypes t){
	m_iPopupMenuType = t;
}

CCPopupMenuTypes CCPopupMenu::GetType(void){
	return m_iPopupMenuType;
}

void CCPopupMenu::Select(int idx){
	for(int i=0; i<m_Children.GetCount(); i++){
		CCMenuItem* pMenuItem = (CCMenuItem *)m_Children.Get(i);
		if(i==idx) pMenuItem->Select(true);
		else pMenuItem->Select(false);
	}
}

void CCPopupMenu::Select(CCMenuItem* pMenuItem){
	for(int i=0; i<m_Children.GetCount(); i++){
		CCMenuItem* pThisMenuItem = (CCMenuItem *)m_Children.Get(i);
		if(pThisMenuItem==pMenuItem) pThisMenuItem->Select(true);
		else pThisMenuItem->Select(false);
	}
}

bool CCPopupMenu::OnCommand(CCWidget* pWindow, const char* szMessage){
	if(GetType()==CCPMT_VERTICAL) Show(false);
	else Select((CCMenuItem*)NULL);
	CCListener* pListener = GetListener();
	if(pListener!=NULL) pListener->OnCommand(this, szMessage);
	return true;
}
