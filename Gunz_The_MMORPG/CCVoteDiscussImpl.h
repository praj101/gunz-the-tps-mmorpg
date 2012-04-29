#pragma once

#include "MVoteMgr.h"


class MVoteDiscussBuilder {
public:
	static MVoteDiscuss* Build(const CCUID& uidDrafter, const CCUID& uidStage, const char* pszDiscuss, const char* pszArg);
};

class MVoteDiscussJoke : public MVoteDiscuss {
public:
	string		m_strJoke;

public:
	MVoteDiscussJoke(const CCUID& uidStage);
	virtual bool OnJudge(bool bJudge);

	virtual string GetImplTarget() { return m_strJoke; }
};

class MVoteDiscussKick : public MVoteDiscuss {
public:
	string		m_strTarget;

public:
	MVoteDiscussKick(const CCUID& uidStage);
	virtual bool OnJudge(bool bJudge);

	virtual string GetImplTarget() { return m_strTarget; }
};
