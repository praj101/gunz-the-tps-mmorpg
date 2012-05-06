#include "stdafx.h"

#include "ZMeshViewList.h"
#include "ZMeshView.h"
#include "CCBCCButton.h"

void ZMeshViewList::OnDraw(CCDrawContext* pDC)
{
	//pDC->SetColor(255, 255, 255);
	//pDC->Rectangle(GetClientRect());
	MGroup::OnDraw(pDC);
}

void ZMeshViewList::OnSize(int w, int h)
{
	RecalcBounds();
}

bool ZMeshViewList::OnCommand(CCWidget* pWidget, const char* szMessage)
{
	//if(pWidget==m_pLeft && strcmp(szMessage, CCBTN_CLK_MSG)==0){
	//	if(m_nItemStartIndex>0){
	//		m_nItemStartIndex--;
	//		RecalcBounds();
	//	}
	//	return true;
	//}
	//else if(pWidget==m_pRight && strcmp(szMessage, CCBTN_CLK_MSG)==0){
	//	if(m_nItemStartIndex+GetVisibleCount()<GetItemCount()){
	//		m_nItemStartIndex++;
	//		RecalcBounds();
	//	}
	//	return true;
	//}
	if(pWidget==m_pBmLeft && strcmp(szMessage, CCBTN_CLK_MSG)==0){
		if(m_nItemStartIndex>0){
			m_nItemStartIndex--;
			RecalcBounds();
		}
		return true;
	}
	else if(pWidget==m_pBmRight && strcmp(szMessage, CCBTN_CLK_MSG)==0){
		if(m_nItemStartIndex+GetVisibleCount()<GetItemCount()){
			m_nItemStartIndex++;
			RecalcBounds();
		}
		return true;
	}
	return false;
}

#define LEFTRIGHT_WIDTH	20
int ZMeshViewList::GetItemVisibleWidth()
{
	sRect r = GetClientRect();
	int w = r.w - LEFTRIGHT_WIDTH*2;
	return w;
}

int ZMeshViewList::GetItemWidth()
{
	int nVisibleCount = GetVisibleCount();
	if(nVisibleCount==0) nVisibleCount = 1;

	return GetItemVisibleWidth()/nVisibleCount;
}

int ZMeshViewList::GetVisibleCount()
{
	return GetItemVisibleWidth()/m_nItemWidth;
}

void ZMeshViewList::RecalcBounds()
{
	sRect r = GetClientRect();

	//m_pLeft->SetBounds(r.x, r.y, LEFTRIGHT_WIDTH, r.h);
	//m_pRight->SetBounds(r.x+r.w-LEFTRIGHT_WIDTH, r.y, LEFTRIGHT_WIDTH, r.h);
	m_pBmLeft->SetBounds(r.x, r.y, LEFTRIGHT_WIDTH, r.h);
	m_pBmRight->SetBounds(r.x+r.w-LEFTRIGHT_WIDTH, r.y, LEFTRIGHT_WIDTH, r.h);

	int nVisibleCount = GetVisibleCount();
	int nItemWidth = GetItemWidth();
	int nItemCount = GetItemCount();
	if(m_nItemStartIndex+nVisibleCount>nItemCount){
		m_nItemStartIndex = nItemCount - nVisibleCount;
		if(m_nItemStartIndex<0) m_nItemStartIndex = 0;
	}

	int nCount = GetItemCount();

	if(m_nItemStartIndex>0) m_pBmLeft->Enable(true);
	else m_pBmLeft->Enable(false);
	//if(m_nItemStartIndex+nVisibleCount<nCount) m_pRight->Enable(true);
	//else m_pRight->Enable(false);
	if(m_nItemStartIndex+nVisibleCount<nCount) m_pBmRight->Enable(true);
	else m_pBmRight->Enable(false);

	for(int i=0; i<nCount; i++){
		CCWidget* pChild = GetItem(i);
		pChild->SetBounds(r.x+LEFTRIGHT_WIDTH+(i-m_nItemStartIndex)*nItemWidth, r.y, nItemWidth, r.h);
		if(i-m_nItemStartIndex<0 || i-m_nItemStartIndex>=nVisibleCount) pChild->Show(false);
		else pChild->Show(true);
	}
}

ZMeshViewList::ZMeshViewList(const char* szName, CCWidget* pParent, CCListener* pListener)
: MGroup(szName, pParent, pListener)
{
	m_nItemStartIndex = 0;
	m_nItemWidth = 70;

	//m_pLeft = new CCButton("<", this, this);
	//m_pRight = new CCButton(">", this, this);
	m_pBmLeft	= new MBCCButton("", this, this);
	m_pBmRight	= new MBCCButton("", this, this);
	m_pBmLeft->SetDownBitmap(CCBitmapManager::Get("button_circle_left.tga"));
	m_pBmLeft->SetOverBitmap(CCBitmapManager::Get("button_circle_left.tga"));
	m_pBmLeft->SetDisableBitmap(CCBitmapManager::Get("button_circle_left.tga"));
	m_pBmLeft->SetUpBitmap(CCBitmapManager::Get("button_circle_left.tga"));
	m_pBmLeft->SetStretch(true);
	m_pBmRight->SetDownBitmap(CCBitmapManager::Get("button_circle_right.tga"));
	m_pBmRight->SetOverBitmap(CCBitmapManager::Get("button_circle_right.tga"));
	m_pBmRight->SetDisableBitmap(CCBitmapManager::Get("button_circle_right.tga"));
	m_pBmRight->SetUpBitmap(CCBitmapManager::Get("button_circle_right.tga"));
	m_pBmRight->SetStretch( true );

	m_ScrollButtonWidth = LEFTRIGHT_WIDTH;

	//for(int i=0; i<2; i++) AddItem(NULL);
}

ZMeshViewList::~ZMeshViewList()
{
	//SAFE_DELETE(m_pLeft);
	//SAFE_DELETE(m_pRight);
	SAFE_DELETE( m_pBmRight );
	SAFE_DELETE( m_pBmLeft );

	RemoveAll();
}

int ZMeshViewList::GetItemCount()
{
	return GetChildCount()-2;	// ÁÂ/¿ì ¹öÆ° 2°³ »©°í
}

ZMeshView* ZMeshViewList::GetItem(int i)
{
	if(i<0 || i>=GetItemCount()) return NULL;
	ZMeshView* pChild = (ZMeshView*)GetChild(i+2);
	return pChild;
}

void ZMeshViewList::Add(RMesh* pMeshRef)
{
	ZMeshView* pNew = new ZMeshView(NULL, this, this);
	//m_Items.insert(m_Items.end(), pNew);
	RecalcBounds();
}

void ZMeshViewList::RemoveAll()
{
	//m_Items.clear();
	for(int i=GetChildCount()-1; i>=0; i--){
		CCWidget* pChild = GetChild(i);
		//if(pChild==m_pLeft || pChild==m_pRight) continue;
		if(pChild==m_pBmLeft || pChild==m_pBmRight) continue;
		delete pChild;
	}
}

void ZMeshViewList::Remove(int i)
{
	int nCount = 0;
	for(int j=2; j<GetChildCount(); j++){
		CCWidget* pChild = GetChild(j);
		if(nCount==i){
			delete pChild;
			return;
		}
	}
}

void ZMeshViewList::SetItemWidth(int nWidth)
{
	m_nItemWidth = nWidth;
	RecalcBounds();
}