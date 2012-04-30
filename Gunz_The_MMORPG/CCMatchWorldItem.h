#pragma once

#include <vector>
#include <list>
#include <map>
using namespace std;

class CCMatchStage;
class CCMatchObject;
class CCZFileSystem;


#define WORLDITEM_EXTRAVALUE_NUM		2
#define WORLDITEM_MAX_NUM				30		// 플로딩 핵에대한 버그로 만들어짐...(메디킷,수리킷 월드맵에 갯수제한)

// 맵에 생성된 아이템
struct MMatchWorldItem
{
	unsigned short		nUID;
	unsigned short		nItemID;
	short				nStaticSpawnIndex;
	float				x;
	float				y;
	float				z;
	int					nLifeTime;			// 아이템 활성 시간( -1이면 무한 )

	union {
		struct {
		    int			nDropItemID;		// 만약 퀘스트일 경우 QuestItem 또는 일반 아이템의 ID
			int			nRentPeriodHour;	// 만약 일반 아이템일 경우 Item ID
		} ;
		int				nExtraValue[WORLDITEM_EXTRAVALUE_NUM];
	};
};

struct UserDropWorldItem
{
	UserDropWorldItem( CCMatchObject* pObj, const int nItemID, const float x, const float y, const float z, unsigned long nDropDelayTime )
	{
		m_pObj			= pObj;
		m_nItemID		= nItemID;
		m_x				= x;
		m_y				= y;
		m_z				= z;
		m_nDropDelayTime = nDropDelayTime;
	}

	CCMatchObject*		m_pObj;
	int					m_nItemID;
	float				m_x;
	float				m_y;
	float				m_z;
	unsigned long		m_nDropDelayTime;
};


typedef map<unsigned short, MMatchWorldItem*> MMatchWorldItemMap;


// 맵의 스폰 정보
struct MMatchWorldItemSpawnInfo
{
	unsigned short		nItemID;
	unsigned long int	nCoolTime;
	unsigned long int	nElapsedTime;
	float x;
	float y;
	float z;
	bool				bExist;
	bool				bUsed;
};


class MMatchWorldItemManager
{
private:
	CCMatchStage*						m_pMatchStage;
	MMatchWorldItemMap					m_ItemMap;				// 맵에 존재하고 있는 아이템 리스트

	vector<MMatchWorldItemSpawnInfo>	m_SpawnInfos;			// 맵의 스폰 아이템 정보
	vector< UserDropWorldItem >			m_UserDropWorldItem;	// 유저가 던지 아이템 정보
	int									m_nSpawnItemCount;		// 스폰 아이템 정보 개수
	unsigned long int					m_nLastTime;

	short								m_nUIDGenerate;
	bool								m_bStarted;

	void AddItem(const unsigned short nItemID, short nSpawnIndex, 
				 const float x, const float y, const float z);
	void AddItem(const unsigned short nItemID, short nSpawnIndex, 
				 const float x, const float y, const float z, int nLifeTime, int* pnExtraValues );
	void DelItem(short nUID);
	void Spawn(int nSpawnIndex);
	void Clear();
	void SpawnInfoInit();
	void ClearItems();

	void RouteSpawnWorldItem(MMatchWorldItem* pWorldItem);
	void RouteObtainWorldItem(const CCUID& uidPlayer, int nWorldItemUID);
	void RouteRemoveWorldItem(int nWorldItemUID);
public:
	MMatchWorldItemManager();
	virtual ~MMatchWorldItemManager();

	// CCMatchStage에서 관리하는 함수
	bool Create(CCMatchStage* pMatchStage);
	void Destroy();

	void OnRoundBegin();
	void OnStageBegin(CCMatchStageSetting* pStageSetting);
	void OnStageEnd();
	void Update();

	bool Obtain(CCMatchObject* pObj, short nItemUID, int* poutItemID, int* poutExtraValues);
	void SpawnDynamicItem(CCMatchObject* pObj, const int nItemID, const float x, const float y, const float z, float fDropDelayTime);
	void SpawnDynamicItem(CCMatchObject* pObj, const int nItemID, const float x, const float y, const float z, 
						  int nLifeTime, int* pnExtraValues );
	void RouteAllItems(CCMatchObject* pObj);

};
