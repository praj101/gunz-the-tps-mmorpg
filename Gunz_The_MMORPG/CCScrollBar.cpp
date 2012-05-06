#include "stdafx.h"
#include "CCScrollBar.h"
#include "CCColorTable.h"
#include "Core.h"

#define CCSCROLLBAR_NAME				"Scroll Bar"


IMPLEMENT_LOOK(CCThumb, CCThumbLook)
IMPLEMENT_LOOK(CCArrow, CCArrowLook)
IMPLEMENT_LOOK(CCScrollBar, CCScrollBarLook)

CCThumb::CCThumb(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCWidget(szName, pParent, pListener){
	LOOK_IN_CONSTRUCTOR()
}

CCArrow::CCArrow(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCButton(szName, pParent, pListener){
	m_iDirection = 0;
	LOOK_IN_CONSTRUCTOR()
}

sSize CCArrow::GetDefaultSize(void){
	return sSize(CONVERT800(CCSCROLLBAR_DEFAULT_WIDTH), CONVERT800(CCSCROLLBAR_DEFAULT_WIDTH));
}

void CCScrollBar::OnRun(void){
	CCListener* pListener = GetListener();
	if(pListener==NULL) 
		return;

	int nCurrTime = timeGetTime();
	if(nCurrTime-m_iPrevThumbRefresh<m_iThumbRefreshDelay) 
		return;

	if(m_pUp->IsButtonDown()==true){
		if(m_iValue>m_iMinValue){
			m_iValue--;
			RecalcThumbPos();
			if(pListener!=NULL) pListener->OnCommand(this, CCLIST_VALUE_CHANGED);
		}
		m_iPrevThumbRefresh = nCurrTime;
	}
	if(m_pDown->IsButtonDown()==true){
		if(m_iValue<m_iMaxValue){
			m_iValue++;
			RecalcThumbPos();
			if(pListener!=NULL) pListener->OnCommand(this, CCLIST_VALUE_CHANGED);
		}
		m_iPrevThumbRefresh = nCurrTime;
	}
}

bool CCScrollBar::OnEvent(CCEvent* pEvent, CCListener* pListener)
{
	sRect r = GetClientRect();
	switch(pEvent->iMessage){
	case CCWM_LBUTTONDOWN:

		if(m_pThumb->GetRect().InPoint(pEvent->sPos)==true){
			SetCapture();
			m_bThumbMove = true;
			if(m_iScrollBarType==CCSBT_VERTICAL){
				m_iThumbPos = m_pThumb->GetRect().y - (r.y+m_pUp->GetRect().h);
				m_iThumbDownPos = pEvent->sPos.y;
			}
			else{
				m_iThumbPos = m_pThumb->GetRect().x - (r.x+m_pUp->GetRect().w);
				m_iThumbDownPos = pEvent->sPos.x;
			}
			return true;
		}
		else
		{
			if(r.InPoint(pEvent->sPos)==true){
				if(!m_pUp->GetRect().InPoint(pEvent->sPos)) {
					if(!m_pUp->GetRect().InPoint(pEvent->sPos)) {

						int nThumbMoveRange = GetThumbMoveRange();
						int nThumbPos;

						if(m_iScrollBarType==CCSBT_VERTICAL)
						{
							nThumbPos  = pEvent->sPos.y;
							nThumbPos -= m_pThumb->GetRect().h/2;
						}
						else
						{
							nThumbPos  = pEvent->sPos.x;
							nThumbPos -= m_pThumb->GetRect().w/2;
						}
						
						if(nThumbPos<0) nThumbPos = 0;
						else if(nThumbPos>=nThumbMoveRange) nThumbPos = nThumbMoveRange-1;

						if(m_iScrollBarType==CCSBT_VERTICAL)
							m_pThumb->SetPosition(m_pThumb->GetRect().x, r.y+m_pUp->GetRect().h + nThumbPos);
						else
							m_pThumb->SetPosition(r.x+m_pUp->GetRect().w + nThumbPos, m_pThumb->GetRect().y);

						float fThumbPos = nThumbPos / (float)nThumbMoveRange;
						m_iValue = int( fThumbPos * (m_iMaxValue-m_iMinValue+1) );
						if(pListener!=NULL) pListener->OnCommand(this, CCLIST_VALUE_CHANGED);

						return true;
					}
				}
			}
		}
		break;
	case CCWM_LBUTTONUP:
		if(m_bThumbMove==true){
			m_bThumbMove = false;
			ReleaseCapture();
			return true;
		}
		break;
	case CCWM_MOUSEMOVE:
		if(m_bThumbMove==true){
			int nThumbMoveRange = GetThumbMoveRange();
			int nThumbPos;
			if(m_iScrollBarType==CCSBT_VERTICAL)
				nThumbPos = m_iThumbPos + pEvent->sPos.y-m_iThumbDownPos;
			else
				nThumbPos = m_iThumbPos + pEvent->sPos.x-m_iThumbDownPos;

			if(nThumbPos<0) nThumbPos = 0;
			else if(nThumbPos>=nThumbMoveRange) nThumbPos = nThumbMoveRange-1;

			if(m_iScrollBarType==CCSBT_VERTICAL)
				m_pThumb->SetPosition(m_pThumb->GetRect().x, r.y+m_pUp->GetRect().h + nThumbPos);
			else
				m_pThumb->SetPosition(r.x+m_pUp->GetRect().w + nThumbPos, m_pThumb->GetRect().y);

			float fThumbPos = nThumbPos / (float)nThumbMoveRange;
			m_iValue = int( fThumbPos * (m_iMaxValue-m_iMinValue+1) ) + m_iMinValue;
			if(pListener!=NULL) pListener->OnCommand(this, CCLIST_VALUE_CHANGED);
			return true;
		}
		break;
	}

	if(pEvent->iMessage!=CCWM_CHAR && r.InPoint(pEvent->sPos)==true) 
		return true;

	return false;
}

void CCScrollBar::OnSize(int w, int h){
	sRect r = GetClientRect();

	sSize s = m_pUp->GetDefaultSize();
	if(m_iScrollBarType==CCSBT_VERTICAL){
		m_pUp->SetBounds(sRect(r.x, r.y, s.w, s.h));
		m_pDown->SetBounds(sRect(r.x, r.y+r.h-s.h, s.w, s.h));
	}
	else{
		m_pUp->SetBounds(sRect(r.x, r.y, s.w, s.h));
		m_pDown->SetBounds(sRect(r.x+r.w-s.w, r.y, s.w, s.h));
	}
	RecalcThumbBounds();
}

int CCScrollBar::GetThumbMoveRange(void){
	sRect r = GetClientRect();
	if(m_iScrollBarType==CCSBT_VERTICAL)
		return ( r.h - (m_pUp->GetRect().h+m_pDown->GetRect().h+m_pThumb->GetRect().h) );
	else
		return ( r.w - (m_pUp->GetRect().w+m_pDown->GetRect().w+m_pThumb->GetRect().w) );
}

int CCScrollBar::GetMoveRange(void){
	sRect r = GetClientRect();
	if(m_iScrollBarType==CCSBT_VERTICAL)
		return ( r.h - (m_pUp->GetRect().h+m_pDown->GetRect().h) );
	else
		return ( r.w - (m_pUp->GetRect().w+m_pDown->GetRect().w) );
}

int CCScrollBar::GetThumbSize(void){
	int nDiff = m_iMaxValue-m_iMinValue;
	int nMoveRange = GetMoveRange();
	int nThumbSize = nMoveRange - nDiff * 3;
	int narrowWidth = ((m_iScrollBarType==CCSBT_VERTICAL) ? GetClientRect().w : GetClientRect().h);
	return max(nThumbSize, narrowWidth);
}

void CCScrollBar::RecalcThumbPos(void){
	sRect r = GetClientRect();
	int nSpace = GetThumbMoveRange();
	int nValueWidth = m_iMaxValue - m_iMinValue;
	int nThumbPos = 0;
	if(nValueWidth!=0) nThumbPos = nSpace * m_iValue / nValueWidth;

	if(m_iScrollBarType==CCSBT_VERTICAL)
		m_pThumb->SetPosition(r.x, r.y+m_pUp->GetRect().h+nThumbPos);
	else
		m_pThumb->SetPosition(r.x+m_pUp->GetRect().w+nThumbPos, r.y);
}

void CCScrollBar::RecalcThumbBounds(void){
	sRect r = GetClientRect();
	if(m_iScrollBarType==CCSBT_VERTICAL)
		m_pThumb->SetSize(r.w, GetThumbSize());
	else
		m_pThumb->SetSize(GetThumbSize(), r.h);

	RecalcThumbPos();
}

void CCScrollBar::Initialize(CCScrollBarTypes t){
	LOOK_IN_CONSTRUCTOR()

	m_iMinValue = 0;
	m_iMaxValue = 99;
	m_iValue = 0;

	m_pUp = new CCArrow(NULL, this, this);
	sSize s = m_pUp->GetDefaultSize();
	m_pUp->SetSize(s.w, s.h);
	m_pDown = new CCArrow(NULL, this, this);
	m_pDown->SetSize(s.w, s.h);
	m_pThumb = new CCThumb(NULL, this, this);
	m_pThumb->SetSize(s.w, s.h);

	SetType(t);

	m_iThumbRefreshDelay = 80;
	m_iPrevThumbRefresh = 0;

	m_bThumbMove = false;
}

CCScrollBar::CCScrollBar(const char* szName, CCWidget* pParent, CCListener* pListener, CCScrollBarTypes t)
: CCWidget(szName, pParent, pListener){
	Initialize(t);
}

CCScrollBar::CCScrollBar(CCWidget* pParent, CCListener* pListener, CCScrollBarTypes t)
: CCWidget(CCSCROLLBAR_NAME, pParent, pListener){
	Initialize(t);
}

CCScrollBar::~CCScrollBar(void){
	if(m_pUp!=NULL) delete m_pUp;
	if(m_pDown!=NULL) delete m_pDown;
	if(m_pThumb!=NULL) delete m_pThumb;
}

void CCScrollBar::SetMinMax(int nMin, int nMax){
	m_iMinValue = nMin;
	m_iMaxValue = nMax;
	_ASSERT(nMin<=nMax);

	RecalcThumbBounds();
}

void CCScrollBar::SetValue(int nValue){
	if(nValue>=m_iMinValue && nValue<=m_iMaxValue){
		m_iValue = nValue;
		RecalcThumbPos();
	}
}

void CCScrollBar::SetValueAdjusted(int nValue){
	m_iValue = nValue;
	m_iValue = max(m_iValue, m_iMinValue);
	m_iValue = min(m_iValue, m_iMaxValue);
	RecalcThumbPos();
}

int CCScrollBar::GetValue(void){
	return m_iValue;
}

CCScrollBarTypes CCScrollBar::GetType(void){
	return m_iScrollBarType;
}

void CCScrollBar::SetType(CCScrollBarTypes t){
	m_iScrollBarType = t;

	m_pThumb->m_iDirection = (t==CCSBT_HORIZONTAL)?0:1;

	sSize s = m_pUp->GetDefaultSize();

	if(m_iScrollBarType==CCSBT_VERTICAL)
		SetSize(s.w, CCSCROLLBAR_DEFAULT_HEIGHT);
	else
		SetSize(CCSCROLLBAR_DEFAULT_HEIGHT, s.h);

	if(t==CCSBT_VERTICAL){
		((CCArrow*)m_pUp)->m_iDirection = 0;
		((CCArrow*)m_pDown)->m_iDirection = 1;
	}
	else{
		((CCArrow*)m_pUp)->m_iDirection = 3;
		((CCArrow*)m_pDown)->m_iDirection = 4;
	}
}

void CCScrollBar::ChangeCustomArrowLook(CCArrowLook *pArrowLook){
	m_pUp->ChangeCustomLook(pArrowLook);
	m_pDown->ChangeCustomLook(pArrowLook);
}

void CCScrollBar::ChangeCustomThumbLook(CCThumbLook *pThumbLook){
	m_pThumb->ChangeCustomLook(pThumbLook);
}

int CCScrollBar::GetDefaultBreadth(void){
	sSize s = m_pUp->GetDefaultSize();
	if(GetType()==CCSBT_VERTICAL) return s.w;
	else return s.h;
}

void CCThumbLook::OnDraw(CCThumb* pThumb, CCDrawContext* pDC){
	pDC->SetColor(sColor(DEFCOLOR_FRAME_OUTLINE));
	pDC->FillRectangle(pThumb->GetClientRect());
}

sRect CCThumbLook::GetClientRect(CCThumb* pThumb, sRect& r){
	return r;
}

void CCArrowLook::OnDrawUpArrow(CCDrawContext* pDC, sRect& r, bool bPressed){
	if(bPressed==true){
		r.x ++;
		r.y ++;
	}

	pDC->SetColor(sColor(DEFCOLOR_FRAME_OUTLINE));
	for(int yy=0; yy<r.h; yy++){
		int rw = r.w*yy/r.h;
		pDC->HLine(r.x+r.w/2-rw/2, r.y+yy, rw);
	}
}
void CCArrowLook::OnDrawDownArrow(CCDrawContext* pDC, sRect& r, bool bPressed){
	if(bPressed==true){
		r.x ++;
		r.y ++;
	}

	pDC->SetColor(sColor(DEFCOLOR_FRAME_OUTLINE));
	for(int yy=0; yy<r.h; yy++){
		int rw = r.w*yy/r.h;
		pDC->HLine(r.x+r.w/2-rw/2, r.y+r.h-1-yy, rw);
	}
}
void CCArrowLook::OnDrawLeftArrow(CCDrawContext* pDC, sRect& r, bool bPressed){
	if(bPressed==true){
		r.x ++;
		r.y ++;
	}

	pDC->SetColor(sColor(DEFCOLOR_FRAME_OUTLINE));
	for(int xx=0; xx<r.w; xx++){
		int rh = r.h*xx/r.w;
		pDC->VLine(r.x+xx, r.y+r.h/2-rh/2, rh);
	}
}
void CCArrowLook::OnDrawRightArrow(CCDrawContext* pDC, sRect& r, bool bPressed){
	if(bPressed==true){
		r.x ++;
		r.y ++;
	}

	pDC->SetColor(sColor(DEFCOLOR_FRAME_OUTLINE));
	for(int xx=0; xx<r.w; xx++){
		int rh = r.h*xx/r.w;
		pDC->VLine(r.x+r.w-1-xx, r.y+r.h/2-rh/2, rh);
	}
}
void CCArrowLook::OnDraw(CCArrow* pArrow, CCDrawContext* pDC){
	int x = 0;
	int y = 0;
	bool bPressed = false;
	if(pArrow->IsButtonDown()==true || (pArrow->GetType()==CCBT_PUSH && pArrow->GetCheck()==true)) bPressed = true;
	sRect r = pArrow->GetRect();
	r.x = r.y = 0;
	switch(pArrow->m_iDirection){
	case 0:
		OnDrawUpArrow(pDC, sRect(r.x+x, r.y+y, r.w, r.h), bPressed);
		break;
	case 1:
		OnDrawDownArrow(pDC, sRect(r.x+x, r.y+y, r.w, r.h), bPressed);
		break;
	case 3:
		OnDrawLeftArrow(pDC, sRect(r.x+x, r.y+y, r.w, r.h), bPressed);
		break;
	case 4:
		OnDrawRightArrow(pDC, sRect(r.x+x, r.y+y, r.w, r.h), bPressed);
		break;
	default:
		pDC->FillRectangle(r.x+x, r.y+y, r.w, r.h);
	}
}

sRect CCArrowLook::GetClientRect(CCArrow* pArrow, sRect& r){
	return r;
}

sSize CCArrowLook::GetDefaultSize(CCArrow* pThumb){
	return sSize(CONVERT800(CCSCROLLBAR_DEFAULT_WIDTH), CONVERT800(CCSCROLLBAR_DEFAULT_WIDTH));
}

void CCScrollBarLook::OnDraw(CCScrollBar* pScrollBar, CCDrawContext* pDC){
	if(pScrollBar->m_iDebugType==3)	{
		int k=0;
	}

	sRect r = pScrollBar->GetInitialClientRect();
	pDC->SetColor(sColor(0xFF000000));
	pDC->FillRectangle(r.x, r.y, r.w, r.h);
}

sRect CCScrollBarLook::GetClientRect(CCScrollBar* pScrollBar, sRect& r)
{
	return r;
}
