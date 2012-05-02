#include "stdafx.h"

#include "ZCanvas.h"

void ZCanvas::OnDraw(CCDrawContext* pDC)
{
	if (m_pOnDrawFunc) 
	{
		m_pOnDrawFunc(this, pDC);
	}
}

ZCanvas::ZCanvas(const char* szName, CCWidget* pParent, CCListener* pListener) : CCWidget(szName, pParent, pListener)
{
	m_pOnDrawFunc = NULL;
}

ZCanvas::~ZCanvas()
{


}
