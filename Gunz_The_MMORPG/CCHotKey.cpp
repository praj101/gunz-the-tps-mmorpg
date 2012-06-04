#include "stdafx.h"
#include "CCHotKey.h"
#include "Core.h"

IMPLEMENT_LOOK(CCHotKey, CCEditLook)

void GetKeyName(char* szKeyName, int nKeyNameSize, UINT nVirtKey, bool bExtended)
{
	if(nVirtKey==VK_SHIFT || nVirtKey==VK_CONTROL || nVirtKey==VK_MENU){
		szKeyName[0] = 0;
		return;
	}

	LONG lScan = MapVirtualKey(nVirtKey, 0) << 16;

	// if it's an extended key, add the extended flag
	if (bExtended)
		lScan |= 0x01000000L;
	//lScan &= ~VK_SHIFT;
	//lScan &= ~VK_CONTROL;

	GetKeyNameText(lScan, szKeyName, nKeyNameSize);
}

/*
static const TCHAR szPlus[] = _T(" + ");
CString CHotKeyCtrl::GetHotKeyName() const
{
	ASSERT(::IsWindow(m_hWnd));

	CString strKeyName;
	WORD wCode;
	WORD wModifiers;

	GetHotKey(wCode, wModifiers);
	if (wCode != 0 || wModifiers != 0)
	{
		if (wModifiers & HOTKEYF_CONTROL)
		{
			strKeyName += GetKeyName(VK_CONTROL, FALSE);
			strKeyName += szPlus;
		}

		if (wModifiers & HOTKEYF_SHIFT)
		{
			strKeyName += GetKeyName(VK_SHIFT, FALSE);
			strKeyName += szPlus;
		}

		if (wModifiers & HOTKEYF_ALT)
		{
			strKeyName += GetKeyName(VK_MENU, FALSE);
			strKeyName += szPlus;
		}

		strKeyName += GetKeyName(wCode, wModifiers & HOTKEYF_EXT);
	}

	return strKeyName;
}
*/


void CCHotKey::OnRun(void)
{
	if(IsFocus()==false) return;

	char szHotKeyName[128] = "";
	GetHotKeyName(szHotKeyName);
	SetText(szHotKeyName);
}

bool CCHotKey::OnEvent(CCEvent* pEvent, CCListener* pListener)
{
	switch(pEvent->iMessage){
	case CCWM_KEYDOWN:
		m_uKey = pEvent->uKey;
		if(pEvent->uKey==VK_SHIFT || pEvent->uKey==VK_CONTROL || pEvent->uKey==VK_MENU) m_uKey = -1;
		m_bCtrl = CCEvent::GetCtrlState();
		m_bAlt = CCEvent::GetAltState();
		m_bShift = CCEvent::GetShiftState();
		return true;
	case CCWM_KEYUP:
		if(pEvent->uKey==VK_SHIFT && m_uKey==-1) m_bShift = false;
		if(pEvent->uKey==VK_CONTROL && m_uKey==-1) m_bCtrl = false;
		if(pEvent->uKey==VK_MENU && m_uKey==-1) m_bAlt = false;
		return true;
	case CCWM_CHAR:
		return true;
	}
	return false;
}

CCHotKey::CCHotKey(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCEdit(szName, pParent, pListener)
{
	LOOK_IN_CONSTRUCTOR();

	m_bCtrl = false;
	m_bAlt = false;
	m_bShift = false;
	m_uKey = -1;
}

void CCHotKey::GetHotKeyName(char* szHotKeyName)
{
	if(m_bCtrl==true){
		strcat(szHotKeyName, "Ctrl");
	}
	if(m_bAlt==true){
		if(szHotKeyName[0]!=0) strcat(szHotKeyName, "+");
		strcat(szHotKeyName, "Alt");
	}
	if(m_bShift==true){
		if(szHotKeyName[0]!=0) strcat(szHotKeyName, "+");
		strcat(szHotKeyName, "Shift");
	}

	if(m_uKey>0){
		char szKey[128];
		GetKeyName(szKey, 128, m_uKey, false);
		if(szKey[0]!=0){
			if(szHotKeyName[0]!=0) strcat(szHotKeyName, "+");
			strcat(szHotKeyName, szKey);
		}
	}
}

void CCHotKey::GetHotKey(unsigned int* pKey, bool* pCtrl, bool* pAlt, bool* pShift)
{
	if(pKey!=NULL) *pKey = m_uKey;
	if(pCtrl!=NULL) *pCtrl = m_bCtrl;
	if(pAlt!=NULL) *pAlt = m_bAlt;
	if(pShift!=NULL) *pShift = m_bShift;
}

int CCHotKey::RegisterHotKey(void)
{
	return Core::GetInstance()->RegisterHotKey((m_bCtrl?CCMODIFIER_CTRL:0)|(m_bAlt?CCMODIFIER_ALT:0)|(m_bShift?CCMODIFIER_SHIFT:0), m_uKey);
}

void CCHotKey::UnregisterHotKey(int nID)
{
	Core::GetInstance()->UnregisterHotKey(nID);
}
