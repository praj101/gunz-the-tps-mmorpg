#ifndef _ZWEAPONSCREENEFFECT_H
#define _ZWEAPONSCREENEFFECT_H

#include "ZPrerequisites.h"

class ZScreenEffect;

class ZWeaponScreenEffect
{
private:
	CCMatchWeaponType		m_nWeaponType;
	CCBitmap*				m_pSniferPanel;
	//ZScreenEffect*			m_pSniferPanelEffect;


	void InitWeapon(CCMatchWeaponType nWeaponType);
	void FinalWeapon(CCMatchWeaponType nWeaponType);
public:
	ZWeaponScreenEffect();
	virtual ~ZWeaponScreenEffect();
public:
	void Draw(CCDrawContext* pDC);
	void OnGadget(CCMatchWeaponType nWeaponType);
	void OnGadgetOff();
	void Create();
	void Destroy();
};




#endif
