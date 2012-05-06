#ifndef _ZCHARACTERBUFF_H
#define _ZCHARACTERBUFF_H

class ZShortBuff : public CCMatchShortBuff{};
class ZBuffSummary : public CCMatchBuffSummary{};

class ZCharacterBuff
{
protected:
public:
	ZCharacterBuff();
	~ZCharacterBuff(){}

protected:	
	ZShortBuff m_ShortBuff[MAX_CHARACTER_SHORT_BUFF_COUNT];

	ZBuffSummary m_BuffSummary;

public:
	void Clear();

	bool SetShortBuff(int nIndex, CCUID& uidBuff, int nBuffID, int nRegTime, int nPeriodRemainder);	
	ZShortBuff* GetShortBuff(int nIndex);
	ZBuffSummary* GetBuffSummary() { return &m_BuffSummary; }
};

#endif
