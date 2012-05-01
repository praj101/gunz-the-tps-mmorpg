#pragma once

#include "CCQuestConst.h"

/// 섹터 링크 정보
struct CCQuestSectorLink
{
	char			szName[64];					///< 이름
	vector<int>		vecTargetSectors;			///< 연결될 수 있는 다음 섹터
	CCQuestSectorLink() 
	{
		szName[0] = 0;
	}
};


/// 섹터 역링크 정보
struct CCQuestSectorBacklink
{
	int nSectorID;
	int nLinkIndex;
};


/// 퀘스트 맵 섹터 정보
struct CCQuestMapSectorInfo
{
	int								nID;								///< ID
	char							szTitle[64];						///< 섹터 이름
	bool							bBoss;								///< 보스방 여부
	int								nLinkCount;							///< 링크수
	CCQuestSectorLink				Links[MAX_SECTOR_LINK];				///< 링크 정보
	vector<CCQuestSectorBacklink>	VecBacklinks;						///< 역링크들
	int								nSpawnPointCount[MNST_END];			///< 스폰포인트 개수

	CCQuestMapSectorInfo()
	{
		nID = -1;
		szTitle[0] = 0;
		nLinkCount = 0;
		bBoss = false;
		memset(nSpawnPointCount, 0, sizeof(nSpawnPointCount));
	}
};


/// 퀘스트 맵 세트 정보
struct CCQuestMapsetInfo
{
	int				nID;									///< ID
	char			szTitle[64];							///< 세트 이름
	int				nLinkCount;								///< 링크수
	vector<int>		vecSectors;								///< 가지고 있는 섹터

	CCQuestMapsetInfo()
	{
		nID = -1;
		szTitle[0] = 0;
		nLinkCount = 0;
	}
};

typedef	std::map<int, CCQuestMapsetInfo*>		CCQuestMapsetMap;
typedef	std::map<int, CCQuestMapSectorInfo*>		CCQuestMapSectorMap;

/// 퀘스트 맵 관리자
class CCQuestMapCatalogue
{
private:
	// 멤버 변수
	CCQuestMapsetMap			m_MapsetInfo;
	CCQuestMapSectorMap		m_SectorInfo;

	// 함수
	void InsertMapset(CCQuestMapsetInfo* pMapset);
	void InsertSector(CCQuestMapSectorInfo* pSector);
	void ParseMapset(CCXmlElement& element);
	void ParseMapsetSector1Pass(CCXmlElement& elementMapset, CCQuestMapsetInfo* pMapset);
	void ParseSector(CCXmlElement& element, CCQuestMapSectorInfo* pSector);
	void InitBackLinks();
public:
	CCQuestMapCatalogue();													///< 생성자
	~CCQuestMapCatalogue();													///< 소멸자

	void Clear();															///< 초기화
	bool ReadXml(const char* szFileName);									///< xml에서 맵정보를 읽는다. (서버용)
	bool ReadXml(CCZFileSystem* pFileSystem,const char* szFileName);			///< xml에서 맵정보를 읽는다. (클라이언트용)
	void DebugReport();														///< 맵정보가 제대로 구성되었는지 확인한다.

	CCQuestMapSectorInfo*	GetSectorInfo(int nSector);						///< 섹터 정보 반환
	CCQuestMapsetInfo*		GetMapsetInfo(int nMapset);						///< 맵셋 정보 반환
	CCQuestMapSectorInfo*	GetSectorInfoFromName(char* szSectorTitle);		///< 섹터 이름으로 섹터 정보 반환

	inline CCQuestMapsetMap*		GetMapsetMap();

	bool IsHacked();											///< 임시해킹방지용..
};


inline CCQuestMapsetMap* CCQuestMapCatalogue::GetMapsetMap()
{
	return &m_MapsetInfo;
}
