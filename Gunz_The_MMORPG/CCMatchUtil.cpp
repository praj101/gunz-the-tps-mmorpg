#include "stdafx.h"
#include "CCMatchUtil.h"
#include "CCZFileSystem.h"


unsigned long int CCGetTimeDistance(unsigned long int nTimeA, unsigned long int nTimeB)
{
	if(nTimeB >= nTimeA)
	{
		return nTimeB - nTimeA;
	}
	else
	{
		return nTimeB + (UINT_MAX - nTimeA + 1);
	}

	return 0;
}

unsigned long CCGetMemoryChecksum(char *pBuffer, int nLen)
{
	unsigned long nChecksum = 0;
	for (int i=0; i<nLen; i++)
		nChecksum += pBuffer[i];

	return nChecksum;
}

unsigned long CCGetCCZFileChecksum(const char* pszFileName) 
{
	CCZFile mzf;
	if(!mzf.Open(pszFileName)) 
		return 0;

	char* pBuffer = NULL;
	int nLen = mzf.GetLength();
	pBuffer = new char[mzf.GetLength()+1];
	pBuffer[nLen] = 0;
	mzf.Read(pBuffer, nLen);
	mzf.Close();

	unsigned long nChecksum = CCGetMemoryChecksum(pBuffer,nLen);
	delete pBuffer;
	return nChecksum;
}

void CCMakeSeedKey(CCPacketCrypterKey* pKey, const CCUID& uidServer, const CCUID& uidClient, unsigned int nTimeStamp)
{
	unsigned char* p = (unsigned char*)pKey->szKey;
	// key
	memset(p, 0, sizeof(CCPacketCrypterKey));
	int nUIDSize = sizeof(CCUID);

	memcpy(p, &nTimeStamp, sizeof(unsigned int));
	memcpy(p+sizeof(unsigned int), &uidServer.Low, sizeof(unsigned int));
	memcpy(p+nUIDSize, &uidClient, nUIDSize);

	const BYTE XOR[16] = {87, 2, 91, 4, 52, 6, 1, 8, 55, 10, 18, 105, 65, 56, 15, 120};

	for (int i = 0; i < 16; i++)
	{
		p[i] ^= XOR[i];
	}

	p = p + 16;

	// iv
	p[0] = 55;
	p[7] = 83;
	p[1] = 4;
	p[2] = 93;
	p[3] = 46;
	p[4] = 67;
	p[5] = MCOMMAND_VERSION;
	p[11] = 201;
	p[6] = 73;
	p[8] = 80;
	p[9] = 5;
	p[10] = 19;
	p[12] = 40;
	p[13] = 164;
	p[14] = 77;
	p[15] = 5;
}

/*
void CCMakeSeedKey(BYTE Key[SEED_USER_KEY_LEN], BYTE IV[SEED_BLOCK_LEN], const CCUID& uidServer, const CCUID& uidClient, unsigned int nTimeStamp)
{
	// key
	memset(Key, 0, sizeof(BYTE)*SEED_USER_KEY_LEN);
	int nUIDSize = sizeof(CCUID);

	memcpy(Key, &nTimeStamp, sizeof(unsigned int));
	memcpy(Key+sizeof(unsigned int), &uidServer.Low, sizeof(unsigned int));
	memcpy(Key+nUIDSize, &uidClient, nUIDSize);

	const BYTE XOR[16] = {87, 2, 91, 4, 52, 6, 1, 8, 55, 10, 18, 105, 65, 56, 15, 120};

	for (int i = 0; i < SEED_USER_KEY_LEN; i++)
	{
		Key[i] ^= XOR[i];
	}

	// iv
	IV[0] = 55;
	IV[7] = 83;
	IV[1] = 4;
	IV[2] = 93;
	IV[3] = 46;
	IV[4] = 67;
	IV[5] = 182;
	IV[11] = 201;
	IV[6] = 73;
	IV[8] = 80;
	IV[9] = 5;
	IV[10] = 19;
	IV[12] = 40;
	IV[13] = 164;
	IV[14] = 77;
	IV[15] = 5;
}
*/