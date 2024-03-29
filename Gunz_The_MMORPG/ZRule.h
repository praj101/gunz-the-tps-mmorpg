#ifndef _ZRULE_H
#define _ZRULE_H


class ZMatch;

class ZRule
{
private:
	ZRule()			{ _ASSERT(0); } // 이건 사용하면 안됨 

protected:
	ZMatch*		m_pMatch;
	virtual void OnUpdate(float fDelta) {}
public:
	ZRule(ZMatch* pMatch);
	virtual ~ZRule();
	void Update(float fDelta);
	virtual bool OnCommand(CCCommand* pCommand);
	virtual void AfterCommandProcessed(CCCommand* pCommand);
	virtual void OnResponseRuleInfo(CCTD_RuleInfo* pInfo);
	virtual void OnSetRoundState(CCMATCH_ROUNDSTATE roundState) {}
	static ZRule* CreateRule(ZMatch* pMatch, CCMATCH_GAMETYPE nGameType);


	// 해당 Rule에서 상속 받아서 사용해야 할 것

	
};




#endif