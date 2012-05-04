#ifndef ZTOOLTIP_H
#define ZTOOLTIP_H

#include "CCWidget.h"
#include "CCToolTip.h"

#define MAX_TOOLTIP_LINE_STRING 40

class ZToolTip : public MToolTip
{
public:
	ZToolTip(const char* szName, CCWidget* pParent, CCAlignmentMode align=CCAM_LEFT|MAM_TOP);
	~ZToolTip();
	virtual void OnDraw(CCDrawContext* pDC);
	virtual void SetBounds(void);

private:
	void GetPosAlignedWithParent(int& x, int& y, int nTextPixelWidth, int nTextPixelHeight);

	//	MTextArea* m_pTextArea;
	CCBitmap* m_pBitmap1;
	CCBitmap* m_pBitmap2;

	CCAlignmentMode m_alignMode;		// 부모 기준 툴팁 위치
};

#endif//ZTOOLTIP_H
