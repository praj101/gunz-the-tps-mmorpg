#pragma once
#pragma warning(disable:4786)

/*
	CCEdit.h 
*/

#include "CCWidget.h"
#include <list>
#include "CCLookNFeel.h"
#include "CCDebug.h"

using namespace std;

#define CCEDIT				"Edit"
#define CCEDIT_KEYDOWN_MSG	"keydown"
#define CCEDIT_CHAR_MSG		"char"
#define CCEDIT_ENTER_VALUE	"entered"
#define CCEDIT_ESC_VALUE	"esc"
#define CCEDIT_KILL_FOCUS	"killfocus"
#define CCEDIT_TEXT_CHANGED	"textchgd"
#define CCEDIT_BLINK_TIME	400

class CCEdit;

class CCEditLook{
protected:
	bool		m_bCustomLook;

public:
	CCEditLook();

	virtual void OnFrameDraw(CCEdit* pEdit, CCDrawContext* pDC);
	virtual void OnTextDraw(CCEdit* pEdit, CCDrawContext* pDC, bool bShowLanguageTab);

	virtual void OnDraw(CCEdit* pEdit, CCDrawContext* pDC, bool bShowLanguageTab=true);
	virtual sRect GetClientRect(CCEdit* pEdit, sRect& r);

	void SetCustomLook(bool b){
		m_bCustomLook = b;
	}
	bool GetCustomLook(){
		return m_bCustomLook;
	}
};


class CCEdit : CCWidget{
protected:
	bool			m_bMouseOver;
	sColor			m_TextColor;
	int				m_iMaxLength;
	char*			m_pBuffer;
	char			m_szIMECompositionString[COMPOSITIONSTRING_LENGTH];
	bool			m_bPassword;
	bool			m_bNumberOnly;
	int				m_iCaretPos;
	int				m_iStartPos;
	CCWidget*		m_pTabHandler;

public:
	int				m_iSelectionRange;

protected:
	list<char*>				m_History;
	list<char*>::iterator	m_iCurrentHistory;

	DECLARE_LOOK(CCEditLook)
	DECLARE_LOOK_CLIENT()

public:
	bool			m_bSupportHistory;

protected:
	virtual bool	OnEvent(CCEvent* pEvent, CCListener* pListener);

	virtual bool	InputFilterKey(int iKey);
	virtual bool	InputFilterChar(int iKey);

	virtual void	OnSetFocus();
	virtual void	OnReleaseFocus();

	virtual void	OnChangedText();

	void Initialize(int iMaxLength, const char* szName);

public:
	CCEdit(int iMaxLength, const char* szName = NULL, CCWidget* pParent= NULL, CCListener* pListener=NULL);
	CCEdit(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCEdit();

	virtual void OnHide();
	virtual void SetText(const char* szText);
	virtual const char* GetText();
	
	void AddText(const char* szText);

	int MoveCaretHome();
	int MoveCaretEnd();
	int MoveCaretPrev();
	int MoveCaretNext();

	void		SetMaxLength(int iMaxLength);
	int			GetMaxLength();
	const char* GetCompositionString();
	int			GetCarretPos();
	int			GetStartPos() { return m_iStartPos; };
	bool		SetStartPos(int iStartPos);
	int			GetPosByScreen(int x);

	void SetPasswordField(bool bPassword);
	bool IsPasswordField();

	void SetNumberField(bool b);
	bool IsNumberField();

	bool GetClipboard(char* szText, int nSize);
	bool SetClipboard(const char* szString);

	void AddHistory(const char* szText);

	CCWidget* GetTabHandler()	{ return m_pTabHandler; };
	void SetTabHandler(CCWidget* pWidget) { m_pTabHandler = pWidget; };

	virtual const char* GetClassName() { return CCEDIT; };
};