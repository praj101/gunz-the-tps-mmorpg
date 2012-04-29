#pragma once

#include "CCEdit.h"

#define CCHOTKEY	"HotKey"

/// HotKey
class CCHotKey : public CCEdit{
protected:
	DECLARE_LOOK(CCEditLook)
	DECLARE_LOOK_CLIENT()

protected:
	unsigned int	m_iKey;
	bool			m_bCtrl;
	bool			m_bAlt;
	bool			m_bShift;

protected:
	virtual void OnRun(void);
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);

public:
	CCHotKey(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);

	virtual const char* GetClassName(void){ return CCHOTKEY; }

	void GetHotKeyName(char* szHotKeyName);
	void GetHotKey(unsigned int* pKey, bool* pCtrl, bool* pAlt, bool* pShift);

	int RegisterHotKey(void);
	void UnregisterHotKey(int nID);
};