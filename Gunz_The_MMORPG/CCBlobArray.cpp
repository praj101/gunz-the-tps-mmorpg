#include "stdafx.h"
#include "MBlobArray.h"
#include <memory.h>

void* CCMakeBlobArray(int nOneBlobSize, int nBlobCount)
{
	// nBlobCount개만큼 Blob과 부가 정보(카운트)를 더한 블럭을 잡고, 맨 처음에 부가 정보(카운트)에 nOneBlobSize와 nBlobCount값을 넣는다.
	unsigned char* pBlob = new unsigned char[sizeof(nOneBlobSize)+sizeof(nBlobCount)+nOneBlobSize*nBlobCount];
	memcpy(pBlob, &nOneBlobSize, sizeof(nOneBlobSize));
	memcpy(pBlob+sizeof(nBlobCount), &nBlobCount, sizeof(nOneBlobSize));
	return pBlob;
}

void CCEraseBlobArray(void* pBlob)
{
	delete[] pBlob;
}

void* CCGetBlobArrayElement(void* pBlob, int i)
{
	int nBlobCount = 0;
	int nOneBlobSize = 0;
	memcpy(&nOneBlobSize, (unsigned char*)pBlob, sizeof(nOneBlobSize));
	memcpy(&nBlobCount, (unsigned char*)pBlob+sizeof(nOneBlobSize), sizeof(nBlobCount));
	if(i<0 || i>=nBlobCount) return 0;

	return ((unsigned char*)pBlob+sizeof(int)*2+nOneBlobSize*i);
}

int CCGetBlobArrayCount(void* pBlob)
{
	int nBlobCount;
	memcpy(&nBlobCount, (unsigned char*)pBlob+sizeof(int), sizeof(nBlobCount));
	return nBlobCount;
}

int CCGetBlobArraySize(void* pBlob)
{
	int nBlobCount, nOneBlobSize;
	memcpy(&nOneBlobSize, (unsigned char*)pBlob, sizeof(nOneBlobSize));
	memcpy(&nBlobCount, (unsigned char*)pBlob+sizeof(int), sizeof(nBlobCount));

	return (nOneBlobSize*nBlobCount+sizeof(int)*2);
}

void* CCGetBlobArrayPointer(void* pBlob)
{
	return ((unsigned char*)pBlob+sizeof(int)*2);
}