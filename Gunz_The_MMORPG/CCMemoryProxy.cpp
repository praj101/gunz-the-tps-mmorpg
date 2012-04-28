#include "stdafx.h"
#include "CCMemoryProxy.h"

void (*g_fpOnCrcFail)() = 0;

CCCrc32Container g_crc32Container;


CCCrc32Container* GetCrcContainer()
{
	return &g_crc32Container;
}

