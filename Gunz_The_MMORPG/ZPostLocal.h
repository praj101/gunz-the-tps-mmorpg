#ifndef _ZPOSTLOCAL_H
#define _ZPOSTLOCAL_H

#include "ZPrerequisites.h"
#include "ZGameClient.h"
#include "ZCommandTable.h"

// Local /////////////////////////////////////////////////////////////////////////////////////////////
inline void ZPostLocalReport119()
{
	ZPOSTCMD0(ZC_REPORT_119);
}

inline void ZPostLocalMessage(int iMessageID)
{
	ZPOSTCMD1(ZC_MESSAGE,CCCommandParameterInt(iMessageID));
}

inline void ZPostLocalEventOptainSpecialWorldItem(const int nWorldItemID)
{
	ZPOSTCMD1(ZC_EVENT_OPTAIN_SPECIAL_WORLDITEM, CCCmdParamInt(nWorldItemID));
}







#endif