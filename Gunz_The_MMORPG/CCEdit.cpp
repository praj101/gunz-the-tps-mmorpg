#include "stdafx.h"
#include "CCEdit.h"
#include "CCColorTable.h"
#include <mmsystem.h>
#include <stdio.h>
#include <memory.h>
#include "Core.h"
#include "CCDebug.h"

#define CCEDIT_DEFAULT_WIDTH		100
#define BUFFERSIZE	1024
#define INDICATOR_WIDTH	10

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
			if(pListener!=NULL) pListener->OnCommand(this, CCEDIT_CHAR_MSG);
			return ret;
		}
		break;
		case CCWM_IMECONVERSION:{
			if(g_bFocus) OnSetFocus();
		}
		return false;
		case CCWM_IMECOMPOSE:{
			strcpy(m_szIMECompositionString, pEvent->szIMECompositionString);
			if(pEvent->szIMECompositionResultString[0]!=NULL){
				if(InsertString(m_pBuffer, pEvent->szIMECompositionResultString, m_iCaretPos, m_iMaxLength)==true){
					m_iCaretPos += strlen(pEvent->szIMECompositionResultString);
					_ASSERT(m_iCaretPos>=0 && m_iCaretPos<=(int)strlen(m_pBuffer));
					OnChangedText();
				}
			}
			CCListener* pListener = GetListener();
			if(pListener!=NULL) 
				pListener->OnCommand(this, CCEDIT_CHAR_MSG);
		}
		return true;
		case CCWM_LBUTTONDOWN:{
			sRect r = GetClientRect();
			if(r.InPoint(pEvent->sPos)==true){
				int nPos = GetPosByScreen(pEvent->sPos.x);
				if(nPos<0) return false;
				m_iCaretPos = nPos;
				bMyEvent = true;
				return true;
			}
			return false;
		}
		case CCWM_MOUSEMOVE:{			
			if( !bMyEvent ) 
				return false;
			m_iSelectionRange = 0;
			int nCaretPos = 0;
			sRect r = GetClientRect();
			if(r.InPoint(pEvent->sPos)==true){
				int nPos = GetPosByScreen(pEvent->sPos.x);
				if(m_iCaretPos==nPos) 
					return false;
				m_iSelectionRange = nPos - m_iCaretPos;
			}
			return true;
		}
		case CCWM_LBUTTONUP:{
			if(!bMyEvent) 
				return false;
            bMyEvent = false;
			return true;
		}
	}
	return false;
}

void CCEdit::OnChangedText(){
	CCListener* pListener = GetListener();
	if(pListener!=NULL) pListener->OnCommand(this, CCEDIT_TEXT_CHANGED);
}

bool CCEdit::InputFilterKey(int iKey){
	if(iKey==VK_DELETE){
		int nCount = DeleteChar(m_pBuffer, m_iCaretPos);
		OnChangedText();
		return true;
	}
	else if(iKey==VK_LEFT){
		MoveCaretPrev();
		return true;
	}
	else if(iKey==VK_RIGHT){
		MoveCaretNext();
		return true;
	}
	else if(iKey==VK_UP){
		if(m_bSupportHistory==false) 
			return true;

		if(m_iCurrentHistory==m_History.end()){
			m_iCurrentHistory = m_History.end();
			m_iCurrentHistory--;
		}
		else if(m_iCurrentHistory!=m_History.begin()){
			m_iCurrentHistory--;
		}

		if(m_iCurrentHistory!=m_History.end())
			SetText(*m_iCurrentHistory);

		return true;
	}
	else if(iKey==VK_DOWN){
		if(m_bSupportHistory==false) 
			return true;

		if(m_iCurrentHistory!=m_History.end()){
			m_iCurrentHistory++;
		}

		if(m_iCurrentHistory!=m_History.end())
			SetText(*m_iCurrentHistory);
		else
			SetText("");

		return true;
	}
	else if(iKey==VK_HOME){
		MoveCaretHome();
		return true;
	}
	else if(iKey==VK_END){
		MoveCaretEnd();
		return true;
	}
	else if(iKey==VK_RETURN){
		if(m_bSupportHistory==true) AddHistory(GetText());
		CCListener* pListener = GetListener();
		if(pListener!=NULL) return pListener->OnCommand(this, CCEDIT_ENTER_VALUE);
		return false;
	}
	else if(iKey==VK_TAB){
		if (GetTabHandler()) {
			if (GetTabHandler()->IsVisible())
				GetTabHandler()->Show(false);
			else
				GetTabHandler()->Show(true,true);
			return true;
		} else {
			return false;
		}			
	}
	return false;
}

bool CCEdit::InputFilterChar(int iKey){
	if(iKey==VK_BACK){
		int nCaretPos = PrevPos(m_pBuffer, m_iCaretPos);
		if(nCaretPos!=m_iCaretPos){
			int nCount = DeleteChar(m_pBuffer, nCaretPos);
			m_iCaretPos = nCaretPos;
			_ASSERT(m_iCaretPos>=0 && m_iCaretPos<=(int)strlen(m_pBuffer));
			OnChangedText();
		}
		if(m_iCaretPos<m_iStartPos) 
			m_iStartPos = m_iCaretPos;

		return true;
	}
	else if(iKey==VK_ESCAPE){
		CCListener* pListener = GetListener();
		if(pListener!=NULL) pListener->OnCommand(this, CCEDIT_ESC_VALUE);
		return true;
	}
	else if(iKey==22){
		char* temp = new char[m_iMaxLength];
		memset(temp, 0, m_iMaxLength);
		if ( GetClipboard(temp, m_iMaxLength)==true){
			char *pFirst = strstr(temp,"\r");
			if(pFirst!=NULL){
				*pFirst = 0;
				*(pFirst+1) = 0;
			}
			pFirst = strstr(temp,"\n");
			if(pFirst!=NULL){
				*pFirst = 0;
				*(pFirst+1) = 0;
			}
			AddText(temp);
		}
		delete temp;
		return true;
	}
	else if(iKey==3){
		SetClipboard(GetText());
		return true;
	}

	switch(iKey){
	case VK_TAB:
	case VK_RETURN:
	case VK_NONCONVERT:
		return true;
	}

	if(iKey<27) return true;

	if(IsNumberField()){
		if (iKey < '0' || '9' < iKey)
			return true;
	}

	return false;
}

void CCEdit::Initialize(int nMaxLength, const char* szName){
	LOOK_IN_CONSTRUCTOR()

	m_iMaxLength = nMaxLength+2;
	m_bMouseOver = false;

	CCFont* pFont = GetFont();
	int w = CCEDIT_DEFAULT_WIDTH;
	int h = pFont->GetHeight()+2;

	m_pBuffer = new char[m_iMaxLength];
	m_pBuffer[0] = NULL;
	if(szName!=NULL) SetText(szName);

	m_szIMECompositionString[0] = NULL;

	m_bPassword = false;
	m_bNumberOnly = false;

	m_iCaretPos = 0;
	m_iStartPos = 0;
	m_iSelectionRange = 0;

	m_iCurrentHistory = m_History.end();
	m_bSupportHistory = true;

	SetFocusEnable(true);

	SetTabHandler(NULL);
}

CCEdit::CCEdit(int nMaxLength, const char* szName, CCWidget* pParent, CCListener* pListener )
 : CCWidget(szName, pParent, pListener){
	Initialize(nMaxLength, szName);
}

CCEdit::CCEdit(const char* szName, CCWidget* pParent, CCListener* pListener)
 : CCWidget(szName, pParent, pListener){
	Initialize(200, szName);
}

CCEdit::~CCEdit(){
	if(m_pBuffer){
		delete []m_pBuffer;
		m_pBuffer = NULL;
	}

	while(m_History.empty()==false){
		char* szText = *m_History.begin();
		delete szText;
		m_History.erase(m_History.begin());
	}
}

void CCEdit::SetText(const char* szText){
	if(m_pBuffer) {
		strncpy(m_pBuffer, szText, m_iMaxLength-1);
		m_pBuffer[m_iMaxLength-1]=0;
		OnChangedText();
	}

	MoveCaretEnd();
	if(m_iStartPos>=m_iCaretPos)
		m_iStartPos=m_iCaretPos;
}

const char* CCEdit::GetText(void){
	return m_pBuffer;
}

void CCEdit::AddText(const char* szText){
	char		temp[ 1024 ]	= {0,};
	const int	nInputSize		= strlen( szText );
	const int	nCurSize		= strlen( GetText() );

	if( 1024 > (nInputSize, nCurSize) ){
		sprintf(temp, "%s%s", GetText(), szText);
		SetText(temp);
	}
	else{
		char szInputText[ 1024 ] = {0,};
		strncpy( szInputText, szText, 1024 - nCurSize );
		sprintf( temp, "%s%s", GetText(), szInputText );
	}
}

int CCEdit::MoveCaretHome(void){
	m_iCaretPos = 0;
	return m_iCaretPos;
}

int CCEdit::MoveCaretEnd(void){
	m_iCaretPos = strlen(m_pBuffer);
	return m_iCaretPos;
}

int CCEdit::MoveCaretPrev(void){
	m_iCaretPos = PrevPos(m_pBuffer, m_iCaretPos);
	if(m_iCaretPos<m_iStartPos)
	{
		m_iStartPos=m_iCaretPos;
	}
	_ASSERT(m_iCaretPos>=0 && m_iCaretPos<=(int)strlen(m_pBuffer));
	return m_iCaretPos;
}

int CCEdit::MoveCaretNext(void){
	m_iCaretPos = NextPos(m_pBuffer, m_iCaretPos);
	_ASSERT(m_iCaretPos>=0 && m_iCaretPos<=(int)strlen(m_pBuffer));
	return m_iCaretPos;
}

int CCEdit::GetMaxLength(void){
	return m_iMaxLength;
}

const char* CCEdit::GetCompositionString(void){
	return m_szIMECompositionString;
}

int CCEdit::GetCarretPos(void){
	return m_iCaretPos;
}

bool CCEdit::SetStartPos(int nStartPos){
	int nTextLen = strlen(GetText());
	if (nTextLen <= 0) {
		m_iStartPos = 0;
		return false;
	}
	if(nStartPos<0 || nStartPos>=nTextLen) 
		return false;
	m_iStartPos = nStartPos;

	return true;
}

int CCEdit::GetPosByScreen(int x){
	int nLen = strlen(m_pBuffer+GetStartPos());
	if(nLen==0) return -1;
	int i=0;
	CCFont* pFont = GetFont();
	sRect r = GetClientRect();
	for(i=nLen; i>0; i=PrevPos(m_pBuffer+GetStartPos(), i)){
		int nWidth = CCGetWidth(pFont, m_pBuffer+GetStartPos(),i);
		if(x>r.x+nWidth){
			return i+GetStartPos();
		}
	}
	if(i==0) 
		return GetStartPos();

	return -1;
}

void CCEdit::SetPasswordField(bool bPassword){
	m_bPassword = bPassword;
}

bool CCEdit::IsPasswordField(void){
	return m_bPassword;
}

void CCEdit::SetNumberField(bool b){
	m_bNumberOnly = b;
}

bool CCEdit::IsNumberField(){
	return m_bNumberOnly;
}

bool CCEdit::GetClipboard(char* szText, int nSize){
	if ( m_bPassword)
		return false;

	if(OpenClipboard(NULL)==FALSE)
		return false;

	HANDLE hClipData = GetClipboardData(CF_TEXT);
	if(hClipData==NULL){
		CloseClipboard();
		return false;
	}
	
	HANDLE hText = GlobalAlloc(GMEM_MOVEABLE, GlobalSize(hClipData));
	if(hText==NULL){
		CloseClipboard();
		return false;
	}
	char* lpClipData = (char *)GlobalLock(hClipData);
	if(lpClipData==NULL){
		CloseClipboard();
		return false;
	}

	if((int)strlen(lpClipData)>(nSize-2)){
		memcpy(szText, lpClipData, nSize-1);
		szText[nSize-1] = 0;
	}
	else{
		strcpy(szText, lpClipData);
	}

	GlobalUnlock(hClipData);

	CloseClipboard();

	GlobalUnlock(hText);

	return true;
}

bool CCEdit::SetClipboard(const char* szText){
	HANDLE hData = GlobalAlloc(GMEM_DDESHARE, strlen(szText)+2);
    if(hData==NULL){
        return false;
    }

	char* lpData = (char*)GlobalLock(hData);
    if(lpData==NULL){
		return false;
    }

	strcpy(lpData, szText);

    GlobalUnlock(hData);

    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        SetClipboardData(CF_TEXT, hData);
        CloseClipboard();
    }
	return true;
}

void CCEdit::AddHistory(const char* szText){
	if(szText[0]==0) return;
	char* szNew = new char[strlen(szText)+2];
	strcpy(szNew, szText);
	m_History.insert(m_History.end(), szNew);
	m_iCurrentHistory = m_History.end();
}

void CCEdit::SetMaxLength(int nMaxLength){
	nMaxLength += 1;
	char *pNewBuffer = new char[nMaxLength];
	ZeroMemory(pNewBuffer,nMaxLength);

	if (m_pBuffer != NULL) {
		strncpy(pNewBuffer,m_pBuffer,min(nMaxLength,m_iMaxLength));
		delete []m_pBuffer;
		m_pBuffer = pNewBuffer;
		OnChangedText();
	}

	m_iMaxLength = nMaxLength;

	MoveCaretEnd();
	if(m_iStartPos>=m_iCaretPos)
		m_iStartPos=m_iCaretPos;
}

void CCEditLook::OnFrameDraw(CCEdit* pEdit, CCDrawContext* pDC){
	sRect r = pEdit->GetInitialClientRect();
	pDC->SetColor(sColor(DEFCOLOR_CCEDIT_PLANE));
	pDC->FillRectangle(r);
	pDC->SetColor(sColor(DEFCOLOR_CCEDIT_OUTLINE));
	pDC->Rectangle(r);
}

void CCEditLook::OnTextDraw(CCEdit* pEdit, CCDrawContext* pDC, bool bShowLanguageTab ){
	char szBuffer[BUFFERSIZE];
	_ASSERT(sizeof(szBuffer)>pEdit->GetMaxLength()+2);
	if(pEdit->GetMaxLength()+2>BUFFERSIZE) 
		return;

	if(pEdit->IsPasswordField()==false){
		wsprintf(szBuffer, "%s", pEdit->GetText());
	}
	else{
		memset(szBuffer, '*', strlen(pEdit->GetText()));
		szBuffer[strlen(pEdit->GetText())] = '\0';
	}

	if(pEdit->IsFocus()==true && pEdit->GetCompositionString()[0]!=NULL){
		InsertString(szBuffer, pEdit->GetCompositionString(), pEdit->GetCarretPos());
	}

	pDC->SetColor(sColor(DEFCOLOR_CCEDIT_TEXT));

	CCFont* pFont = pDC->GetFont();
	sRect r = pEdit->GetClientRect();
	r.x-=2;
	sRect scr=MClientToScreen(pEdit,r);
	pDC->SetClipRect(scr);

	if( bShowLanguageTab )
		r.w-=pFont->GetHeight();

	if(pEdit->GetCarretPos()<pEdit->GetStartPos())
		pEdit->SetStartPos(pEdit->GetCarretPos());

	_ASSERT(pEdit->GetCarretPos()>=pEdit->GetStartPos());

	int nCompositionStringLength = strlen(pEdit->GetCompositionString());

	char* szTemp = NULL;
	szTemp = szBuffer+pEdit->GetStartPos();
	int nTestLen = pEdit->GetCarretPos()-pEdit->GetStartPos()+nCompositionStringLength;

	while(szTemp[0]!=NULL && r.w-INDICATOR_WIDTH < pFont->GetWidth(szTemp, nTestLen)){
		int nStartPos = NextPos(pEdit->GetText(), pEdit->GetStartPos());
		if(pEdit->SetStartPos(nStartPos)==false) break;
		szTemp = szBuffer+pEdit->GetStartPos();
		nTestLen = pEdit->GetCarretPos()-pEdit->GetStartPos()+nCompositionStringLength;
	}

	char* szStartText = szBuffer+pEdit->GetStartPos();

	if(pEdit->IsFocus()==true){
		int nFontHeight = pFont->GetHeight();
		Core* pCore = Core::GetInstance();

		int nInsertPosInWidget = pFont->GetWidth(szStartText, pEdit->GetCarretPos()-pEdit->GetStartPos());
		int nCaretPosInWidget = pFont->GetWidth(szStartText, pEdit->GetCarretPos()+pCore->m_iCompositionCaretPosition-pEdit->GetStartPos());

		sRect r = pEdit->GetClientRect();
		sPoint cp = CCClientToScreen(pEdit, sPoint(r.x+nInsertPosInWidget, r.y));
		pCore->SetCandidateListPosition(cp, r.h);

		int nSelStartPos=0;
		int nSelEndPos=0;{
			long nCurrTime = timeGetTime();
			if((nCurrTime%(CCEDIT_BLINK_TIME*2))>CCEDIT_BLINK_TIME){
				r.x+=nCaretPosInWidget;
				pDC->Text(r, "|", CCD_LEFT);
			}
		}

		r = pEdit->GetClientRect();

		sPoint p;
		pDC->GetPositionOfAlignment(&p, r, szStartText, CCD_LEFT);
		p.x += nInsertPosInWidget;
		pCore->DrawCompositionAttributes(pDC, p, pEdit->GetCompositionString());
	}

	r = pEdit->GetClientRect();
	pDC->Text(r, szStartText, CCD_LEFT);

	if(pEdit->IsFocus()==true){
		if(bShowLanguageTab){
			Core* pCore = Core::GetInstance();
			pCore->DrawIndicator(pDC, pEdit->GetClientRect());
		}
	}
}

void CCEditLook::OnDraw(CCEdit* pEdit, CCDrawContext* pDC, bool bShowLanguageTab){
	OnFrameDraw(pEdit, pDC);
	OnTextDraw(pEdit, pDC, bShowLanguageTab);
}

sRect CCEditLook::GetClientRect(CCEdit* pEdit, sRect& r){
	return sRect(r.x, r.y, r.w, r.h);
}