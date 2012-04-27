#pragma once
/* 
	CCFont.h
*/

#include "CCTypes.h"

#define CCFONT_NAME_LENGTH 32

class CCFont{
public:
	char	m_szName[CCFONT_NAME_LENGTH];

	int		m_iOutlineStyle;
	ulong	m_ColorArg1;
	ulong	m_ColorArg2;

	CCFont();
	virtual ~CCFont();

	virtual bool	Create(const char* szName);
	virtual void	Destroy();
	virtual int		GetHeight() = 0;
	virtual int		GetWidth(const char* szText, int iSize=-1) = 0;
	virtual int		GetWidthWithoutAmpersand(const char* szText, int iSize=-1);
	virtual bool	Resize(float ratio, int nMinimumHeight) { _ASSERT(0); return true; };
};