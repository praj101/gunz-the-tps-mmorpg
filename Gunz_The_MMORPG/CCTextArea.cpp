#include "stdafx.h"
#include "CCTextArea.h"
#include "CCColorTable.h"
#include "CCScrollBar.h"
#include "CCEdit.h"
#include "Core.h"

#define CCTEXTAREA_DEFAULT_WIDTH				100

IMPLEMENT_LOOK(CCTextArea, CCTextAreaLook)

bool CCTextArea::GetCaretPosition(sPoint *pOut,int nSize,const char* szText,int nPos,bool bFirst){
	CCFont *pFont=GetFont();

	int nLine = 0;
	int nLength = strlen(szText);

	bool bResult = false;

	int nCurPos=0;
	do {
		int nIndentation = (nLine==0 ? 0 : m_nIndentation);
		int nOriginalCharCount = CCGetNextLinePos(pFont,szText+nCurPos,GetClientWidth()-nIndentation,m_bWordWrap,m_bColorSupport);
		if(nOriginalCharCount==-1) return false;

		for(int i=0; i<nSize && nPos+i<=nLength; i++){
			if(nPos+i>=nCurPos && nPos+i<=nCurPos+nOriginalCharCount){
				pOut[i].x = CCGetWidth(pFont, szText+nCurPos, nPos+i-nCurPos, m_bColorSupport) + nIndentation;
				pOut[i].y = nLine;
				bResult = true;
			}
		}

		nCurPos+=nOriginalCharCount;
		nLine++;
	} while(nCurPos<nLength);

	return bResult;
}

int CCTextArea::GetCharPosition(const char* szText,int nX,int nLine){

	CCFont *pFont = GetFont();

	int nCur= CCGetLinePos(pFont,szText,GetClientWidth(),m_bWordWrap,m_bColorSupport,nLine,m_nIndentation);
	
	int nRealX =  (nLine>0) ? nX-m_nIndentation : nX;
	int x = CCGetNextLinePos(pFont,szText+nCur,nRealX,m_bWordWrap,m_bColorSupport);
	return nCur+x;
}

bool CCTextArea::IsDoubleCaretPos(){
	const char *szText = m_CurrentLine->text.c_str();
	int nPos = m_CaretPos.x;

	int nLine = 0;
	int nLength = strlen(szText);

	if(nPos==nLength) return false;

	int nCurPos=0;
	do {
		int nRealWidth = (nLine==0 ? GetClientWidth() : GetClientWidth()-m_nIndentation);
		int nOriginalCharCount = CCGetNextLinePos(GetFont(),szText+nCurPos,nRealWidth,m_bWordWrap,m_bColorSupport);
		if(nCurPos+nOriginalCharCount==nPos) {
			return true;
		}

		nCurPos+=nOriginalCharCount;
		nLine++;
	} while(nCurPos<nLength);

	return false;
}

void CCTextArea::ScrollDown(){
	const char *szText = GetTextLine(m_nStartLine);
	int nLine = CCGetLineCount(GetFont(),szText,GetClientWidth(),m_bWordWrap,m_bColorSupport,m_nIndentation);
	if(m_nStartLineSkipLine+1 < nLine) {
		m_nStartLineSkipLine++;
		return;
	}

	if(GetLineCount()>m_nStartLine+1)
	{
		m_nStartLine++;
		m_nStartLineSkipLine=0;
	}
}

void CCTextArea::ScrollUp(){
	if(m_nStartLineSkipLine > 0 ) {
		m_nStartLineSkipLine--;
		return;
	}

	if(m_nStartLine>0)
	{
		m_nStartLine--;

		const char *szText = GetTextLine(m_nStartLine);
		int nLine = CCGetLineCount(GetFont(),szText,GetClientWidth(),m_bWordWrap,m_bColorSupport,m_nIndentation);
		m_nStartLineSkipLine=nLine-1;
	}
}

void CCTextArea::MoveFirst(){
}

void CCTextArea::MoveLast(){
}

int CCTextArea::GetTotalLineCount(int& nStartLine, int& nCurrentLine){
	int nTotalLine = 0;
	CCLINELISTITERATOR itr = m_Lines.begin();
	for(int i=0;i<GetLineCount();i++)
	{
		sRect rectScrollBar;
		rectScrollBar = m_pScrollBar->GetRect();
		int nCntWidth = GetClientWidth() - ( (IsScrollBarVisible()) ? rectScrollBar.w : 0);

		const char *szText = itr->text.c_str();
		int nLine = CCGetLineCount(GetFont(),szText,nCntWidth,m_bWordWrap,m_bColorSupport,m_nIndentation);
		if(nLine==-1) 
		{
			int nLine = CCGetLineCount(GetFont(),szText,nCntWidth,m_bWordWrap,m_bColorSupport,m_nIndentation);
			return 0;
		}

		if(i==GetStartLine()) {
			nStartLine=nTotalLine+m_nStartLineSkipLine;
		}

		if(itr==m_CurrentLine) {
			sPoint carpos;
			if(GetCaretPosition(&carpos,1,szText,GetCaretPos().x,m_bCaretFirst))
			{
				nCurrentLine = nTotalLine+carpos.y;
			}
		}

		nTotalLine += nLine;
		itr++;
	}
	return nTotalLine;
}

void SSTextArea::UpdateScrollBar(bool bAdjustStart){	
	CCFont *pFont=GetFont();
	sRect r=GetClientRect();

	int nStartLine = 0;
	int nCurrentLine = 0;

	int nTotalLine = GetTotalLineCount(nStartLine, nCurrentLine);

	if(bAdjustStart && CCWidget::m_pCapturedWidget!=m_pScrollBar){
		int nVisibleCount = r.h / GetLineHeight() ;
		
		if(nCurrentLine>=nStartLine+nVisibleCount) {
            int nCount = nCurrentLine-(nStartLine+nVisibleCount)+1;
			for(int j=0;j<nCount;j++){
				ScrollDown();
			}
			nStartLine+=nCount;
		}

		if(nCurrentLine<nStartLine) {
            int nCount = nStartLine-nCurrentLine;
			for(int j=0;j<nCount;j++){
				ScrollUp();
			}
			nStartLine-=nCount;
		}
	}

	int nPosLines = nTotalLine - r.h / GetLineHeight();
	int nMax=max(nPosLines,0);
	m_pScrollBar->SetMinMax(0,nMax);
	
	if(m_nStartLine>nMax)
		m_nStartLine=nMax;

	m_pScrollBar->SetValue(nStartLine);
	bool bShow= m_bScrollBarEnable && 
		(r.h<int(GetLineHeight() *nTotalLine) || m_nStartLine!=0);
	m_pScrollBar->Show(bShow,false);
}

void CCTextArea::OnScrollBarChanged(int nPos){
	CCFont *pFont=GetFont();
	sRect r=GetClientRect();

	int nStartLine = 0;
	int nCurrentLine = 0;

	int nTotalLine = 0;
	CCLINELISTITERATOR itr = m_Lines.begin();
	for(int i=0;i<GetLineCount();i++){
		const char *szText = itr->text.c_str();
		int nLine = CCGetLineCount(GetFont(),szText,GetClientWidth(),m_bWordWrap,m_bColorSupport,m_nIndentation);
		if(nLine==-1) 
			return;

		if(nTotalLine<=nPos && nPos<nTotalLine+nLine) {
			m_nStartLine = i;
			m_nStartLineSkipLine = nPos-nTotalLine;
			return;
		}

		nTotalLine += nLine;
		itr++;
	}
}

bool CCTextArea::OnCommand(CCWidget* pWindow, const char* szMessage){
	if(pWindow==m_pScrollBar && strcmp(szMessage, CCLIST_VALUE_CHANGED)==0){
		OnScrollBarChanged(m_pScrollBar->GetValue());
		return true;
	}
	return false;
}

bool MTextArea::MoveLeft(bool bBackspace){
	if(IsDoubleCaretPos() && m_bCaretFirst==false && !bBackspace){
		m_bCaretFirst=true;
		return true;		
	}

	if(m_CaretPos.x>1 && IsHangul(*(m_CurrentLine->text.begin()+m_CaretPos.x-1)))
		m_CaretPos.x-=2;
	else
	if(m_CaretPos.x>0)
		m_CaretPos.x--;
	else
	if(m_CaretPos.y>0)
	{
		m_CaretPos.y--;
		m_CurrentLine--;
		m_CaretPos.x=m_CurrentLine->text.size();
		UpdateScrollBar(true);
	}
	else{
		return false;
	}

	return true;
}

void CCTextArea::MoveRight(){
	if(IsDoubleCaretPos() && m_bCaretFirst)
	{
		m_bCaretFirst=false;
		return;
	}

	if((int)m_CurrentLine->text.size()>m_CaretPos.x+1 && IsHangul(*(m_CurrentLine->text.begin()+m_CaretPos.x)))
		m_CaretPos.x+=2;
	else
	if((int)m_CurrentLine->text.size()>m_CaretPos.x)
		m_CaretPos.x++;
	else
	if(m_CaretPos.y+1<(int)m_Lines.size()){
		m_CaretPos.y++;
		m_CurrentLine++;
		m_CaretPos.x=0;
		UpdateScrollBar(true);
	}
}

void CCTextArea::MoveUp(){
	if(GetCaretPos().x==m_CurrentLine->text.size()) 
		m_bCaretFirst=true;

	const char *szCurrent = m_CurrentLine->text.c_str();

	sPoint carpos;
	if(!GetCaretPosition(&carpos,1,szCurrent,GetCaretPos().x,m_bCaretFirst))
		return;

	carpos.x++;
	if(carpos.y>0) {
		if(!m_bVerticalMoving){
			m_bVerticalMoving=true;
			m_nVerticalMoveAxis=carpos.x;
		}

		m_CaretPos.x=GetCharPosition(szCurrent,m_nVerticalMoveAxis,carpos.y-1);
		UpdateScrollBar(true);
		return ;
	}

	if(m_CaretPos.y>0){
		CCFont *pFont=GetFont();
		if(!m_bVerticalMoving){
			m_bVerticalMoving=true;
			m_nVerticalMoveAxis=carpos.x;
		}
		m_CurrentLine--;
		m_CaretPos.y--;

		szCurrent = m_CurrentLine->text.c_str();
		int nCurrentLineLineCount = CCGetLineCount(GetFont(),szCurrent,GetClientWidth(),m_bWordWrap,m_bColorSupport,m_nIndentation );
		m_CaretPos.x=GetCharPosition(szCurrent,m_nVerticalMoveAxis,nCurrentLineLineCount-1);
		UpdateScrollBar(true);
	}
}

void CCTextArea::MoveDown(){
	if(GetCaretPos().x==0) 
		m_bCaretFirst=false;

	const char *szCurrent = m_CurrentLine->text.c_str();

	int nCurrentLineLineCount = CCGetLineCount(GetFont(),szCurrent,GetClientWidth(),m_bWordWrap,m_bColorSupport,m_nIndentation );

	sPoint carpos;

	if(!GetCaretPosition(&carpos, 1, szCurrent, GetCaretPos().x,m_bCaretFirst))
		return;

	carpos.x++;

	if(carpos.y+1<nCurrentLineLineCount) {
		if(!m_bVerticalMoving){
			m_bVerticalMoving=true;
			m_nVerticalMoveAxis=carpos.x;
		}

		m_CaretPos.x=GetCharPosition(szCurrent,m_nVerticalMoveAxis,carpos.y+1);
		UpdateScrollBar(true);
		return ;
	}

	if(m_CaretPos.y+1<(int)m_Lines.size()){
		CCFont *pFont=GetFont();
		if(!m_bVerticalMoving)
		{
			m_bVerticalMoving=true;
			m_nVerticalMoveAxis=carpos.x;
		}
		m_CurrentLine++;
		m_CaretPos.y++;

		szCurrent = m_CurrentLine->text.c_str();
		m_CaretPos.x=GetCharPosition(szCurrent,m_nVerticalMoveAxis,0);
		UpdateScrollBar(true);
	}
}

void CCTextArea::OnHome(){
	const char *szCurrent = m_CurrentLine->text.c_str();

	sPoint carpos;

	if(!GetCaretPosition(&carpos, 1, szCurrent, GetCaretPos().x, m_bCaretFirst))
		return;

	m_CaretPos.x=GetCharPosition(szCurrent,0,carpos.y);
	m_bCaretFirst = false;
}

void CCTextArea::OnEnd(){
	const char *szCurrent = m_CurrentLine->text.c_str();

	sPoint carpos;

	if(!GetCaretPosition(&carpos, 1, szCurrent, GetCaretPos().x, m_bCaretFirst))
		return;

	int nNextLinePos = CCGetLinePos(GetFont(),szCurrent,GetClientWidth(),m_bWordWrap,m_bColorSupport,carpos.y+1,m_nIndentation);
	m_CaretPos.x = nNextLinePos;
	m_bCaretFirst = true;
}

void CCTextArea::DeleteCurrent(){
	if(m_CaretPos.x+1<(int)m_CurrentLine->text.size() && IsHangul(*(m_CurrentLine->text.begin()+m_CaretPos.x))){
		m_CurrentLine->text.erase(m_CurrentLine->text.begin()+m_CaretPos.x,m_CurrentLine->text.begin()+m_CaretPos.x+2); 
		m_nCurrentSize-=2;
	}
	else
	if(m_CaretPos.x<(int)m_CurrentLine->text.size()){
		m_CurrentLine->text.erase(m_CurrentLine->text.begin()+m_CaretPos.x);
		m_nCurrentSize--;
	}
	else
	if(m_CaretPos.y+1<(int)m_Lines.size()){
		CCLINELISTITERATOR willbedel=m_CurrentLine;
		willbedel++;
		m_CurrentLine->text+=willbedel->text;
		m_Lines.erase(willbedel);
		UpdateScrollBar();
	}
}

bool CCTextArea::OnLButtonDown(sPoint pos){
	sRect r = GetClientRect();

	pos.x-=r.x;
	pos.y-=r.y;
	
	int y = 0;

	m_CurrentLine = GetIterator(GetStartLine());
	for(int i=GetStartLine();i<GetLineCount();i++){
		const char *szText = m_CurrentLine->text.c_str();
		int nLine = CCGetLineCount(GetFont(),szText,GetClientWidth(),m_bWordWrap,m_bColorSupport,m_nIndentation);
		if(pos.y <= y + nLine*GetLineHeight() ||
			i==GetLineCount()-1) {
			int n = min(nLine-1,(pos.y - y) / GetLineHeight());

			m_CaretPos.x=GetCharPosition(szText,pos.x,n);
			m_CaretPos.y=i;
			return true;
		}

		y+= nLine * GetLineHeight();
		m_CurrentLine++;
	}
	_ASSERT(FALSE);
	return false;
}

bool CCTextArea::OnEvent(CCEvent* pEvent, CCListener* pListener){
	if(CCWidget::m_pFocusedWidget!=this) return false;
	sRect r = GetClientRect();
	switch(pEvent->nMessage){

	case CCWM_KEYDOWN:
		if (m_bEditable)
		{
			bool bResult = InputFilterKey(pEvent->nKey,pEvent->bCtrl);
			UpdateScrollBar(true);
			return bResult;
		}
		break;
	case CCWM_CHAR:
		if(IsFocus() && m_bEditable && GetLength()<GetMaxLen() && InputFilterChar(pEvent->nKey)==false){
			m_CurrentLine->text.insert(m_CurrentLine->text.begin()+m_CaretPos.x,(char)pEvent->nKey);
			m_CaretPos.x++;
			m_nCurrentSize++;
			UpdateScrollBar(true);
			return true;
		}
		break;
	case CCWM_IMECOMPOSE:
		strcpy(m_szIMECompositionString, pEvent->szIMECompositionString);
		if(IsFocus() && m_bEditable && GetLength()+1<GetMaxLen() && pEvent->szIMECompositionResultString[0]!=NULL){
			int length=strlen(pEvent->szIMECompositionResultString);
			m_CurrentLine->text.insert(
				m_CurrentLine->text.begin()+m_CaretPos.x,
				pEvent->szIMECompositionResultString,
				pEvent->szIMECompositionResultString+length);
			m_CaretPos.x+=length;
			m_nCurrentSize+=length;
			UpdateScrollBar(true);
			return true;
		}
		break;
	case CCWM_MOUSEWHEEL:
		if(r.InPoint(pEvent->Pos)==false) 
			break;

		if(pEvent->nDelta<0){
			ScrollDown();
			ScrollDown();
		}else{
			ScrollUp();
			ScrollUp();
		}
		UpdateScrollBar();

		return false;

	case MWM_LBUTTONDOWN: 
		{
			sRect r = GetClientRect();
			if(r.InPoint(pEvent->Pos)==true){
				return OnLButtonDown(pEvent->Pos);
			}
		}
	}

	return false;
}

void CCTextArea::OnSetFocus(void){
	Core::GetInstance()->EnableIME(true);
}

void CCTextArea::OnReleaseFocus(void){
	Core::GetInstance()->EnableIME(false);
}

bool CCTextArea::InputFilterKey(int nKey,bool bCtrl){
	if(nKey!=VK_UP && nKey!=VK_DOWN)
		m_bVerticalMoving=false;

	switch(nKey){
	case VK_LEFT : MoveLeft();return true;
	case VK_RIGHT : MoveRight();return true;
	case VK_UP : if(bCtrl) ScrollUp(); else MoveUp(); return true;
	case VK_DOWN : if(bCtrl) ScrollDown(); else MoveDown(); return true;
	case VK_HOME : OnHome();return true;
	case VK_END	: OnEnd();return true;

	case VK_DELETE : DeleteCurrent();return true;
	case VK_BACK : if(MoveLeft(true)) DeleteCurrent();return true;

	case VK_RETURN :
		if(GetLength()<GetMaxLen()){
			MLineItem newline(m_CurrentLine->color,string(m_CurrentLine->text.begin()+m_CaretPos.x,m_CurrentLine->text.end()));
			m_CurrentLine->text.erase(m_CaretPos.x,m_CurrentLine->text.size());
			m_CurrentLine++;
			m_CurrentLine=m_Lines.insert(m_CurrentLine,newline);
			m_CaretPos.y++;
			m_CaretPos.x=0;
			UpdateScrollBar(true);
			return true;
		}
		break;
	case VK_TAB :
		return true;
	}
	return false;
}

bool CCTextArea::InputFilterChar(int nKey){
	if(nKey==VK_BACK){
		return true;
	}
	else if(nKey==VK_ESCAPE){
		CCListener* pListener = GetListener();
		if(pListener!=NULL) pListener->OnCommand(this, CCTEXTAREA_ESC_VALUE);
		return true;
	}
	else if(nKey==22){	// Ctrl+'V'
		return true;
	}
	else if(nKey==3){	// Ctrl+'C'
		return true;
	}

	switch(nKey){
	case VK_TAB:
	case VK_RETURN:
		return true;
	}
	return false;
}

void CCTextArea::SetMaxLen(int nMaxLen){
	m_Lines.erase(m_Lines.begin(),m_Lines.end());

	m_nMaxLen = nMaxLen;

	m_nStartLine=0;
	m_nStartLineSkipLine=0;
	m_nCurrentSize=0;

	m_CaretPos=sPoint(0,0);

	m_Lines.push_back(CCLineItem(CCTEXTAREA_DEFAULT_TEXT_COLOR,string()));
	m_CurrentLine=m_Lines.begin();
}

CCTextArea::CCTextArea(int nMaxLen, const char* szName, CCWidget* pParent, CCListener* pListener)
 : CCWidget(szName, pParent, pListener) , m_TextOffset(0, 0){
	LOOK_IN_CONSTRUCTOR()

	m_bScrollBarEnable = true;
	m_bWordWrap = true;
	m_bColorSupport = true;
	m_bMouseOver = false;
	m_bVerticalMoving = false;
	m_nIndentation = 0;

	CCFont* pFont = GetFont();
	m_nCustomLineHeight = -1;
	int w = CCTEXTAREA_DEFAULT_WIDTH;
	int h = GetLineHeight()+2;
	SetTextOffset(sPoint(1, 1));

	m_TextColor = CCCOLOR(DEFCOLOR_CCEDIT_TEXT); 

	m_szIMECompositionString[0] = NULL;
	m_bEditable = true;
	
	m_pScrollBar = new MScrollBar(this, this);
	m_pScrollBar->Show(false,false);

	SetFocusEnable(true);

	SetMaxLen(nMaxLen);
}

void CCTextArea::SetTextOffset(sPoint p){
	m_TextOffset = p;
}

void CCTextArea::SetTextColor(sColor color){
	m_TextColor = color;
}

sColor CCTextArea::GetTextColor(void){
	return m_TextColor;
}

CCTextArea::~CCTextArea(){
	delete m_pScrollBar;
}

bool CCTextArea::GetText(char *pBuffer,int nBufferSize){
	if(GetLength()>nBufferSize) return false;

	int nSize=0;
	char *temp=pBuffer;
	temp[0]=0;
	CCLINELISTITERATOR i;
	for(i=m_Lines.begin();i!=m_Lines.end();i++){
		strcpy(temp,i->text.c_str());temp+=i->text.size();
		strcat(temp,"\n");temp++;
		nSize+=i->text.length()+1;
	}
	temp--;*temp=0;
	_ASSERT(GetLength()==nSize);

	return true;
}

const char* CCTextArea::GetTextLine(int nLine){
	if(nLine>=(int)m_Lines.size()) return NULL;
	CCLINELISTITERATOR i=m_Lines.begin();
	for(int j=0;j<nLine;j++,i++);
	return i->text.c_str();
}

CCLINELISTITERATOR CCTextArea::GetIterator(int nLine){
	if(nLine>=(int)m_Lines.size()) return m_Lines.end();
	CCLINELISTITERATOR i=m_Lines.begin();
	for(int j=0;j<nLine;j++,i++);
	return i;
}


void CCTextArea::SetText(const char *szText){
	m_nCurrentSize=0;

	m_Lines.erase(m_Lines.begin(),m_Lines.end());
	int nLength=strlen(szText);
	string text=string(szText,szText+nLength);
	int nStart=0,nNext;
	while(nStart<nLength){
		nNext=text.find(10,nStart);
		if(nNext==-1) nNext=nLength;
		m_Lines.push_back(CCLineItem(m_TextColor,text.substr(nStart,nNext-nStart)));
		m_nCurrentSize+=nNext-nStart;
		nStart=nNext+1;
	}

	m_nStartLine=0;
	m_nStartLineSkipLine=0;
	m_CaretPos=sPoint(0,0);

	if(!m_Lines.size())
		m_Lines.push_back(CCLineItem(m_TextColor,string()));
	m_CurrentLine=m_Lines.begin();

	UpdateScrollBar();
}

void CCTextArea::Clear(){
	SetText("");
}

void CCTextArea::OnSize(int w, int h){
	sRect cr = GetClientRect();
	if(m_pScrollBar->IsVisible()==true)
		m_pScrollBar->SetBounds(sRect(cr.x+cr.w-m_pScrollBar->GetDefaultBreadth(), cr.y+1, m_pScrollBar->GetDefaultBreadth(), cr.h-1));
	else
		m_pScrollBar->SetBounds(sRect(cr.x+cr.w-m_pScrollBar->GetDefaultBreadth(), cr.y+1, m_pScrollBar->GetDefaultBreadth(), cr.h-1));
	
	if(m_bWordWrap)
		UpdateScrollBar();
}

void CCTextAreaLook::OnFrameDraw(CCTextArea* pTextArea, CCDrawContext* pDC){
	if (m_bgColor.a != 0){
		sRect r = pTextArea->GetInitialClientRect();
		pDC->SetColor(m_bgColor);
		pDC->FillRectangle(r);
	}
}

const char* CCTextArea::GetCompositionString(void){
	return m_szIMECompositionString;
}

int CCTextArea::GetClientWidth(){
	return GetClientRect().w;
}

//[Translate]
// TODO 디버깅 중이다
#pragma optimize( "", off )

void CCTextAreaLook::OnTextDraw_WordWrap(CCTextArea* pTextArea, CCDrawContext* pDC){
	bool bColorSupport = true;

	CCFont *pFont=pDC->GetFont();

	sRect r = pTextArea->GetClientRect();

	if(r.w<1 || r.h<1) 
		return;

	pDC->SetClipRect(CCClientToScreen(pTextArea,r));

	sRect textrt=r;
	textrt.x+=2;	// default margin
	textrt.y+=2;
	sRect rectScrollBar;
	rectScrollBar = pTextArea->m_pScrollBar->GetRect();
	textrt.w-= (pTextArea->IsScrollBarVisible()) ? rectScrollBar.w : 0;

	pDC->SetColor(pTextArea->GetTextColor());

	int nCarY = -1;
	sPoint pt=pTextArea->GetCaretPos();

	bool bCurrentLine;
	int nStartLine = pTextArea->GetStartLine();
	int nTotalLineCount = pTextArea->GetLineCount();

	int i = nStartLine;

	char szText[1024]="";
	int nStartLineSkipLine = pTextArea->m_nStartLineSkipLine;
	int nIndentation = pTextArea->m_nIndentation;
	
	CCLINELISTITERATOR itor=pTextArea->GetIterator(pTextArea->GetStartLine());
	
	for(;itor!=pTextArea->m_Lines.end();itor++)
	{
		sColor color = itor->color; 

		pDC->SetColor(color);

		textrt.h=(r.y+r.h)-textrt.y;

		int nLine=0;

		string text;

		bCurrentLine = (i==pTextArea->GetCaretPos().y);

		if(bCurrentLine){
			text=string(pTextArea->GetTextLine(i));

			text.insert(pTextArea->GetCaretPos().x,pTextArea->GetCompositionString());
			strcpy(szText,text.c_str());
		}
		else {
			if(itor->text.length()>sizeof(szText)){
				strncpy(szText,itor->text.c_str(),sizeof(szText)-2);
				szText[sizeof(szText)-1]=0;
			}
			else
				strcpy(szText,itor->text.c_str());
		}

		int nSkipLine = (i==pTextArea->GetStartLine()) ? pTextArea->m_nStartLineSkipLine : 0;

		int nTextLen = strlen(szText);
		if(nTextLen>0){
			sPoint* pPositions = NULL;
			if(bCurrentLine==true) pPositions = new sPoint[nTextLen];
			nLine = pDC->TextMultiLine(textrt, szText, 0, true, nIndentation, nSkipLine, pPositions);

			if(pTextArea->IsFocus() && bCurrentLine==true){
				Mint* pMint = Mint::GetInstance();
				const char* szComposition = pTextArea->GetCompositionString();
				int nCompLen = strlen(szComposition);
				for(int i=0; i<nCompLen; i+=(IsHangul(szComposition[i])?2:1)) {
					int pos = pTextArea->GetCaretPos().x+i;
					if (pPositions && pos < nTextLen)
						pMint->DrawCompositionAttribute(pDC, pPositions[pos], pTextArea->GetCompositionString(), i);
				}
			}

			if(pTextArea->IsFocus()){
				int caretPos = pTextArea->GetCaretPos().x;
				if (pPositions && caretPos < nTextLen){
					Core* pCore = Core::GetInstance();
					sPoint cp = CCClientToScreen(pTextArea, pPositions[caretPos]);
					pCore->SetCandidateListPosition(cp, pTextArea->GetLineHeight());
				}
			}

			if(pPositions!=NULL) delete[] pPositions;

		}

		if(pt.y==i) nCarY = textrt.y;
		textrt.y+=nLine*pTextArea->GetLineHeight();
		if(textrt.y>=r.y+r.h) break;
		
		sPoint carpos;
		Core* pCore = Core::GetInstance();
		if(bCurrentLine==true && nCarY>=0 && pTextArea->GetCaretPosition(&carpos, 1, szText, pTextArea->GetCaretPos().x+pMint->m_nCompositionCaretPosition, pTextArea->m_bCaretFirst)){
			carpos.x+=textrt.x;
			carpos.y=nCarY + carpos.y*pTextArea->GetLineHeight();

			if(pTextArea->IsFocus() && pTextArea->GetEditable() 
				&& timeGetTime()%(CCEDIT_BLINK_TIME*2)>CCEDIT_BLINK_TIME)
			{
				pDC->Line(carpos.x,carpos.y,carpos.x,carpos.y+pTextArea->GetLineHeight());
			}
		}
		i++;
	}
}

#pragma optimize( "", on )

void CCTextAreaLook::OnTextDraw(CCTextArea* pTextArea, CCDrawContext* pDC)
{
	CCFont *pFont=pDC->GetFont();

	sRect r = pTextArea->GetClientRect();
	r.w -= pTextArea->IsScrollBarVisible() ? pTextArea->GetScrollBarWidth() : 0;
	pDC->SetClipRect(MClientToScreen(pTextArea,r));

	sRect textrt=r;
	textrt.h=pTextArea->GetLineHeight();

	pDC->SetColor(pTextArea->GetTextColor());

	int toline=min(pTextArea->GetLineCount(),pTextArea->GetStartLine()+r.h/pTextArea->GetLineHeight());
	for(int i=pTextArea->GetStartLine();i<toline;i++)
	{
		if(i==pTextArea->GetCaretPos().y)
		{
			string text=string(pTextArea->GetTextLine(i));

			text.insert(pTextArea->GetCaretPos().x,pTextArea->GetCompositionString());
			pDC->Text(textrt,text.c_str(),MAM_LEFT);

		}
		else {
			pDC->Text(textrt,pTextArea->GetTextLine(i),MAM_LEFT);
		}
		textrt.y+=pTextArea->GetLineHeight();
	}

	if(pTextArea->IsFocus() && pTextArea->GetEditable() 
		&& timeGetTime()%(CCEDIT_BLINK_TIME*2)>CCEDIT_BLINK_TIME)
	{
		sPoint pt=pTextArea->GetCaretPos();
		const char *pCarLine=pTextArea->GetTextLine(pt.y);
		string str;
		if(pCarLine)
			str=string(pCarLine,pCarLine+pt.x);
		textrt.x=r.x+pFont->GetWidth(str.c_str());
		textrt.y=r.y+pTextArea->GetLineHeight()*(pt.y-pTextArea->GetStartLine());
		if(r.y<=textrt.y && textrt.y+pTextArea->GetLineHeight()<=r.y+r.h)
			pDC->Text(textrt,"|",CCD_LEFT);
	}

}

void CCTextAreaLook::OnDraw(CCTextArea* pTextArea, CCDrawContext* pDC){
	OnFrameDraw(pTextArea, pDC);
	OnTextDraw_WordWrap(pTextArea, pDC);
}

sRect CCTextAreaLook::GetClientRect(CCTextArea* pTextArea, sRect& r){
	return sRect(r.x, r.y, r.w, r.h);
}

void CCTextArea::AddText(const char *szText,sColor color){
	m_CurrentLine = m_Lines.end();
	m_CurrentLine--;

	int nLineCount = CCGetLineCount(NULL,szText,0,false,m_bColorSupport);
	
	const char *szCurrent=szText;
	for(int i=0;i<nLineCount;i++){
		int nCharCount=CCGetNextLinePos(NULL,szCurrent,0,false,m_bColorSupport);

		m_CurrentLine->color=color;
		m_CurrentLine->text.append(string(szCurrent,nCharCount));
		
		m_Lines.push_back(MLineItem(color,string()));
		m_CurrentLine = m_Lines.end();
		m_CurrentLine--;
		
		szCurrent+=nCharCount;
	}

	m_CaretPos.x=0;
	m_CaretPos.y=GetLineCount()-1;

	m_CurrentLine = m_Lines.end();
	m_CurrentLine--;

	UpdateScrollBar(true);
}

void CCTextArea::AddText(const char *szText){
	m_CurrentLine = m_Lines.end();
	m_CurrentLine--;

	AddText(szText,m_CurrentLine->color);
}

void CCTextArea::DeleteFirstLine(){
	if(GetLineCount()<2) return;

	if(m_CurrentLine==m_Lines.begin())
	{
		m_CurrentLine++;
		m_CaretPos.x=0;
		m_CaretPos.y++;
	}

	m_Lines.erase(m_Lines.begin());
	if(m_nStartLine>0)
		m_nStartLine--;
	UpdateScrollBar();
}

int CCTextArea::GetLineHeight( void){
	if (m_nCustomLineHeight != -1)
		return m_nCustomLineHeight;
	return GetFont()->GetHeight();
}

void CCTextArea::SetCustomLineHeight( int nHeight){
	m_nCustomLineHeight = nHeight;
}

void CCTextArea::MultiplySize( float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight ){
	CCWidget::MultiplySize( byIDLWidth, byIDLHeight, byCurrWidth, byCurrHeight );

	if (m_nCustomLineHeight != -1)
		m_nCustomLineHeight = int(m_nCustomLineHeight * byCurrHeight +0.5f);
}

void CCTextArea::AdjustHeightByContent()
{
	int nStartLine(0), nCurrline(0), nTotalLine(0);
	nTotalLine = GetTotalLineCount(nStartLine, nCurrline);
	int newHeight = nTotalLine * GetLineHeight();
	sRect rcTextArea = GetRect();
	SetSize(rcTextArea.w, newHeight);
}