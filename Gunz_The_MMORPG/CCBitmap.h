#pragma once
#include "CCList.h"
#include "CCTypes.h"

#define CCBM_Normal 0
#define CCBM_FlipLR 1
#define CCBM_FlipUD 1<<1
#define CCBM_RotL90 1<<2
#define CCBM_RotR90 1<<3

////////////////////////////////////////////////////
// Bitmap abstract class
//		시스템에 맞는 class로 override해서 사용한다.
#define MBITMAP_NAME_LENGTH		128
class CCBitmap{
public:
#ifdef _DEBUG
	int		m_nTypeID;	// 같은 레벨의 클래스(sFont, MDrawContex)를 사용하기 위한 ID
#endif

public:
	char	m_szName[MBITMAP_NAME_LENGTH];
	DWORD	m_DrawMode;
public:
	CCBitmap();
	virtual ~CCBitmap();

	void CreatePartial(CCBitmap *pBitmap,sRect rt,const char *szName);		// partial bitmap 으로 생성한다

	virtual bool Create(const char* szName);
	virtual void Destroy();

	virtual void SetDrawMode(DWORD md) { m_DrawMode = md; }
	virtual DWORD GetDrawMode() { return m_DrawMode; }

	virtual int GetX() { return 0; }
	virtual int GetY() { return 0; }
	virtual int GetWidth() { return 0; };
	virtual int GetHeight(){ return 0; };

	virtual CCBitmap *GetSourceBitmap() { return this; }
};

class CCPartialBitmap : public CCBitmap {
	CCBitmap	*m_pSource;
	sRect	m_Rect;
public:
	CCPartialBitmap();
	CCPartialBitmap(CCBitmap *pBitmap, sRect rt);

	virtual int GetX() { return m_Rect.x; }
	virtual int GetY() { return m_Rect.y; }
	virtual int GetWidth() { return m_Rect.w; }
	virtual int GetHeight() { return m_Rect.h; }

	virtual CCBitmap *GetSourceBitmap() { return m_pSource; }
};

class CCAniBitmap{
protected:
#ifdef _DEBUG
	friend class CCDrawContext;
	int		m_nTypeID;		// 같은 레벨의 클래스(sFont, MDrawContex)를 사용하기 위한 ID
#endif
public:
	char	m_szName[MBITMAP_NAME_LENGTH];
private:
	CCLinkedList<CCBitmap>	m_Bitmaps;
protected:
	int		m_nCurFrame;
	int		m_nDelay;
public:
	CCAniBitmap();
	virtual ~CCAniBitmap();
	bool Create(const char* szName);
	void Destroy();

	void Add(CCBitmap* pBitmap);
	CCBitmap* Get(int nFrame);
	CCBitmap* Get();

	int GetFrameCount();
	int GetCurFrame();

	bool MoveNext();
	bool MovePrevious();
	void MoveFirst();
	void MoveLast();
	bool Move(int nFrame);

	int GetDelay();
	void SetDelay(int nDelay) { m_nDelay = nDelay; }
};
