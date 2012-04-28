#pragma once
#include "CCWidget.h"

#define CCPICTURE	"Picture"

/// Picture
class CCPicture : public CCWidget{
protected:
	CCBitmap* m_pBitmap;
	int		m_iStretch;
	int		m_iAnimType;			// 0:��->��, 1:��->��
	float	m_fAnimTime;			// �ִϸ��̼� �Ǵ� �ð�
	DWORD	m_dwCurrentTime;
	bool	m_bAnim;
	DWORD	m_DrawMode;

	sColor	m_BitmapColor;
	sColor	m_BitmapReseveColor;
	bool	m_bSwaped;

protected:
	/// Draw Handler
	virtual void OnDraw(CCDrawContext* pDC);
	void OnAnimDraw(CCDrawContext* pDC, int x, int y, int w, int h, int bx, int by, int bw, int bh );

public:
	CCPicture(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);

	/// ��Ʈ�� ����
	void SetBitmap(CCBitmap* pBitmap);
	/// ��Ʈ�� ���
	CCBitmap* GetBitmap(void);
	int		GetStretch() { return m_iStretch; }
	void SetStretch(int i) { m_iStretch = i; }

	void SetAnimation( int animType, float fAnimTime);

	void SetBitmapColor( sColor color );	
	sColor GetBitmapColor() const {
		return m_BitmapColor;
	}
	sColor GetReservedBitmapColor() const{
		return m_BitmapReseveColor;	
	}

	void SetDrawMode( DWORD mode ) {
		m_DrawMode = mode;
	}
	DWORD GetDrawMode( ) const {
		return m_DrawMode;
	}

	bool IsAnim() const { return m_bAnim;}
	virtual const char* GetClassName(void){ return CCPICTURE; }
};
