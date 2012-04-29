#pragma once
/* 
	CCResourceManager.h
*/

#include "CCBitmap.h"
#include "CCFont.h"
#include "CCCursor.h"
#include "CCList.h"

#include <map>
#include <string>

using namespace std;

#define CC_SMALL		"small"
#define CC_MEDIUM		"medium"
#define CC_SMALL_BOLD	"smallbold"
#define CC_MEDIUM_BOLD	"mediumbold"

#define CCCURSOR_ARROW	"arrow"
#define CCCURSOR_WAIT	"wait"
#define CCCURSOR_INPUT	"input"
//
//
//
class CCBitmapManager{
	static CCLinkedList<CCBitmap>		m_Bitmaps;
	static CCLinkedList<CCAniBitmap>	m_AniBitmaps;

public:
	static void Destroy();
	static void DestroyAniBitmap();
	static void Add(CCBitmap* pBitmap);
	static void Add(CCAniBitmap* pAniBitmap);
	static void Delete(const char* szName);

	static CCBitmap* Get(const char* szBitmapName);
	static CCBitmap* Get(int i);
	
	static int	GetCount();
	static CCAniBitmap* GetAniBitmap(const char* szBitmapName);
};

class CCFontManager{
	static map<string, CCFont*>	m_Fonts;
	static CCFont*	m_pDefaultFont;

public:
	CCFontManager();

	static void Destroy();
	static void Add(CCFont* pFont);
	static CCFont* Get(const char* szFontName);
	static int GetCount();
	static void SetDefaultFont(CCFont* pFont);
	static void Resize(float ratio, int iMinHeight);
};

class CCCursorSystem{
	static CCCursor*				m_pCursor;
	static CCLinkedList<CCCursor>	m_Cursors;
	static bool						m_bShow;
public:
	static void	Destroy();
	
	static CCCursor*	Set(CCCursor* pCursor);
	static CCCursor*	Set(const char* szName);
	
	static CCCursor*	Get();
	static CCCursor*	Get(const char* szName);

	static void			Add(CCCursor* pCursor);
	static void			Draw(CCDrawContext* pDC, int x, int y);
	static void			Show(bool bShow);
	static bool			IsVisible();
};