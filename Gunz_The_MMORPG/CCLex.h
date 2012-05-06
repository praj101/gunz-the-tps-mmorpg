#pragma once

#define CCLEX_LOWER(c)   (((c)>='A'  && (c) <= 'Z') ? ((c)+('a'-'A')) : (c))
#define CCLEX_UPPER(c)   (((c)>='a'  && (c) <= 'z') ? ((c)+('A'-'a')) : (c))


typedef bool(CCLEXPROC)(void* pData, int nLevel, char* pszArg);


struct CCLEXNODE {
	char*		szCmd;
	int			nLevel;
	CCLEXPROC*	pProc;		
};


class CCLex {
private:
	CCLEXNODE*	m_pCmdTable;

private:
	void SkipSpaces(char **ppszString);
	int FindCommand(char* szString);

public:
	CCLex(CCLEXNODE* pNode = 0)		{ m_pCmdTable = pNode; }
	virtual ~CCLex()		{}

	char* GetOneArg(char *pszArg, char *pszOutArg);
	char* GetOneArg(char *pszArg, char *pszOutArg, char *pszOutArg2);//jintriple3  귓속말 할때 to: 다음에 소문자로만 나오던 캐릭터 이름을 원래 이름으로 처리.
	char* GetTwoArgs(char* pszArg, char* pszOutArg1, char* pszOutArg2);
	bool SplitValue(char* pszSource, char* pszSeperator, char* pszField, char* pszValue);

	void SetCmdTable(CCLEXNODE* pCmdTable) { m_pCmdTable = pCmdTable; }
	CCLEXNODE* GetCmdTable() { return m_pCmdTable; }
	void Interprete(void* pData, char* pszString);
};
