#include "stdafx.h"
#include "Core4Gunz.h"
#include "ZDirectInput.h"
//#include "ZGame.h"

extern ZDirectInput	g_DInput;

const char* Core4Gunz::GetActionKeyName(unsigned long int nKey){
	return g_DInput.GetKeyName(nKey);
}
