#include "CCObjectTypes.h"

const unsigned long int	g_nHPRegenTickInteval = 1000;		// (1 sec)
const unsigned long int g_nENRegenTickInteval = 1000;		// (1 sec)


int CCGetHP_LCM(int nLevel, CCCharacterClass nClass);
float CCGetEN_LCM(int nLevel, CCCharacterClass nClass);

int CCGetChangingModeTickCount(CCCharacterMode mode);		///< 캐릭터 모드 진입시간(msec단위)

int CCCalculateMaxHP(int nLevel, CCCharacterClass nClass, int nCON, int nDCS);	///< 최대 HP 계산
int CCCalculateMaxEN(int nLevel, CCCharacterClass nClass, int nINT, int nDMS);	///< 최대 EN 계산
float CCCalculateRegenHPPerTick(int nLevel, CCCharacterRace nRace, int nDCS);		///< 초당 리젠 HP 계산
float CCCalculateRegenENPerTick(int nLevel, CCCharacterClass nClass, int nDMS);	///< 초당 리젠 EN 계산
float CCCalculateAtkDamage();	///< Attack Damage 계산
