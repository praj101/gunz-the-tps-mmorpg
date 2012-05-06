#pragma once

#include "CCWidget.h"
#include "CCLookNFeel.h"
#include "CCButton.h"

#define CCFRAME					"Frame"
#define CCFRAME_CLOSE_MSG		"close"
#define CCFRAME_MINIMIZE_MSG	"minimize"

class CCFrame;

struct CCFrameBtn
{
	bool		m_bVisible;
	bool		m_bMouseOver;
	bool		m_bLButtonDown;
	sRect		m_Rect;
	sAnchors	m_Anchors;
	CCFrameBtn() { 
		m_bVisible = false; 
		m_bLButtonDown = false; 
		m_bMouseOver = false; 
		m_Rect = sRect(0,0,0,0);
	}
	bool IsButtonDown(){ 
		if ((m_bLButtonDown==true) && (m_bMouseOver==true)) 
			return true; 
		return false; 
	}
};

class CCFrameLook{
public:
	virtual void OnDraw(CCFrame* pFrame, CCDrawContext* pDC);
	virtual sRect GetClientRect(CCFrame* pFrame, sRect& r);
};



//A widget that can be moved to the frame
class CCFrame : public CCWidget{
	DECLARE_LOOK(CCFrameLook)
	DECLARE_LOOK_CLIENT()

protected:
	bool	m_bDragWidget;		//Drag Widget
	sPoint	m_DragPoint;

	bool	m_bMovable;			//whether to move
	bool	m_bCanShade;		//Shade whether it can be - false, if you can not Shade.
	bool	m_bShade;
	sSize	m_BeforeShade;		//Before Shade

	sRect		m_OldRect;
	CCFrameBtn	m_BtnClose;
	CCFrameBtn	m_BtnMinimize;

public:
	bool		m_bTitleBar;	//Use Title Bar

protected:
	virtual bool OnCommand(CCWidget* pWindow, const char* szMessage);
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);
	virtual void OnSize(int w, int h);
	virtual bool OnShow();

	void ResizeBtnsByAnchors(int w, int h);
	void OnCloseButtonClick();
	void OnMinimizeButtonClick();

public:
	CCFrame(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCFrame();

	void SetShade(bool bShade);
	void SetMovable(bool bValue) { m_bMovable = bValue; }
	CCFrameBtn*	GetCloseButton() { return &m_BtnClose; }
	CCFrameBtn*	GetMinimizeButton() { return &m_BtnMinimize; }



	virtual const char* GetClassName(){ return CCFRAME; }
};