#ifndef _ZSTAGEINFOBOX_H
#define _ZSTAGEINFOBOX_H

#include "CCListBox.h"
#include "CCBListBoxLook.h"

class ZStageInfoBox;

class ZStageInfoBoxLook : public CCListBoxLook {
public:
	virtual void OnDraw(ZStageInfoBox* pBox, CCDrawContext* pDC);
};

class ZStageInfoBox : public CCListBox{
//	DECLARE_LOOK(ZStageInfoBoxLook)
	CCBListBoxLook *m_pLook;
public:
	ZStageInfoBox(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZStageInfoBox();

	virtual void OnDraw(CCDrawContext* pDC);
	void SetLook(CCBListBoxLook *pLook) { m_pLook=pLook; }
};


#endif