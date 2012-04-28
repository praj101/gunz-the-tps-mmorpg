#include "stdafx.h"
#include "MAnimation.h"

#define DELAY_CONSTANT	1

void CCAnimation::OnDraw(CCDrawContext* pDC){
	if ( m_pAniBitmap == NULL)
		return;

	int nCurTime = timeGetTime();						// Get current time
	int nCurFrame = GetCurrentFrame();					// Get current frame
	int nFrameCount = m_pAniBitmap->GetFrameCount();	// Get frame count

	if ( m_nPlayMode == CCPM_FORWARDONCE){
		m_nCurrFrame = nCurFrame;

		if ( nCurFrame >= nFrameCount)
            m_nCurrFrame = nFrameCount - 1;
	}

	else if ( m_nPlayMode == CCPM_REPETITION)
		m_nCurrFrame = nCurFrame % nFrameCount;

	else if ( m_nPlayMode == CCPM_FORWARDNBACKWARD)
	{
		int nIterCount = nCurFrame / nFrameCount;
		m_nCurrFrame = nCurFrame % nFrameCount;
		if ( (nIterCount % 2) == 1)
			m_nCurrFrame = nFrameCount - m_nCurrFrame - 1;
	}

	CCBitmap* pBitmap = m_pAniBitmap->Get( m_nCurrFrame);
	pDC->SetBitmap( pBitmap);
	sRect r = GetClientRect();
	pDC->Draw( r.x, r.y, r.w, r.h);
}

CCBitmap* CCAnimation::GetBitmap(){
	if(m_pAniBitmap == NULL) return NULL;
	int nCurTime = timeGetTime();
	int nCurFrame;
	if(m_nDelay!=0) nCurFrame = (nCurTime-m_nStartTime) / (m_nDelay*DELAY_CONSTANT);
	else nCurFrame = (nCurTime-m_nStartTime) / DELAY_CONSTANT;
	int nFrameCount = m_pAniBitmap->GetFrameCount();
	if(nFrameCount==0) return NULL;

	if(m_nPlayMode==CCPM_FORWARDONCE){
		if(nCurFrame>=nFrameCount) nCurFrame = nFrameCount-1;
	}
	else if(m_nPlayMode==CCPM_REPETITION){
		nCurFrame = nCurFrame % nFrameCount;
	}
	else if(m_nPlayMode==CCPM_FORWARDNBACKWARD){
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
	if(pAniBitmap!=NULL) m_nDelay = pAniBitmap->GetDelay();
	else m_nDelay = 1;	// Default
	m_nStartTime = timeGetTime();

	m_nPlayMode = CCPM_FORWARDONCE;
	m_nCurrFrame = 0;
	m_bRunAnimation = true;
}

void CCAnimation::SetAniBitmap(CCAniBitmap* pAniBitmap){
	m_pAniBitmap = pAniBitmap;
	if(pAniBitmap!=NULL) m_nDelay = pAniBitmap->GetDelay();
}

void CCAnimation::InitStartTime(){
	m_nStartTime = timeGetTime();
}


void CCAnimation::SetRunAnimation( bool bRun){
	m_bRunAnimation = bRun;
	SetCurrentFrame( m_nCurrFrame);
}

int MAnimation::GetCurrentFrame( void){
	if ( !m_bRunAnimation)
		return m_nCurrFrame;

	int nCurrTime = timeGetTime();		// Get current time
	int nCurrFrame;						// Get current frame

	if ( m_nDelay != 0)
		nCurrFrame = (nCurrTime - m_nStartTime) / (m_nDelay * DELAY_CONSTANT);
	else
		nCurrFrame = (nCurrTime - m_nStartTime) / DELAY_CONSTANT;

	return nCurrFrame;
}

void CCAnimation::SetCurrentFrame( int nFrame){
	m_nCurrFrame = nFrame;

	if ( nFrame < m_pAniBitmap->GetFrameCount())
        m_nStartTime = timeGetTime() - ( nFrame * m_nDelay * DELAY_CONSTANT);
}
