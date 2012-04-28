#ifndef _MFILEDIALOG_H
#define _MFILEDIALOG_H

#include "CCWidget.h"
#include "CCButon.h"
#include "CCFileBox.h"
#include "CCEdit.h"
#include "CCFrame.h"
#include "CCMsgBox.h"

#define CCFILEDIALOG		"FileDialog"
#define CCFILEDIALOG_OK		"OK"
#define CCFILEDIALOG_CANCEL	"Cancel"

/// 파일 다이얼로그 타입
enum CCFileDialogType{
	CCFDT_OPEN = 0,	///< 오픈 다이얼로그
	CCFDT_SAVE,		///< 세이브 다이얼로그
};

/// 파일 다이얼로그 박스
class CCFileDialog : public CCFrame{
	CCButon*		m_pOK;
	CCButon*		m_pCancel;
	CCFileBox*		m_pFileBox;
	CCEdit*			m_pFileName;
	CCFileDialogType	m_nType;
	CCMsgBox*		m_pMsgBox;
	char			m_szTempPathName[256];

protected:
	virtual bool OnCommand(CCWidget* pWindow, const char* szMessage);
	bool IsExistFile(const char* szFileName);

public:
	CCFileDialog(const char* szFilter, CCWidget* pParent, CCListener* pListener=NULL);
	~CCFileDialog(void);

	const char* GetFileName(void);
	const char* GetPathName(void);
	void Refresh(const char* szFilter=NULL);

	const char* GetBaseDir(void);

	void SetType(CCFileDialogType fdt);
	virtual const char* GetClassName(void){ return CCFILEDIALOG; }
};