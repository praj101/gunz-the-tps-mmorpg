#pragma once

#include "CCVector3.h"
#include "CCBaseQuest.h"
#include "CCUID.h"
#include "CCQuestDropTable.h"
#include <map>
using namespace std;

class CCMatchStage;
struct CCQuestPlayerInfo;

/// NPC 오브젝트의 플래그 - 아직까진 별다른데 사용하지 않는다.
enum NPCOBJECT_FLAG
{
	NOF_NONE			= 0,

};


/// NPC 오브젝트
class CCMatchNPCObject
{
private:
	CCUID				m_UID;						///< ID
	CCQUEST_NPC			m_nType;					///< NPC타입
	CCUID				m_uidController;			///< 조종자
	CCVector3			m_Pos;						///< 위치
	unsigned long int	m_nFlags;					///< 플래그 모음
	CCQuestDropItem		m_DropItem;					///< 가지고 있는 아이템 - 없을 경우 nDropItemType가 QDIT_NA이다.

public:
	/// 생성자
	/// @param uid		NPC UID
	/// @param nType	NPC 종류
	/// @param nFlags	플래스
	CCMatchNPCObject(CCUID& uid, CCQUEST_NPC nType, unsigned long int nFlags=0);
	/// 소멸자
	~CCMatchNPCObject() { }
	/// NPC 조종을 플레이어에게 할당한다.
	/// @param uidPlayer	할당할 플레이어 UID
	void AssignControl(CCUID& uidPlayer);		
	/// NPC 조종자 해제
	void ReleaseControl();
	/// Drop할 아이템을 설정한다.
	/// @param pDropItem	아이템 정보
	void SetDropItem(CCQuestDropItem* pDropItem);

	// gets
	CCUID GetUID()					{ return m_UID; }				///< NPC UID 반환
	CCQUEST_NPC	GetType()			{ return m_nType; }				///< NPC 종류 반환
	CCUID& GetController()			{ return m_uidController; }		///< NPC 조종자(플레이어) UID 반환
	CCQuestDropItem* GetDropItem()	{ return &m_DropItem; }			///< 드롭 아이템 정보 반환

	inline void SetFlag(unsigned int nFlag, bool bValue);			///< 플래그 설정
	inline bool CheckFlag(unsigned int nFlag);						///< 플래그 체크
	inline void SetFlags(unsigned int nFlags);						///< 플래그 설정
	inline unsigned long GetFlags();								///< 플래그 반환
	inline bool HasDropItem();										///< 드롭 아이템을 가지고 있는지 반환

};

typedef map<CCUID, CCMatchNPCObject*>		CCMatchNPCObjectMap;


class CCQuestPlayerManager;

/// NPC 오브젝트 관리자
class CCMatchNPCManager
{
private:
	// var
	CCMatchStage*					m_pStage;
	CCQuestPlayerManager*			m_pPlayerManager;
	CCMatchNPCObjectMap				m_NPCObjectMap;

	unsigned long int				m_nLastSpawnTime;		// for test

	CCUID							m_uidKeyNPC;				// keyNPC
	int								m_nNPCCount[MNST_END];		// 스폰타입별 살아있는 NPC수
	int								m_nBossCount;				// 살아있는 보스 수
	bool							m_bBossDie;					// 보스가 모두 죽었는지 확인
	bool							m_bKeyNPCDie;				// keyNPC가 죽었는지 확인

	// func
	CCUID NewUID();
	bool AssignControl(CCUID& uidNPC, CCUID& uidPlayer);
	bool Spawn(CCUID& uidNPC, CCUID& uidController, unsigned char nSpawnPositionIndex);
	void Clear();
	bool FindSuitableController(CCUID& out, CCQuestPlayerInfo* pSender);

	void SetNPCObjectToControllerInfo(CCUID& uidChar, CCMatchNPCObject* pNPCObject);
	void DelNPCObjectToControllerInfo(CCUID& uidChar, CCMatchNPCObject* pNPCObject);
public:
	/// 생성자
	CCMatchNPCManager();
	/// 소멸자
	~CCMatchNPCManager();
	/// 초기화
	/// @param pStage				스테이지 클래스
	/// @param pPlayerManager		퀘스트룰에서의 PlayerManager
	void Create(CCMatchStage* pStage, CCQuestPlayerManager* pPlayerManager);
	/// 해제
	void Destroy();
	/// 모든 NPC를 없앤다.
	void ClearNPC();
	/// NPC 오브젝트 생성
	/// @param nType					NPC 종류
	/// @param nSpawnPositionIndex		스폰 위치
	/// @param bKeyNPC					이 NPC가 keyNPC인가
	CCMatchNPCObject* CreateNPCObject(CCQUEST_NPC nType, unsigned char nSpawnPositionIndex, bool bKeyNPC);
	/// NPC 오브젝트 해제
	/// @param uidNPC					NPC UID
	/// @param outItem					NPC가 드롭하는 아이템 반환값
	bool DestroyNPCObject(CCUID& uidNPC, CCQuestDropItem& outItem);
	/// NPC 오브젝트 반환
	/// @param uidNPC					NPC UID
	CCMatchNPCObject* GetNPCObject(CCUID& uidNPC);
	/// 플레이어가 스테이지에서 나갈때 호출된다.
	/// @param uidPlayer				플레이어 UID
	void OnDelPlayer(const CCUID& uidPlayer);
	/// 해당 플레이어가 해당 NPC를 조종하고 있는지 체크
	/// @param uidChar					플레이어 UID
	/// @param uidNPC					NPC UID
	bool IsControllersNPC(CCUID& uidChar, CCUID& uidNPC);
	/// NPC 오브젝트수 반환
	int GetNPCObjectCount();
	/// 해당 스폰타입의 NPC 오브젝트수 반환
	/// @param nSpawnType				NPC 스폰 타입
	int GetNPCObjectCount(CCQuestNPCSpawnType nSpawnType);
	int GetBossCount() { return m_nBossCount; }
	/// 보스가 죽었는지 확인
	bool IsBossDie()		{ return m_bBossDie;	}
	/// keyNPC가 죽었는지 확인(현재 서바이벌 전용..)
	bool IsKeyNPCDie()		{ return m_bKeyNPCDie; }
	/// keyNPC관련 변수들 초기화
	void ClearKeyNPCState();

	void RemovePlayerControl(const CCUID& uidPlayer);
};



// inlines //////////////////////////////////////////////////////////////////////////////////
inline void CCMatchNPCObject::SetFlags(unsigned int nFlags)
{
	if (m_nFlags != nFlags)
	{
		m_nFlags = nFlags;
	}
}

inline void CCMatchNPCObject::SetFlag(unsigned int nFlag, bool bValue)
{
	if (bValue) m_nFlags |= nFlag;
	else m_nFlags &= ~nFlag;
}

inline bool CCMatchNPCObject::CheckFlag(unsigned int nFlag)
{
	return ((m_nFlags & nFlag) != 0);
}

inline unsigned long CCMatchNPCObject::GetFlags() 
{ 
	return m_nFlags; 
}

inline int CCMatchNPCManager::GetNPCObjectCount()
{
	return (int)m_NPCObjectMap.size();
}

inline bool CCMatchNPCObject::HasDropItem()
{
	return (m_DropItem.nDropItemType != QDIT_NA);
}


inline int CCMatchNPCManager::GetNPCObjectCount(CCQuestNPCSpawnType nSpawnType)
{
	return m_nNPCCount[nSpawnType];
}
