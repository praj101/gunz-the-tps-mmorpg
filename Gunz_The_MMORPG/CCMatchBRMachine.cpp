#include "stdafx.h"
#include "CCMatchBRMachine.h"
#include "CCMath.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CCMatchBRDescription::CCMatchBRDescription(int nBRID, string strName, string strResetDesc, 
										 int nBRTID, int nRewardMinutePeriod, int nRewardCount, int nRewardKillCount)
{
	m_nBRID = nBRID;
	m_nBRTID = nBRTID;

	m_strName = strName;
	m_strResetDesc = strResetDesc;

	m_nRewardMinutePeriod = nRewardMinutePeriod;
	m_nRewardCount = nRewardCount;
	m_nRewardKillCount = nRewardKillCount;

	m_nTotalRate = 0;
}

CCMatchBRDescription::~CCMatchBRDescription()
{
	vector< CCMatchBRItem* >::iterator iter = m_RewardItemList.begin();
	while( iter != m_RewardItemList.end() )
	{
		delete (*iter);
		iter = m_RewardItemList.erase(iter);
	}
}

void CCMatchBRDescription::AddRewardItem(CCMatchBRItem* pRewardItem)
{
	if( pRewardItem == NULL ) { _ASSERT(0); return; }
	if( GetBRID() != pRewardItem->GetBRID() ) { _ASSERT(0); return; }
	if( m_nTotalRate + pRewardItem->GetRate() > 1000 ) { _ASSERT(0); return; }

	m_nTotalRate += pRewardItem->GetRate();
	pRewardItem->SetRateRange(m_nTotalRate);

	m_RewardItemList.push_back(pRewardItem);
}

CCMatchBRItem* CCMatchBRDescription::GetRewardItem()
{
	int nRateRage = RandomNumber(0, MAX_REWARD_ITEM_RATE);

	vector< CCMatchBRItem* >::const_iterator it, end;
	for( it = m_RewardItemList.begin(); it != m_RewardItemList.end(); ++it ) 
	{
		if( nRateRage < (*it)->GetRateRange() )
			return (*it);
	}

	return NULL;
}

DWORD CCMatchBRDescription::GetCRC32()
{
	CCMatchCRC32XORCache CRC32;

	CRC32.Reset();

	CRC32.CRC32XOR(GetBRID());
	CRC32.CRC32XOR(GetBRTID());
	CRC32.CRC32XOR(GetRewardMinutePeriod());
	CRC32.CRC32XOR(GetRewardCount());
	CRC32.CRC32XOR(GetRewardkillCount());

	for(int i = 0; i < (int)m_RewardItemList.size(); i++)
	{
		CRC32.CRC32XOR(m_RewardItemList[i]->GetBRID());
		CRC32.CRC32XOR(m_RewardItemList[i]->GetBRIID());
		CRC32.CRC32XOR(m_RewardItemList[i]->GetItemIDMale());
		CRC32.CRC32XOR(m_RewardItemList[i]->GetItemIDFemale());
		CRC32.CRC32XOR(m_RewardItemList[i]->GetItemCnt());
		CRC32.CRC32XOR(m_RewardItemList[i]->GetRentHourPeriod());		
		CRC32.CRC32XOR(m_RewardItemList[i]->GetRate());
	}	

	return CRC32.GetCRC32();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CCMatchBRDescriptionMap::CCMatchBRDescriptionMap()
{
	MakeCRC32();	
}

void CCMatchBRDescriptionMap::Clear()
{
	iterator iter = begin();
	while( iter != end() ) {
		delete iter->second;
		iter = erase(iter);
	}
}

void CCMatchBRDescriptionMap::MakeCRC32()
{
	m_CRC32.Reset();

	for( iterator iter = begin(); iter != end(); iter++)
	{
		CCMatchBRDescription* pDesc = iter->second;
		m_CRC32.CRC32XOR(pDesc->GetCRC32());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CCMatchBRMachine::CCMatchBRMachine()
{
}

CCMatchBRMachine::~CCMatchBRMachine()
{
	m_RewardDescription.Clear();
}

void CCMatchBRMachine::SetBattleTimeRewardMachine(CCMatchBRDescriptionMap DescriptionMap)
{
	if( m_RewardDescription.GetCRC32() != DescriptionMap.GetCRC32() )
	{
		m_RewardDescription.Clear();

		cclog("=======================================================================\n");
		CCMatchBRDescriptionMap::iterator iter = DescriptionMap.begin();
		for( ; iter != DescriptionMap.end(); iter++) 
		{
			CCMatchBRDescription *pDesc = iter->second;

			cclog(" BRID(%d), RewardMinutePeriod(%d), RewardCount(%d)\n", 
				pDesc->GetBRID(), pDesc->GetRewardMinutePeriod(), pDesc->GetRewardCount());

			vector<CCMatchBRItem*> v = pDesc->GetBattleRewardItemList();

			for(int i = 0; i < (int)v.size(); i++)
			{
				cclog("    BRIID(%d), ItemID(M=%d, F=%d), RentHourPeriod(%d), Rate(%d), RateRange(%d)\n", 
					v[i]->GetBRIID(), v[i]->GetItemIDMale(), v[i]->GetItemIDFemale(), v[i]->GetRentHourPeriod(), v[i]->GetRate(), v[i]->GetRateRange());
			}

			
			m_RewardDescription.insert(pair<int, CCMatchBRDescription*>(iter->first, iter->second));
		}
		m_RewardDescription.MakeCRC32();

		cclog("=======================================================================\n");
	}
	else
	{
		DescriptionMap.Clear();
	}
}

CCMatchBRDescription* CCMatchBRMachine::GetBattleTimeRewardDescription(int nBRID)
{
	CCMatchBRDescriptionMap::iterator iter = m_RewardDescription.find(nBRID);
	if( iter != m_RewardDescription.end() ) return iter->second;

	return NULL;
}
