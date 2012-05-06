#ifndef _ZCANVAS_H
#define _ZCANVAS_H


#include "CCWidget.h"


typedef void (*ZC_ONDRAW)(void* pSelf, CCDrawContext* pDC);

class ZCanvas : public CCWidget
{
private:
protected:
	ZC_ONDRAW			m_pOnDrawFunc;
	virtual void OnDraw(CCDrawContext* pDC);
public:
	ZCanvas(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZCanvas();
	void SetOnDrawCallback(ZC_ONDRAW pCallback) { m_pOnDrawFunc = pCallback; }

	#define CORE_CANVAS			"Canvas"
	virtual const char* GetClassName(){ return CORE_CANVAS; }
};






#endif