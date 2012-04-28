#pragma once

#include "CCFrame.h"
#include "CCButton.h"
#include "CCLabel.h"
#include "CCTextArea.h"

#define CCMSGBOX_OK		"OK"
#define CCMSGBOX_CANCEL	"Cancel"
#define CCMSGBOX_YES	"Yes"
#define CCMSGBOX_NO		"No"

enum CCMsgBoxType{
	CCT_OK,
	CCT_CANCEL,
	CCT_OKCANCEL,
	MT_YESNO,

	MT_NOTDECIDED,	// 미리 만들어놓지 않기 위해
};

class MMsgBox : public CCFrame{
protected:
//	CCLabel*		m_pMessage;
	CCTextArea*	m_pMessage;
	CCButton*	m_pOK;
	CCButton*	m_pCancel;
	CCMsgBoxType	m_nType;
	//char		m_szMessage[256];

protected:
	/*
	virtual void OnDrawText(MDrawContext* pDC);
	virtual void OnDraw(MDrawContext* pDC);
	*/
	virtual bool OnShow(void);
	virtual void OnSize(int w, int h);

	virtual bool OnCommand(MWidget* pWindow, const char* szMessage);

public:
	MMsgBox(const char* szMessage, MWidget* pParent, MListener* pListener=NULL, CCMsgBoxType nType=MT_NOTDECIDED );
	virtual ~MMsgBox(void);

	void SetType(CCMsgBoxType nType);
	void SetTitle(const char* szTitle);
	const char* GetTitle(void);
	void SetMessage(const char* szMessage);
	const char* GetMessage(void);
	virtual void SetText(const char* szText);
	virtual const char* GetText(void);

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);
	void AdjustSize();

#define MINT_MSGBOX	"MsgBox"
	virtual const char* GetClassName(void){ return MINT_MSGBOX; }
};
