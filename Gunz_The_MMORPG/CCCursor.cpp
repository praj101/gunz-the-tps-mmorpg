#include "stdafx.h"
#include "CCCursor.h"
#include "CCBitmap.h"
#include "CCDrawContext.h"
#include "CCAnimation.h"

CCCursor::CCCursor(const char* szName)
{
	_ASSERT(strlen(szName)<CCCURSOR_NAME_LENGTH);
	strcpy(m_szName, szName);
}

void CCBitmapCursor::Draw(CCDrawContext* pDC, int x, int y)
{
	pDC->SetBitmap(m_pBitmap);
	pDC->Draw(x, y);
}

CCBitmapCursor::CCBitmapCursor(const char* szName, CCBitmap* pBitmap) : CCCursor(szName)
{
	m_pBitmap = pBitmap;
}

void CCAniBitmapCursor::Draw(CCDrawContext* pDC, int x, int y)
{
	//m_pAnimation->SetScreenPosition(x, y);
	//sPoint p = MClientToScreen(m_pAnimation, sPoint(x, y));
	//m_pAnimation->SetPosition(p);
	//m_pAnimation->SetPosition(sPoint(x,y));
	//m_pAnimation->Draw(pDC);
	CCBitmap* pBitmap = m_pAnimation->GetBitmap();
	if( pBitmap == NULL ) return;
	
	pDC->SetBitmap(pBitmap);
	pDC->Draw(x,y);
}

CCAniBitmapCursor::CCAniBitmapCursor(const char* szName, CCAniBitmap* pAniBitmap) : CCCursor(szName)
{
	m_pAnimation = new CCAnimation(szName, pAniBitmap);
	m_pAnimation->m_iPlayMode = CCPM_REPETITION;
}

CCAniBitmapCursor::~CCAniBitmapCursor(void)
{
	delete m_pAnimation;
}

