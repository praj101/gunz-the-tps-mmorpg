#pragma once

#include "CCZFileSystem.h"
#include "CCXml.h"
#include "CCTypes.h"
#include "CCWidget.h"
#include "CCFrame.h"

#include <string>
#include <list>
#include <map>
#include <algorithm>
using namespace std;

class CCFrame;
class CCLabel;
class CCButton;
class CCEdit;
class CCListBox;
class CCPicture;
class CCScrollBar;
class CCSlider;
class CCGroup;
class CCComboBox;
class CCToolTip;
class CCPopupMenu;
class CCAniBitmap;
class CCAnimation;
class CCBmButton;
class CCMenuItem;
class CCBmLabel;
class CCTextArea;
class CCTabCtrl;
class CCPanel;
class CCButtonGroup;

// Look&Feel
class CCBLabelLook;
class CCBButtonLook;
class CCBGroupLook;
class CCBFrameLook;
class CCBEditLook;
class CCBListBoxLook;
class CCBMultiColListBoxLook;
class CCBScrollBarLook;
class CCBArrowLook;
class CCBThumbLook;
class CCBSliderLook;
class CCBGroupLook;
class CCBitmap;
class CCHotKey;
//class MActionKey;
class CCBTextAreaLook;
class CCBSliderThumbLook;
class CCBTabCtrlLook;


#define IDL_ROOT	"XML"

#define IDL_BOUNDS		"BOUNDS"
#define IDL_TEXT		"TEXT"
#define IDL_TEXTCOLOR	"TEXTCOLOR"

#define IDL_LABEL		"LABEL"
#define IDL_FRAME		"FRAME"
#define IDL_BUTTON		"BUTTON"

#define IDL_ATTR_ITEM	"item"
#define IDL_ATTR_PARENT	"parent"

class CCWidgetMMap : public multimap<string, CCWidget*>
{
public:
	virtual ~CCWidgetMMap(){ Clear(); }

	void Clear() {
		while(empty()==false)
		{
			delete (*begin()).second;
			erase(begin());
		}
	}
};

class CCWidgetList : public list<CCWidget*> { };

/// Maiet Interface Definition Language Resource
class CCIDLResource
{
private:

protected:
	CCWidget*						m_pParent;		///< 추가될 최상위 위젯

	map<string, CCBLabelLook*>		m_LabelLookMap;
	map<string, CCBButtonLook*>		m_ButtonLookMap;
	map<string, CCBGroupLook*>		m_GroupLookMap;
	map<string, CCBFrameLook*>		m_FrameLookMap;
	map<string, CCBEditLook*>		m_EditLookMap;
	map<string, CCBListBoxLook*>		m_ListBoxLookMap;
	map<string, CCBMultiColListBoxLook*>	m_MultiColListBoxLookMap;
	map<string, CCBScrollBarLook*>	m_ScrollBarLookMap;
	map<string, CCBArrowLook*>		m_ArrowLookMap;
	map<string, CCBThumbLook*>		m_ThumbLookMap;
	map<string, CCBSliderLook*>		m_SliderLookMap;
	map<string, CCAniBitmap*>		m_AniBitmapMap;
	map<string, CCBTextAreaLook*>	m_TextAreaLookMap;
	map<string, CCBTabCtrlLook*>		m_TabCtrlLookMap;

	CCWidgetMMap						m_WidgetMap;

	map<string, CCButtonGroup*>		m_ButtonGroupMap;

	// 기본요소
	sPoint			GetPoint(::CCXmlElement& element);
	sRect			GetRect(::CCXmlElement& element);
	sSize			GetSize(::CCXmlElement& element);
	sColor			GetColor(::CCXmlElement& element);
	CCBitmap*		GetBitmap(::CCXmlElement& element);
	CCBitmap*		GetBitmapAlias(::CCXmlElement& element);	// 부분bitmap을 특정 이름으로 alias
	sAnchors		GetAnchors(::CCXmlElement& element);
	CCAlignmentMode	GetAlignmentMode(::CCXmlElement& element);

	// Widget
	CCWidget*		GetParentWidget(::CCXmlElement& element);
	CCFrame*		GetFrame(::CCXmlElement& element);
	CCLabel*		GetLabel(::CCXmlElement& element);
	CCButton*		GetButton(::CCXmlElement& element);
	CCBmButton*		GetBmButton(::CCXmlElement& element);
	CCEdit*			GetEdit(::CCXmlElement& element);
	CCListBox*		GetListBox(::CCXmlElement& element);
	CCPicture*		GetPicture(::CCXmlElement& element);
	CCScrollBar*	GetScrollBar(::CCXmlElement& element);
	CCSlider*		GetSlider(::CCXmlElement& element);
	CCGroup*		GetGroup(::CCXmlElement& element);
	CCComboBox*		GetComboBox(::CCXmlElement& element);
	CCPopupMenu*	GetPopupMenu(::CCXmlElement& element);
	CCAniBitmap*	GetAniBitmap(::CCXmlElement& element);
	CCAnimation*	GetAnimation(::CCXmlElement& element);
	CCCursor*		GetCursor(::CCXmlElement& element);
	CCBmLabel*		GetBmLabel(::CCXmlElement& element);
	CCFont*			GetFont(::CCXmlElement& element);
	CCHotKey*		GetHotKey(::CCXmlElement& element);
//	MActionKey*		GetActionKey(::CCXmlElement& element);
	CCTextArea*		GetTextArea(::CCXmlElement& element);
	CCTabCtrl*		GetTabCtrl(::CCXmlElement& element);
	CCPanel*		GetPanel(::CCXmlElement& element);

	// Look&Feel
	CCBLabelLook*	GetLabelLook(::CCXmlElement& element);
	CCBButtonLook*	GetButtonLook(::CCXmlElement& element);
	CCBGroupLook*	GetGroupLook(::CCXmlElement& element);
	CCBFrameLook*	GetFrameLook(::CCXmlElement& element);
	CCBEditLook*		GetEditLook(::CCXmlElement& element);
	CCBListBoxLook*	GetListBoxLook(::CCXmlElement& element, int nType);	///< nType: 0=ListBox, 1=ComboListBox
	CCBScrollBarLook* GetScrollBarLook(::CCXmlElement& element);
	CCBArrowLook*	GetArrowLook(::CCXmlElement& element);
	CCBThumbLook*	GetThumbLook(::CCXmlElement& element);
	CCBSliderLook*	GetSliderLook(::CCXmlElement& element);
	CCBTextAreaLook*	GetTextAreaLook(::CCXmlElement& element);
	CCBSliderThumbLook* GetSliderThumbLook(::CCXmlElement& element);
	CCBTabCtrlLook*	GetTabCtrlLook(::CCXmlElement& element);

	// Helper
	CCPopupMenu* GetSubMenu(CCMenuItem* pParentMenuItem, ::CCXmlElement& element);
	CCMenuItem* GetMenuItem(CCPopupMenu* pPopupMenu, ::CCXmlElement& element);
	void GetRebounds(::CCXmlElement& element);

	void GetFrameBtn(CCFrameBtn* pFrameBtn, CCBFrameLook* pFrameLook, ::CCXmlElement& element);
	void GetBmButtonBitmaps(CCBitmap** ppBitmaps, ::CCXmlElement& element);
	void GetBitmaps(CCBitmap** ppBitmaps, ::CCXmlElement& element, const int nBitmapCount);
	void InsertWidget(::CCXmlElement& element, CCWidget* pWidget);
	void InsertWidget( const char* pItemName, CCWidget* pWidget );
	bool GetCommonWidgetProperty(CCWidget* pWidget, ::CCXmlElement& element, const char* szTagName);
	void ClearLooks();

	// 저장 관련 - 나중에 필요할때 만들자.
	void SetLabel(::CCXmlElement& element, CCLabel* pLabel);
	void SetPoint(::CCXmlElement& element, sPoint* pPoint, const char* szTagName);
	void SetRect(::CCXmlElement& element, sRect* pRect, const char* szTagName);
	void SetSize(::CCXmlElement& element, sSize* pSize, const char* szTagName);
	void SetColor(::CCXmlElement& element, sColor* pColor, const char* szTagName);

	virtual CCFrame*	CreateFrame(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual CCFont* CreateFont(char* szAliasName, char* szFontName, int nHeight, 
		bool bBold = false, bool bItalic = false, int nOutlineStyle = 1, bool bAntialiasing = false, DWORD nColorArg1=0, DWORD nColorArg2=0);

	virtual void Parse(::CCXmlElement& element);
	virtual void TransText(char* szSrc, char* szOut);		// 국제화를 위한 함수
public:
	CCIDLResource();
	virtual ~CCIDLResource();
	bool LoadFromFile(char* szFileName, CCWidget* pParent=NULL,CCZFileSystem *pfs=NULL);
	bool SaveToFile(const char* szFileName);
	virtual void Clear();
	CCWidgetMMap* GetWidgetMap() { return &m_WidgetMap; }
	CCWidget* FindWidget(string szItem);
	void FindWidgets(CCWidgetList& widgetList, string szItem);

	CCBFrameLook* FindFrameLook(string szItem);

	
};