#include "stdafx.h"
#include "ZLoading.h"

#define LOAD_BEGIN_PERCENTAGE		0
#define LOAD_ANIMATION_PERCENTAGE	30
#define LOAD_BSP_PERCENTAGE			80
#define LOAD_END_PERCENTAGE			100

const int	ciPercentageList[NUM_LOADING_STATE] = 
{
	LOAD_BEGIN_PERCENTAGE,
	LOAD_ANIMATION_PERCENTAGE,
	LOAD_BSP_PERCENTAGE,
	LOAD_END_PERCENTAGE,
};


ZLoading::ZLoading(const char* szName, CCWidget* pParent, CCListener* pListener)
: ZInterface(szName, pParent, pListener)
{
	if (pParent)
		SetBounds(0, 0, pParent->GetRect().w, pParent->GetRect().h);

	m_pBackGround	= NULL;
	m_pBitmap		= NULL;
	m_iPercentage	= 0;
}

ZLoading::~ZLoading()
{
	OnDestroy();
}

bool ZLoading::OnCreate()
{
	m_pBackGround=new CCPicture("background",this,this);	// 직접그린다.

	m_pBitmap = new CCBitmapR2;
	m_pBitmap->Create("loading", RGetDevice(), "Interface/Default/Loading/loading_screen.tga");

	//*/
//	m_pBitmap=(CCBitmapR2*)CCBitmapManager::Get("loading01.png");

	
	m_pBackGround->SetBitmap(m_pBitmap);
	m_pBackGround->SetPosition(0, 0);
	m_pBackGround->SetBounds(0, 0, GetRect().w, GetRect().h);
	m_pBackGround->SetStretch(true);

	return true;
}
void ZLoading::OnDestroy()
{
	SAFE_DELETE(m_pBackGround);
	SAFE_DELETE(m_pBitmap);
}

void ZLoading::OnDraw(CCDrawContext* pDC)
{
}

bool ZLoading::OnEvent(CCEvent* pEvent, CCListener* pListener)
{
	return true;
}

void ZLoading::Progress( eLoaindgState state )
{
	m_iPercentage = ciPercentageList[state];
}

int	ZLoading::GetProgress() const
{
	return m_iPercentage;
}