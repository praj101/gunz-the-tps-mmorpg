#include "stdafx.h"
#include "CCEdit.h"
#include "CCColorTable.h"
#include <mmsystem.h>
#include <stdio.h>
#include <memory.h>
#include "Core.h"
#include "CCDebug.h"

#define CCEDIT_DEFAULT_WIDTH		100
IMPLEMENT_LOOK(CCEdit, CCEditLook);

static bool g_bFocus = false;

bool InsertString(char* szTarget, const char* szInsert, int nPos, int nMaxTargetLen=-1){
	int nTargetLen = strlen(szTarget);
	int nInsertLen = strlen(szInsert);
	if(nPos>nTargetLen) 
		return false;
	if(nMaxTargetLen>0 && nTargetLen+nInsertLen>=nMaxTargetLen) 
		return false;
	char* temp = new char[nTargetLen-nPos+2];
	strcpy(temp, szTarget+nPos);
	strcpy(szTarget+nPos, szInsert);
	strcpy(szTarget+nPos+nInsertLen, temp);
	delete[] temp;

	return true;
}

int DeleteChar(char* szTarget, int nPos){
	int nTargetLen = strlen(szTarget);
	if(nPos>=nTargetLen || nPos<0) 
		return 0;
	int nCount = (IsHangul(szTarget[nPos])==true)?2:1;
	if(nPos+nCount>nTargetLen) nCount = 1;
	char* temp = new char[nTargetLen+2];
	strcpy(temp, szTarget);
	strcpy(szTarget+nPos, temp+nPos+nCount);
	delete[] temp;

	return nCount;
}

int NextPos(const char* szText, int nPos){
	int nLen = strlen(szText);
	if(nPos>=nLen) 
		return nLen;
	if(IsHangul(szText[nPos])==true && nPos<nLen) 
		return (nPos+2);
	return (nPos+1);
}

int PrevPos(char* szText, int nPos){
	int nLen = strlen(szText);
	if(nPos<=1) return 0;
	int nCurPos=0;
	while(1)	{
		int nNext = nCurPos + (IsHangul(szText[nCurPos]) ? 2 : 1);
		if(nNext>=nPos) return nCurPos;
		nCurPos=nNext;
	}
}

void CCEdit::OnHide(){
	if(GetTabHandler()){
		GetTabHandler()->Show(false);
	}
}

void CCEdit::OnSetFocus(){
	Core::GetInstance()->EnableIME(true);
}

void CCEdit::OnReleaseFocus(){
	Core::GetInstance()->EnableIME(false);

	CCListener* pListener = GetListener();
	if(pListener!=NULL)
		pListener->OnCommand(this, CCEDIT_KILL_FOCUS);
}

bool CCEdit::OnEvent(CCEvent *pEvent, CCListener *pListener){
	static bool bMyEvent = false;
	if(CCWidget::m_pFocusedWidget!=this)
		return false;
	switch(pEvent->iMessage){
		case CCWM_KEYDOWN:{
			bool ret= InputFilterKey(pEvent->iKey);
			CCListener* pListener = GetListener();
			if(pListener!=NULL)
				pListener->OnCommand(this, CCEDIT_KEYDOWN_MSG);
			return ret;
		}
		break;
		case CCWM_CHAR:{
			bool ret = false;
			if(InputFilterChar(pEvent->iKey)==false){
				int nLen = strlen(m_pBuffer);
				if(nLen < m_iMaxLength-1){
					char temp[2] = {(char)pEvent->iKey, 0};
					if(InsertString(m_pBuffer, temp, m_iCaretPos, m_iMaxLength)==true){
						m_iCaretPos++;
						OnChangedText();
					}
					_ASSERT(m_iCaretPos>=0 && m_iCaretPos<=(int)strlen(m_pBuffer));
				}
				ret = true;
			}
			CCListener* pListener = GetListener();
		}
	}
}