#pragma once

#include "CCFrame.h"
#include "CCButton.h"
#include "CCLabel.h"
#include "CCTextArea.h"

#define CCMSGBOX		"MsgBox"
#define CCMSGBOX_OK		"OK"
#define CCMSGBOX_CANCEL	"Cancel"
#define CCMSGBOX_YES	"Yes"
#define CCMSGBOX_NO		"No"

enum CCMsgBoxType{
	CCT_OK,
	CCT_CANCEL,
	CCT_OKCANCEL,
	MT_YESNO,

	MT_NOTDECIDED
};

class CCMsgBox : public CCFrame{
protected:
	CCTextArea*	m_pMessage;
	CCButton*	m_pOK;
	CCButton*	m_pCancel;
	CCMsgBoxType	m_nType;

protected:
	virtual bool OnShow(void);
	virtual void OnSize(int w, int h);

	virtual bool OnCommand(CCWidget* pWindow, const char* szMessage);

public:
	CCMsgBox(const char* szMessage, CCWidget* pParent, CCListener* pListener=NULL, CCMsgBoxType nType=MT_NOTDECIDED );
	virtual ~CCMsgBox(void);

	void SetType(CCMsgBoxType nType);
	void SetTitle(const char* szTitle);
	const char* GetTitle(void);
	void SetMessage(const char* szMessage);
	const char* GetMessage(void);
	virtual void SetText(const char* szText);
	virtual const char* GetText(void);

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);
	void AdjustSize();

	virtual const char* GetClassName(void){ return CCMSGBOX; }
};
