#pragma once
#pragma warning(disable:4786)

#include <list>
#include <string>
#include "CCWidget.h"
#include "CCLookNFeel.h"
#include "CCScrollBar.h"

using namespace std;

class CCTextArea;
class CCScrollBar;

#define CCTEXTAREA						"TextArea"
#define CCTEXTAREA_ESC_VALUE			"esc"
#define CCTEXTAREA_ENTER_VALUE			"entered"
#define CCTEXTAREA_DEFAULT_TEXT_COLOR	sColor(224,224,224);

struct sLineItem{
	sColor color;
	string text;

	sLineItem(sColor _color, string &_text){
		color=_color;
		text=_text;
	}
	sLineItem(string &_text){
		color = CCTEXTAREA_DEFAULT_TEXT_COLOR;
		text =_text;
	}
};

//
typedef list<sLineItem>				SLINELIST;
typedef list<sLineItem>::iterator	SLINELISTITERATOR;

class CCTextAreaLook{
protected:
	sColor m_bgColor;

public: 
	CCTextAreaLook() : m_bgColor(0, 0, 0, 0) {};

	virtual void	OnDraw(CCTextArea* pTextArea, CCDrawContext* pDC);
	virtual sRect	GetClientRect(CCTextArea* pTextArea, sRect& r);
	virtual void	SetBgColor(sColor& c) { m_bgColor = c; };

private:
	virtual void	OnFrameDraw(CCTextArea* pTextArea, CCDrawContext* pDC);
	virtual void	OnTextDraw(CCTextArea* pTextArea, CCDrawContext* pDC);
	virtual void	OnTextDraw_WordWrap(CCTextArea* pTextArea, CCDrawContext* pDC);
};

class CCTextArea : public CCWidget{
	friend CCTextAreaLook;
protected:
	bool		m_bScrollBarEnable;
	int			m_iIndentation;
	bool		m_bWordWrap;
	bool		m_bColorSupport;
	sPoint		m_TextOffset;
	bool		m_bMouseOver;
	sColor		m_TextColor;
	int			m_iMaxLen;
	char		m_szIMECompositionString[COMPOSITIONSTRING_LENGTH];
	bool		m_bEditable;
	int			m_iStartLine;
	int			m_iStartLineSkipLine;
	int			m_iCurrentSize;
	bool		m_bVerticalMoving;
	int			m_iVerticalMoveAxis;
	int			m_iCustomLineHeight;
	sPoint		m_CaretPos;		
	bool		m_bCaretFirst;	

	SLINELIST			m_Lines;
	SLINELISTITERATOR	m_CurrentLine;

	CCScrollBar*		m_pScrollBar;

	DECLARE_LOOK(CCTextAreaLook)
	DECLARE_LOOK_CLIENT()

protected:
	virtual void OnSize(int w, int h);
	virtual bool OnCommand(CCWidget* pWindow, const char* szMessage);
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);
	virtual void OnSetFocus(void);
	virtual void OnReleaseFocus(void);
	
	virtual bool InputFilterKey(int iKey,bool bCtrl);	// CCWM_KEYDOWN
	virtual bool InputFilterChar(int iKey);	// MWM_CHAR

	bool OnLButtonDown(sPoint pos);
	void OnScrollBarChanged(int nPos);

	bool MoveLeft(bool bBackspace=false);
	void MoveRight();
	void MoveDown();
	void MoveUp();
	void DeleteCurrent();
	void ScrollDown();
	void ScrollUp();
	void MoveFirst();
	void MoveLast();
	void OnHome();
	void OnEnd();

	bool GetCaretPosition(sPoint *pOut,int nSize,const char* szText,int nPos,bool bFirst);
	int GetCharPosition(const char* szText,int nX,int nLine);

	bool IsDoubleCaretPos();	

	void UpdateScrollBar(bool bAdjustStart=false);

	SLINELISTITERATOR GetIterator(int nLine);

public:
	CCTextArea(int nMaxLen = 120, const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCTextArea();

	virtual const char* GetClassName(void){ return CCTEXTAREA; }

	sPoint	GetCaretPos() { return m_CaretPos; }
	int		GetStartLine() { return m_iStartLine; }

	bool	IsScrollBarVisible() { return m_pScrollBar->IsVisible();	}
	int		GetScrollBarWidth() { return m_pScrollBar->GetRect().w;	}
	int		GetScrollBarMax() { return m_pScrollBar->GetMax(); }

	int		GetClientWidth();

	int		GetLength() { return (int)(m_iCurrentSize+m_Lines.size()); }
	int		GetLineCount() { return (int)m_Lines.size(); }			//??? that exists inside
	int		GetTotalLineCount(int& nStartLine, int& nCurrentLine);	//wordwrap adjusted when the number of lines actually output

	bool	GetText(char *pBuffer,int nBufferSize);
	const char* GetTextLine(int nLine);

	void	SetMaxLen(int nMaxLen);
	int		GetMaxLen() { return m_iMaxLen; }

	const char* GetCompositionString(void);

	void	SetEditable(bool editable){ m_bEditable = editable; }
	bool	GetEditable() { return m_bEditable; }

	void	SetScrollBarEnable(bool bEnable) { m_bScrollBarEnable = bEnable; }
	bool	GetScrollBarEnable() { return m_bScrollBarEnable; }
	
	void	SetTextOffset(sPoint p);

	void	SetIndentation(int nIndentation) { m_iIndentation = nIndentation; }
	
	void	SetTextColor(sColor color);
	sColor	GetTextColor(void);

	void	Clear();

	void	SetText(const char *szText);
	void	AddText(const char *szText,sColor color);
	void	AddText(const char *szText);

	void	DeleteFirstLine();
	int		GetLineHeight( void);
	void	SetCustomLineHeight( int nHeight);

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);

	void	AdjustHeightByContent();
};