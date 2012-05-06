#include "stdafx.h"
#include "CCTabCtrl.h"
#include "CCColorTable.h"
#include "CCButton.h"

IMPLEMENT_LOOK(CCTabCtrl, CCTabCtrlLook)//MTabCtrlLook
void CCTabCtrlLook::OnDraw(CCTabCtrl* pTabCtrl, CCDrawContext* pDC){
	sRect r = pTabCtrl->GetInitialClientRect();
	const int nButtonHeight = 24;
	r.y += nButtonHeight;
	r.h -= nButtonHeight;
	pDC->SetColor(128,128,128,255);
	pDC->Rectangle(r);
}

sRect CCTabCtrlLook::GetClientRect(CCTabCtrl* pTabCtrl, sRect& r){
	sRect t = r;
	t.Enlarge(-1);
	return t;
}

CCTabItem::CCTabItem(CCWidget *zpFrame,CCButton *zpButton,CCListener *zpListener)//MTabItem{
	pFrame = zpFrame;
	pButton = zpButton;
	pButtonListener= zpListener;
}

CCTabCtrl::CCTabCtrl(const char* szName, CCWidget* pParent, CCListener* pListener) : CCWidget(szName, pParent, pListener)//MTabCtrl{
	LOOK_IN_CONSTRUCTOR()
}

CCTabCtrl::CCTabCtrl(CCWidget* pParent, CCListener* pListener) : CCWidget("TabCtrl", pParent, pListener){
	LOOK_IN_CONSTRUCTOR()
}

CCTabCtrl::~CCTabCtrl(void){
	RemoveAll();
}

void CCTabCtrl::Add(CCButton *pButton, CCWidget *pFrame){
	CCTabItem *pItem = new CCTabItem(pFrame,pButton,pButton->GetListener());
	pButton->SetListener(this);
	m_TabList.push_back(pItem);
}

void CCTabCtrl::RemoveAll(void){
	while(m_TabList.size() > 0) {
		CCTabItem* pItem = *m_TabList.begin();
		delete pItem;
		m_TabList.pop_front();
	}
}

int CCTabCtrl::GetCount(void){
	return m_TabList.size();
}

int CCTabCtrl::GetSelIndex(void){
	return m_iCurrentSel;
}

bool CCTabCtrl::SetSelIndex(int nIndex){
	CCTabItem *pSelItem = NULL;
	int nCurIndex = 0;
	for(CCTabList::iterator i=m_TabList.begin();i!=m_TabList.end();i++)
	{
		CCTabItem *pItem = *i;
		if(nIndex==nCurIndex) {
			pSelItem = pItem;
			m_iCurrentSel = nCurIndex;
		}
		else {
			pItem->pButton->SetCheck(false);
			if(pItem->pFrame)
				pItem->pFrame->Show(false);
		}
		nCurIndex++;
	}

	if(pSelItem){
		pSelItem->pButton->SetCheck(true);
		if(pSelItem->pFrame)
			pSelItem->pFrame->Show(true);
	}
	return true;
}

bool CCTabCtrl::OnCommand(CCWidget* pWidget, const char* szMessage){
	int nIndex = 0;
	for(CCTabList::iterator i=m_TabList.begin();i!=m_TabList.end();i++){
		MTabItem *pItem = *i;
		if(pItem->pButton == pWidget) {
			if(strcmp(szMessage, CCTN_CLK_MSG)==0) {
				SetSelIndex(nIndex);
			}
			return pItem->pButtonListener->OnCommand(pWidget,szMessage);
		}
		nIndex++;
	}
	return false;
}

void CCTabCtrl::UpdateListeners()
{
	int nIndex = 0;
	for(CCTabList::iterator i=m_TabList.begin();i!=m_TabList.end();i++){
		CCTabItem *pItem = *i;
		if(pItem->pButton->GetListener()!=this){
			pItem->pButtonListener = pItem->pButton->GetListener();
			pItem->pButton->SetListener(this);
		}
	}
}