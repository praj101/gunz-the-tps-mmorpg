#pragma once

#include "CCWidget.h"

#define CC_ANIMATION "Animation"

enum CCPlayMode{
	CCPM_FORWARDONCE,
	CCPM_FORWARDNBACKWARD,
	CCPM_REPETITION
};

class CCAnimation : public CCWidget{
private:
	uint			m_iStartTime;
	CCAniBitmap*	m_pAniBitmap;

public:
	int				m_iDelay;
	CCPlayMove		m_iPlayMode;

protected:
	virtual void	OnDraw(CCDrawContext* pDC);

public:
	CCAnimation(const char* szName=NULL, CCAniBitmap* pAniBitmap=NULL, CCWidget* pParent=NULL);
	
	void SetAniBitmap(CCAniBitmap* pAniBitmap);
	void InitStartTime();
	CCBitmap* GetBitamp();

	virtual const char* GetClassName() { return CC_ANIMATION; };

protected:
	int				m_iCurrentFrame;

public:
	bool			m_bRunAnimation;
	bool			GetRunAnimation(){ return m_bRunAnimation;};
	void			SetRunAnimation(bool b);
	int				GetCurrentFrame();
	void			SetCurrentFrame(int iFrame);
};