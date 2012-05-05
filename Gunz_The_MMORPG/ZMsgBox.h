#pragma once

#include "CCMsgBox.h"

class ZMsgBox : public CCMsgBox{
protected:
	bool				m_bExclusive;
	unsigned long int	m_nShowTime;
	bool				m_bNextVisible;
	CCListener*			m_pCustomListener;

	virtual void OnDraw(CCDrawContext* pDC);

public:
	ZMsgBox(const char* szMessage, CCWidget* pParent, CCListener* pListener=NULL, CCMsgBoxType nType=MT_NOTDECIDED );
	virtual ~ZMsgBox(void);

	void SetCustomListener(CCListener* pListener) { m_pCustomListener = pListener; }
	CCListener* GetCustomListener() { return m_pCustomListener; }

	void Show(bool bVisible, bool bModal=false);
};


CCListener* ZGetMsgBoxListener(void);
CCListener* ZGetConfirmMsgBoxListener(void);