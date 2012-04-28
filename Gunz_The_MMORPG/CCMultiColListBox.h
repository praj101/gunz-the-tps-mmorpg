#pragma once
#include "CCWidget.h"
#include "CCScrollBar.h"
#include "CCColorTable.h"
#include "CCListBox.h"
#include <list>

class CCMultiColListBox;

#define CCMULTICOLLISTBOX	"MultiColListBox"


typedef std::list<CCMultiColListItem*>		ListMultiColListItem;
typedef list<CCMultiColListItem*>::iterator	ItorMultiColListItem;

class CCMultiColListBoxLook
{
	CCAlignmentMode	m_ItemTextAlignmentMode;

public:
	CCMultiColListBoxLook();

	void	OnDraw(CCMultiColListBox* pListBox, CCDrawContext* pDC);
	sRect	GetClientRect(CCMultiColListBox* pListBox, sRect& r);
};

class CCMultiColListItem
{
public:
	virtual ~CCMultiColListItem() {}
	virtual void OnDraw(sRect& r, CCDrawContext* pDC, bool bSelected, bool bMouseOver);
	virtual const char* GetString() { return ""; }

	virtual bool GetDragItem(CCBitmap** ppDragBitmap, char* szDragString, char* szDragItemString){
		return false;
	}

	virtual int GetSortHint() { return 0; }
};

class CCMultiColListBox : public CCWidget
{
	ListMultiColListItem m_items;

	int				m_numColumn;		// 컬럼 갯수
	int				m_desiredNumRow;	// 한번에 보여주고 싶은 행수
	int				m_itemHeight;

	int				m_maxRowCanShow; // 한 화면에 보여질 수 있는 최대 행 수

	CCScrollBar*	m_pScrollBar;

	int				m_nOverItem;			// 커서에 의해 가리켜진 아이템
	int				m_nSelItem;
	bool			m_bDragAndDrop;

	ZCB_ONDROP		m_pOnDropFunc;

	void SetItemHeight(int h);
	void InsertSortedPos(CCMultiColListItem* pItem);

public:
	CCAlignmentMode m_FontAlign;

public:
	CCMultiColListBox(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCMultiColListBox();


	ListMultiColListItem& GetItemList() { return m_items; }

	void	SetDesiredNumRow(int n);
	void	CalcItemHeight();

	int		GetItemHeight() { return m_itemHeight; }
	int		GetItemWidth() { return GetClientRect().w / m_numColumn; }

	void	SetNumColumn(int n);
	int		GetNumColumn() { return m_numColumn; }

	int		GetNumItem() { return (int)m_items.size(); }

	CCScrollBar* GetScrollBar() { return m_pScrollBar; }
	int	GetMaxRowCanShow() { return m_maxRowCanShow; }

	int	GetRowFirstVisible() {
		return GetScrollBar()->GetValue();
	}
	int GetRowLastVisible() {
		return GetRowFirstVisible() + GetMaxRowCanShow() - 1;
	}
	int GetItemFirstVisible() {
		return GetRowFirstVisible() * m_numColumn;
	}
	int GetItemLastVisible() {
		int idxItemLastShow = GetItemFirstVisible() + (GetMaxRowCanShow() * m_numColumn) - 1;
		return min(idxItemLastShow, GetNumItem()-1);
	}

	void	SetRowFirstVisible(int n) { GetScrollBar()->SetValueAdjusted(n); }

	void	Add(CCMultiColListItem* pItem);
	void	Remove(CCMultiColListItem* pItem);
	void	RemoveAll();

	bool	GetItemRowCol(int idx, int& out_row, int& out_col);
	int		GetSelIndex() { return m_nSelItem; }
	int		GetMouseOverIndex() { return m_nOverItem; }
	CCMultiColListItem* GetItemByIdx(int idx);
	CCMultiColListItem* GetSelItem();
	bool	SetSelIndex(int i);

	bool	CalcItemRect(int idx, sRect& out);
	int		FindItem(sPoint& p);

	void	EnableDragAndDrop( bool bEnable) { m_bDragAndDrop = bEnable; }
	int		FindNextItem(int i, char c);	

	const char* GetSelItemString();

	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);
	virtual bool IsDropable(CCWidget* pSender) { return m_bDragAndDrop; }

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);

	CCMultiColListItem* Get(int i);
	const char* GetString(int i);
	bool	IsSelected() { return m_nSelItem != -1; }
	void	ShowItem(int i);

	void	SetOnDropCallback(ZCB_ONDROP pCallback) { m_pOnDropFunc = pCallback; }
	bool	OnDrop(CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString);

	bool	GetItemPos(sPoint* p, int i) { return false; }

protected:
	virtual void OnSize(int w, int h);
	void	UpdateScrollBar();

public:
	DECLARE_LOOK(CCMultiColListBoxLook)
	DECLARE_LOOK_CLIENT()

	virtual const char* GetClassName(void){ return CCMULTICOLLISTBOX; }
};
