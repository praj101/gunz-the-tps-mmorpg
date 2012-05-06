#ifndef _ZSCREENDEBUGGER_H
#define _ZSCREENDEBUGGER_H


class CCDrawContext;
class ZActor;
class ZScreenDebugger
{
private:
	int		m_nShowDebugInfo;
	int		m_nY;

	void OnDrawAIDebugInfo(CCDrawContext *pDC);
	void DrawActorInfo(int num, CCDrawContext *pDC, ZActor* pActor);
	void PrintText(CCDrawContext* pDC, const char* buffer);
public:
	ZScreenDebugger();
	~ZScreenDebugger();
	void DrawDebugInfo(CCDrawContext *pDC);
	void SwitchDebugInfo();

	bool IsVisible()	{ return (m_nShowDebugInfo != 0); }
};




#endif