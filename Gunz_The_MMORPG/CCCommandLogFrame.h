#pragma once

#include "CCFrame.h"
#include "CCListBox.h"

class CCCommand;

class CCCommandLogFrame : public CCFrame{
protected:
	CCListBox*	m_pCommandList;
protected:
	void OnSize(int w, int h);
public:
	CCCommandLogFrame(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCCommandLogFrame();
	void AddCommand(unsigned long int nGlobalClock, CCCommand* pCmd);
	CCListBox* GetCommandList() { return m_pCommandList; }
};