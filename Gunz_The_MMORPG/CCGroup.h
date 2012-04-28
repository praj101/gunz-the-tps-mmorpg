#pragma once

#include "CCWidget.h"
#include "CCLookNFeel.h"

#define CCGROUP	"Group"

class CCGroup;

class CCGroupLook{
public:
	virtual void OnDraw(CCGroup* pGroup, CCDrawContext* pDC);
	virtual sRect GetClientRect(CCGroup* pGroup, sRect& r);
};

class CCGroup : public CCWidget{
	DECLARE_LOOK(CCGroupLook)
	DECLARE_LOOK_CLIENT()

public:
	CCGroup(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCGroup();

	virtual const char* GetClassName(){ return CCGROUP; }
};
