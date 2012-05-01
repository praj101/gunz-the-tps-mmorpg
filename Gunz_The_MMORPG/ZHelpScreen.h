#ifndef _ZHelpScreen_h
#define _ZHelpScreen_h

class CCBitmapR2;

class ZHelpScreen
{
public:
	ZHelpScreen();
	~ZHelpScreen();

	void ChangeMode();

	void DrawHelpScreen();

public:

	CCBitmapR2* m_pHelpScreenBitmap;

	bool m_bDrawHelpScreen;

};

#endif//_ZHelpScreen_h