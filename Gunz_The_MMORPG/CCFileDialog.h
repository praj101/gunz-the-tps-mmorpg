#ifndef _MFILEDIALOG_H
#define _MFILEDIALOG_H

#include "CCWidget.h"
#include "CCButton.h"
#include "CCFileBox.h"
#include "CCEdit.h"
#include "CCFrame.h"
#include "CCMsgBox.h"

#define CCFILEDIALOG		"FileDialog"
#define CCFILEDIALOG_OK		"OK"
#define CCFILEDIALOG_CANCEL	"Cancel"

//File type dialog
enum CCFileDialogType{
	CCFDT_OPEN = 0,	//open dialogue
	CCFDT_SAVE,		//save dialog
};

//File dialog box
class CCFileDialog : public CCFrame{
	CCButon*		m_pOK;
	CCButon*		m_pCancel;
	CCFileBox*		m_pFileBox;
	CCEdit*			m_pFileName;
	CCFileDialogType	m_iType;
	CCMsgBox*		m_pMsgBox;
	char			m_szTempPathName[256];

protected:
	virtual bool OnCommand(CCWidget* pWindow, const char* szMessage);
	bool IsExistFile(const char* szFileName);

public:
	CCFileDialog(const char* szFilter, CCWidget* pParent, CCListener* pListener=NULL);
	~CCFileDialog();

	const char* GetFileName();
	const char* GetPathName();
	void Refresh(const char* szFilter=NULL);

	const char* GetBaseDir();

	void SetType(CCFileDialogType fdt);
	virtual const char* GetClassName(){ return CCFILEDIALOG; }
};