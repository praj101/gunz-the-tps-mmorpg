#pragma once
#pragma comment(lib, "Shlwapi.lib")

#include "CCListBox.h"

#define CCFILEBOX	"FileBox"

//File list box.
class CCFileBox : public CCListBox{
	char	m_szFilter[256];
	char	m_szBaseDir[256];
public:
	//@ Param szFilter file filters (for example: *. Exe)
	CCFileBox(const char* szFilter, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCFileBox(void);

	//Refresh the list filter values
	//@ Param szFilter file filters (for example: *. Exe)
	void Refresh(const char* szFilter=NULL);
	const char* GetBaseDir(void);

	virtual const char* GetClassName(void){ return CCFILEBOX; }
};
