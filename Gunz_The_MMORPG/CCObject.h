#pragma once
#include "CCUID.h"
#include "CCGridMap.h"
#include "CCObjectTypes.h"

using namespace std;
#include <algorithm>

class CCMap;
class CCObject;

typedef CCGridMap<CCObject*>	CCObjectGridMap;


/// 변수를 이용한 RTTI 구현. Class Declaration
#define DECLARE_RTTI()								public: static const char* _m_RTTI_szClassName; virtual const char* GetClassName(void){ return _m_RTTI_szClassName; }
/// 변수를 이용한 RTTI 구현. Class Implementation
#define IMPLEMENT_RTTI(_Class)						const char* _Class::_m_RTTI_szClassName = #_Class;
/// 같은 클래스타입인가?
#define ISEQUALCLASS(_Class, _InstancePointer)		(_Class::_m_RTTI_szClassName==(_InstancePointer)->GetClassName())	// 포인터 비교

//#define SAFECAST(_Class, _InstancePointer)			_InstancePointer-
/*
/// 이동에 관한 정보
struct MOBJECTPATH{
	unsigned long int	nTime;		///< 해당 시간
	rvector				Pos;		///< 해당 위치
	int					nPathNode;	///< Path Node ID
};
#define MOBJECTMOVE	MOBJECTPATH	// 임시, 나중에 이름을 다 바꿔준다.
*/
class CCObject;
enum OBJECTCACHESTATE {	OBJECTCACHESTATE_KEEP, OBJECTCACHESTATE_NEW, OBJECTCACHESTATE_EXPIRE };
class CCObjectCacheNode {
public:
	CCObject*			m_pObject;
	OBJECTCACHESTATE	m_CacheState;
};
class CCObjectCache : public list<CCObjectCacheNode*> {
	int		m_nUpdateCount;	// New, Expire만 카운트
public:
	int GetUpdateCount() { return m_nUpdateCount; }
	CCObjectCacheNode* FindCacheNode(CCObject* pObj);
	void Invalidate();
	void Update(CCObject* pObject);
	void RemoveExpired();
};


/// 서버내에 존재하는 모든 오브젝트의 Abstract Class
class CCObject{
protected:
	CCUID			m_UID;

	CCObjectType		m_ObjectType;		///< 우선 이것을 가지고 PC, NPC를 판별한다.
private:
	//rvector			m_Pos;		///< 위치
	//rvector				m_StartPos, m_EndPos, m_Pos;
	//unsigned long int	m_nStartTime, m_nEndTime;

//	vector<MOBJECTPATH>	m_Path;		///< 패스
//	rvector				m_Pos;		///< 현재 위치
//	int					m_nSpawnID;

//protected:
//	CCMap*					m_pMap;		///< 현재 속해있는 맵
//	CCObjectGridMap::HREF	m_hRef;		///< 맵에서의 위치 핸들


public:
	CCObjectCache	m_ObjectCache;
	list<CCUID>		m_CommListener;

	/*
protected:
	void UpdateObjectCache(void);
	*/

protected:
	/// 시간에 따른 위치 얻어내기
//	rvector GetPos(unsigned long int nTime);

public:
	CCObject();
//	CCObject(CCUID& uid, CCMap* pMap, rvector& Pos);	// Map Navication Object
	CCObject(const CCUID& uid);	// No Map Object
	virtual ~CCObject(void)	{};

	inline const CCUID GetUID(void) const { return m_UID; }

	/// 현재 위치 설정
//	void SetPos(rvector& Pos){ SetPos(0, 0, Pos, Pos); }
	/// 시간에 따른 위치 지정
//	void SetPos(unsigned long int nStartTime, unsigned long int nEndTime, rvector& StartPos, rvector& EndPos);
	/// 패스로 지정
//	void SetPath(MOBJECTPATH* p, int nCount);
	/// 현재 시간의 오브젝트 위치 얻기
//	const rvector GetPos(void){ return m_Pos; }
	/// SpawnID 얻기 (Spawn Object 아니면 0)
//	int GetSpawnID() { return m_nSpawnID; }
//	void SetSpawnID(int nSpawnID) { m_nSpawnID = nSpawnID; }

	void SetObjectType(CCObjectType type) { m_ObjectType = type; }
	CCObjectType GetObjectType()	{ return m_ObjectType; }

	virtual bool CheckDestroy(int nTime) { return false; }

	/// 틱 처리
	virtual void Tick(unsigned long int nTime)	{}

	/// 현재 맵 설정
//	void SetMap(CCMap* pMap){ m_pMap = pMap; }
	/// 현재 속해있는 맵 얻기
//	CCMap* GetMap(void){ return m_pMap; }
	

	void AddCommListener(CCUID ListenerUID);
	void RemoveCommListener(CCUID ListenerUID);
	bool IsCommListener(CCUID ListenerUID);
	bool HasCommListener() { if (m_CommListener.size() > 0) return true; else return false; }
	const CCUID GetCommListener() { 
		if (HasCommListener())
			return *m_CommListener.begin(); 
		else
			return CCUID(0,0);
	}

	void InvalidateObjectCache() { m_ObjectCache.Invalidate(); } 
	void UpdateObjectCache(CCObject* pObject) { m_ObjectCache.Update(pObject); }
	void ExpireObjectCache(CCObject* pObject) { 
		CCObjectCacheNode* pNode = m_ObjectCache.FindCacheNode(pObject);
		if (pNode == NULL) return;
		pNode->m_CacheState = OBJECTCACHESTATE_EXPIRE;
	}
	void RemoveObjectCacheExpired() { m_ObjectCache.RemoveExpired(); }
	int GetObjectCacheUpdateCount() { return m_ObjectCache.GetUpdateCount(); }

	DECLARE_RTTI()
};
