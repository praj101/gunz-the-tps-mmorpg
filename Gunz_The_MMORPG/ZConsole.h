#ifndef _ZCONSOLE_H
#define _ZCONSOLE_H

#include "CCConsoleFrame.h"

void CreateConsole(CCCommandManager* pCM);
void DestroyConsole();
void OutputToConsole(const char* pFormat,...);
void ConsoleInputEvent(const char* szInputStr);


CCConsoleFrame* ZGetConsole();

#endif