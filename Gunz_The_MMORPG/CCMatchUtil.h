#pragma once
#include <limits.h>
#include "MPacketCrypter.h"
#include "CCUID.h"

// 유틸리티 펑션들
unsigned long int MGetTimeDistance(unsigned long int nTimeA, unsigned long int nTimeB);

class MZFileSystem;
unsigned long MGetMemoryChecksum(char *pBuffer, int nLen);
unsigned long MGetMZFileChecksum(const char* pszFileName);
void MMakeSeedKey(MPacketCrypterKey* pKey, const CCUID& uidServer, const CCUID& uidClient, unsigned int nTimeStamp);

struct MShortVector
{
	short x;
	short y;
	short z;
};

inline float ShortToDirElement(short x)
{
	return ((1.f/32000.f) * x);
}

inline short DirElementToShort(float x)
{
	return (short)(32000*x);
}
