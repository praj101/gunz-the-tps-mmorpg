#include "stdafx.h"
#include "CCBitmap.h"
#include "CCDrawContext.h"


// CCBitmap Implementation
/////////////////////////
CCBitmap::CCBitmap(void)
{
#ifdef _DEBUG
	m_nTypeID = CORE_BASE_CLASS_TYPE;
#endif
	m_DrawMode = MBM_Normal;
}

CCBitmap::~CCBitmap(void)
{
	Destroy();
}

bool CCBitmap::Create(const char* szName)
{
	_ASSERT(strlen(szName)<MBITMAP_NAME_LENGTH);
	strcpy(m_szName, szName);

	return true;
}

void CCBitmap::Destroy(void)
{
}

CCAniBitmap::CCAniBitmap(void)
{
	m_nCurFrame = 0;
}

CCAniBitmap::~CCAniBitmap(void)
{
	Destroy();
}

bool CCAniBitmap::Create(const char* szName)
{
	_ASSERT(strlen(szName)<MBITMAP_NAME_LENGTH);
	strcpy(m_szName, szName);

	return true;
}

void CCAniBitmap::Destroy(void)
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

CCBitmap* CCAniBitmap::Get(void)
{
	return Get(m_nCurFrame);
}

int CCAniBitmap::GetFrameCount(void)
{
	return m_Bitmaps.GetCount();
}

int CCAniBitmap::GetCurFrame(void)
{
	return m_nCurFrame;
}

bool CCAniBitmap::MoveNext(void)
{
	if(m_nCurFrame+1>=GetFrameCount()) return false;
	m_nCurFrame++;
	return true;
}

bool CCAniBitmap::MovePrevious(void)
{
	if(m_nCurFrame-1<0) return false;
	m_nCurFrame--;
	return true;
}

void CCAniBitmap::MoveFirst(void)
{
	m_nCurFrame = 0;
}

void CCAniBitmap::MoveLast(void)
{
	m_nCurFrame = GetFrameCount();
}

bool CCAniBitmap::Move(int nFrame)
{
	if(nFrame<0 || nFrame>=GetFrameCount()) return false;
	m_nCurFrame = nFrame;
	return true;
}

int CCAniBitmap::GetDelay(void)
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
