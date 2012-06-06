#include "stdafx.h"
#include "CCFrame.h"
#include "CCColorTable.h"
#include "Core.h"

//#define MTITLEBAR_HEIGHT		18	//800*600 기준 높이
#define MTITLEBAR_HEIGHT		(int(18 * CCGetWorkspaceHeight() / 600.f))	//해상도에 따라 적정 배율 적용

/*
class MTitleBar : public CCWidget{
protected:
	virtual void OnDraw(CCDrawContext* pDC){
		pDC->SetColor(sColor(DEFCOLOR_CCMENUBAR_PLANE));
		pDC->FillRectangle(m_Rect);
		pDC->SetColor(sColor(DEFCOLOR_CCMENUBAR_TEXT));
		pDC->Text(m_Rect, m_szName, MAM_LEFT);
	}
public:
	MTitleBar(char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL)
		: CCWidget(szName, pParent, pListener){}
	//virtual ~MTitleBar(void){}
};

class CCClient : public CCWidget{
protected:
	virtual void OnDraw(CCDrawContext* pDC){
	}
public:
	CCClient(char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL)
		: CCWidget(szName, pParent, pListener){}
};
*/

#define FRAME_W	300
#define FRAME_X	10
#define FRAME_H	300
#define FRAME_Y	10

void CCFrameLook::OnDraw(CCFrame* pFrame, CCDrawContext* pDC)
{
	sRect r = pFrame->GetInitialClientRect();
	pDC->SetColor(sColor(DEFCOLOR_CCFRAME_PLANE));
	pDC->FillRectangle(r);
	pDC->SetColor(128,128,128,255);
	pDC->Rectangle(r);

	/*
	// Frame Board
	sRect r(0, 0, pFrame->m_Rect.w, pFrame->m_Rect.h);
	pDC->SetColor(sColor(DEFCOLOR_MFRAME_PLANE));
	pDC->FillRectangle(r);
	pDC->SetColor(sColor(DEFCOLOR_MFRAME_OUTLINE));
	pDC->Rectangle(r);
	*/

	// TitleBar
	if(pFrame->m_bTitleBar==true){
		sRect TitleBarRect(r.x, r.y, r.w, MTITLEBAR_HEIGHT);
		pDC->SetColor(sColor(DEFCOLOR_CCMENUBAR_PLANE));
		pDC->FillRectangle(TitleBarRect);
		pDC->SetColor(sColor(DEFCOLOR_CCMENUBAR_TEXT));
		pDC->Text(TitleBarRect, pFrame->m_szName, CCD_LEFT);
	}
}

sRect CCFrameLook::GetClientRect(CCFrame* pFrame, sRect& r)
{
	int nTitleBarHeight = (pFrame->m_bTitleBar==true?MTITLEBAR_HEIGHT:0);
	return sRect(r.x+1, r.y + nTitleBarHeight, r.w-2, r.h - nTitleBarHeight-2);
}

IMPLEMENT_LOOK(CCFrame, CCFrameLook)


bool CCFrame::OnEvent(CCEvent* pEvent, CCListener* pListener)
{
	if (IsFocusEnable() == false)
		return false;

	//if(m_bTitleBar==true) return false;	// 타이틀바가 없는 프레임은 이벤트를 받지 않는다.

	sRect TitleBarRect(0, 0, m_Rect.w, MTITLEBAR_HEIGHT);
	sRect WidgetRect(0, 0, m_Rect.w, m_Rect.h);
	sPoint sp = CCClientToScreen(this, pEvent->sPos);
	//sPoint sp(m_Rect.x, m_Rect.y);
	switch(pEvent->iMessage){
	case CCWM_LBUTTONDOWN:

		if(m_bTitleBar==true && TitleBarRect.InPoint(pEvent->sPos)==true) {

			if ( m_BtnClose.m_Rect.InPoint(pEvent->sPos)==true)
			{
				m_BtnClose.m_bLButtonDown = true;
			}
			else if (m_BtnMinimize.m_Rect.InPoint(pEvent->sPos)==true)
			{
				m_BtnMinimize.m_bLButtonDown = true;
			}
			else
			{
				SetCapture();
				m_bDragWidget = true;
				sPoint wp = CCClientToScreen(GetParent(), sPoint(m_Rect.x, m_Rect.y));
				sp.x -= wp.x;
				sp.y -= wp.y;
				m_DragPoint = sp;
			}
			return true;
		}
		else if(WidgetRect.InPoint(pEvent->sPos)==true){
			return true;
		}
		break;
	case CCWM_LBUTTONUP:
		if (m_bTitleBar==true && m_BtnClose.m_Rect.InPoint(pEvent->sPos)==true)
		{
			if (m_BtnClose.m_bLButtonDown==true) OnCloseButtonClick();
		}
		else if (m_bTitleBar==true && m_BtnMinimize.m_Rect.InPoint(pEvent->sPos)==true)
		{
			if (m_BtnMinimize.m_bLButtonDown==true) OnMinimizeButtonClick();
		}

		m_BtnClose.m_bLButtonDown = m_BtnMinimize.m_bLButtonDown = false;

		if(m_bDragWidget==true){
			ReleaseCapture();
			m_bDragWidget = false;
			return true;
		}
		break;
	case CCWM_MOUSEMOVE:
		if(m_bDragWidget==true){
			sp.x -= m_DragPoint.x;
			sp.y -= m_DragPoint.y;
			if(sp.x<0) sp.x = 0;
			if(sp.y<0) sp.y = 0;
			if(sp.x+m_Rect.w>CCGetWorkspaceWidth()-1) sp.x = CCGetWorkspaceWidth()-m_Rect.w-1;
			if(sp.y+m_Rect.h>CCGetWorkspaceHeight()-1) sp.y = CCGetWorkspaceHeight()-m_Rect.h-1;
			sPoint p = CCScreenToClient(GetParent(), sp);
			if (m_bMovable == true) {
				SetPosition(p.x, p.y);
			}
			
			return true;
		}
		else if(m_bTitleBar==true)
		{
			if(m_BtnClose.m_Rect.InPoint(pEvent->sPos)==true)
			{
				if(m_BtnClose.m_bMouseOver==false) m_BtnClose.m_bMouseOver = true;
			}
			else
			{
				if(m_BtnClose.m_bMouseOver==true) m_BtnClose.m_bMouseOver = false;
			}
			if(m_BtnMinimize.m_Rect.InPoint(pEvent->sPos)==true)
			{
				if(m_BtnMinimize.m_bMouseOver==false) m_BtnMinimize.m_bMouseOver = true;
			}
			else
			{
				if(m_BtnMinimize.m_bMouseOver==true) m_BtnMinimize.m_bMouseOver = false;
			}

		}
		break;
	case CCWM_LBUTTONDBLCLK:
		// m_BtnClose 가 초기화가 안되어서 debug 에서는 동작을 안함. 일단 주석
		/*
		if (m_bTitleBar==true && m_BtnClose.m_Rect.InPoint(pEvent->sPos)==true)
		{
			OnCloseButtonClick();
		}
		else if (m_bTitleBar==true && m_BtnMinimize.m_Rect.InPoint(pEvent->sPos)==true)
		{
			OnMinimizeButtonClick();
		}

		// Shade
		else if(m_bTitleBar==true && TitleBarRect.InPoint(pEvent->sPos)==true){
			SetShade(!m_bShade);
			return true;
		}

		else if(WidgetRect.InPoint(pEvent->sPos)==true){
			return true;
		}
		return false;
		*/
		break;
	}
	return false;
}

bool CCFrame::OnShow(void)
{
	/*
	// 에디트 컨트롤에 우선순위를 두고...
	for(int i=0; i<GetChildCount(); i++){
		CCWidget* pWidget = GetChild(i);
		if(strcmp(pWidget->GetClassName(), "Edit")==0){
			pWidget->SetFocus();
			return true;
		}
	}
	*/
	// Children의 첫번째 Widget에게 포커스를 준다.
	if(GetChildCount()>0) GetChild(0)->SetFocus();
	return true;
}

CCFrame::CCFrame(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCWidget(szName, pParent, pListener)
{
	LOOK_IN_CONSTRUCTOR()

	SetBounds(sRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H));
	m_OldRect = m_Rect;

	Show(false);

	m_bDragWidget = false;

	m_bZOrderChangable = true;

	// Resizable
	m_bResizable = true;
	m_bMovable = true;

	m_bShade = false;
	m_bCanShade = true;

	m_bTitleBar = true;

	m_iMinWidth = 300;
	m_iMinHeight = 200;
}

CCFrame::~CCFrame(void)
{

}

void CCFrame::OnSize(int w, int h)
{
	ResizeBtnsByAnchors(w, h);
	m_OldRect = m_Rect;
}

bool CCFrame::OnCommand(CCWidget* pWindow, const char* szMessage)
{

	return false;
}

void CCFrame::SetShade(bool bShade)
{
	if (!m_bCanShade) return;

	if (m_bShade == bShade) return;

	if(m_bShade==false){
		m_BeforeShade = sSize(m_Rect.w, m_Rect.h);
		SetSize(m_Rect.w, MTITLEBAR_HEIGHT);
		m_bShade = true;
		m_bResizable = false;
	}
	else{
		SetSize(m_BeforeShade);
		m_bShade = false;
		m_bResizable = true;
	}
}

void ResizeByAnchors(CCFrameBtn* pFrameBtn, sRect parentRect, int w, int h)
{
	sRect r = pFrameBtn->m_Rect;
	if(pFrameBtn->m_Anchors.m_bLeft==true && pFrameBtn->m_Anchors.m_bRight==true)
	{
		r.w += (w-parentRect.w);
	}
	else if(pFrameBtn->m_Anchors.m_bRight==true)
	{
		r.x += (w-parentRect.w);
	}
	if(pFrameBtn->m_Anchors.m_bTop==true && pFrameBtn->m_Anchors.m_bBottom==true)
	{
		r.h += (h-parentRect.h);
	}
	else if(pFrameBtn->m_Anchors.m_bBottom==true)
	{
		r.y += (h-parentRect.h);
	}
	pFrameBtn->m_Rect = r;
}

void CCFrame::ResizeBtnsByAnchors(int w, int h)
{
	ResizeByAnchors(&m_BtnClose, m_OldRect, w, h);
	ResizeByAnchors(&m_BtnMinimize, m_OldRect, w, h);
}

void CCFrame::OnCloseButtonClick()
{
	Show(false);
}
void CCFrame::OnMinimizeButtonClick()
{
	SetShade(!m_bShade);
}
