#pragma once

#include "CCUID.h"

#include <list>
#include <string>
using namespace std;

class CCMatchAntiHack
{
private:
	static list<unsigned int>	m_clientFileListCRC;

public:
	CCMatchAntiHack()		{}
	~CCMatchAntiHack()		{}

	static size_t			GetFielCRCSize();
	static void				ClearClientFileList() { m_clientFileListCRC.clear(); }
	static void				InitClientFileList();
	static bool				CheckClientFileListCRC(unsigned int crc, const CCUID& uidUser );
};
