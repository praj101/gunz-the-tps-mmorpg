#include "stdafx.h"
#include "CCMatchDebug.h"
#include "CCPdb.h"

//void SetSecurityErrorHandler(_secerr_handler_func func)
//{
//	_set_security_error_handler(func);
//}

void __cdecl ReportBufferOverrun(int code, void * unused)
{
//   if(code == _SECERR_BUFFER_OVERRUN)
   {
		cclog("\n!!!ERROR: Buffer overrun detected! Program will end.\n");

		// string str;
		// GetCrashInfo(NULL, str);
		// cclog(str.c_str());

		char szCrashLogFileName[ 1024 ] = {0,};
		_snprintf( szCrashLogFileName, 1024, "Crash_%s", CCGetLogFileName() );
		WriteCrashInfo( NULL, szCrashLogFileName );
   }

   exit(1);
}
