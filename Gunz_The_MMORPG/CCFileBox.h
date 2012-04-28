#pragma once
#pragma comment(lib, "Shlwapi.lib")

#include "CCListBox.h"

#define CCFILEBOX	"FileBox"

/// 파일 리스트 박스
class CCFileBox : public CCListBox{
	char	m_szFilter[256];
	char	m_szBaseDir[256];
public:
	/// @param	szFilter	파일 필터 ( 예: *.exe )
	CCFileBox(const char* szFilter, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCFileBox(void);

	/// 필터값으로 목록 리프레쉬
	/// @param	szFilter	파일 필터 ( 예: *.exe )
	void Refresh(const char* szFilter=NULL);
	const char* GetBaseDir(void);

	virtual const char* GetClassName(void){ return CCFILEBOX; }
};
