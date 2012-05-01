#pragma once

/*
	CCWidget.h
*/

#include "CCPtrList.h"
#include "CCDrawContext.h"
#include "CCTypes.h"
#include "CCEvent.h"


#define CCWIDGET_NAME_LENGTH	256
#define CCWIDGET_NAME			"Widget"

class CCWidget;
class CCToolTip;
class CCResourceMap;
//Widget's Z-order
enum CCOrder{
	CC_TOP = 0,		//To the front
	CC_Bottom		//Back
}; 

struct sAnchors{
	bool m_bLeft;
	bool m_bRight;
	bool m_bTop;
	bool m_bBottom;
	sAnchors(){
		m_bLeft		= true;
		m_bRight	= false;
		m_bTop		= true;
		m_bBottom	= false;
	}
	sAnchors(bool l, bool r, bool t, bool b){
		m_bLeft		= l;
		m_bRight	= r;
		m_bTop		= t;
		m_bBottom	= b;
	}
};

class CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage) = 0;
};
//Base Widget Class
class CCWidget : public CCListener{
private:
	bool					m_bEnable;			//Enable
	bool					m_bFocusEnable;		//Focus Enable

	CCListener*				m_pListener;		//Listener
	CCToolTip*				m_pToolTip;			//Default Tooltip

	int						m_iAndPos;			
	int						m_iAccelerator;		//Accelerator Key
protected:
	bool					m_bVisible;			//Visible Flag
	
	CCPtrList<CCWidget>		m_Children;			//Children Widgets
	
	CCWidget*				m_pParent;			//Parent Widget
	CCPtrList<CCWidget>		m_Exclusive;		//< Exclusive Child Widget Stack ( for Modal Support )

	CCCursor*				m_pCursor;			//Default Cursor for this Widget
	CCFont*					m_pFont;			//Default Font for this Widget

	static CCWidget*		m_pCapturedWidget;	// SetWidget()/ReleaseWidget()
	static CCWidget*		m_pFocusedWidget;	// SetFocus()

	bool					m_bZOrderChangable;	//Order change by L-Button Click
	bool					m_bResizable;		//Resizeable?
	int						m_iResizeSide;		//0: N / A 1: Up 2: Right 4: Down 8: Left (BitFlag)

	uchar					m_iOpacity;			//Opacity

	bool					m_bClipByParent;	//Parent Widget is that by the Clip?

	CCAlignmentMode			m_BoundsAlignment;	//m_Rect for Alignment

public:
	char					m_szName[CCWIDGET_NAME_LENGTH];		//Name
	char					m_szIDLName[CCWIDGET_NAME_LENGTH];	//Name

	sRect					m_Rect;				// Parent widget rect
	sRect					m_IDLRect;			// IDL read that in the initial rect

	sAnchors				m_Anchors;					//Parent Widget is proportional to the position?
	int						m_iMinWidth, m_iMinHeight;	//The minimum size of the widget

	bool					m_bIsListBox;				
	int						m_iDebugType;
	bool					m_bEventAcceleratorCall;	//Broken English: "keystroke events whether it was a fire detection"

protected:
	bool					m_bEnableDesignerMode;		//Designer Mode Enable Flag
	int						m_iDragWidget;			//Move or resize the widgets flag
	sPoint					m_CCDragPoint;				//Move or resize the widgets position
	bool					m_bModifiedByDesigner;		//Designer by the location, size is modified
	bool					m_bAddedByDesigner;			//Designer is added by
	int						m_iID;						//ID of the widget

private:
	void MakeLocalEvent(CCEvent* pLocalEvent, const CCEvent* pEvent);	//Local Coordinate, Local Event, Converted?
	bool EventResize(CCEvent* pEvent);									//Event handling in the resize section

protected:
	void InsertChild(CCWidget* pWidget);		//Child widget added to the beginning
	void AddChild(CCWidget* pWidget);			//Child widget additional
	void RemoveChild(CCWidget* pWidget);		//Child widget delete

	void AddExclusive(CCWidget* pWidget);		//Exclusive stack adds
	bool RemoveExclusive(CCWidget* pWidget);	//Exclusive stack is removed from

	virtual void OnRun();												//Runtime loop
	virtual void OnDraw(CCDrawContext* pDC);							//Draw handler
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);		//Event handler, Event has been handled return true, or return false
	virtual bool OnHotKey(int nID){ return false; }						//Event handler that is called when a hotkey is pressed
	virtual bool OnShow();												//Show() by calling the event handler
	virtual void OnHide();
	virtual void OnSetFocus(){}											//Focus handler
	virtual void OnReleaseFocus(){}
	virtual void OnSize(int w, int h);									//SetSize(), SetBounds(), by calling the event handler
	virtual bool OnTab(bool bForward=true);								//Tab Key
	virtual bool OnDrop(CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString);	//Drop & Drop will be laid by the handler
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){ return false; }	//Listener Event Handler (If you follow the specified message is to return true. )

	void OnShow(bool bVisible);											//Show(), Hide(), by calling the event handler
	void ResizeChildrenByAnchor(int w, int h);							//Anchor Children Resize according to the setting
	void GetBoundsAlignmentPosition(sPoint* p, CCAlignmentMode am, int w=-1, int h=-1);	//Bounds Alignment according to the position value

public:
	CCWidget(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);	//Param szName: The widget name - pParent: parent widget - szName recieves your message listeners, that is usually the parent widget
	virtual ~CCWidget();

	void Run();												//Loop Widget System, each iteration of the run while the handle is twelve.
	void Draw(CCDrawContext* pDC);							//This draws a Child Widget Widget, including:
	virtual void DrawAfterWidgets(CCDrawContext* pDC);		//UI draw directly on top of it all after the pDC using a special opportunity to perform rendering operations
	void Redraw();											//Now draw back
	bool Event(CCEvent* pEvent);							//Event handler called to handle
	bool EventAccelerator(CCEvent* pEvent);					//Keyboard events handling Accelerator
	bool EventDefaultKey(CCEvent* pEvent);					//The default key (Enter, ESC) handler is invoked
	
	virtual void Show(bool bVisible=true, bool bModal=false);	//Widget shows. Param bVisible: Visible Flag - bModal: Dialog Flag
	void Hide(){ Show(false); }									//Hide the widget
	void Enable(bool bEnable=true);								//Widget activated, Param bEnable: Enable Flag
	
	bool IsVisible();						//Widget is visible?
	bool IsEnable();						//Widget has been activated?

	void SetResizable(bool bEnable);		
	bool IsResizable();

	virtual void SetListener(CCListener* pListener);	//Specify the listener, Param pListener: specify the listener
	virtual CCListener* GetListener();					//Get the current listener, return the current listener


	int GetID();						//Get widget ID
	void SetID(int nID);				//Specify an ID to UZES :: Use??

	virtual void SetText(const char* szText);		//Widget text (Name) to specify
	virtual const char* GetText();					//Widget text (Name) to obtain

	void SetCapture();						//Now go to the widget Capture all mouse messages
	void ReleaseCapture();					//Release the captured widget

	void SetFocusEnable(bool bEnable);		
	bool IsFocusEnable();

	void SetFocus();						//Widget is placed in the focus. Focused on the keyboard, the message first enters the widget.
	void ReleaseFocus();					//Frees the focus to the widget.
	bool IsFocus();							//Widget has focus on the?

	CCWidget* GetParent();						//Gets the parent widget
	int GetChildCount();						//Gets the number of child widgets
	CCWidget* GetChild(int i);					//Gets the child widget, Param i: index of child widget
	int GetChildIndex(CCWidget* pWidget);		//Gets the index of the child widget, Param pWidget: child widget, return the index of the child widget

	void SetExclusive();						//Exculsive mode (Modal) is converted into. Exclusive mode, all messages, the widget is set to be exclusive.
	void ReleaseExclusive();					//Exclusive mode is turned off
	CCWidget* GetLatestExclusive();				//Exclusive Stack widget gets the very last.
	bool IsExclusive(CCWidget* pWidget);		//Child Widget test whether the Exclusive

	CCCursor* SetCursor(CCCursor* pCursor);		//Set the cursor to the widget, Param pCursor: set the cursor to the widget, return a cursor the was previously set
	CCCursor* GetCursor();						//Gets the current cursor

	CCFont* SetFont(CCFont* pFont);				//Set the font for the widget.
	CCFont* GetFont();							//Gets the current font.

	void SetSize(int w, int h);					//Size to specify, Horizontal and vertical
	void SetSize(sSize& s);						//Size to specify, Param s: the size

	void SetPosition(int x, int y);				//Parent Widget Poition to specify the coordinate system of the Local
	void SetPosition(sPoint& p);				//Parent Widget Poition to specify the coordinate system of the Local, Param p: position

	void SetInitialBounds(sRect& r);			//Parent Widget Initial Rect to specify the coordinate system of the Local

	void SetBounds(sRect& r);						//Parent Widget Rect to specify the coordinate system of the Local, Param r: rectangle
	void SetBounds(int x, int y, int w, int h);		//Parent Widget Rect to specify the coordinate system of the Local, Param x, y, w, h: Rectangle

	sPoint GetPosition();											//Parent Widget Position in the coordinate system of the Local
	sRect GetRect();												//Parent Widget Rect in the coordinate system of the Local
	sRect GetIDLRect();												//Idl gets read from the rect

	void SetBoundsAlignment(CCAlignmentMode am, int w, int h);		//Specify the location for the alignment	
	CCAlignmentMode GetBoundsAlignment();							//Get the location for the alignment

	void SetOpacity(uchar nOpacity);								//Screen Rect in the coordinate system
	uchar GetOpacity();

	//Deprecated because it can confuse the global coordinate system and replace them with the local coordinate system function, 
	//if you want to know the global coordinates CCClientToScreen (), MScreenToClient () using the
	sRect GetScreenRect();

	void AttachToolTip(const char* szToolTipString=NULL);		//Specifies the tooltip string
	void AttachToolTip(CCToolTip* pToolTip);					//Specifies the tooltip object
	void DetachToolTip();										//Tooltips eliminating
	CCToolTip* GetToolTip();									//Now get the tooltip object

	void SetAccelerator(int a);								//Accelerator specify
	void SetLabelAccelerator();								//Label to the next character in the Accelerator and specify
	char GetLabelAccelerator();								//Get the current Accelerator
	char GetToolTipAccelerator();							//Get the current of the ToolTip Accelerator

	virtual sRect GetClientRect();					//Widget's customized Client Rect
	sRect GetInitialClientRect();					//Widget of the original Client Rect (0, 0, width, height)

	void SetZOrder(CCOrder z);										//Child Widget changing the order to adjust the drawing order.
	CCWidget* FindExclusiveDescendant();							//The descendants of the very first discovery Exclusive Widget Widget return

	CCWidget* Find(int x, int y){ return Find(sPoint(x, y)); }					//Location search widget
	CCWidget* Find(sPoint& p);													//Location search widget
	CCWidget* FindDropAble(sPoint& p);											

	virtual bool IsDropable(CCWidget* pSender){ return false; }												//The ID of the Drop Drag Object is it possible?
	bool Drop(CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString);		

	void SetVisible(bool b) { m_bVisible = b; }
	bool GetVisible() { return m_bVisible; }

	void GetHierarchicalName(char* szName);										//Hierarchy, including the name eoteonaegi (eg: Parent / Child)
	CCWidget* FindWidgetByHierarchicalName(const char* szName);					//Find the widget hierarchy, including the name

	virtual bool DefaultCommand(){ return false; }								//The widget can be done by Accelerator, etc. Specify the Default Command (should have overridden to return true if.

	static bool IsMsg(const char* szMsg1, const char* szMsg2);					//That matches the message?

	virtual void* Query(const char* szQuery);									//Query Anything
		
	void SetClipByParent(bool b) {
		m_bClipByParent = b;
	}

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);		//General resize functions for non-simply apply the scale cpp See more comments



	virtual const char* GetClassName(){ return CCWIDGET_NAME; }		//Get the class name
};


int		GetAndPos(const char* szText);			//And it tells you where it is located. m_szName szText is NULL, the location of the finds from the &.
char	GetAndChar(const char* szText);			//And the character gets stuck behind.

int		RemoveAnd(char* szText);											//& Characters removed
int		RemoveAnd(char* szRemovedText, const char* szText);					//Param szRemovedText [out] string is removed, Param szText [in] the source string
int		RemoveAnd(char* szRemovedFrontText, char* cUnderlineChar, char szRemovedBackText, const char* szText);		//Param szRemovedFrontText [out] & front string, Param cUnderLineChar [out] & stick the string due to the underlined, Param szRemovedBackText [out] & the back of the string, Param szText [in] the source string

sPoint	CCClientToScreen(CCWidget* pWidget, sPoint &p);		//Convert to a local location to location throughout the
sPoint	CCScreenToClient(CCWidget* pWidget, sPoint &p);		//Convert a global location where the local
sRect	CCClientToScreen(CCWidget* pWidget, sRect &p);		//Convert to local areas throughout the region
sRect	CCScreenToScreen(CCWidget* pWidget, sRect &p);		//Throughout the area converted to the local area

sPoint	GetCursorPosition();		//Get the current cursor position

inline bool	IsSameWidgetClass(CCWidget* pWidget, const char* szClassName);	//Checks for the same widget class
inline bool	IsSameWidgetClass(CCWidget* pWidgetA, CCWidget* pWidgetB);		//Checks for the same widget class