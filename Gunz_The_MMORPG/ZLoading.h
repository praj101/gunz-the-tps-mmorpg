#pragma once
#include "ZInterface.h"
#include "CCPicture.h"

enum	eLoaindgState
{
	LOADING_BEGIN,
	LOADING_ANIMATION,
	LOADING_BSP,
	LOADING_END,
	NUM_LOADING_STATE,
};

class ZLoading : public ZInterface
{
protected:
	CCPicture*		m_pBackGround;
	CCBitmapR2*		m_pBitmap;

	int				m_iPercentage;

public:
	ZLoading(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZLoading();
	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual void OnDraw(CCDrawContext* pDC);
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);

	void	Progress( eLoaindgState state );	
	int		GetProgress() const;
};
