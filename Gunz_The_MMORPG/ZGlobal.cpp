#include "stdafx.h"
#include "ZGlobal.h"
#include "ZApplication.h"
#include "ZQuest.h"

// Added R347a
#include "ZGameInterface.h"

//#include "SVNRevision/SVNRevision.cpp"

bool ZIsLaunchDevelop() { 
	return ZApplication::GetInstance()->IsLaunchDevelop(); 
}
bool ZIsLaunchTest() { 
	return ZApplication::GetInstance()->IsLaunchTest(); 
}

RMeshMgr* ZGetNpcMeshMgr() { 
	return ZApplication::GetNpcMeshMgr(); 
}

RMeshMgr* ZGetMeshMgr() { 
	return ZApplication::GetMeshMgr(); 
}

RMeshMgr* ZGetWeaponMeshMgr() { 
	return ZApplication::GetWeaponMeshMgr(); 
}

RAniEventMgr* ZGetAniEventMgr()
{
	return ZApplication::GetAniEventMgr();
}

ZSoundEngine* ZGetSoundEngine() { 
	return ZApplication::GetSoundEngine(); 
}

ZEffectManager*	ZGetEffectManager() { 
//	return &g_pGame->m_EffectManager; 
	return ZGetGameInterface()->GetEffectManager(); 
}

ZScreenEffectManager* ZGetScreenEffectManager() { 
	return ZGetGameInterface()->GetScreenEffectManager(); 
}

int ZGetSVNRevision()
{
	return 0;
	//return GlobalGetSVNRevision();
}
