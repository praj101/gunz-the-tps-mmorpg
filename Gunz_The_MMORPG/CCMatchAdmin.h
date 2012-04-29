#pragma once
#include <string>
#include <list>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>
using namespace std;

#include "CCUID.h"

#define	CONSOLE_TEXTSIZE	4096	///< Console Command Usage, Help 문자열 길이
#define	CONSOLE_NAMESIZE	256		///< Console Name 문자열 길이
#define	ARGVNoMin (-1)
#define ARGVNoMax (-1)

#define ADMIN_COMMAND_PREFIX		'@'

struct MAdminArgvInfo
{
	char *cargv[255];
	int cargc;
	char argbuf[2048];		// 실제 argv 값이 들어가는 buf
};

class CCMatchServer;

class MMatchAdmin
{
private:
protected:
	CCMatchServer*		m_pMatchServer;
	bool MakeArgv(char* szStr, MAdminArgvInfo* pAi);
public:
	MMatchAdmin();
	virtual ~MMatchAdmin();
	bool Create(CCMatchServer* pServer);
	void Destroy();
	bool Execute(const MUID& uidAdmin, const char* szStr);
};