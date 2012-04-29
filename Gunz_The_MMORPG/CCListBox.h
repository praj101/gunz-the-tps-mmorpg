#pragma once
#pragma warning(disable: 4996 )

#include "CCWidget.h"
#include "CCList.h"
#include "CCScrollBar.h"
#include "CCColorTable.h"

#define CCLISTBOX			"ListBox"
#define CCLB_ITEM_SEL		"selected"
#define CCLB_ITEM_SEL2		"selected2"
#define CCLB_ITEM_DBLCLK	"dclk"
#define CCLB_ITEM_SELLOST	"lost"
#define CCLB_ITEM_DEL		"del"
#define CCLB_ITEM_START		"start"			
#define CCLB_ITEM_CLICKOUT	"clickout"

typedef void (*ZCB_ONDROP)(void* pSelf, CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString);

enum CCListViewStyle
{
	CCVS_LIST = 0,
	CCVS_ICON
};
struct CCLISTFIELD{
	char			szFieldName[256];
	int				nTabSize;
};

class CCListBox;


class CCListItem{
public:
	bool m_bSelected;

	CCListItem(void) { m_bSelected = false; }
	virtual ~CCListItem(void){}

	virtual const char* GetString(void) = 0;
	virtual const char* GetString(int i){
		if(i==0) return GetString();
		return NULL;
	}
	virtual void SetString(const char *szText){
	}

	CCBitmap* GetBitmap(void){
		return GetBitmap(0);
	}
	virtual CCBitmap* GetBitmap(int i){
		return NULL;
	}

	virtual const sColor GetColor(void) { return GetColor(0); }
	virtual const sColor GetColor(int i) { 
		return sColor(DEFCOLOR_CCLIST_TEXT);
	}

	virtual bool GetDragItem(CCBitmap** ppDragBitmap, char* szDragString, char* szDragItemString){
		return false;
	}
};

class CCListFieldItem{
protected:
	sColor		m_Color;
	char*		m_szString;
	CCBitmap*	m_pBitmap;
public:
	CCListFieldItem(const char* szString, sColor color) {
		m_Color = color;
		m_szString = NULL;
		SetString(szString);
		m_pBitmap = NULL;
	}
	CCListFieldItem(const char* szString){
		m_Color = sColor(DEFCOLOR_CCLIST_TEXT);
		m_szString = NULL;
		SetString(szString);
		m_pBitmap = NULL;
	}
	CCListFieldItem(CCBitmap* pBitmap){
		m_Color = sColor(DEFCOLOR_CCLIST_TEXT);
		m_szString = NULL;
		m_pBitmap = pBitmap;
	}
	virtual ~CCListFieldItem(void){
		if(m_szString!=NULL){
			delete[] m_szString;
			m_szString = NULL;
		}
		m_szString = NULL;
	}

	virtual const char* GetString(void){
		return m_szString;
	}
	virtual void SetString(const char* szString){
		if(m_szString!=NULL) delete[] m_szString;
		m_szString = new char[strlen(szString)+2];
		strcpy(m_szString, szString);
	}

	void SetColor(sColor color) { m_Color = color; }
	virtual const sColor GetColor() { return m_Color; }

	CCBitmap* GetBitmap(void){ return m_pBitmap; }
	void SetBitmap(CCBitmap* pBitmap){ m_pBitmap = pBitmap; }
};


class CCDefaultListItem : public CCListItem{
	CMPtrList<CCListFieldItem>	m_Items;
public:
	CCDefaultListItem(void){
	}
	CCDefaultListItem(const char* szText, const sColor color) {
		CCListFieldItem* pNew = new CCListFieldItem(szText, color);
		m_Items.Add(pNew);
	}
	CCDefaultListItem(const char* szText){
		CCListFieldItem* pNew = new CCListFieldItem(szText);
		m_Items.Add(pNew);
	}
	CCDefaultListItem(CCBitmap* pBitmap, const char* szText){
		CCListFieldItem* pNew = new CCListFieldItem(pBitmap);
		m_Items.Add(pNew);
		pNew = new CCListFieldItem(szText);
		m_Items.Add(pNew);
	}
	virtual ~CCDefaultListItem(void){
		for(int i=0; i<m_Items.GetCount(); i++){
			CCListFieldItem* pItem = m_Items.Get(i);
			delete pItem;
		}
	}
	virtual const char* GetString(void){
		if(m_Items.GetCount()>0) return m_Items.Get(0)->GetString();
		return NULL;
	}
	virtual const char* GetString(int i){
		if(i<m_Items.GetCount()) return m_Items.Get(i)->GetString();
		return NULL;
	}
	virtual void SetString(const char *szText){
		if(m_Items.GetCount()){
			delete m_Items.Get(0);
			m_Items.Delete(0);
		}
		CCListFieldItem* pNew = new CCListFieldItem(szText);
		m_Items.MoveFirst();
		m_Items.InsertBefore(pNew);
	}

	virtual CCBitmap* GetBitmap(int i){
		if(i<m_Items.GetCount()) return m_Items.Get(i)->GetBitmap();
		return NULL;
	}

	virtual const sColor GetColor() { 
		if (m_Items.GetCount()>0) return m_Items.Get(0)->GetColor();
		return sColor(DEFCOLOR_CCLIST_TEXT);
	}
};

class CCListBoxLook{
public:
	sColor	m_SelectedPlaneColor;
	sColor	m_SelectedTextColor;
	sColor	m_UnfocusedSelectedPlaneColor;
	CCAlignmentMode	m_ItemTextAlignmentMode;
	bool			m_bItemTextMultiLine;
	CCBitmap*	m_pItemSlotBitmap;
protected:
	virtual void OnHeaderDraw(CCDrawContext* pDC, sRect& r, const char* szText);
	virtual int OnItemDraw(CCDrawContext* pDC, sRect& r, const char* szText, sColor color, bool bSelected, bool bFocus, int nAdjustWidth = 0);
	virtual int OnItemDraw(CCDrawContext* pDC, sRect& r, CCBitmap* pBitmap, bool bSelected, bool bFocus, int nAdjustWidth = 0);
	virtual void OnFrameDraw(CCListBox* pListBox, CCDrawContext* pDC);
public:
	CCListBoxLook(void);

	virtual void OnDraw(CCListBox* pListBox, CCDrawContext* pDC);
	virtual sRect GetClientRect(CCListBox* pListBox, sRect& r);
};

class CCListBox : public CCWidget{
protected:
	class SortedItemList : public CMLinkedList<CCListItem>{
	public:
		bool	m_bAscend;
	public:
		SortedItemList(void){
			m_bAscend = true;
		}
		virtual int Compare(CCListItem *lpRecord1,CCListItem *lpRecord2){
			int nCompare = stricmp(lpRecord1->GetString(0), lpRecord2->GetString(0));
			if(m_bAscend==true) return nCompare;
			else return -nCompare;
		}
	} m_Items;
	int				m_iOverItem;		
	int				m_iSelItem;			
	int				m_iShowItemCount;	
	int				m_iStartItemPos;	
	int				m_iItemHeight;		
	CCScrollBar*		m_pScrollBar;

	CCLinkedList<CCLISTFIELD>	m_Fields;

	bool			m_bVisibleHeader;
	bool			m_bSelected;
	CCListViewStyle	m_ViewStyle;
	bool			m_bAlwaysVisibleScrollbar;
	bool			m_bDragAndDrop;
	ZCB_ONDROP		m_pOnDropFunc;

public:
	bool			m_bAbsoulteTabSpacing;
	bool			m_bHideScrollBar;
	bool			m_bNullFrame;
	bool			m_bMultiSelect;

protected:
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);
	virtual bool OnDrop(CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString);
	virtual bool IsDropable(CCWidget* pSender)		{ return m_bDragAndDrop; }

	void RecalcList(void);			
	void RecalcScrollBar(void);		

	int FindNextItem(int i, char c);

	virtual void OnSize(int w, int h);

	virtual bool OnCommand(CCWidget* pWindow, const char* szMessage);

	void Initialize(void);

public:
	sColor m_FontColor;
	CCAlignmentMode m_FontAlign;

public:
	CCListBox(const char* szName, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	CCListBox(CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCListBox(void);

	void	Add(const char* szItem);
	void	Add(const char* szItem, sColor color);
	void	Add(CCListItem* pItem);
	const char* GetString(int i);
	CCListItem* Get(int i);
	bool	Set(int i, const char* szItem);
	bool	Set(int i, CCListItem* pItem);
	void	Remove(int i);
	void	RemoveAll(void);
	bool	Swap(int i, int j);
	int		GetCount(void);
	int		GetSelIndex(void);
	bool	SetSelIndex(int i);
	const char* GetSelItemString(void);
	CCListItem* GetSelItem(void);

	int		GetSelCount(void);

	int		FindItem(sPoint& p);
	bool	GetItemPos(sPoint* p, int i);

	int		GetItemHeight(void);
	void	SetItemHeight(int nHeight);

	bool	IsShowItem(int i);
	void	ShowItem(int i);
	void	SetStartItem(int i);
	int		GetStartItem(void);
	int		GetShowItemCount(void);

	CCScrollBar* GetScrollBar(void);

	void	Sort(bool bAscend=true);

	void	AddField(const char* szFieldName, int nTabSize);
	void	RemoveField(const char* szFieldName);
	CCLISTFIELD* GetField(int i);
	int		GetFieldCount(void);
	void	RemoveAllField(void);

	bool	IsVisibleHeader(void);
	void	SetVisibleHeader(bool bVisible);

	bool	IsAlwaysVisibleScrollbar(void);
	void	SetAlwaysVisibleScrollbar(bool bVisible);

	bool	IsSelected() { return m_bSelected; }
	void	SetSelected(bool bSelected) { m_bSelected = bSelected; }
	CCListViewStyle GetViewStyle() { return m_ViewStyle; }
	void	SetViewStyle(CCListViewStyle ViewStyle);
	int		GetTabSize();
	void	EnableDragAndDrop( bool bEnable);

	void	SetOnDropCallback(ZCB_ONDROP pCallback) { m_pOnDropFunc = pCallback; }

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);


	DECLARE_LOOK(CCListBoxLook)
	DECLARE_LOOK_CLIENT()


	virtual const char* GetClassName(void){ return CCLISTBOX; }
};

