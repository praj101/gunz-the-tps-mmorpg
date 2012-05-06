#pragma once

#include "CCScrollBar.h"
#include "CCLookNFeel.h"

class CCSliderThumb;
class CCBSliderThumbLook;

#define CCSLIDER	"Slider"

class CCSliderThumbLook{
protected:
public:
	virtual void OnDraw(CCSliderThumb* pThumb, CCDrawContext* pDC);
	virtual sRect GetClientRect(CCSliderThumb* pThumb, sRect& r);
	virtual sSize GetDefaultSize(CCSliderThumb* pThumb);
public:
	CCSliderThumbLook(){};
};

class CCSliderThumb : public CCThumb{
	DECLARE_LOOK(CCSliderThumbLook)
	DECLARE_LOOK_CLIENT()
public:
	CCSliderThumb(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual sSize GetDefaultSize();
};

class CCSlider : public CCScrollBar{
	DECLARE_LOOK(CCScrollBarLook)
	DECLARE_LOOK_CLIENT()
protected:
	virtual int GetThumbSize();
	void Initialize();
public:
	CCSlider(const char* szName, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	CCSlider(CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCSlider();

	virtual const char* GetClassName(){ return CCSLIDER; }
};