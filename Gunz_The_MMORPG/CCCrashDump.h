#pragma once

#include "CCDebug.h"
#include "Shlwapi.h"
#include <dbghelp.h>

DWORD CrashExceptionDump(PEXCEPTION_POINTERS ExceptionInfo, const char* szDumpFileName, bool bCCLog=false);
