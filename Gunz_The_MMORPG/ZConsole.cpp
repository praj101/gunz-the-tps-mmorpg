#include "stdafx.h"

#include "ZGameClient.h"
#include "ZConsole.h"
#include "CCCommand.h"
#include "CCCommandManager.h"
#include "ZApplication.h"
#include "ZPost.h"

// Added R347a
#include "ZGameInterface.h"

static CCConsoleFrame* g_pConsole = NULL;
CCConsoleFrame* ZGetConsole() { return g_pConsole; }

void OutputToConsole(const char* pFormat,...)
{
	if (!g_pConsole) return;

	va_list args;
	static char temp[1024];

	va_start(args, pFormat);
	vsprintf(temp, pFormat, args);

	g_pConsole->OutputMessage(temp);

	va_end(args);
}

// 콘솔에서 입력창에 입력했을때의 이벤트를 받는 함수
void ConsoleInputEvent(const char* szInputStr)
{
	char szErr[256];

	if (ZGetGameClient())
	{
#ifdef _DEBUG		// 오직 디버그 모드에서만 실행 가능하게 변경
		if ((szInputStr[0] == '@') && (szInputStr[1] == '@') && (szInputStr[2] == ' '))
		{
			char szMsg[1024]; 
			strcpy(szMsg, &szInputStr[3]);
			ZPostAdminTerminal(ZGetGameClient()->GetPlayerUID(), szMsg);
			return;
		}

		if (strcmp(szInputStr, ""))
		{
			if (!ZGetGameClient()->Post(szErr, 256, szInputStr)) OutputToConsole(szErr);
		}
#endif	
	}
}

// 콘솔 초기화
void CreateConsole(CCCommandManager* pCM)
{
	g_pConsole = new CCConsoleFrame("Console", Core::GetInstance()->GetMainFrame(), Core::GetInstance()->GetMainFrame());
	g_pConsole->Show(false);
	g_pConsole->SetInputCallback(ConsoleInputEvent);
	g_pConsole->SetBounds(0, 0, CCGetWorkspaceWidth()/2-20, CCGetWorkspaceHeight()/2);

	for(int i=0; i<pCM->GetCommandDescCount(); i++){
		CCCommandDesc* pCD = pCM->GetCommandDesc(i);
		g_pConsole->AddCommand(pCD->GetName());
	}
}

void DestroyConsole()
{
	if (g_pConsole)
	{
		delete g_pConsole; g_pConsole = NULL;
	}
}