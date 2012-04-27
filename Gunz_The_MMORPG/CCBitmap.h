#pragma once
/* 
	CCBitmap.h
*/

#include "CCList.h"
#include "CCTypes.h"

#define CCBM_Normal	0
#define CCBM_FlipLR	1
#define CCBM_FlipUD	1<<1
#define CCBM_RotL90 1<<2
#define CCBM_RotR90 1<<3

#define CCBITMAP_NAME_LENGTH	128

class CCBitmap{
public:
	char	m_szName[CCBITMAP_NAME_LENGTH];
	DWORD	m_DrawMode;

	CCBitmap();
	virtual ~CCBitmap();

	void CreatePartial(CCBitmap* pBitmap, sRect rt, const char* szName);

	virtual bool	Create(const char* szName);
	virtual void	Destroy();

	virtual void	SetDrawMode(DWORD md) { m_DrawMode=md; };
	virtual DWORD	GetDrawMode() { return m_DrawMode; };

	virtual int		GetX() { return 0; };
	virtual int		GetY() { return 0; };
	virtual int		GetWidth() = 0;
	virtual int		GetHeight()= 0;

	virtual CCBitmap* GetSourceBitmap() { return this; };
};

class CCPartialBitmap : public CCBitmap {
	CCBitmap*	m_pSource;
	sRect		m_rect;
public:
	CCPartialBitmap();
	CCPartialBitmap(CCBitmap *pBitmap, sRect rt);

	virtual int		GetX()		{ return m_rect.x; };
	virtual int		GetY()		{ return m_rect.y; };
	virtual int		GetWidth()	{ return m_rect.w; };
	virtual int		GetHeight()	{ return m_rect.h; };

	virtual CCBitmap* GetSourceBitmap() { return m_pSource; };
};

class CCAniBitmap{
public:
	char	m_szName[CCBITMAP_NAME_LENGTH];
private:
	CCLinkedList<CCBitmap>	m_Bitmaps;

protected:
	int		m_iCurrentFrame;
	int		m_iDelay;

public:
	CCAniBitmap();
	virtual ~CCAniBitmap();

	bool Create(const char* szName);
	bool Destroy();

	void Add(CCBitmap* pBitmap);

	CCBitmap* Get(int iFrame);
	CCBitmap* Get();

	int	GetFrameCount();
	int	GetCurrentFrame();

	bool MoveNext();
	bool MovePrev();
	void MoveFirst();
	void MoveLast();
	bool Move(int iFrame);

	int GetDelay();
	void SetDelay(int d) { m_iDelay = d; };

};