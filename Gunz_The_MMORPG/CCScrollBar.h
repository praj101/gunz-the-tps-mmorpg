#pragma once
#include "CCWidget.h"
#include "CCButton.h"

#include "CCLookNFeel.h"

#define CCSCROLLBAR_DEFAULT_WIDTH	22
#define CCSCROLLBAR_DEFAULT_HEIGHT	100
#define CCSCROLLBAR_BUTTON_WIDTH	22
#define CCSCROLLBAR_BUTTON_HEIGHT	22
#define CCSCROLLBAR_THUMB_WIDTH		22
#define CCSCROLLBAR_THUMB_HEIGHT	22
#define CCSCROLLBAR					"ScrollBar"
#define CCLIST_VALUE_CHANGED		"value changed"

enum CCScrollBarTypes{
	CCSBT_VERTICAL = 0,
	CCSBT_HORIZONTAL = 1
};


class CCThumb;
class CCScrollBar;
class CCArrow;

class CCThumbLook{
protected:
public:
	virtual void OnDraw(CCThumb* pThumb, CCDrawContext* pDC);
	virtual sRect GetClientRect(CCThumb* pThumb, sRect& r);
};

class CCArrowLook{
protected:
	virtual void OnDrawUpArrow(CCDrawContext* pDC, sRect& r, bool bPressed);
	virtual void OnDrawDownArrow(CCDrawContext* pDC, sRect& r, bool bPressed);
	virtual void OnDrawLeftArrow(CCDrawContext* pDC, sRect& r, bool bPressed);
	virtual void OnDrawRightArrow(CCDrawContext* pDC, sRect& r, bool bPressed);
public:
	virtual void OnDraw(CCArrow* pArrow, CCDrawContext* pDC);
	virtual sRect GetClientRect(CCArrow* pArrow, sRect& r);
	virtual sSize GetDefaultSize(CCArrow* pThumb);
};

class CCScrollBarLook{
protected:
public:
	virtual void OnDraw(CCScrollBar* pScrollBar, CCDrawContext* pDC);
	virtual sRect GetClientRect(CCScrollBar* pScrollBar, sRect& r);
};

class CCThumb : public CCWidget{
	DECLARE_LOOK(CCThumbLook)
	DECLARE_LOOK_CLIENT()

public:
	int		m_iDirection;	// 0: Horizontal, 1: Vertical
	CCThumb(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
};

class CCArrow : public CCButton{
	DECLARE_LOOK(CCArrowLook)

public:
	int		m_iDirection;	// 0: Up, 1: Down, 2: Left, 3: Right
	CCArrow(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	sSize GetDefaultSize();
};



class CCScrollBar : public CCWidget{
protected:

	int			m_iMinValue;
	int			m_iMaxValue;
	int			m_iValue;

	int			m_iPrevThumbRefresh;
	int			m_iThumbRefreshDelay;

	bool		m_bThumbMove;
	int			m_iThumbPos;
	int			m_iThumbDownPos;

	CCScrollBarTypes	m_iScrollBarType;

	DECLARE_LOOK(CCSCROLLBARLook)
	DECLARE_LOOK_CLIENT()

protected:
	virtual void OnRun();
	virtual bool OnEvent(MEvent* pEvent, CCListener* pListener);
	virtual void OnSize(int w, int h);

	int GetThumbMoveRange();
	int GetMoveRange();
	virtual int GetThumbSize();
	void RecalcThumbPos();
	void RecalcThumbBounds();

	void Initialize(CCScrollBarTypes t);

public:
	CCArrow*		m_pUp;
	CCArrow*		m_pDown;
	CCThumb*		m_pThumb;

	CCSCROLLBAR(const char* szName, CCWidget* pParent=NULL, CCListener* pListener=NULL, CCScrollBarTypes t=CCSBT_VERTICAL);
	CCSCROLLBAR(CCWidget* pParent=NULL, CCListener* pListener=NULL, CCScrollBarTypes t=CCSBT_VERTICAL);
	virtual ~CCScrollBar();

	void ChangeCustoCCArrowLook(CCArrowLook *pArrowLook);
	void ChangeCustoCCThumbLook(CCThumbLook *pThumbLook);

	void SetMinMax(int nMin, int nMax);
	int GetMin(){ return m_iMinValue; }
	int GetMax(){ return m_iMaxValue; }

	void SetValue(int nValue);			
	void SetValueAdjusted(int nValue);	
	int GetValue();

	CCScrollBarTypes GetType();
	void SetType(CCScrollBarTypes t);

	int GetDefaultBreadth();

	virtual const char* GetClassName(){ return MINT_SCROLLBAR; }
};

