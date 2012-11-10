#pragma once

//#include "winsock2.h"	// Done
#include "CCXml.h"
#include <windows.h>

#include "RNameSpace.h"
#include "CCUtil.h"
#include "RTypes.h"
#include "RealSpace2.h"
_USING_NAMESPACE_REALSPACE2

//#include "RBspObject.h"
//#include "RMeshMgr.h"
//#include "RVisualMeshMgr.h"
#include "RMaterialList.h"
#include "Core4R2.h"
#include "RParticleSystem.h"

#include "CCErrorTable.h"
#include "Config.h"

#include "ZGlobal.h"
#include "ZMessages.h"


#define __BP(i,n) CCBeginProfile(i,n);
#define __EP(i) CCEndProfile(i);
#define __SAVEPROFILE(i) CCSaveProfile(i);

