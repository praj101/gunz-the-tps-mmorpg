#include "stdafx.h"
#include "CCQuestNPC.h"
#include "CCZFileSystem.h"
#include "stdlib.h"
#include "CCMath.h"

CCQuestNPCSpawnType CCQuestNPCInfo::GetSpawnType()
{
	if (nGrade >= NPC_GRADE_BOSS)
	{
		return MNST_BOSS;
	}
	else if (nNPCAttackTypes & NPC_ATTACK_RANGE)
	{
		return MNST_RANGE;
	}

	return MNST_MELEE;
}

///////////////////////////////////////////////////////////////////////////////

void CCQuestNPCCatalogue::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		delete (*itor).second;
	}

	clear();
}

CCQuestNPCCatalogue::CCQuestNPCCatalogue()
{
	m_GlobalAIValue.m_fAttack_ShakingRatio = 0.0f;
	m_GlobalAIValue.m_fPathFinding_ShakingRatio = 0.0f;
	m_GlobalAIValue.m_fSpeed_ShakingRatio = 0.0f;
	for (int i=0;i<NPC_INTELLIGENCE_STEPS;i++)
		m_GlobalAIValue.m_fPathFindingUpdateTime[i] = 1;
	for (int i=0;i<NPC_AGILITY_STEPS;i++)
		m_GlobalAIValue.m_fPathFindingUpdateTime[i] = 1;
}

CCQuestNPCCatalogue::~CCQuestNPCCatalogue() 
{ 
	Clear();
}

CCQuestNPCInfo* CCQuestNPCCatalogue::GetInfo(CCQUEST_NPC nNpc)
{
	iterator itor = find(nNpc);
	if (itor != end())
	{
		return (*itor).second;
	}

	_ASSERT(0);
	return NULL;
}

CCQuestNPCInfo* CCQuestNPCCatalogue::GetPageInfo( int nPage)
{
	int nCount = 0;
	for ( iterator itor = begin();  itor != end();  itor++)
	{
		if ( nCount++ == nPage)
			return (*itor).second;
	}

	return NULL;
}

void CCQuestNPCCatalogue::Insert(CCQuestNPCInfo* pNPCInfo)
{
	if( 0 == pNPCInfo )
	{
		ASSERT( 0 );
		return;
	}

	CCQUEST_NPC nID = pNPCInfo->nID;
	insert(value_type(nID, pNPCInfo));
}


CCQuestNPCInfo* CCQuestNPCCatalogue::GetIndexInfo( int nIndex )
{
	map< int, CCQUEST_NPC >::iterator it = m_MonsterBibleCatalogue.find( nIndex );
	if( m_MonsterBibleCatalogue.end() == it )
		return 0;

	return GetInfo( it->second );
}


//////////////////////////////////////////////////////
#define MTOK_NPC						"NPC"
#define MTOK_NPC_AI_VALUE				"AI_VALUE"

// 기본정보
#define	MTOK_NPC_AI_SHAKING							"SHAKING"
#define	MTOK_NPC_AI_SHAKING_ATTR_PATHFINDING_UPDATE	"pathfinding_update"
#define	MTOK_NPC_AI_SHAKING_ATTR_ATTACK_UPDATE		"attack_update"
#define	MTOK_NPC_AI_SHAKING_ATTR_SPEED				"speed"
#define	MTOK_NPC_AI_INTELLIGENCE					"INTELLIGENCE"
#define	MTOK_NPC_AI_AGILITY							"AGILITY"
#define	MTOK_NPC_AI_TIME							"TIME"
#define	MTOK_NPC_AI_ATTR_STEP						"step"


#define MTOK_NPC_TAG_COLLISION			"COLLISION"
#define MTOK_NPC_TAG_ATTACK				"ATTACK"
#define MTOK_NPC_TAG_SPEED				"SPEED"
#define MTOK_NPC_TAG_FLAG				"FLAG"
#define MTOK_NPC_TAG_SKILL				"SKILL"
#define MTOK_NPC_TAG_DROP				"DROP"
#define MTOK_NPC_TAG_SOUND				"SOUND"

#define MTOK_NPC_ATTR_ID				"id"
#define MTOK_NPC_ATTR_NAME				"name"
#define MTOK_NPC_ATTR_DESC				"desc"
#define MTOK_NPC_ATTR_SKILL				"skill"
#define MTOK_NPC_ATTR_MESHNAME			"meshname"
#define MTOK_NPC_ATTR_GRADE				"grade"
#define MTOK_NPC_ATTR_MAXHP				"max_hp"
#define MTOK_NPC_ATTR_MAXAP				"max_ap"
#define MTOK_NPC_ATTR_RADIUS			"radius"
#define MTOK_NPC_ATTR_HEIGHT			"height"
#define MTOK_NPC_ATTR_TYPE				"type"
#define MTOK_NPC_ATTR_RANGE				"range"
#define MTOK_NPC_ATTR_WEAPONITEM_ID		"weaponitem_id"
#define MTOK_NPC_ATTR_DEFAULT			"default"
#define MTOK_NPC_ATTR_ROTATE			"rotate"
#define MTOK_NPC_ATTR_SCALE				"scale"
#define MTOK_NPC_ATTR_DC				"dc"
#define MTOK_NPC_ATTR_HITRATE			"hitrate"
#define MTOK_NPC_ATTR_TABLE				"table"
#define MTOK_NPC_ATTR_INT				"int"
#define MTOK_NPC_ATTR_AGILITY			"agility"
#define MTOK_NPC_ATTR_COOLTIME			"cooltime"
#define MTOK_NPC_ATTR_VIEW_ANGLE		"view_angle"
#define MTOK_NPC_ATTR_DYINGTIME			"dyingtime"
#define MTOK_NPC_ATTR_PICK				"pick"
#define MTOK_NPC_ATTR_ANGLE				"angle"
#define MTOK_NPC_ATTR_TREMBLE			"tremble"
#define MTOK_NPC_ATTR_OFFENSETYPE		"offensetype"
#define MTOK_NPC_ATTR_FRIENDLY			"friendly"


#define MTOK_NPC_ATTR_NEVER_BLASTED			"never_blasted"
#define MTOK_NPC_ATTR_NEVER_DAMAGED_MELEE	"never_damaged_melee"
#define MTOK_NPC_ATTR_NEVER_DAMAGED_RANGE	"never_damaged_range"
#define MTOK_NPC_ATTR_NEVER_PUSHED			"never_pushed"
#define MTOK_NPC_ATTR_NEVER_ATTACK_CANCEL	"never_attack_cancel"

#define MTOK_NPC_ATTR_SHADOW				"shadow"

#define MTOK_NPC_ATTR_ATTACK				"attack"
#define MTOK_NPC_ATTR_DEATH					"death"
#define MTOK_NPC_ATTR_WOUND					"wound"


bool CCQuestNPCCatalogue::ReadXml(const char* szFileName)
{
	CCXmlDocument xmlIniData;

	xmlIniData.Create();

	if (!xmlIniData.LoadFromFile(szFileName)) {
		xmlIniData.Destroy();
		return false;
	}

	CCXmlElement rootElement, chrElement, attrElement;

	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();

	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++) {

		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MTOK_NPC))
		{
			ParseNPC(chrElement);
		}
		else if (!stricmp(szTagName, MTOK_NPC_AI_VALUE))
		{
			ParseGlobalAIValue(chrElement);
		}
	}

	xmlIniData.Destroy();
	return true;
}

bool CCQuestNPCCatalogue::ReadXml(CCZFileSystem* pFileSystem, const char* szFileName)
{

	CCXmlDocument	xmlIniData;
	xmlIniData.Create();

	char *buffer;
	CCZFile mzf;

	if(pFileSystem) {
		if(!mzf.Open(szFileName,pFileSystem))  {
			if(!mzf.Open(szFileName))  {
				xmlIniData.Destroy();
				return false;
			}
		}
	} 
	else  {

		if(!mzf.Open(szFileName)) {

			xmlIniData.Destroy();
			return false;
		}
	}

	buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;

	mzf.Read(buffer,mzf.GetLength());

	if(!xmlIniData.LoadFromMemory(buffer)) {
		xmlIniData.Destroy();
		return false;
	}

	delete[] buffer;
	mzf.Close();


	CCXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++) {

		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MTOK_NPC)) {
			ParseNPC(chrElement);
		}
		else if (!stricmp(szTagName, MTOK_NPC_AI_VALUE))
		{
			ParseGlobalAIValue(chrElement);
		}
	}

	xmlIniData.Destroy();

	return true;
}

void CCQuestNPCCatalogue::ParseNPC(CCXmlElement& element)
{
	char szTemp[256]="";
	int n = 0;
	char szAttrValue[256];
	char szAttrName[64];
	char szTagName[128];

	CCQuestNPCInfo* pNPCInfo = new CCQuestNPCInfo();
	pNPCInfo->SetDefault();

	// NPC 태그 속성값 --------------------
	int nAttrCount = element.GetAttributeCount();
	for (int i = 0; i < nAttrCount; i++)
	{
		element.GetAttribute(i, szAttrName, szAttrValue);
		if (!stricmp(szAttrName, MTOK_NPC_ATTR_ID))
		{
			pNPCInfo->nID = (CCQUEST_NPC)atoi(szAttrValue);
		}
		else if (!stricmp(szAttrName, MTOK_NPC_ATTR_NAME))
		{
			strcpy(pNPCInfo->szName, CCGetStringResManager()->GetStringFromXml(szAttrValue));
		}
		else if (!stricmp(szAttrName, MTOK_NPC_ATTR_DESC))
		{
			strcpy(pNPCInfo->szDesc, CCGetStringResManager()->GetStringFromXml(szAttrValue));
		}
		else if (!stricmp(szAttrName, MTOK_NPC_ATTR_MESHNAME))
		{
			strcpy(pNPCInfo->szMeshName, szAttrValue);
		}
		else if (!stricmp(szAttrName, MTOK_NPC_ATTR_GRADE))
		{
			if(!stricmp(szAttrValue,		"veteran"))		pNPCInfo->nGrade = NPC_GRADE_VETERAN;
			else if(!stricmp(szAttrValue,	"elite"))		pNPCInfo->nGrade = NPC_GRADE_ELITE;
			else if(!stricmp(szAttrValue,	"boss"))		pNPCInfo->nGrade = NPC_GRADE_BOSS;
			else if(!stricmp(szAttrValue,	"legendary"))	pNPCInfo->nGrade = NPC_GRADE_LEGENDARY;
			else											pNPCInfo->nGrade = NPC_GRADE_REGULAR;
		}
		else if (!stricmp(szAttrName, MTOK_NPC_ATTR_MAXHP))
		{
			pNPCInfo->nMaxHP = atoi(szAttrValue);
		}
		else if (!stricmp(szAttrName, MTOK_NPC_ATTR_MAXAP))
		{
			pNPCInfo->nMaxAP = atoi(szAttrValue);
		}
		else if (!stricmp(szAttrName, MTOK_NPC_ATTR_INT))
		{
			pNPCInfo->nIntelligence = atoi(szAttrValue);
		}
		else if (!stricmp(szAttrName, MTOK_NPC_ATTR_AGILITY))
		{
			pNPCInfo->nAgility = atoi(szAttrValue);
		}
		else if (!stricmp(szAttrName, MTOK_NPC_ATTR_SCALE))
		{
			sscanf(szAttrValue,"%f %f %f",&pNPCInfo->vScale.x,&pNPCInfo->vScale.y,&pNPCInfo->vScale.z);
		}
		else if (!stricmp(szAttrName, MTOK_NPC_ATTR_DC))
		{
			pNPCInfo->nDC = atoi(szAttrValue);
		}
		else if (!stricmp(szAttrName, MTOK_NPC_ATTR_VIEW_ANGLE))
		{
			int nAngle = atoi(szAttrValue);
			pNPCInfo->fViewAngle = ToRadian(nAngle);
		}
		else if (!stricmp(szAttrName, MTOK_NPC_ATTR_DYINGTIME))
		{
			pNPCInfo->fDyingTime = (float)atof(szAttrValue);
		}
		else if (!stricmp(szAttrName, MTOK_NPC_ATTR_OFFENSETYPE))
		{
			pNPCInfo->nOffenseType = atoi(szAttrValue);
		}
		else if (!stricmp(szAttrName, MTOK_NPC_ATTR_FRIENDLY))
		{
			if ( !stricmp( szAttrValue, "true"))
				pNPCInfo->bFriendly = true;
		}
	}


	int iChildCount = element.GetChildNodeCount();
	CCXmlElement chrElement;
	for (int i = 0; i < iChildCount; i++)
	{
		chrElement = element.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		// COLLISION 태그 --------------------
		if (!stricmp(szTagName, MTOK_NPC_TAG_COLLISION))
		{
			int nAttrCount = chrElement.GetAttributeCount();
			for (int i = 0; i < nAttrCount; i++)
			{
				chrElement.GetAttribute(i, szAttrName, szAttrValue);
				if (!stricmp(szAttrName, MTOK_NPC_ATTR_RADIUS))
				{
					pNPCInfo->fCollRadius = (float)atof(szAttrValue);
				}
				else if (!stricmp(szAttrName, MTOK_NPC_ATTR_HEIGHT))
				{
					pNPCInfo->fCollHeight = (float)atof(szAttrValue);
				}
				else if (!stricmp(szAttrName, MTOK_NPC_ATTR_PICK))
				{
					if(!stricmp(szAttrValue, "true")) pNPCInfo->bColPick = true;
				}
				else if (!stricmp(szAttrName, MTOK_NPC_ATTR_TREMBLE))
				{
					pNPCInfo->fTremble = (float)atof(szAttrValue);
				}
			}
		}
		// ATTACK 태그 -----------------------
		else if (!stricmp(szTagName, MTOK_NPC_TAG_ATTACK))
		{
			int nAttrCount = chrElement.GetAttributeCount();
			for (int i = 0; i < nAttrCount; i++)
			{
				chrElement.GetAttribute(i, szAttrName, szAttrValue);
				if (!stricmp(szAttrName, MTOK_NPC_ATTR_TYPE))
				{
					if(!stricmp(szAttrValue,		"melee"))   pNPCInfo->nNPCAttackTypes = NPC_ATTACK_MELEE;
					else if(!stricmp(szAttrValue,	"range"))   pNPCInfo->nNPCAttackTypes = NPC_ATTACK_RANGE;
					else if(!stricmp(szAttrValue,	"magic"))   pNPCInfo->nNPCAttackTypes = NPC_ATTACK_MAGIC;
					else										pNPCInfo->nNPCAttackTypes = NPC_ATTACK_NONE;
				}
				else if (!stricmp(szAttrName, MTOK_NPC_ATTR_RANGE))
				{
					pNPCInfo->fAttackRange = (float)atof(szAttrValue);
				}
				else if (!stricmp(szAttrName, MTOK_NPC_ATTR_ANGLE))
				{
					int nAngle = atoi(szAttrValue);
					pNPCInfo->fAttackRangeAngle = ToRadian(nAngle);
				}
				else if (!stricmp(szAttrName, MTOK_NPC_ATTR_WEAPONITEM_ID))
				{
					pNPCInfo->nWeaponItemID = atoi(szAttrValue);
				}
				else if (!stricmp(szAttrName, MTOK_NPC_ATTR_COOLTIME))
				{
					pNPCInfo->fAttackCoolTime = (float)atof(szAttrValue);
				}
				else if (!stricmp(szAttrName, MTOK_NPC_ATTR_HITRATE))
				{
					int nPercent = atoi(szAttrValue);
					pNPCInfo->fAttackHitRate = (float)nPercent / 100.0f;
				}
			}
		}
		// SPEED 태그 ------------------------
		else if (!stricmp(szTagName, MTOK_NPC_TAG_SPEED))
		{
			int nAttrCount = chrElement.GetAttributeCount();
			for (int i = 0; i < nAttrCount; i++)
			{
				chrElement.GetAttribute(i, szAttrName, szAttrValue);
				if (!stricmp(szAttrName, MTOK_NPC_ATTR_DEFAULT))
				{
					pNPCInfo->fSpeed = (float)atof(szAttrValue);
				}
				else if (!stricmp(szAttrName, MTOK_NPC_ATTR_ROTATE))
				{
					pNPCInfo->fRotateSpeed = (float)atof(szAttrValue);
				}
			}
		}
		// FLAG 태그 -------------------------
		else if (!stricmp(szTagName, MTOK_NPC_TAG_FLAG))
		{
			int nAttrCount = chrElement.GetAttributeCount();
			for (int i = 0; i < nAttrCount; i++)
			{
				chrElement.GetAttribute(i, szAttrName, szAttrValue);
				if (!stricmp(szAttrName, MTOK_NPC_ATTR_NEVER_BLASTED))
				{
					if(!stricmp(szAttrValue, "true"))	pNPCInfo->bNeverBlasted = true;
					else								pNPCInfo->bNeverBlasted = false;
				}
				else if (!stricmp(szAttrName, MTOK_NPC_ATTR_NEVER_DAMAGED_MELEE))
				{
					if(!stricmp(szAttrValue, "true"))	pNPCInfo->bMeleeWeaponNeverDamaged = true;
					else								pNPCInfo->bMeleeWeaponNeverDamaged = false;
				}
				else if (!stricmp(szAttrName, MTOK_NPC_ATTR_NEVER_DAMAGED_RANGE))
				{
					if(!stricmp(szAttrValue, "true"))	pNPCInfo->bRangeWeaponNeverDamaged = true;
					else								pNPCInfo->bRangeWeaponNeverDamaged = false;
				}
				else if (!stricmp(szAttrName, MTOK_NPC_ATTR_SHADOW))
				{
					if(!stricmp(szAttrValue, "true"))	pNPCInfo->bShadow = true;
					else								pNPCInfo->bShadow = false;
				}
				else if (!stricmp(szAttrName, MTOK_NPC_ATTR_NEVER_PUSHED))
				{
					if(!stricmp(szAttrValue, "true"))	pNPCInfo->bNeverPushed = true;
					else								pNPCInfo->bNeverPushed = false;
				}
				else if (!stricmp(szAttrName, MTOK_NPC_ATTR_NEVER_ATTACK_CANCEL))
				{
					if(!stricmp(szAttrValue, "true"))	pNPCInfo->bNeverAttackCancel = true;
					else								pNPCInfo->bNeverAttackCancel = false;
				}
			}
		}
		else if (!stricmp(szTagName, MTOK_NPC_TAG_SKILL))
		{
			int nSkillID;
			chrElement.GetAttribute(&nSkillID,MTOK_NPC_ATTR_ID,0);
			_ASSERT(pNPCInfo->nSkills<MAX_SKILL);
			if(pNPCInfo->nSkills<MAX_SKILL)
			{
				pNPCInfo->nSkillIDs[pNPCInfo->nSkills++]=nSkillID;
				// 기술이 하나라도 있으면 플래그 체크해둔다
				pNPCInfo->nNPCAttackTypes|=NPC_ATTACK_MAGIC;
			}
		}
		// SOUND 태그 --------------------
		if (!stricmp(szTagName, MTOK_NPC_TAG_SOUND))
		{
			int nAttrCount = chrElement.GetAttributeCount();
			for (int i = 0; i < nAttrCount; i++)
			{
				chrElement.GetAttribute(i, szAttrName, szAttrValue);
				if (!stricmp(szAttrName, MTOK_NPC_ATTR_ATTACK))
				{
					strcpy(pNPCInfo->szSoundName[NPC_SOUND_ATTACK], szAttrValue);
				}
				else if (!stricmp(szAttrName, MTOK_NPC_ATTR_WOUND))
				{
					strcpy(pNPCInfo->szSoundName[NPC_SOUND_WOUND], szAttrValue);
				}
				else if (!stricmp(szAttrName, MTOK_NPC_ATTR_DEATH))
				{
					strcpy(pNPCInfo->szSoundName[NPC_SOUND_DEATH], szAttrValue);
				}
			}
		}
		// DROP 태그 -------------------------
		else if (!stricmp(szTagName, MTOK_NPC_TAG_DROP))
		{
			int nAttrCount = chrElement.GetAttributeCount();
			for (int i = 0; i < nAttrCount; i++)
			{
				chrElement.GetAttribute(i, szAttrName, szAttrValue);
				if (!stricmp(szAttrName, MTOK_NPC_ATTR_TABLE))
				{
					strcpy(pNPCInfo->szDropTableName, szAttrValue);
				}
			}
		}
	}

	Insert( pNPCInfo );
}

void CCQuestNPCCatalogue::ParseGlobalAIValue(CCXmlElement& element)
{
	char szTemp[256]="";
	int n = 0;
	char szAttrValue[256];
	char szAttrName[64];
	char szTagName[128];

	int iChildCount = element.GetChildNodeCount();
	CCXmlElement chrElement;
	for (int i = 0; i < iChildCount; i++)
	{
		chrElement = element.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		// SHAKING 태그 --------------------
		if (!stricmp(szTagName, MTOK_NPC_AI_SHAKING))
		{
			int nAttrCount = chrElement.GetAttributeCount();
			for (int i = 0; i < nAttrCount; i++)
			{
				chrElement.GetAttribute(i, szAttrName, szAttrValue);
				if (!stricmp(szAttrName, MTOK_NPC_AI_SHAKING_ATTR_PATHFINDING_UPDATE))
				{
					m_GlobalAIValue.m_fPathFinding_ShakingRatio = (float)atof(szAttrValue);
				}
				else if (!stricmp(szAttrName, MTOK_NPC_AI_SHAKING_ATTR_ATTACK_UPDATE))
				{
					m_GlobalAIValue.m_fAttack_ShakingRatio = (float)atof(szAttrValue);
				}
				else if (!stricmp(szAttrName, MTOK_NPC_AI_SHAKING_ATTR_SPEED))
				{
					m_GlobalAIValue.m_fSpeed_ShakingRatio = (float)atof(szAttrValue);
				}
			}
		}
		// INTELLIGENCE 태그 -----------------------
		else if (!stricmp(szTagName, MTOK_NPC_AI_INTELLIGENCE))
		{
			int nChild = chrElement.GetChildNodeCount();
			CCXmlElement TimeElement;
			char szChrTagName[64];
			for (int j = 0; j < nChild; j++)
			{
				TimeElement = chrElement.GetChildNode(j);
				TimeElement.GetTagName(szChrTagName);
				if (szChrTagName[0] == '#') continue;

				if (!stricmp(szChrTagName, MTOK_NPC_AI_TIME))
				{
					int nStep = 0;
					float fTime = 1.0f;

					TimeElement.GetContents(&fTime);
					TimeElement.GetAttribute(&nStep, MTOK_NPC_AI_ATTR_STEP);

					nStep -= 1;
					if ((nStep >= 0) && (nStep < NPC_INTELLIGENCE_STEPS))
					{
						m_GlobalAIValue.m_fPathFindingUpdateTime[nStep] = fTime;
					}
				}
			}
		}
		// AGILITY 태그 -----------------------
		else if (!stricmp(szTagName, MTOK_NPC_AI_AGILITY))
		{
			int nChild = chrElement.GetChildNodeCount();
			CCXmlElement TimeElement;
			char szChrTagName[64];
			for (int j = 0; j < nChild; j++)
			{
				TimeElement = chrElement.GetChildNode(j);
				TimeElement.GetTagName(szChrTagName);
				if (szChrTagName[0] == '#') continue;

				if (!stricmp(szChrTagName, MTOK_NPC_AI_TIME))
				{
					int nStep = 0;
					float fTime = 1.0f;

					TimeElement.GetContents(&fTime);
					TimeElement.GetAttribute(&nStep, MTOK_NPC_AI_ATTR_STEP);

					nStep -= 1;
					if ((nStep >= 0) && (nStep < NPC_AGILITY_STEPS))
					{
						m_GlobalAIValue.m_fAttackUpdateTime[nStep] = fTime;
					}
				}
			}
		}
	}


}
////////////////////////////////////////////////////////////////////////////////////////////////////

void CCQuestNPCSetCatalogue::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		delete (*itor).second;
	}

	clear();

	m_NameMap.clear();
}

CCQuestNPCSetCatalogue::CCQuestNPCSetCatalogue()
{

}

CCQuestNPCSetCatalogue::~CCQuestNPCSetCatalogue() 
{ 
	Clear();
}

CCQuestNPCSetInfo* CCQuestNPCSetCatalogue::GetInfo(int nID)
{
	iterator itor = find(nID);
	if (itor != end())
	{
		return (*itor).second;
	}

	return NULL;
}

CCQuestNPCSetInfo* CCQuestNPCSetCatalogue::GetInfo(const char* szName)
{
	char szLwrName[64];
	strcpy(szLwrName, szName);
	string strName = _strlwr(szLwrName);

	map<string, CCQuestNPCSetInfo*>::iterator itor = m_NameMap.find(strName);
	if (itor != m_NameMap.end())
	{
		return (*itor).second;
	}

	_ASSERT(0);
	return NULL;
}

void CCQuestNPCSetCatalogue::Insert(CCQuestNPCSetInfo* pNPCSetInfo)
{
	int nID = pNPCSetInfo->nID;

	if (GetInfo(nID))
	{
		// 똑같은 ID의 NPCSET가 존재한다.
		_ASSERT(0);
		return;
	}

	insert(value_type(nID, pNPCSetInfo));

	char szLwrName[64];
	strcpy(szLwrName, pNPCSetInfo->szName);

	string strName = _strlwr(szLwrName);
	m_NameMap.insert(map<string, CCQuestNPCSetInfo*>::value_type(strName, pNPCSetInfo));
}


#define MTOK_NPCSET							"NPCSET"
#define MTOK_NPCSET_TAG_ADDNPC				"ADDNPC"

#define MTOK_NPCSET_ATTR_ID					"id"
#define MTOK_NPCSET_ATTR_NAME				"name"
#define MTOK_NPCSET_ATTR_BASENPC			"basenpc"
#define MTOK_NPCSET_ATTR_NPC_ID				"npc_id"
#define MTOK_NPCSET_ATTR_MIN_RATE			"min_rate"
#define MTOK_NPCSET_ATTR_MAX_RATE			"max_rate"
#define MTOK_NPCSET_ATTR_MAX_COUNT			"max_count"



bool CCQuestNPCSetCatalogue::ReadXml(const char* szFileName)
{
	CCXmlDocument xmlIniData;

	xmlIniData.Create();

	if (!xmlIniData.LoadFromFile(szFileName)) {
		xmlIniData.Destroy();
		return false;
	}

	CCXmlElement rootElement, chrElement, attrElement;

	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();

	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++) {

		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MTOK_NPCSET))
		{
			ParseNPCSet(chrElement);
		}
	}

	xmlIniData.Destroy();
	return true;
}

bool CCQuestNPCSetCatalogue::ReadXml(CCZFileSystem* pFileSystem, const char* szFileName)
{

	CCXmlDocument	xmlIniData;
	xmlIniData.Create();

	char *buffer;
	CCZFile mzf;

	if(pFileSystem) {
		if(!mzf.Open(szFileName,pFileSystem))  {
			if(!mzf.Open(szFileName))  {
				xmlIniData.Destroy();
				return false;
			}
		}
	} 
	else  {

		if(!mzf.Open(szFileName)) {

			xmlIniData.Destroy();
			return false;
		}
	}

	buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;

	mzf.Read(buffer,mzf.GetLength());

	if(!xmlIniData.LoadFromMemory(buffer)) {
		xmlIniData.Destroy();
		return false;
	}

	delete[] buffer;
	mzf.Close();


	CCXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++) {

		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MTOK_NPCSET)) {
			ParseNPCSet(chrElement);
		}
	}

	xmlIniData.Destroy();

	return true;
}

void CCQuestNPCSetCatalogue::ParseNPCSet(CCXmlElement& element)
{
	char szTemp[256]="";
	int n = 0;
	char szAttrValue[256];
	char szAttrName[64];
	char szTagName[128];

	CCQuestNPCSetInfo* pNPCSetInfo = new CCQuestNPCSetInfo();

	// NPCSET 태그 속성값 --------------------
	int nAttrCount = element.GetAttributeCount();
	for (int i = 0; i < nAttrCount; i++)
	{
		element.GetAttribute(i, szAttrName, szAttrValue);
		if (!stricmp(szAttrName, MTOK_NPCSET_ATTR_ID))
		{
			pNPCSetInfo->nID = CCQUEST_NPC(atoi(szAttrValue));
		}
		else if (!stricmp(szAttrName, MTOK_NPCSET_ATTR_NAME))
		{
			strcpy(pNPCSetInfo->szName, szAttrValue);
		}
		else if (!stricmp(szAttrName, MTOK_NPCSET_ATTR_BASENPC))
		{
			pNPCSetInfo->nBaseNPC = CCQUEST_NPC(atoi(szAttrValue));
		}
	}


	int iChildCount = element.GetChildNodeCount();
	CCXmlElement chrElement;
	for (int i = 0; i < iChildCount; i++)
	{
		chrElement = element.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		// ADDNPC 태그 --------------------
		if (!stricmp(szTagName, MTOK_NPCSET_TAG_ADDNPC))
		{
			CCNPCSetNPC add_npc;

			int nAttrCount = chrElement.GetAttributeCount();
			for (int i = 0; i < nAttrCount; i++)
			{
				chrElement.GetAttribute(i, szAttrName, szAttrValue);
				if (!stricmp(szAttrName, MTOK_NPCSET_ATTR_NPC_ID))
				{
					add_npc.nNPC = CCQUEST_NPC(atoi(szAttrValue));
				}
				else if (!stricmp(szAttrName, MTOK_NPCSET_ATTR_MIN_RATE))
				{
					add_npc.nMinRate = atoi(szAttrValue);
					//add_npc.fMinRate = ((float)atoi(szAttrValue) / 100.0f);
				}
				else if (!stricmp(szAttrName, MTOK_NPCSET_ATTR_MAX_RATE))
				{
					add_npc.nMaxRate = atoi(szAttrValue);
					//add_npc.fMaxRate = ((float)atoi(szAttrValue) / 100.0f);
				}
				else if (!stricmp(szAttrName, MTOK_NPCSET_ATTR_MIN_RATE))
				{
					add_npc.nMaxSpawnCount = atoi(szAttrValue);
				}
			}

			pNPCSetInfo->vecNPCs.push_back(add_npc);
		}
	}

	Insert(pNPCSetInfo);
}
