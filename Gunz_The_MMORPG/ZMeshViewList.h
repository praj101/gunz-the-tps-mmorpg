#ifndef ZMESHVIEWLIST_H
#define ZMESHVIEWLIST_H

#include "CCGroup.h"
#include "RMesh.h"

//class CCButton;
class CCBmButton;
class ZMeshView;

// 프레임을 그리기 위해 CCGroup을 상속받는다.
class ZMeshViewList : public CCGroup{
protected:
	int	m_nItemStartIndex;	// 아이템 시작 인덱스
	int	m_nItemWidth;		// 아이템 가로 크기
	//CCButton*	m_pLeft;	// 왼쪽 이동
	//CCButton*	m_pRight;	// 오른쪽 이동
	CCBmButton* m_pBmLeft;
	CCBmButton* m_pBmRight;
	//list<ZMeshView*>	m_Items;
	float m_ScrollButtonWidth;

protected:
	virtual void OnDraw(CCDrawContext* pDC);
	virtual void OnSize(int w, int h);
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage);
	int GetItemVisibleWidth();
	int GetItemWidth();
	int GetVisibleCount();
	void RecalcBounds();
public:
	ZMeshViewList(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZMeshViewList();

	int GetItemCount();
	ZMeshView* GetItem(int i);

	void Add(RealSpace2::RMesh* pMeshRef);
	void RemoveAll();
	void Remove(int i);

	void SetItemWidth(int nWidth);
};

#endif