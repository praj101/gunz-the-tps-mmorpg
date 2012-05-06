#ifndef ZCHARACTERVIEWLIST_H
#define ZCHARACTERVIEWLIST_H

#include "ZPrerequisites.h"
#include "ZMeshViewList.h"
#include "CCUID.h"
#include "ZGameInterface.h"

class ZCharacterView;
class CCMatchObjCacheMap;
class CCMatchObjCache;

class ZCharacterViewList : public ZMeshViewList{
protected:
	virtual bool OnShow();
	virtual void OnHide();
protected:
	CCMatchCharItemParts m_nVisualWeaponParts;
public:
	ZCharacterViewList(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZCharacterViewList();

	//void Add(const char* szName);
	void Add(const CCUID& uid, const char* szName, CCMatchSex nSex, unsigned int nHair, unsigned int nFace, 
		unsigned int nLevel, unsigned long int* itemids, bool bFireWall = false);
	void Add(CCMatchObjCache* pCache);

	void Remove(const CCUID& uid);

	ZCharacterView* Get(const CCUID& uid);

	void Assign(CCMatchObjCacheMap* pObjCacheMap);

	void OnDraw(CCDrawContext* pDC);

	void SetSelectCharacter(ZCharacterView* pSelectView);

	void ChangeMyCharacterInfo();
	void ChangeCharacterInfo();

	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage);

	CCLabel*	GetLobbyCharNameViewer();
	CCLabel*	GetStageCharNameViewer();

	ZCharacterView* GetLobbyCharViewer();
	ZCharacterView* GetStageCharViewer();
	ZCharacterView* GetLobbyCharEqViewer();
	ZCharacterView* GetLobbyCharShopViewer();

	void ChangeLobbyCharacterView(ZCharacterView* pSelectView);
	void ChangeStageCharacterView(ZCharacterView* pSelectView);

	void RefreshCharInfo();

//	void UpdateSelectView();
	ZCharacterView* FindSelectView(CCUID uid);

public:

	CCLabel* m_pLobbyCharNameViewer;
	CCLabel* m_pStageCharNameViewer;

	ZCharacterView* m_pLobbyCharViewer;
	ZCharacterView* m_pStageCharViewer;
	ZCharacterView* m_pLobbyCharEqViewer;
	ZCharacterView* m_pLobbyCharShopViewer;


private:
//	ZCharacterView* m_SelectView;
//	ZCharacterView* m_MySelectView;
	CCUID			m_MyUid;
	CCUID			m_SelectViewUID;

public:
	void ChangeVisualWeaponParts(CCMatchCharItemParts nVisualWeaponParts);
};

ZCharacterViewList* ZGetCharacterViewList(GunzState nState);

#endif