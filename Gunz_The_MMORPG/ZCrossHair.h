#ifndef _ZCROSSHAIR_H
#define _ZCROSSHAIR_H


#include "ZIDLResource.h"
#include <windows.h>
#include <io.h>
//#include "ZGame.h"
#include "RealSpace2.h"
#include "RBaseTexture.h"
#include "Core.h"
#include "Core4R2.h"
#include "CCBitmap.h"
#include "ZFilePath.h"

#define NUM_DEFAULT_CROSSHAIR		5



#define FN_CROSSHAIR_HEADER			"crosshair"
#define FN_CROSSHAIR_TAILER			".png"
#define FN_CROSSHAIR_LEFT			"_l"
#define FN_CROSSHAIR_TOP			"_t"
#define FN_CROSSHAIR_BOTTOM			"_b"
#define FN_CROSSHAIR_RIGHT			"_r"
#define FN_CROSSHAIR_PICK			"_pick"

enum ZCROSSHAIR_STATUS 
{
	ZCS_NORMAL		= 0,
	ZCS_PICKENEMY
};

enum ZCrossHairPreset
{
	ZCSP_INDEX1		= 0,		// 조준점1
	ZCSP_INDEX2		= 1,		// 조준점2
	ZCSP_INDEX3		= 2,		// 조준점3
	ZCSP_INDEX4		= 3,		// 조준점4
	ZCSP_INDEX5		= 4,		// 조준점5
	ZCSP_CUSTOM		= 5			// 사용자 지정
};

class ZCrossHair
{
private:
	enum _DIR
	{
		CH_CENTER	= 0,
		CH_TOP		= 1,
		CH_RIGHT	= 2,
		CH_BOTTOM	= 3,
		CH_LEFT		= 4,
		CH_MAX		= 5
	};

	ZCROSSHAIR_STATUS		m_nStatus;
	bool					m_bVisible;
	CCBitmap*				m_pBitmaps[CH_MAX];
	CCBitmap*				m_pPickBitmaps[CH_MAX];
	void Change(ZCrossHairPreset nPreset);
	static void GetBitmaps(CCBitmap** ppoutBitmap, CCBitmap** ppoutPickBitmap, ZCrossHairPreset nPreset);
	static void DrawCrossHair(CCDrawContext* pDC, CCBitmap** ppBitmaps, sPoint& center, float fSizeFactor, float fCAFactor);
public:
	ZCrossHair();
	virtual ~ZCrossHair();
	void ChangeFromOption();
	bool Create();
	void Destroy();
	void Draw(CCDrawContext* pDC);
	void SetState(ZCROSSHAIR_STATUS nState)	{ m_nStatus = nState; }
	int GetHeight();
	int GetWidth();
	void Show(bool bVisible)
	{
		if (bVisible == m_bVisible) return;
		m_bVisible = bVisible;
	}

	static void OnDrawOptionCrossHairPreview(void* pCanvas, CCDrawContext *pDC);
};






#endif