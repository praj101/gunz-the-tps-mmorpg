#ifndef _ZMYITEM_H
#define _ZMYITEM_H


#include <map>
using std::map;

class ZMyItemNode : public CCBaseItem
{
protected:
	unsigned long int		m_nItemID;
	CCUID					m_UID;
	DWORD					m_dwWhenReceivedClock;
	
public:
	
	ZMyItemNode() : CCBaseItem(), m_nItemID(0), m_UID(CCUID(0,0)) { }
	virtual	~ZMyItemNode() { }

	void Create(CCUID& uidItem, unsigned long int nItemID, int nCount=1, 
				bool bIsRentItem=false, int nRentMinutePeriodRemainder=RENT_MINUTE_PERIOD_UNLIMITED, int iMaxUseHour = 0)	// Update sgk 0614 [int iMaxUseHour = 0] 추가
	{
		m_UID = uidItem;
		m_nItemID = nItemID;
		m_bIsRentItem = bIsRentItem;
		m_nRentMinutePeriodRemainder = nRentMinutePeriodRemainder;
		m_wRentHourPeriod = iMaxUseHour;
		m_dwWhenReceivedClock = timeGetTime();

		m_nCount = nCount;
	}
	void Create(unsigned long int nItemID, int nCount=1, 
				bool bIsRentItem=false, int nRentMinutePeriodRemainder=RENT_MINUTE_PERIOD_UNLIMITED, int iMaxUseHour = 0)	// Update sgk 0614 [int iMaxUseHour = 0] 추가
	{
		m_nItemID = nItemID;
		m_bIsRentItem = bIsRentItem;
		m_nRentMinutePeriodRemainder = nRentMinutePeriodRemainder;
		m_wRentHourPeriod = iMaxUseHour;
		m_dwWhenReceivedClock = timeGetTime();

		m_nCount = nCount;
	}
	DWORD GetWhenReceivedClock()
	{
		return m_dwWhenReceivedClock;
	}

	unsigned long int GetItemID()	{ return m_nItemID; }
	CCUID& GetUID()					{ return m_UID; }
};

typedef map<CCUID, ZMyItemNode*> MITEMNODEMAP;
typedef map<int, ZMyItemNode*> MACCOUNT_ITEMNODEMAP;




#endif