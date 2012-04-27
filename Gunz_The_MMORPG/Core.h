#pragma once
/*
	C_elestial c_O_ding g_R_aphical int_E_rface
	CORE.h - Related to Mint.h
*/

#include <windows.h>
#include <map>
#include "CCTypes.h"
#include "CCEvent.h"

using namespace std;

class CCDrawContext;
class CCWidget;
class CCFont;
class CCBitmap;
class CCResourceMap;
class CCIDLResource;
class CCListener;
class CCEvent;

#define CORE_VERSION			2
#define CCVersion()				CORE_VERSION
#define CONVERT800(x)			(int((x)/800.0f * CCGetWorkspaceWidth()))
#define CONVERT600(x)			(int((x)/600.0f * CCGetWorkspaceHeight()))
#define CCMODIFIER_ALT			MOD_ALT
#define CCMODIFIER_CTRL			MOD_CONTROL
#define CCMODIFIER_SHIFT		MOD_SHIFT
#define CCIsActionKeyPressed(i)	(Core::GetInstance()->IsActionKeyPressed(i))
#define IsHangul(x)				((uchar)(x)>127)

typedef bool (MGLOBALEVENTCALLBACK)(CCEvent* pEvent);

class Core{
protected:
	static Core*			m_pInstance;
	CCWidget*				m_pMainFrame;
	CCDrawContext*			m_pDC;
	MGLOBALEVENTCALLBACK*	m_fnGlobalEventCallBack;

	// Drag and drop
	char		m_szDragObjectString[256];
	char		m_szDragObjectItemString[256];
	CCBitmap*	m_pDragObjectBitmap;
	sPoint		m_GrabPoint;
	bool		m_bVisibleDragObject;
	CCWidget*	m_pDropableObject;
	CCWidget*	m_pDragSourceObject;

	// Workspace size
	int			m_iWorkspaceWidth;
	int			m_iWorkspaceHeight;

	void*		m_pCandidateList;
	int			m_iCandidateListSize;
	sPoint		m_CondidateListPos;

public:
	DWORD		m_iCompositionAttributeSize;
	BYTE		m_iCompositionAttributes[COMPOSITIONSTRING_LENGTH];
	int			m_iCompositionCaretPosition;

protected:
	virtual void DrawCandidateList(CCDrawContext* pDC, sPoint& p);

public:
	Core();
	virtual ~Core();

	bool	Init(int iwidth, int iheight, CCDrawContext* pDC, CCFont* pDefaultFont);
	void	Finalize();

	bool	ProcessEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	virtual void Run();
	virtual void Draw();
	virtual void Update();

	CCWidget* GetMainFrame();
	CCDrawContext* GetDrawContext();

	static Core* GetInstance();

	void SetHWND(HWND hwnd);
	HWND GetHWND();

	HIMC m_hImc;

	void EnableIME(bool bEnable);
	bool IsEnableIME();

	int RegisterHotKey(ulong int iModifier, ulong int iVirtKey);
	void UnregisterHotKey(int iID);

	CCWidget* SetDragObject(CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString);
	CCWidget* GetDragObject();

	virtual CCWidget* NewWidget(const char* szClass, const char* szName, CCWidget* pParent, CCListener* pListener);

	CCWidget* FindWidgetDropAble(sPoint& p);
	CCWidget* FindWidget(sPoint& p);
	CCWidget* FindWidget(int x, int y);

	int GetWorkspaceWidth();
	int GetWorkspaceHeight();
	void SetWorkspaceSize(int w, int h);

	virtual CCBitmap*	OpenBitmap(const char* szName) = 0;
	virtual CCFont*		OpenFont(const char* szName, int iHeight) = 0;

	void SetGlobalEvent(MGLOBALEVENTCALLBACK pGlobalEventCallback);
	
	int GetPrimaryLanguageIdentifier() const;
	int GetSubLanguageIdentifier() const;
	const char* GetLanguageIndicatorString() const;

	bool IsNativeIME() const;

	void OpenCandidateList();
	void CloseCandidateList();
	const char* GetCandidate(int iIndex) const;
	int GetCandidateCount() const;
	int GetCandidateSelection() const;
	int GetCandidatePageStart() const;
	int GetCandidatePageSize() const;

	void SetCandidateListPosition(sPoint& p, int iWidgetHeight);
	int GetCandidateListWidth();
	int GetCandidateListHeight();

	DWORD GetCompositionAttributeSize() const { return m_iCompositionAttributeSize; };
	const BYTE* GetCompositionAttributes() const { return m_iCompositionAttributes; };

	int DrawCompositionAttribute(CCDrawContext* pDC, sPoint &p, const char* szComposition, int i);
	void DrawCompositionAttributes(CCDrawContext* pDC, sPoint& p, const char* szComposition);
	void DrawIndicator(CCDrawContext* pDC, sRect& r);
};

inline int CCGetWorkspaceWidth(){
	return Core::GetInstance()->GetWorkspaceWidth();
}
inline int CCGetWorkspaceHeight(){
	return Core::GetInstance()->GetWorkspaceHeight();
}