#pragma once
/*
	CCCursor.h
*/

class CCBitmap;
class CCAniBitmap;
class CCAnimation;
class CCDrawContext;

#define CCCURSOR_NAME_LENGTH	32

class CCCursor{
	char	m_szName[CCCURSOR_NAME_LENGTH];
protected:
	friend class CCCursorSystem;
	virtual void Draw(CCDrawContext* pDC, int x, int y){}
public:
	CCCursor(const char* szName);
	virtual ~CCCursor(){}
};

class CCBitmapCursor : public CCCursor{
	CCBitmap*	m_pBitmap;
protected:
	virtual void Draw(CCDrawContext* pDC, int x, int y);
public:
	CCBitmapCursor(const char *szName, CCBitmap* pBitmap);
};

class CCAniBitmapCursor : public CCCursor{
	CCAnimation* m_pAnimation;
protected:
	virtual void Draw(CCDrawContext* pDC, int x, int y);
public:
	CCAniBitmapCursor(const char* szName, CCAniBitmap* pAniBitmap);
	virtual ~CCAniBitmapCursor();
};