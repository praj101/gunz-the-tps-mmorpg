#pragma once
#include "Core.h"
#include "CCWidget.h"
#include "CCFrame.h"
#include "CCButton.h"
#include "CCEdit.h"
#include "CCListBox.h"

#include <string>
#include <list>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>
using namespace std;

class CCConsoleFrame;

/////////////////////////////////////////////////////////////////////////////////
#define CONSOLE_LINES_MAX 250

typedef void(CCCONSOLE_KEYDOWN_CALLBACK)(int nKey);
typedef void(CCCONSOLE_INPUT_CALLBACK)(const char* szInputStr);

class CCConsoleEdit: public CCEdit
{
private:
	CCConsoleFrame*		m_pConsoleFrame;
protected:
	virtual bool InputFilterKey(int nKey);	// MWM_KEYDOWN
	virtual bool InputFilterChar(int nKey);	// MWM_CHAR
	virtual bool OnTab(bool bForward=true);

	CCCONSOLE_KEYDOWN_CALLBACK*		m_pfnKeyDown;
	CCCONSOLE_INPUT_CALLBACK*		m_pfnInput;
public:
	CCConsoleEdit(CCConsoleFrame* pConsoleFrame, int nMaxLength, const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	CCConsoleEdit(CCConsoleFrame* pConsoleFrame, const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCConsoleEdit();

	void SetKeyDownCallback(CCCONSOLE_KEYDOWN_CALLBACK* pfnKeyDown) { m_pfnKeyDown = pfnKeyDown; }
	void SetInputCallback(CCCONSOLE_INPUT_CALLBACK* pfnInput) { m_pfnInput = pfnInput; }
};

/// 실제 콘솔 클래스
class CCConsoleFrame : public CCFrame
{
private:
	CCListBox*		m_pListBox;
	CCConsoleEdit*	m_pInputEdit;
	list<string>	m_Commands;
protected:
	virtual void OnDraw(CCDrawContext* pDC);
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage);
	friend CCConsoleEdit;
	virtual void OnBrowseCommand(void);
	virtual bool OnShow(void);
public:
	CCConsoleFrame(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCConsoleFrame(void);

	void OutputMessage(const char* sStr);
	void ClearMessage();
	void SetBounds(sRect& r);
	void SetBounds(int x, int y, int w, int h);

	void AddCommand(const char* szCommand);

	CCConsoleEdit* GetInputEdit() { return m_pInputEdit; }

	void SetInputCallback(CCCONSOLE_INPUT_CALLBACK* pfnInput) { m_pInputEdit->SetInputCallback(pfnInput); }
};