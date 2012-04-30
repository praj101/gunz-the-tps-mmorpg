#include "stdafx.h"
#include "CCMatchConfig.h"

///////////////////// client file list

list<unsigned int>	CCMatchAntiHack::m_clientFileListCRC;

void CCMatchAntiHack::InitClientFileList()
{
	FILE* fp = fopen("filelistcrc.txt", "r");
	if (fp == NULL) return;

	char str[256];

	while (fgets(str, 256, fp) != NULL)
	{
		unsigned int crc;
		sscanf(str, "%u", &crc);
		m_clientFileListCRC.push_back(crc);
	}

	fclose(fp);

	cclog("Inited client file list (%d)\n", (int)m_clientFileListCRC.size());
}

bool CCMatchAntiHack::CheckClientFileListCRC( unsigned int crc, const CCUID& uidUser )
{
	bool bFound = m_clientFileListCRC.end() != find(m_clientFileListCRC.begin(),m_clientFileListCRC.end(),crc);
	return bFound;
}


//size_t CCMatchAntiHack::GetHashMapSize() 
//{ 
//	return m_ClientHashValueList.size(); 
//}


size_t CCMatchAntiHack::GetFielCRCSize() 
{ 
	return m_clientFileListCRC.size(); 
}