#ifndef _MAIN_H
#define _MAIN_H

//#pragma once

#pragma warning(disable:4099)
#ifdef _DEBUG
//#pragma comment(lib,"cmld.lib")
//#pragma comment(lib,"core2d.lib")
#pragma comment(lib,"RealSound.lib")
#pragma comment(lib,"fmodvc.lib")
//#pragma comment(lib,"RealSpace2d.lib")
//#pragma comment(lib,"CSCommonDE.lib")
//#pragma comment(lib,"SafeUDP.lib")
#pragma comment(lib,"dxerr9.lib")
#pragma comment(lib,"zlib.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "winmm.lib")
//
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib") // if i remember correct 
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dinput.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxerr8.lib")
#pragma comment(lib, "dxerr.lib")

#else
//#pragma comment(lib,"cml.lib")
//#pragma comment(lib,"core2.lib")
#pragma comment(lib,"RealSound.lib")
//#pragma comment(lib,"RealSpace2.lib")
//#pragma comment(lib,"CSCommonE.lib")
//#pragma comment(lib,"SafeUDP.lib")
#pragma comment(lib,"fmodvc.lib")
#pragma comment(lib,"dxerr9.lib")
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib") // if i remember correct 
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dinput.lib")
#pragma comment(lib, "dxerr8.lib")
#pragma comment(lib, "dxerr.lib")

#endif


#pragma comment(lib,"winmm.lib")



#endif