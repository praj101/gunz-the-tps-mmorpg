#pragma once
/* 
	CCDrawContext.h
*/

#include "CCList.h"
#include "CCTypes.h"
#include "CCBitmap.h"
#include "CCFont.h"
#include "CCCursor.h"
#include "CCResourceManager.h"

struct sRect;
struct sPoint;

#define CC_BASE_CLASS_TYPE	0x1030

typedef int CCAlignmentMode;
#define CCD_NOTALIGN	0
#define CCD_LEFT		1
#define CCD_RIGHT	2
#define CCD_HCENTER	4
#define CCD_EDIT		8
#define CCD_TOP		16
#define CCD_BOTTOM	32
#define CCD_VCENTER	64

enum CCDrawEffect{
	CCDE_NORMAL = 0,
	CCDE_ADD,
	CCDE_MULTIPLY,
	CCDE_MAX
};

class CCDrawContext{
protected:
	sColor			m_Color;
	sColor			m_HighlightColor;
	sColor			m_ColorKey;
	sColor			m_BitmapColor;
	CCBitmap*		m_pBitmap;
	CCFont*			m_pFont;
	sRect			m_Clip;
	sPoint			m_Origin;
	CCDrawEffect	m_Effect;
	uchar			m_cOpacity;

public:
	CCDrawContext();
	virtual ~CCDrawContext();

	// Color settings
	sColor SetBitmapColor(sColor& color);
	sColor SetBitmapColor(uchar r, uchar g, uchar b, uchar a=255);
	sColor GetBitmapColor();
	sColor SetColor(sColor& color);
	sColor SetColor(uchar r, uchar g, uchar b, uchar a=255);
	sColor GetColor();
	sColor SetHighlightColor(sColor& color);
	sColor SetHighlightColor(uchar r, uchar g, uchar b, uchar a=255);
	sColor GetHighlightColor();
	sColor SetColorKey(sColor& color);
	sColor SetColorKey(uchar r, uchar g, uchar b, uchar a=255);
	sColor GetColorKey();

	// Font settings
	CCFont* SetFont(CCFont* pFont);
	CCFont* GetFont() { return m_pFont; };

	// Bitmap Setting
	CCBitmap* SetBitmap(CCBitmap* pBitmap);
	CCBitmap* GetBitmap(){ return m_pBitmap; }

	// Clipping ( Screen Coordinate )
	virtual void SetClipRect(sRect& r);
	void SetClipRect(int x, int y, int w, int h);
	sRect GetClipRect();

	// Logical Origin
	void SetOrigin(int x, int y);
	void SetOrigin(sPoint& p);
	sPoint GetOrigin();

	// Draw Effect
	void SetEffect(CCDrawEffect effect);
	CCDrawEffect GetEffect() { return m_Effect; }

	// Basic Drawing Functions
	virtual void SetPixel(int x, int y, sColor& color) = 0;
	virtual void HLine(int x, int y, int len) = 0;
	virtual void VLine(int x, int y, int len) = 0;
	virtual void Line(int sx, int sy, int ex, int ey) = 0;
	virtual void Rectangle(int x, int y, int cx, int cy);
	void Rectangle(sRect& r);
	virtual void FillRectangle(int x, int y, int cx, int cy) = 0;
	virtual void FillRectangleW(int x, int y, int cx, int cy) = 0;
	void FillRectangle(sRect& r);

	// Bitmap Drawing
	void Draw(int x, int y);
	void Draw(int x, int y, int w, int h);
	void Draw(int x, int y, int sx, int sy, int sw, int sh);
	void Draw(sPoint &p);
	void Draw(sRect& r);
	void Draw(int x, int y, sRect& s);

	virtual void Draw(sRect& d, sRect& s);
	virtual void DrawInverse( int x, int y, int w, int h, bool bMirrorX, bool bMirrorY );
	virtual void DrawInverse( int x, int y, int w, int h, int sx, int sy, int sw, int sh, bool bMirrorX, bool bMirrorY);
	virtual void DrawEx(int tx1, int ty1, int tx2, int ty2, 
		                int tx3, int ty3, int tx4, int ty4);
	virtual void Draw(int x, int y, int w, int h, int sx, int sy, int sw, int sh);

	// Text
	virtual bool BeginFont() = 0;
	virtual bool EndFont() = 0;
	virtual int Text(int x, int y, const char* szText) = 0;
	int Text(sPoint& p, const char* szText){ return Text(p.x, p.y, szText); }
	virtual int TextMultiLine(sRect& r, const char* szText,int nLineGap=0,bool bAutoNextLine=true,int nIndentation=0,int nSkipLine=0, sPoint* pPositions=NULL);
	virtual int TextMultiLine2(sRect& r, const char* szText,int nLineGap=0,bool bAutoNextLine=true,CCAlignmentMode am=(CCD_HCENTER|CCD_VCENTER) );

	int TextWithHighlight(int x, int y, const char* szText);
	void GetPositionOfAlignment(sPoint* p, sRect& r, const char* szText, CCAlignmentMode am, bool bAndInclude=true);
	int Text(sRect& r, const char* szText, CCAlignmentMode am=(CCD_HCENTER|CCD_VCENTER));
	int TextWithHighlight(sRect& r, const char* szText, CCAlignmentMode am=(CCD_HCENTER|CCD_VCENTER));

	// Multiple Color Support
	void TextMC(int x, int y, const char* szText);
	void TextMC(sRect& r, const char* szText, CCAlignmentMode am);
	
	// Multiple Color
	static char* GetPureText(const char *szText);

	// Opacity
	unsigned char SetOpacity(unsigned char nOpacity);
	unsigned char GetOpacity();

private:
	virtual void Draw(CCBitmap *pBitmap,int x, int y, int w, int h, int sx, int sy, int sw, int sh, bool bMirrorX=false, bool bMirrorY=false) = 0;		// User Defined Draw Funtion
};


class CCDrawContext3D{
};

int CCGetWidth		(CCFont* pFont, const char* szText, int nSize, bool bColorSupport=false);
int CCGetNextLinePos(CCFont* pFont, const char* szText, int nWidth, bool bAutoNextLine=true, bool bColorSupport=false);
int CCGetLinePos	(CCFont* pFont, const char* szText, int nWidth, bool bAutoNextLine=true, bool bColorSupport=false, int nLine=1, int nIndentation=0);
int CCGetLineCount	(CCFont* pFont, const char* szText,	int nWidth, bool bAutoNextLine=true, bool bColorSupport=false, int nIndentation=0);