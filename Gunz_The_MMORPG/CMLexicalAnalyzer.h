#ifndef CMLEXICALANALYZER_H
#define CMLEXICALANALYZER_H

#include "CCPtrList.h"

class CCLexicalAnalyzer{
	CCPtrList<char>		m_Tokens;
	//char				m_szOriginal[256];
public:
	CCLexicalAnalyzer(void);
	~CCLexicalAnalyzer(void);

	bool Create(const char *pStr);
	void Destroy(void);
	char *GetByStr(int i);
	int GetByInt(int i);
	long GetByLong(int i);
	float GetByFloat(int i);
	int GetCount(void);

	bool IsNumber(int i);

	//char *GetOrgStr(void);
};

#endif
