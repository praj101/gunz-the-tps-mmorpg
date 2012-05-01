#pragma once

#include "CCVoteMgr.h"


class CCVoteDiscussBuilder {
public:
	static CCVoteDiscuss* Build(const CCUID& uidDrafter, const CCUID& uidStage, const char* pszDiscuss, const char* pszArg);
};

class CCVoteDiscussJoke : public CCVoteDiscuss {
public:
	string		m_strJoke;

public:
	CCVoteDiscussJoke(const CCUID& uidStage);
	virtual bool OnJudge(bool bJudge);

	virtual string GetImplTarget() { return m_strJoke; }
};

class CCVoteDiscussKick : public CCVoteDiscuss {
public:
	string		m_strTarget;

public:
	CCVoteDiscussKick(const CCUID& uidStage);
	virtual bool OnJudge(bool bJudge);

	virtual string GetImplTarget() { return m_strTarget; }
};
