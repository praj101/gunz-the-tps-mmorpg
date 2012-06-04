#include "stdafx.h"
#include "CCMatchBuff.h"
#include "CCMatchCRC32XORCache.h"

CCMatchBuffDesc::CCMatchBuffDesc() : m_nBuffID(0)
{
	m_pBuffInfo = new CCProtectValue<CCMatchBuffInfo>;
	memset(&m_pBuffInfo->Ref(), 0, sizeof(CCMatchBuffInfo));

	m_pBuffInfo->Ref().fSpeed_Ratio = 1.0f;
	m_pBuffInfo->MakeCrc();

	m_nBuffEffectType.Set_MakeCrc(MMBET_NORMAL);
	m_nBuffPeriodType.Set_MakeCrc(MMBPT_NONE);
	m_nBuffPeriod.Set_MakeCrc(0);

	memset(m_szBuffName, 0, sizeof(m_szBuffName));
	memset(m_szBuffDesc, 0, sizeof(m_szBuffDesc));
	memset(m_szBuffIconName, 0, sizeof(m_szBuffIconName));
}

CCMatchBuffDesc::~CCMatchBuffDesc()
{
	delete m_pBuffInfo;
}

void CCMatchBuffDesc::CacheCRC32( CCMatchCRC32XORCache& crc )
{
	crc.CRC32XOR( m_nBuffID );

	crc.CRC32XOR( m_nBuffPeriodType.Ref() );
	crc.CRC32XOR( m_nBuffPeriod.Ref() );
}

void CCMatchBuffDesc::ShiftFugitiveValues()
{
	m_nBuffEffectType.ShiftHeapPos_CheckCrc();
	m_nBuffPeriodType.ShiftHeapPos_CheckCrc();
	m_nBuffPeriod.ShiftHeapPos_CheckCrc();
	m_pBuffInfo->ShiftHeapPos_CheckCrc();
}

///////////////////////////////////////////////////////////////////////////////
// CCMatchItemDescMgr //////////////////////////////////////////////////////////
CCMatchBuffDescMgr::CCMatchBuffDescMgr() : m_nChecksum(0)
{

}

CCMatchBuffDescMgr::~CCMatchBuffDescMgr()
{
	Clear();
}

bool CCMatchBuffDescMgr::ReadXml(const char* szFileName)
{
	m_nChecksum = CCGetCCZFileChecksum(szFileName);

	CCXmlDocument xmlIniData;

	xmlIniData.Create();

	if (!xmlIniData.LoadFromFile(szFileName))
	{
		m_nChecksum = 0;
		xmlIniData.Destroy();
		return false;
	}

	CCXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MICTOK_BUFF))
		{
			if( !ParseItem(chrElement) ) {
				Clear();
				return false;
			}
		}
	}

	xmlIniData.Destroy();
	return true;
}


bool CCMatchBuffDescMgr::ReadXml(CCZFileSystem* pFileSystem, const char* szFileName)
{
	CCXmlDocument xmlIniData;
	xmlIniData.Create();

	char *buffer;
	CCZFile mzf;

	if(pFileSystem) 
	{
		if(!mzf.Open(szFileName,pFileSystem)) 
		{
			if(!mzf.Open(szFileName)) 
			{
				xmlIniData.Destroy();
				return false;
			}
		}
	} 
	else 
	{
		if(!mzf.Open(szFileName))
		{
			xmlIniData.Destroy();
			return false;
		}
	}

	buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;
	memset( buffer, 0, mzf.GetLength()+1 );

	mzf.Read(buffer,mzf.GetLength());

	m_nChecksum = CCGetMemoryChecksum(buffer,mzf.GetLength());

	if(!xmlIniData.LoadFromMemory(buffer))
	{
		m_nChecksum = 0;
		xmlIniData.Destroy();
		return false;
	}
	delete[] buffer;
	mzf.Close();

	//	<------------------


	CCXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MICTOK_BUFF))
		{
			if( !ParseItem(chrElement) ) {
				Clear();
				return false;
			}
		}
	}

	xmlIniData.Destroy();
	return true;
}

void CCMatchBuffDescMgr::Clear()
{
	while(!empty())
	{
		CCMatchBuffDesc* pBuffDesc = (*begin()).second;
		delete pBuffDesc;
		erase(begin());
	}
}

CCMatchBuffDesc* CCMatchBuffDescMgr::GetBuffDesc(unsigned long int nBuffID)
{
	iterator itor = find(nBuffID);
	if (itor != end()) {
		return (*itor).second;
	}

	_ASSERT(0);
	return NULL;
}

bool CCMatchBuffDescMgr::ParseItem(::CCXmlElement& element)
{
	CCMatchBuffDesc* pNewDesc = new CCMatchBuffDesc;

	int n = 0;
	char szAttrValue[256];
	char szAttrName[64];

	int nAttrCount = element.GetAttributeCount();
	for (int i = 0; i < nAttrCount; i++)
	{
		memset( szAttrValue, 0, 256 );
		memset( szAttrName, 0, 64 );
		element.GetAttribute(i, szAttrName, szAttrValue);

		if (!stricmp(szAttrName, MICTOK_ATTRNAME_BUFF_ID))
		{
			pNewDesc->m_nBuffID = atoi(szAttrValue);
		}
		else if (!stricmp(szAttrName, MICTOK_ATTRNAME_BUFF_ICON_NAME))
		{
			strcpy(pNewDesc->m_szBuffIconName, szAttrValue);
		}
		else if (!stricmp(szAttrName, MICTOK_ATTRNAME_BUFF_PERIOD_TYPE))
		{
			if (!stricmp(szAttrValue, MICTOK_ATTRVAL_PERIOD_TYPE_LONG))			pNewDesc->m_nBuffPeriodType.Set(MMBPT_LONG);
			else if (!stricmp(szAttrValue, MICTOK_ATTRVAL_PERIOD_TYPE_SHORT))	pNewDesc->m_nBuffPeriodType.Set(MMBPT_SHORT);
			else _ASSERT(0);
		}
		else if (!stricmp(szAttrName, MICTOK_ATTRNAME_BUFF_PERIOD))
		{			
			pNewDesc->m_nBuffPeriod.Set_MakeCrc(atoi(szAttrValue));
		}
		else if (!stricmp(szAttrName, MICTOK_ATTRNAME_BUFF_EFFECT_TYPE))
		{			
			if (!stricmp(szAttrValue, MICTOK_ATTRVAL_BUFF_EFFECT_TYPE_NORMAL))		pNewDesc->m_nBuffEffectType.Set(MMBET_NORMAL);
			else if (!stricmp(szAttrValue, MICTOK_ATTRVAL_BUFF_EFFECT_TYPE_DOTE))	pNewDesc->m_nBuffEffectType.Set(MMBET_DOTE);
			else _ASSERT(0);
		}		
		else if (!stricmp(szAttrName, MICTOK_ATTRNAME_BUFF_HP_VALUE))
		{
			pNewDesc->m_pBuffInfo->Ref().nHP = atoi(szAttrValue);
			pNewDesc->m_pBuffInfo->MakeCrc();
		}
		else if (!stricmp(szAttrName, MICTOK_ATTRNAME_BUFF_AP_VALUE))
		{
			pNewDesc->m_pBuffInfo->Ref().nAP = atoi(szAttrValue);
			pNewDesc->m_pBuffInfo->MakeCrc();
		}
		else if (!stricmp(szAttrName, MICTOK_ATTRNAME_BUFF_RESPAWN_DECREASE_TIME))
		{
			pNewDesc->m_pBuffInfo->Ref().nRespawnDecTime = atoi(szAttrValue);
			pNewDesc->m_pBuffInfo->MakeCrc();
		}
		else 
		{
			_ASSERT(0);

			delete pNewDesc;
			return false;
		}
	}

	iterator tempitor = find(pNewDesc->m_nBuffID);
	if (tempitor != end()) {
		_ASSERT(0);		///< 같은 ID의 아이템이 존재한다.
		delete pNewDesc;
		return false;
	}

	insert(value_type(pNewDesc->m_nBuffID, pNewDesc));
	return true;
}

bool CCMatchBuffDescMgr::SetBuffName(CCMatchItemDescMgr* pItemDescMgr)
{
	CCMatchItemDescMgr::iterator iter = pItemDescMgr->begin();
	for( ; iter != pItemDescMgr->end(); iter++) 
	{
		CCMatchItemDesc* pItemDesc = iter->second;

		if( pItemDesc->m_nSpendType.Ref() == MMCT_LONGBUFF || pItemDesc->m_nSpendType.Ref() == MMCT_SHORTBUFF ) 
		{
			CCMatchBuffDesc* pBuffDesc = CCGetMatchBuffDescMgr()->GetBuffDesc(pItemDesc->m_nID);
			if( pBuffDesc == NULL) {
				_ASSERT(0);
				return false;
			}

			if( pBuffDesc->m_nBuffPeriodType.Ref() == MMBPT_LONG && pItemDesc->m_nSpendType.Ref() != MMCT_LONGBUFF ) {
				_ASSERT(0);
				return false;
			}

			if( pBuffDesc->m_nBuffPeriodType.Ref() == MMBPT_SHORT && pItemDesc->m_nSpendType.Ref() != MMCT_SHORTBUFF ) {
				_ASSERT(0);
				return false;
			}

			strcpy(pBuffDesc->m_szBuffName, pItemDesc->m_pMItemName->Ref().m_szItemName);
			strcpy(pBuffDesc->m_szBuffDesc, pItemDesc->m_szDesc);
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CCMatchBuff
void CCMatchBuff::Reset()
{
	m_uidBuff = CCUID(0, 0);

	m_nBuffID				= 0;
	m_nRegTime				= 0;
	m_nBuffPeriodRemainder	= 0;

	m_pBuffDesc = NULL;
}

bool CCMatchBuff::Set(CCUID& uidBuff, int nBuffID, int nRegTime, int nBuffPeriodRemainder)
{
	m_pBuffDesc = CCGetMatchBuffDescMgr()->GetBuffDesc(nBuffID);
	if( m_pBuffDesc == NULL ) return false;

	m_uidBuff				= uidBuff;

	m_nBuffID				= nBuffID;
	m_nRegTime				= nRegTime;
	m_nBuffPeriodRemainder	= nBuffPeriodRemainder;

	return true;
}

bool CCMatchBuff::IsExpired(int nGlobalTick)
{
	if( GetBuffPeriodRemainder(nGlobalTick) <= 0 )	return true;
	else											return false;
}

int CCMatchBuff::GetBuffPeriodRemainder(int nGlobalTick)
{ 
	if( m_pBuffDesc == NULL ) {
		return 0;
	}

	int nTick = (m_nRegTime + m_nBuffPeriodRemainder) - nGlobalTick;

	if( nTick < 0 ) return 0;
	else			return nTick;
}


/////////////////////////////////////////////////////////////////////////////
// CCMatchShortBuffMap
void CCMatchShortBuffMap::Clear() 
{
	while(!empty()) {
		delete (CCMatchShortBuff*)begin()->second;
		erase(begin());
	}
}

void CCMatchShortBuffMap::Remove(CCUID& uidBuff)
{
	iterator iter = find(uidBuff);
	if( iter != end() ) {
		delete (CCMatchShortBuff*)iter->second;
		erase(iter);		
	}
}

bool CCMatchShortBuffMap::Insert(CCUID& uidBuff, CCMatchShortBuff* pBuff)
{
	iterator iter = find(uidBuff);
	if( iter != end() ) { _ASSERT(0); return false; }
	insert(pair<CCUID, CCMatchShortBuff*>(uidBuff, pBuff));
	return true;
}

CCMatchShortBuff* CCMatchShortBuffMap::GetShortBuffByBuffID(int nBuffID)
{
	for(iterator iter = begin(); iter != end(); iter++) {
		CCMatchShortBuff* pInfo = iter->second;
		if(pInfo->GetBuffID() == nBuffID) return pInfo;
	}
	return NULL;
}
