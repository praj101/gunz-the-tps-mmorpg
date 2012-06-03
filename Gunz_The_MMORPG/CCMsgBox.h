#pragma once

#include "CCFrame.h"
#include "CCButton.h"
#include "CCLabel.h"
#include "CCTextArea.h"

#define CORE_CCMSGBOX	"MsgBox"
#define CCMSGBOX_OK		"OK"
#define CCMSGBOX_CANCEL	"Cancel"
#define CCMSGBOX_YES	"Yes"
#define CCMSGBOX_NO		"No"

enum CCMsgBoxType{
	CCT_OK,
	CCT_CANCEL,
	CCT_OKCANCEL,
	CCT_YESNO,

	CCT_NOTDECIDED
};

class CCMsgBox : public CCFrame{
protected:
	CCTextArea*	m_pMessage;
	CCButton*	m_pOK;
	CCButton*	m_pCancel;
	CCMsgBoxType	m_iType;

protected:
	virtual bool OnShow();
	virtual void OnSize(int w, int h);

	virtual bool OnCommand(CCWidget* pWindow, const char* szMessage);

public:
	CCMsgBox(const char* szMessage, CCWidget* pParent, CCListener* pListener=NULL, CCMsgBoxType nType=CCT_NOTDECIDED );
	virtual ~CCMsgBox();

	void SetType(CCMsgBoxType nType);
	void SetTitle(const char* szTitle);
	const char* GetTitle();
	void SetMessage(const char* szMessage);
	const char* GetMessage();
	virtual void SetText(const char* szText);
	virtual const char* GetText();

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);
	void AdjustSize();

	virtual const char* GetClassName(){ return CORE_CCMSGBOX; }
};
