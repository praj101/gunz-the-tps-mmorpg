#ifndef _ZLOCALEDEFINE_H
#define _ZLOCALEDEFINE_H


/////////////////////////////////////////////////////////////////////////////////////////
// 지역에 따른 include //////////////////////////////////////////////////////////////////
#ifdef LOCALE_US			// 인터네셔날


#else
#ifdef LOCALE_JAPAN			// 일본
//	#include "extern/NetmarbleJP/NMCrypt.h"
	#include "ZGameOnJPAuth.h"
//	#pragma comment ( lib, "extern/NetmarbleJP/NMCrypt.lib"  )
#else						// 한국
	#include "extern/Netmarble/NMCrypt.h"
	#include "ZNetmarble.h"

	#pragma comment ( lib, "extern/Netmarble/NMCrypt.lib"  )
#endif
#endif
/////////////////////////////////////////////////////////////////////////////////////////







#endif