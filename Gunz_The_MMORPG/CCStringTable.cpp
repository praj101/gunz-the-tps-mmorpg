#include "stdafx.h"
#include "CCStringTable.h"
#include <string.h>

static char g_StringTable[][256] = {
	"MINT",
	"확인",
	"취소",
	"예",
	"아니오",
	"메시지",
	"Overwrite?",
};

const char* CCGetString(int nID)
{
	int nCount = sizeof(g_StringTable)/sizeof(char*);
	if(nID<0 || nID>=nCount) return NULL;

	return g_StringTable[nID];
}

void CCSetString(int nID, const char* szString)
{
	int nCount = sizeof(g_StringTable)/sizeof(char*);
	if(nID<0 || nID>=nCount) return;

	strcpy(g_StringTable[nID], szString);
}
