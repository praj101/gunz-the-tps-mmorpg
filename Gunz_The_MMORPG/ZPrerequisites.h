#pragma once

#include "winsock2.h"	// Done
#include "CCXml.h"
#include <windows.h>

#include "RNameSpace.h"
#include "CCUtil.h"
#include "RTypes.h"
#include "RealSpace2.h"
_USING_NAMESPACE_REALSPACE2

#include "RBspObject.h"
#include "RMeshMgr.h"
#include "RVisualMeshMgr.h"
#include "RMaterialList.h"
#include "RAnimationMgr.h"	// This one
#include "Core4R2.h"
#include "RParticleSystem.h"

#include "CCObject.h"
//#include "CCObjectCharacter.h"
#include "CCMatchObject.h"
#include "CCMatchStage.h"
#include "CCMatchItem.h"
#include "CCMatchMap.h"
#include "CCSafeUDP.h"
#include "CCGameClient.h"
#include "CCMatchTransDataType.h"
#include "CCErrorTable.h"
#include "Config.h"

#include "ZGlobal.h"
#include "ZMessages.h"

//#define _BIRDCAMERA


#ifndef _PUBLISH
	#define _FASTDEBUG
//	#define _BIRDTEST



#define __BP(i,n) CCBeginProfile(i,n);
#define __EP(i) CCEndProfile(i);
#define __SAVEPROFILE(i) CCSaveProfile(i);

#else
	#define _DO_NOT_USE_PROFILER
	#define __BP(i,n) ;
	#define __EP(i) ;
	#define __SAVEPROFILE(i) ;
#endif
