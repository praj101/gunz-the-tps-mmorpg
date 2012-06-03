#pragma once
#pragma comment(lib, "Shlwapi.lib")

#include "CCListBox.h"

#define CORE_CCFILEBOX	"FileBox"

//File list box.
class CCFileBox : public CCListBox{
	char	m_szFilter[256];
	char	m_szBaseDir[256];
public:
	//@ Param szFilter file filters (for example: *. Exe)
	CCFileBox(const char* szFilter, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCFileBox();

	//Refresh the list filter values
	//@ Param szFilter file filters (for example: *. Exe)
	void Refresh(const char* szFilter=NULL);
	const char* GetBaseDir();

	virtual const char* GetClassName(){ return CORE_CCFILEBOX; }
};
