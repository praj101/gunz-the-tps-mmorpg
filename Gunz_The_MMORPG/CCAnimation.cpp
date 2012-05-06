#include "stdafx.h"
#include "CCAnimation.h"

#define DELAY_CONSTANT	1

void CCAnimation::OnDraw(CCDrawContext* pDC){
	if ( m_pAniBitmap == NULL)
		return;

	int nCurTime = timeGetTime();						// Get current time
	int nCurFrame = GetCurrentFrame();					// Get current frame
	int nFrameCount = m_pAniBitmap->GetFrameCount();	// Get frame count

	if ( m_iPlayMode == CCPM_FORWARDONCE){
		m_iCurrentFrame = nCurFrame;

		if ( nCurFrame >= nFrameCount)
            m_iCurrentFrame = nFrameCount - 1;
	}

	else if ( m_iPlayMode == CCPM_REPETITION)
		m_iCurrentFrame = nCurFrame % nFrameCount;

	else if ( m_iPlayMode == CCPM_FORWARDNBACKWARD)
	{
		int nIterCount = nCurFrame / nFrameCount;
		m_iCurrentFrame = nCurFrame % nFrameCount;
		if ( (nIterCount % 2) == 1)
			m_iCurrentFrame = nFrameCount - m_iCurrentFrame - 1;
	}

	CCBitmap* pBitmap = m_pAniBitmap->Get( m_iCurrentFrame);
	pDC->SetBitmap( pBitmap);
	sRect r = GetClientRect();
	pDC->Draw( r.x, r.y, r.w, r.h);
}

CCBitmap* CCAnimation::GetBitmap(){
	if(m_pAniBitmap == NULL) return NULL;
	int nCurTime = timeGetTime();
	int nCurFrame;
	if(m_iDelay!=0) nCurFrame = (nCurTime-m_iStartTime) / (m_iDelay*DELAY_CONSTANT);
	else nCurFrame = (nCurTime-m_iStartTime) / DELAY_CONSTANT;
	int nFrameCount = m_pAniBitmap->GetFrameCount();
	if(nFrameCount==0) return NULL;

	if(m_iPlayMode==CCPM_FORWARDONCE){
		if(nCurFrame>=nFrameCount) nCurFrame = nFrameCount-1;
	}
	else if(m_iPlayMode==CCPM_REPETITION){
		nCurFrame = nCurFrame % nFrameCount;
	}
	else if(m_iPlayMode==CCPM_FORWARDNBACKWARD){
		int nIterCount = nCurFrame/nFrameCount;
		nCurFrame = (nCurFrame%nFrameCount);
		if(nIterCount%2==1){
			nCurFrame = nFrameCount - nCurFrame - 1;
		}
	}

	return m_pAniBitmap->Get(nCurFrame);
}

CCAnimation::CCAnimation(const char* szName, CCAniBitmap* pAniBitmap, CCWidget* pParent)
: CCWidget(szName, pParent, NULL){
	m_pAniBitmap = pAniBitmap;
	if(pAniBitmap!=NULL) m_iDelay = pAniBitmap->GetDelay();
	else m_iDelay = 1;	// Default
	m_iStartTime = timeGetTime();

	m_iPlayMode = CCPM_FORWARDONCE;
	m_iCurrentFrame = 0;
	m_bRunAnimation = true;
}

void CCAnimation::SetAniBitmap(CCAniBitmap* pAniBitmap){
	m_pAniBitmap = pAniBitmap;
	if(pAniBitmap!=NULL) m_iDelay = pAniBitmap->GetDelay();
}

void CCAnimation::InitStartTime(){
	m_iStartTime = timeGetTime();
}


void CCAnimation::SetRunAnimation( bool bRun){
	m_bRunAnimation = bRun;
	SetCurrentFrame( m_iCurrentFrame);
}

int CCAnimation::GetCurrentFrame( void){
	if ( !m_bRunAnimation)
		return m_iCurrentFrame;

	int nCurrTime = timeGetTime();		// Get current time
	int nCurrFrame;						// Get current frame

	if ( m_iDelay != 0)
		nCurrFrame = (nCurrTime - m_iStartTime) / (m_iDelay * DELAY_CONSTANT);
	else
		nCurrFrame = (nCurrTime - m_iStartTime) / DELAY_CONSTANT;

	return nCurrFrame;
}

void CCAnimation::SetCurrentFrame( int nFrame){
	m_iCurrentFrame = nFrame;

	if ( nFrame < m_pAniBitmap->GetFrameCount())
        m_iStartTime = timeGetTime() - ( nFrame * m_iDelay * DELAY_CONSTANT);
}
