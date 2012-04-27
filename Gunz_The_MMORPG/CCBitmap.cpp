#include "stdafx.h"
#include "CCBitmap.h"
#include "CCDrawContext.h"

/*
	CCBitmap.cpp
		Implementing CCBitmap.h
*/

// CCBitmap class
CCBitmap::CCBitmap(){
	m_DrawMode = CCBM_Normal;
}
CCBitmap::~CCBitmap(){
	Destroy();
}
bool CCBitmap::Create(const char *szName){
	_ASSERT(strlen(szName)<CCBITMAP_NAME_LENGTH);
	strcpy(m_szName, szName);
	return true;
}
void CCBitmap::Destroy(){}




// CCAniBitmap class
CCAniBitmap::CCAniBitmap(){
	m_iCurrentFrame = 0;
}
CCAniBitmap::~CCAniBitmap(){
	Destroy();
}
bool CCAniBitmap::Create(const char *szName){
	_ASSERT(strlen(szName)<CCBITMAP_NAME_LENGTH);
	strcpy(m_szName, szName);
	return true;
}
void CCAniBitmap::Destroy(){
	m_Bitmaps.DeleteRecordAll();
}
void CCAniBitmap::Add(CCBitmap *pBitmap){
	m_Bitmaps.Add(pBitmap);
}
CCBitmap* CCAniBitmap::Get(int iFrame){
	if(iFrame<0||iFrame>=GetFrameCount()) return NULL;
	return m_Bitmaps.Get(iFrame);
}
CCBitmap* CCAniBitmap::Get(){
	return Get(m_iCurrentFrame);
}
int CCAniBitmap::GetFrameCount(){
	return m_Bitmaps.GetCount();
}
int CCAniBitmap::GetCurrentFrame(){
	return m_iCurrentFrame;
}
bool CCAniBitmap::MoveNext(){
	if(m_iCurrentFrame+1>=GetFrameCount()) return false;
	m_iCurrentFrame++;
	return true;
}
bool CCAniBitmap::MovePrev(){
	if(m_iCurrentFrame-1<0) return false;
	m_iCurrentFrame--;
	return true;
}
void CCAniBitmap::MoveFirst(){
	m_iCurrentFrame =0;
}
void CCAniBitmap::MoveLast(){
	m_iCurrentFrame = GetFrameCount();
}
bool CCAniBitmap::Move(int iFrame){
	if(iFrame<0||iFrame>=GetFrameCount()) return false;
	m_iCurrentFrame = iFrame;
	return true;
}
int CCAniBitmap::GetDelay(){
	return m_iDelay;
}



// CCPartialBitmap class
CCPartialBitmap::CCPartialBitmap(){
	m_pSource = NULL;
}
CCPartialBitmap::CCPartialBitmap(CCBitmap *pBitmap, sRect rt){
	m_pSource = pBitmap;
	m_rect = rt;
}