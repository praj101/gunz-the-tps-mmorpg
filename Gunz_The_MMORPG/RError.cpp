#include "stdafx.h"
#include "RError.h"

static int g_nRErrorCode = ROK;

void RSetError(int nErrCode)
{
	g_nRErrorCode = nErrCode;	

}

int RGetLastError()
{
	return g_nRErrorCode;
}
