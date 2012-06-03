#ifndef CCBitmapDrawer_H
#define CCBitmapDrawer_H

#include "CCTypes.h"


#define FRAME_OUTLINE_WIDTH	1
#define FRAME_WIDTH		5
#define FRAME_INLINE_WIDTH 1

class CCDrawContext;
class CCBitmap;

void HLineBitmap(CCDrawContext* pDC, int x, int y, int w, CCBitmap* pBm, bool bStretch, float fScale = 1.f);

void DrawBitmapFrame2(CCDrawContext* pDC, sRect& r, sRect& cliprect,CCBitmap* Bms[9]);
void DrawBitmapFrame9(CCDrawContext* pDC, sRect& r, CCBitmap* Bms[9],bool bStretch = true, float fScale = 1.f);

void DrawBitmapFrameH3(CCDrawContext* pDC, sRect& r, CCBitmap* Bms[3]);	// Draw Horizontal 3 Bitmaps
void DrawBitmapFrameV3(CCDrawContext* pDC, sRect& r, CCBitmap* Bms[3]);	// Draw Vertical 3 Bitmaps
void DrawBitmapFrameCustom1(CCDrawContext* pDC, sRect& r, CCBitmap* Bms[9], bool bStretch = true );
void DrawBitmapFrameCustom2(CCDrawContext* pDC, sRect& r, CCBitmap* Bms[9], sColor bgColor, bool bStretch = true );
void DrawBitmapButtonCustom1( CCDrawContext* pDC, sRect& r, CCBitmap* Bms[9], bool bDown = false, bool bStretch = true );

bool IsNull(CCBitmap** ps, int nCount);

// Safety Call
#define GETWIDTH(_Image)	((_Image==NULL)?0:_Image->GetWidth())
#define GETHEIGHT(_Image)	((_Image==NULL)?0:_Image->GetHeight())


#endif