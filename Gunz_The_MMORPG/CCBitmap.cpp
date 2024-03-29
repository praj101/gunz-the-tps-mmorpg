#include "stdafx.h"
#include "CCBitmap.h"
#include "CCDrawContext.h"


// CCBitmap Implementation
/////////////////////////
CCBitmap::CCBitmap()
{
#ifdef _DEBUG
	m_nTypeID = CC_BASE_CLASS_TYPE;
#endif
	m_DrawMode = CCBM_Normal;
}

CCBitmap::~CCBitmap()
{
	Destroy();
}

bool CCBitmap::Create(const char* szName)
{
	_ASSERT(strlen(szName)<MBITMAP_NAME_LENGTH);
	strcpy(m_szName, szName);

	return true;
}

void CCBitmap::Destroy()
{
}

CCAniBitmap::CCAniBitmap()
{
	m_nCurFrame = 0;
}

CCAniBitmap::~CCAniBitmap()
{
	Destroy();
}

bool CCAniBitmap::Create(const char* szName)
{
	_ASSERT(strlen(szName)<MBITMAP_NAME_LENGTH);
	strcpy(m_szName, szName);

	return true;
}

void CCAniBitmap::Destroy()
{
	m_Bitmaps.DeleteRecordAll();
}

void CCAniBitmap::Add(CCBitmap* pBitmap)
{
	m_Bitmaps.Add(pBitmap);
}

CCBitmap* CCAniBitmap::Get(int nFrame)
{
	if(nFrame<0 || nFrame>=GetFrameCount()) return NULL;
	return m_Bitmaps.Get(nFrame);
}

CCBitmap* CCAniBitmap::Get()
{
	return Get(m_nCurFrame);
}

int CCAniBitmap::GetFrameCount()
{
	return m_Bitmaps.GetCount();
}

int CCAniBitmap::GetCurFrame()
{
	return m_nCurFrame;
}

bool CCAniBitmap::MoveNext()
{
	if(m_nCurFrame+1>=GetFrameCount()) return false;
	m_nCurFrame++;
	return true;
}

bool CCAniBitmap::MovePrevious()
{
	if(m_nCurFrame-1<0) return false;
	m_nCurFrame--;
	return true;
}

void CCAniBitmap::MoveFirst()
{
	m_nCurFrame = 0;
}

void CCAniBitmap::MoveLast()
{
	m_nCurFrame = GetFrameCount();
}

bool CCAniBitmap::Move(int nFrame)
{
	if(nFrame<0 || nFrame>=GetFrameCount()) return false;
	m_nCurFrame = nFrame;
	return true;
}

int CCAniBitmap::GetDelay()
{
	return m_nDelay;
}

CCPartialBitmap::CCPartialBitmap()
{
	m_pSource = NULL;
}

CCPartialBitmap::CCPartialBitmap(CCBitmap *pBitmap, sRect rt)
{
	m_pSource = pBitmap;
	m_Rect = rt;
}
