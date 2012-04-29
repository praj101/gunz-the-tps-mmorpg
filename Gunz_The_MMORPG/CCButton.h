#pragma once
 
#include "CCWidget.h"
#include "CCLookNFeel.h"

#define CCBTN_CLK_MSG		"click"		//button is clicked, the message that occurs when a Listener to
#define CCBTN_RCLK_MSG		"rclick"	//R button is clicked, the message that occurs when a Listener to
#define CCBTN_DOWN			"down"		//button ?
#define CCBTN_UP			"up"		//button eases no space
#define CCBTN_RDN_MSG		"rdown"		//R button
#define	CCBTN_RUP_MSG		"rup"		//R button eases no space
#define CCBTN_IN_MSG		"in"		//button, place the cursor over the eases
#define CCBTN_OUT_MSG		"out"		//button, place your cursor over the flush eases

#define CCBUTTON			"Button"


class CCMsgBox;
class CCButtonGroup;
class CCButton;
//Button type
enum CCButtonType{
	CCBT_NORMAL =0,
	CCBT_PUSH,
	CCBT_PUSH2
};
//The default key properties
enum CCButtoiKeyAssigned{
	CCBKA_NONE=0,
	CCBKA_ENTER,
	CCBKA_ESC
};
//Button Draw code of the classes, by subclassing this class can have a custom look.
class CCButtonLook{
protected:
	bool	m_bWireLook;
protected:
	virtual void OnDownDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual void OnUpDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual void OnOverDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual void OnDisableDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual void OnCheckBoxDraw(CCButton* pButton, CCDrawContext* pDC, bool bPushed){};
public:
	virtual void OnDrawText(CCButton* pButton, sRect& r, CCDrawContext* pDC);
	virtual void OnDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual sRect GetClientRect(CCButton* pButton, sRect& r);
	void SetWireLook(bool b) {	m_bWireLook = b;	}
	bool GetWireLook() const{	return m_bWireLook; }
public:
	CCButtonLook():m_bWireLook(false){}
};
//Button class
class CCButton : public CCWidget{
protected:
	bool				m_bMouseOver;
	bool				m_bLButtonDown;
	bool				m_bRButtonDown;
	sColor				m_TextColor;
	bool				m_bShowText;
	CCAlignmentMode		m_AlignmentMode;
	CCButtonType		m_Type;
	bool				m_bChecked;			//MBT_PUSH case of	
	bool				m_bComboDropped;
	bool				m_bStretch;
	
	CCButtonGroup		*m_pButtonGroup;		//belonging to a group
	int					m_iIndexInGroup;		//index within the same group

public:
	bool				m_bEnableEnter;			//button, press the Enter key when focus is activated (Default: true)
	bool				m_bHighlight;			//Highlight (&) support
	CCBitmap*			m_pIcon;				//Icon	
	CCButtoiKeyAssigned	m_iKeyAssigned;			//Key Assigned	
	CCMsgBox*			m_pMsgBox;				//the confirmation message with a message box
	string				m_strIDLConfirmText;	//xml defined in the confirmation message

	sPoint				m_ClickPos;
	sPoint				m_LDragStartClickPos;
	int					m_LDragVariationX;
	int					m_LDragVariationY;

protected:
	virtual void OnMouseIn();			//Mouse button event handler that occurs when you drove in
	virtual void OnMouseOut();			//Mouse button event handler that occurs when exiting out of
	virtual void OnButtonDown();		//Button event handler that occurs
	virtual void OnButtonUp();			//Button event handler that occurs when a blank space eases
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);	

	virtual void OnButtonClick();	//Is clicked

	virtual bool OnShow();
	virtual void OnHide();
public:
	CCButton(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCButton();
	void SetTextColor(sColor color);			//Buttons to specify the text color
	sColor GetTextColor();						//Get the button text color
	void ShowText(bool bShow=true);				//Button to show the text in
	virtual bool DefaultCommand();

	CCAlignmentMode GetAlignment();						//Get the text alignment
	CCAlignmentMode SetAlignment(CCAlignmentMode am);	//Specify the text alignment

	void SetType(CCButtonType t);			//Button to specify the type of
	CCButtonType GetType();					//Get button type

	void SetCheck(bool bCheck);			//Push buttons, check if
	bool GetCheck();					//Push buttons, if you get

	bool IsButtonDown();	//Buttons are pressed?, Async Button Down?
	bool IsMouseOver();		//Mice are raised on the button?

	/// Confirm Message Box
	void SetConfirmMessageBox(const char* szMessage);				//default confirmation message
	void SetAlterableConfirmMessage(const char* szMessage);			//special case can be swapped while the confirmation message
	void RestoreIDLConfirmMessage();								//Check to make the switch until the message is responsible side should try to restore ~

	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage);

	int GetLDragVariationX() {
		return m_LDragVariationX;
	}

	int GetLDragVariationY() {
		return m_LDragVariationY;
	}

	DECLARE_LOOK(CCButtonLook)
	DECLARE_LOOK_CLIENT()

	void SetComboDropped(bool b) {
		m_bComboDropped = b;
	}
	bool GetComboDropped() const{
		return m_bComboDropped;
	}

	void SetStretch( bool b) {
		m_bStretch = b;
	}

	bool GetStretch( ) const {
		return m_bStretch;
	}

	void SetButtonGroup(CCButtonGroup *pGroup);

	int GetIndexInGroup() { 
		return m_iIndexInGroup; 
	}

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);


	virtual const char* GetClassName(){ return CCBUTTON; }
};

//Button group classes needed to implement
class CCButtonGroup{
	friend CCButton;
public:
	CCButtonGroup();
	~CCButtonGroup();
protected:
	int			m_iCount;
	CCButton*	m_pPrev;
};