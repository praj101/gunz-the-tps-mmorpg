#include "stdafx.h"
#include "CCResourceManager.h"

// CCBitmapManager Implementation
//////////////////////////////
CCLinkedList<CCBitmap> CCBitmapManager::m_Bitmaps;
CCLinkedList<CCAniBitmap> CCBitmapManager::m_AniBitmaps;

void CCBitmapManager::Destroy(void)
{
	for(int i=0; i<m_Bitmaps.GetCount(); i++){
		CCBitmap* pBitmap = m_Bitmaps.Get(i);
		pBitmap->Destroy();
	}
	m_Bitmaps.DeleteAll();
}

void CCBitmapManager::DestroyAniBitmap(void)
{
	for(int i=0; i<m_AniBitmaps.GetCount(); i++){
		CCAniBitmap* pAniBitmap = m_AniBitmaps.Get(i);
		pAniBitmap->Destroy();
	}
	m_AniBitmaps.DeleteAll();
}

void CCBitmapManager::Add(CCBitmap* pBitmap)
{
	m_Bitmaps.Add(pBitmap);
}

void CCBitmapManager::Add(CCAniBitmap* pAniBitmap)
{
	m_AniBitmaps.Add(pAniBitmap);
}

void CCBitmapManager::Delete(const char* szName)
{
	for(int i=0; i<m_Bitmaps.GetCount(); i++){
		CCBitmap* pBitmap = m_Bitmaps.Get(i);
		if(strcmp(pBitmap->m_szName, szName)==0){
			m_Bitmaps.Delete(i);
			return;
		}
	}
	for(int i=0; i<m_AniBitmaps.GetCount(); i++){
		CCAniBitmap* pAniBitmap = m_AniBitmaps.Get(i);
		if(strcmp(pAniBitmap->m_szName, szName)==0){
			m_AniBitmaps.Delete(i);
			return;
		}
	}
}

CCBitmap* CCBitmapManager::Get(const char* szBitmapName)
{
	if(szBitmapName==NULL)	// Default Font
		if(m_Bitmaps.GetCount()>0)
			return m_Bitmaps.Get(0);

	for(int i=0; i<m_Bitmaps.GetCount(); i++){
		CCBitmap* pBitmap = m_Bitmaps.Get(i);
		if(pBitmap && stricmp(pBitmap->m_szName, szBitmapName)==0) return pBitmap;
	}

	return NULL;
}

CCBitmap* CCBitmapManager::Get(int i)
{
	return m_Bitmaps.Get(i);
}

int CCBitmapManager::GetCount(void)
{
	return m_Bitmaps.GetCount();
}

CCAniBitmap* CCBitmapManager::GetAniBitmap(const char* szBitmapName)
{
	if(szBitmapName==NULL)	// Default Font
		if(m_AniBitmaps.GetCount()>0)
			return m_AniBitmaps.Get(0);

	for(int i=0; i<m_AniBitmaps.GetCount(); i++){
		CCAniBitmap* pBitmap = m_AniBitmaps.Get(i);
		if(strcmp(pBitmap->m_szName, szBitmapName)==0) return pBitmap;
	}

	return NULL;
}


// CCFontManager Implementation
//////////////////////////////
//CCLinkedList<CCFont> CCFontManager::m_Fonts;
map<string,CCFont*>	CCFontManager::m_Fonts;
CCFont *CCFontManager::m_pDefaultFont=NULL;

void CCFontManager::SetDefaultFont(CCFont *pFont)
{
	m_pDefaultFont=pFont;
}

void CCFontManager::Destroy(void)
{
	/*
	for(int i=0; i<m_Fonts.GetCount(); i++){
		CCFont* pFont = m_Fonts.Get(i);
		pFont->Destroy();
	}
	m_Fonts.DeleteAll();
	*/

	while(!m_Fonts.empty())
	{
		delete m_Fonts.begin()->second;
		m_Fonts.erase(m_Fonts.begin());
	}
}

void CCFontManager::Add(CCFont* pFont)
{
//	m_Fonts.Add(pFont);

	map<string,CCFont*>::iterator i=m_Fonts.find(string(pFont->m_szName));
	if(i!=m_Fonts.end())
	{
		delete i->second;
		m_Fonts.erase(i);
	}

	m_Fonts.insert(map<string,CCFont*>::value_type(string(pFont->m_szName),pFont));
}

CCFont* CCFontManager::Get(const char* szFontName)
{
	if(szFontName)
	{
		map<string,CCFont*>::iterator i=m_Fonts.find(string(szFontName));
		if(i!=m_Fonts.end())
		{
			return i->second;
		}
	}

	if(m_pDefaultFont) return m_pDefaultFont;

	return NULL;

	/*
	if(szFontName==NULL)	// Default Font
		if(m_Fonts.GetCount()>0)
			return m_Fonts.Get(0);

	for(int i=0; i<m_Fonts.GetCount(); i++){
		CCFont* pFont = m_Fonts.Get(i);
		if(strcmp(pFont->m_szName, szFontName)==0) return pFont;
	}

	return NULL;
	*/
}

/*
CCFont* CCFontManager::Get(int i)
{
	if(i<0 || i>=m_Fonts.GetCount()) return NULL;
	return m_Fonts.Get(i);
}
*/

int CCFontManager::GetCount(void)
{
//	return m_Fonts.GetCount();
	return m_Fonts.size();
}

void CCFontManager::Resize(float ratio, int nMinimumHeight)
{
	map<string,CCFont*>::iterator i = m_Fonts.begin();
	while (i!=m_Fonts.end()) {
		if (i->second) {
			i->second->Resize(ratio, nMinimumHeight);
		}
		++i;
	}
}

CCCursor* CCCursorSystem::m_pCursor;
CCLinkedList<CCCursor> CCCursorSystem::m_Cursors;
bool CCCursorSystem::m_bShow = true;

void CCCursorSystem::Destroy(void)
{
	m_Cursors.DeleteAll();
	m_pCursor=NULL;
}

CCCursor* CCCursorSystem::Set(CCCursor* pCursor)
{
	CCCursor* temp = m_pCursor;
	m_pCursor = pCursor;
	return temp;
}

CCCursor* CCCursorSystem::Set(const char* szName)
{
	if(szName==NULL){
		//...
	}
	CCCursor* pCursor = Get(szName);
	if(pCursor!=NULL) return Set(pCursor);
	return NULL;
}

CCCursor* CCCursorSystem::Get(void)
{
	return m_pCursor;
}

CCCursor* CCCursorSystem::Get(const char* szName)
{
	for(int i=0; i<m_Cursors.GetCount(); i++){
		CCCursor* pCursor = m_Cursors.Get(i);
		if(strcmp(pCursor->m_szName, szName)==0) return pCursor;
	}
	return NULL;
}

void CCCursorSystem::Add(CCCursor* pCursor)
{
	m_Cursors.Add(pCursor);
}

void CCCursorSystem::Draw(CCDrawContext* pDC, int x, int y)
{
	if(m_bShow==false) return;
	if(m_pCursor!=NULL) m_pCursor->Draw(pDC, x, y);
}

void CCCursorSystem::Show(bool bShow)
{
	m_bShow = bShow;
}

bool CCCursorSystem::IsVisible(void)
{
	return m_bShow;
}
