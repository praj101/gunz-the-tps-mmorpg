#pragma once


#define MAUTHINFO_BUFLEN	4096


class CCMatchAuthInfo {
public:
	CCMatchAuthInfo()			{}
	virtual ~CCMatchAuthInfo()	{}

	virtual const char* GetUserID() = 0;
	virtual const char* GetUniqueID() = 0;
	virtual int GetAge() = 0;
	virtual int GetSex() = 0;
	virtual int GetCCode() = 0;
};

class CCMatchAuthBuilder {
public:
	CCMatchAuthBuilder()				{}
	virtual ~CCMatchAuthBuilder()	{}

	virtual bool ParseAuthInfo(const char* pszData, CCMatchAuthInfo** ppoutAutoInfo) = 0;
};
