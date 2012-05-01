#ifndef _ZCHARACTERMANAGER_H
#define _ZCHARACTERMANAGER_H

#include "CCUID.h"
#include "RVisualMeshMgr.h"
#include "ZObject.h"
#include "ZCharacter.h"
#include "CCObjectTypes.h"

_USING_NAMESPACE_REALSPACE2

class ZCharacterManager : public map<CCUID, ZCharacter*>
{
private:
	int		m_nLiveCount;
	//void DrawMyCharacter();
public:
//	int		m_nOnDrawCnt;
//	int		m_nRenderedCnt;

	ZCharacterManager();
	virtual ~ZCharacterManager();
//	void Draw();
	

	void Add(ZCharacter *pCharacter);
	ZCharacter* Add(CCUID uid, rvector pos,bool bMyCharacter=false);

	void Delete(CCUID uid);
	void Clear();
//	void Update(float fElapsed);

//	ZCharacter* Pick(ZCharacter* pMyChar,rvector& pos,rvector& dir, RPickInfo* pInfo = NULL);
//	ZCharacter* Pick(int x,int y,RPickInfo* pInfo=NULL);
//	ZCharacter* Pick(rvector& pos,rvector& dir, RPickInfo* pInfo = NULL);
	ZCharacter* Find(CCUID uid);
//	ZCharacter* Pick( rvector& pos, float Radius );

	/// 해당 플레이어의 순서를 구한다. - 순차검색함. 플레이어가 없으면 -1 반환
	/// @param uid					찾는 플레이어 UID
	/// @param bIncludeHiddenChar	admin hidden 캐릭터 포함여부
	int GetCharacterIndex(CCUID& uid, bool bIncludeHiddenChar=true);		

	void OutputDebugString_CharacterState();

	int GetLiveCount();
	int GetCount() { return (int)size(); }
	ZCharacter* Get(int index);

	bool ToggleClothSimulation();

	void OnInvalidate();
	void OnRestore();

	void InitRound();
};




#endif 