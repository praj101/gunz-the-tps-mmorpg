#include "stdafx.h"
#include "CCDrawContext.h"
#include "CCWidget.h"
#include <crtdbg.h>
#include "Core.h"
#include <math.h>
#undef _DEBUG
// CCDrawContex Implementation
/////////////////////////////
CCDrawContext::CCDrawContext(void)
: m_Color(0, 255, 0), m_HighlightColor(255, 255, 0)
{
#ifdef _DEBUG
	m_iTypeID = CORE_BASE_CLASS_TYPE;
#endif
	m_Color = 0x00000000;
	m_BitmapColor = 0xFFFFFFFF;
	m_ColorKey = sColor(0xFF, 0, 0xFF);
	m_pBitmap = 0;
	m_pFont = 0;
	m_Clip.x = 0;
	m_Clip.y = 0;
	m_Clip.w = 	CCGetWorkspaceWidth();
	m_Clip.h = CCGetWorkspaceHeight();
	m_Origin.x = 0;
	m_Origin.y = 0;
	m_iOpacity = 0xFF;
	m_Effect = CCDE_NORMAL;
}

CCDrawContext::~CCDrawContext()
{
}

sColor CCDrawContext::SetBitmapColor( sColor& color )
{
	m_BitmapColor = color;
	return m_BitmapColor;
}
sColor CCDrawContext::SetBitmapColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a/* =255 */ )
{
	return SetBitmapColor( sColor(r,g,b,a));
}

sColor CCDrawContext::GetBitmapColor() 
{
	return m_BitmapColor;
}

sColor CCDrawContext::SetColor(sColor& color)
{
	sColor temp = m_Color;
	if (m_iOpacity != 0xFF)
	{
		unsigned char a = (unsigned char)(color.a * (float)(m_iOpacity / 255.0f));
		m_Color = sColor(color.r, color.g, color.b, a);
	}
	else m_Color = color;
	return temp;
}

sColor CCDrawContext::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	if (m_iOpacity != 0xFF) a = (unsigned char)(a * (float)(m_iOpacity / 255.0f));
	return SetColor(sColor(r, g, b, a));
}

sColor CCDrawContext::GetColor(void)
{
	return m_Color;
}

sColor CCDrawContext::SetHighlightColor(sColor& color)
{
	sColor temp = m_HighlightColor;
	if (m_iOpacity != 0xFF) 
	{
		unsigned char a = (unsigned char)(color.a * (float)(m_iOpacity / 255.0f));
		m_HighlightColor = sColor(color.r, color.g, color.b, a);
	}
	else m_HighlightColor = color;
	return temp;
}

sColor CCDrawContext::SetHighlightColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	if (m_iOpacity != 0xFF) a = (unsigned char)(a * (float)(m_iOpacity / 255.0f));
	return SetHighlightColor(sColor(r, g, b, a));
}

sColor CCDrawContext::GetHighlightColor(void)
{
	return m_HighlightColor;
}

sColor CCDrawContext::SetColorKey(sColor& color)
{
	sColor temp = m_ColorKey;
	m_ColorKey = color;
	return temp;
}

sColor CCDrawContext::SetColorKey(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return SetColorKey(sColor(r, g, b, a));
}

sColor CCDrawContext::GetColorKey(void)
{
	return m_ColorKey;
}

CCBitmap* CCDrawContext::SetBitmap(CCBitmap* pBitmap)
{
	/*
#ifdef _DEBUG
	// 같은 레벨의 오브젝트이여야 한다.
	if(pBitmap!=NULL) _ASSERT(m_nTypeID==pBitmap->m_nTypeID);
#endif
	*/

	CCBitmap* temp = m_pBitmap;
	m_pBitmap = pBitmap;
	return temp;
}

CCFont* CCDrawContext::SetFont(CCFont* pFont)
{
#ifdef _DEBUG
	// 같은 레벨의 오브젝트이여야 한다.
	if(pFont!=NULL) _ASSERT(m_nTypeID==pFont->m_nTypeID);
#endif

	CCFont* temp = m_pFont;
	m_pFont = pFont;
	if(m_pFont==NULL){
		m_pFont = CCFontManager::Get(0);	// Default Font
	}
	return temp;
}

void CCDrawContext::SetClipRect(int x, int y, int w, int h)
{
	SetClipRect(sRect(x, y, w, h));
}

void CCDrawContext::SetClipRect(sRect &r)
{
	memcpy(&m_Clip, &r, sizeof(sRect));

	// Screen Coordinate Clipping
	if(m_Clip.x<0) m_Clip.x = 0;
	if(m_Clip.y<0) m_Clip.y = 0;
	if(m_Clip.x+m_Clip.w>CCGetWorkspaceWidth()) m_Clip.w = max(CCGetWorkspaceWidth()-m_Clip.x, 0);
	if(m_Clip.y+m_Clip.h>CCGetWorkspaceHeight()) m_Clip.h = max(CCGetWorkspaceHeight()-m_Clip.y, 0);
}

void CCDrawContext::Rectangle(int x, int y, int cx, int cy)
{
	HLine(x, y, cx);
	HLine(x, y+cy, cx);
	VLine(x, y, cy);
	VLine(x+cx, y, cy);
}

void CCDrawContext::Rectangle(sRect& r)
{
	Rectangle(r.x, r.y, r.w, r.h);
}

void CCDrawContext::SetOrigin(int x, int y)
{
	m_Origin.x = x;
	m_Origin.y = y;
}

void CCDrawContext::SetOrigin(sPoint& p)
{
	m_Origin = p;
}

sPoint CCDrawContext::GetOrigin(void)
{
	return m_Origin;
}

void CCDrawContext::SetEffect(CCDrawEffect effect)
{
	m_Effect = effect;
}

void CCDrawContext::FillRectangle(sRect& r)
{
	FillRectangle(r.x, r.y, r.w, r.h);
}

/*
int GetStrLength(CCFont* pFont, char* szText, int nSize)
{
	char temp[256];
	memcpy(temp, szText, nSize);
	temp[nSize] = 0;

	return pFont->GetWidth(temp);
}

int GetStrLengthOfWidth(CCFont* pFont, int w, char* szText)
{
	int tw = pFont->GetWidth(szText);
	float fw = w / (float)tw;
	int nStrLen = strlen(szText);
	int nRecStrLen = int(nStrLen * fw);

	int nRecWidth = GetStrLength(pFont, szText, nRecStrLen);

	if(nRecWidth>w){	// 넘치는 경우
		do{
			nRecStrLen--;
			nRecWidth = GetStrLength(pFont, szText, nRecStrLen);
		}while(nRecWidth<w || nRecStrLen==0);
		return nRecStrLen;
	}
	else if(nRecWidth<w){
		do{
			nRecStrLen++;
			nRecWidth = GetStrLength(pFont, szText, nRecStrLen);
		}while(nRecWidth>=w || nRecStrLen==nStrLen);
		return nRecStrLen-1;
	}

	return nRecStrLen;
}
*/

int CCDrawContext::TextWithHighlight(int x, int y, const char* szText)
{
	char szFront[CCWIDGET_NAME_LENGTH];// = {0, };
	char szBack[CCWIDGET_NAME_LENGTH];// = {0, };
	char cHighlight;

	int nPos = RemoveAnd(szFront, &cHighlight, szBack, szText);
	if(nPos==-1){	// Highlight(Underline) 문자가 없을 경우
		return Text(x, y, szText);
	}
	else{
		if(m_pFont!=NULL){
			char temp[2] = {cHighlight, 0};
			int nFrontWidth = m_pFont->GetWidth(szFront);
			int nHighlightWidth = m_pFont->GetWidth(temp);
			//int nBackWidth = m_pFont->GetWidth(szBack);
			Text(x, y, szFront);
			sColor tmpColor = m_Color;
			SetColor(m_HighlightColor);
			Text(x+nFrontWidth, y, temp);
			SetColor(tmpColor);
			return Text(x+nFrontWidth+nHighlightWidth, y, szBack);
		}
		else{
			return Text(x, y, szText);		// Font가 로드되지 않았을 경우 &가 있는 상태 그대로 표현
		}
	}
}

void CCDrawContext::GetPositionOfAlignment(sPoint* p, sRect& r, const char* szText, CCAlignmentMode am, bool bAndInclude)
{
	if(m_pFont!=NULL){
		int w;
		if(bAndInclude==true) w = m_pFont->GetWidth(szText);
		else w = m_pFont->GetWidthWithoutAmpersand(szText);
		int h = m_pFont->GetHeight();

#define DEFAULT_ALIGN_MARGIN	2	// 좌우 정렬시 좌우에 남겨질 최소의 여백
		p->x = r.x;
		p->y = r.y;
		if(am&CCD_LEFT){
			p->x = r.x + DEFAULT_ALIGN_MARGIN;
			p->y = r.y + (r.h-h) / 2;
		}
		if(am&CCD_RIGHT){
			p->x = r.x + (r.w-w-DEFAULT_ALIGN_MARGIN);
			p->y = r.y + (r.h-h) / 2;
		}
		if(am&CCD_EDIT){
			if(w+DEFAULT_ALIGN_MARGIN<r.w) p->x = r.x + DEFAULT_ALIGN_MARGIN;
			else p->x = r.x + (r.w-w);
			p->y = r.y + (r.h-h) / 2;
		}
		if(am&CCD_HCENTER){
			p->x = r.x + (r.w-w) / 2;
		}
		if(am&CCD_VCENTER){
			p->y = r.y + (r.h-h) / 2;
		}
		if(am&CCD_TOP){
			p->y = r.y;
		}
		if(am&CCD_BOTTOM){
			p->y = r.y + r.h - h;
		}
	}
}

int CCDrawContext::Text(sRect& r, const char* szText, CCAlignmentMode am)
{
	sPoint p;
	GetPositionOfAlignment(&p, r, szText, am);
	return Text(p.x, p.y, szText);
}

int CCDrawContext::TextWithHighlight(sRect& r, const char* szText, CCAlignmentMode am)
{
	char szFront[CCWIDGET_NAME_LENGTH];// = {0, };
	char szBack[CCWIDGET_NAME_LENGTH];// = {0, };
	char cHighlight;

	int nPos = RemoveAnd(szFront, &cHighlight, szBack, szText);
	if(nPos==-1){	// Highlight(Underline) 문자가 없을 경우
		return Text(r, szText, am);
	}
	else{
		sPoint p;
		char szTextWithoutAnd[CCWIDGET_NAME_LENGTH];
		RemoveAnd(szTextWithoutAnd, szText);
		GetPositionOfAlignment(&p, r, szTextWithoutAnd, am);
		//p.x += m_pFont->GetWidth("-");

		if(m_pFont!=NULL){
			char temp[2] = {cHighlight, 0};
			int nFrontWidth = m_pFont->GetWidth(szFront);
			int nHighlightWidth = m_pFont->GetWidth(temp);
			//int nBackWidth = m_pFont->GetWidth(szBack);
			Text(p.x, p.y, szFront);
			sColor tmpColor = m_Color;
			SetColor(m_HighlightColor);
			Text(p.x+nFrontWidth, p.y, temp);
			SetColor(tmpColor);
			return Text(p.x+nFrontWidth+nHighlightWidth, p.y, szBack);
		}
	}
	return -1;
}

sRect CCDrawContext::GetClipRect(void)
{
	return m_Clip;
}

void CCDrawContext::TextMC(sRect& r, const char* szText, CCAlignmentMode am)
{
	sPoint p;
	char* pPText;

	if(GetFont() == NULL) return;

	if((pPText = GetPureText(szText)) == NULL) return;
	GetPositionOfAlignment(&p, r, pPText, am);
	free(pPText);	// Release duplicated string buffer.

	TextMC(p.x, p.y, szText);
}

unsigned long int MMColorSet[] = {
		0xFF808080,	//0
		0xFFFF0000,	//1
		0xFF00FF00,	//2
		0xFF0000FF,	//3
		0xFFFFFF00,	//4

		0xFF800000,	//5
		0xFF008000,	//6
		0xFF000080,	//7
		0xFF808000,	//8
		0xFFFFFFFF,	//9
	};

static bool TestDigit(int c)
{
	if(c >= 48 && c <= 57){
		return true;
	} else {	
		return false;
	}
}

void CCDrawContext::TextMC(int x, int y, const char* szText)
{
	unsigned int nPos = 0, nLen, nOffset = 0;
	const char *pSrc = szText;
	char *pText;

	if(GetFont() == NULL) return;

	while(true){
		nPos = strcspn(pSrc, "^");
		pText = (char *)malloc(nPos+1);
		if(pText){
			strncpy(pText, pSrc, nPos);				// 사이즈만큼 카피
			pText[nPos] = '\0';						// 문자열 만들고,
			Text(x+nOffset, y, pText);		// 출력
			nOffset += GetFont()->GetWidth(pText);
			free(pText);							// 텍스트 버퍼 삭제
		}

		nLen = strlen(pSrc);

		if(nPos + 1 < strlen(pSrc)){
			//컬러 지정
			if(TestDigit(pSrc[nPos+1])){
				SetColor(sColor(MMColorSet[pSrc[nPos+1] - '0']));
				pSrc = pSrc + nPos + 2;
			} else {
				Text(x+nOffset, y, "^");
				nOffset += GetFont()->GetWidth("^");
				pSrc = pSrc + nPos + 1;
			}
		} else {
			if(nPos+1 == nLen && TestDigit(pSrc[nPos]) == false){
				Text(x+nOffset, y, "^");
				nOffset += GetFont()->GetWidth("^");
				//pSrc = pSrc + 1;
			}
			if(nPos == nLen && (pSrc[nPos-1] == '^')){
				Text(x+nOffset, y, "^");
				nOffset += GetFont()->GetWidth("^");
			}
			break;
		}
	}
}

char *CCDrawContext::GetPureText(const char *szText)
{
	unsigned int nPos = 0, nLen;
	const char *pSrc = szText;
	char *pText;

	nLen = strlen(szText);
	pText = (char *)malloc(nLen+1);
	memset(pText, 0, nLen+1);

	while(true){
		nPos = strcspn(pSrc, "^");

		strncat(pText, pSrc, nPos);				// 사이즈만큼 카피
		nLen = strlen(pSrc);

		if(nPos + 1 < strlen(pSrc)){
			if(TestDigit(pSrc[nPos+1])){
				pSrc = pSrc + nPos + 2;
			} else {
				pSrc = pSrc + nPos + 1;
				strcat(pText, "^");
			}
		} else {
			if(nPos+1 == nLen && TestDigit(pSrc[nPos]) == false){
				strcat(pText, "^");
				//pSrc = pSrc + 1;
			}
			if(nPos == nLen && (pSrc[nPos-1] == '^')){
				strcat(pText, "^");				
			}
			break;
		}
	}
	return pText;
}

#define MAX_CHAR_A_LINE		255

#include "CCDebug.h"

// 둘째줄부터 nIndentation만큼 들여쓰기를 한다, skipline만큼 윗라인을 빼고 출력한다.
int CCDrawContext::TextMultiLine(sRect& r, const char* szText,int nLineGap,bool bAutoNextLine,int nIndentation,int nSkipLine, sPoint* pPositions)
{
	bool bColorSupport=true;

	CCBeginProfile(99,"CCDrawContext::TextMultiLine");

	int nLine = 0;
	CCFont* pFont = GetFont();

	int nLength = strlen(szText);

	int y = r.y;
	const char* szCurrent=szText;
	sPoint* pCurrentPos = pPositions;
	do {
		int nX = nLine==0 ? 0 : nIndentation;

		int nOriginalCharCount = CCGetNextLinePos(pFont,szCurrent,r.w-nX,bAutoNextLine,true);
		
		if(nSkipLine<=nLine) 
		{
			int nCharCount = min(nOriginalCharCount,MAX_CHAR_A_LINE);
			char buffer[256];
			if(bColorSupport) {

// Text가 그려지는 포지션 정보를 채워 넣는다.
#define FLUSHPOS(_Pos)		if(pCurrentPos!=NULL){	\
								for(int i=0; buffer[i]!=NULL; i++){	\
									pCurrentPos[i+szCurrent-szText].x = _Pos+pFont->GetWidth(buffer, i);	\
									pCurrentPos[i+szCurrent-szText].y = y;	\
								}	\
							}

#define FLUSH				if(buffer[0]) { Text(r.x+nLastX, y, buffer); FLUSHPOS(r.x+nLastX); nLastX=nX; buffer[0]=0;pcurbuf=buffer; }

				int nLastX=nX;

				buffer[0]=0;
				char *pcurbuf=buffer;
				for(int i=0; i<nCharCount; i++){

					unsigned char c  = szCurrent[i], cc  = szCurrent[i+1];

					if(c=='^' && ('0'<=cc) && (cc<='9'))
					{
						FLUSH;
						// 채팅룸 스트링 색적용
						SetColor(sColor(MMColorSet[cc - '0']));
						i++;
						continue;
					}

					int w;

					*(pcurbuf++)=c;
					if(c>127 && i<nCharCount){
						*(pcurbuf++)=cc;
						w = pFont->GetWidth(szCurrent+i,2);
						i++;
					}
					else w = pFont->GetWidth(szCurrent+i,1);

					*pcurbuf=0;

					nX += w;
				}

				FLUSH;
			}else
			{
				strncpy(buffer,szCurrent,nCharCount);
				buffer[nCharCount]=0;
				Text(r.x+nX, y,buffer);
				FLUSHPOS(r.x+nX);
			}
			y+=pFont->GetHeight()+nLineGap;
		}

		szCurrent+=nOriginalCharCount;
		nLine++;
		if(y>=r.y+r.h) break;
	} while(szCurrent<szText+nLength);

	CCEndProfile(99);
	return nLine-nSkipLine;
}

// TODO: TextMultiLine2를 없애고 align을 TextMultiLine에 통합한다
int CCDrawContext::TextMultiLine2( sRect& r, const char* szText,int nLineGap,bool bAutoNextLine,CCAlignmentMode am )
{
	CCFont* pFont = GetFont();
	int nHeight = pFont->GetHeight()+nLineGap;
	int nWidth = pFont->GetWidth(szText);
	int i=0;
	int nLine = CCGetLineCount( pFont, szText, r.w, bAutoNextLine );
	int nStrLen = strlen(szText);
	int nX = 0;
	int nCurrLine = 0;
	int IncY = nHeight;//( r.h	/ nLine ) ; // 세로축 증분

#define		MAX_WIDGET_LINE_LENGTH	1024
	int clip = 0;
	char	TempStr[MAX_WIDGET_LINE_LENGTH];

	sRect rTemp;

	for(i=0; i<nStrLen; i++)
	{
		char temp[3] = {0, 0, 0};
		temp[0] = szText[i];
		unsigned char c = temp[0];

		if(c=='\n'){	// Carriage Return
			nX = 0;
				
			rTemp.x	= r.x;
			rTemp.y	= r.y + IncY * nCurrLine;
			rTemp.w	= r.w;
			rTemp.h	= IncY;

            strncpy( TempStr, szText + clip, i - clip );
			TempStr[i-clip] = '\0';
			Text( rTemp, TempStr, am );

			clip	= i+1;
			++nCurrLine;

			continue;
		}

		if(c=='^' && ('0'<=szText[i+1]) && (szText[i+1]<='9'))
		{
			SetColor(sColor(MMColorSet[szText[i+1] - '0']));
			i++;
			continue;
		}

		if(c>127)
		{
			i++;
			if(i<nStrLen) temp[1] = szText[i];
		}

		int w = pFont->GetWidth(temp);
		if( nX +w > r.w && bAutoNextLine )
		{
			nX = 0;

			rTemp.x	= r.x;
			rTemp.y	= r.y + IncY * nCurrLine;
			rTemp.w	= r.w;
			rTemp.h	= IncY;
			++nCurrLine;

			strncpy( TempStr, szText + clip, i - clip );
			TempStr[i-clip] = '\0';
			Text( rTemp, TempStr, am );
			
			// "clip"은 한줄 두줄 넘어갈때마다 다음줄 시작할 스트링 위치 넘버를 넣어준다.
			// 다음줄 처음 시작할 스트링이 1Byte 혹은 2Byte 따라서 클립 위치가 다르다.
			clip = c>127 ? i-1 : i;

			continue;
		}
		nX += w;
	}

	rTemp.x	= r.x;
	rTemp.y	= r.y + IncY * nCurrLine;
	rTemp.w	= r.w;
	rTemp.h	= IncY;

	// 이 함수는 멀티라인 출력용이라서 VCENTER 정렬이 먹지 않게 된다. 왜냐하면 rTemp.h = IncY는 곧 폰트 높이이므로
	// 세로 중앙 정렬해봤자 의미가 없다. VCENTER 정렬 문제를 제대로 처리하기 위해서는 전체 줄수를 미리 계산하고
	// 텍스트를 출력해야 할 것이다.
	// 일단 문자열이 1줄짜리인 경우에 한해서만 VCENTER 정렬이 먹지 않는 문제를 땜빵하는 코드를 아래와 같이 삽입했다.
	if (nCurrLine == 0)
		rTemp.h = r.h;


	strncpy( TempStr, szText + clip, i - clip );
	TempStr[i-clip] = '\0';
	Text( rTemp, TempStr, am );

	return nLine;
}

unsigned char CCDrawContext::SetOpacity(unsigned char nOpacity)
{
	unsigned char nPrevOpacity = m_iOpacity;
	m_iOpacity = nOpacity;
	return nPrevOpacity;
}

unsigned char CCDrawContext::GetOpacity()
{
	return m_iOpacity;
}


void CCDrawContext::Draw(int x, int y)
{
	if(m_pBitmap==NULL) return;
	int w = m_pBitmap->GetWidth();
	int h = m_pBitmap->GetHeight();
	Draw(x, y, w, h, 0, 0, w, h);
}

void CCDrawContext::Draw(int x, int y, int w, int h)
{
	if(m_pBitmap==NULL) return;
	int bw = m_pBitmap->GetWidth();
	int bh = m_pBitmap->GetHeight();
	Draw(x, y, w, h, 0, 0, bw, bh);
}

void CCDrawContext::DrawInverse(  int x, int y, int w, int h, bool bMirrorX, bool bMirrorY  )
{
	if(m_pBitmap==NULL) return;
	int bw = m_pBitmap->GetWidth();
	int bh = m_pBitmap->GetHeight();
	DrawInverse( x,y,w,h,0,0,bw,bh, bMirrorX, bMirrorY );
}

void CCDrawContext::DrawInverse( int x, int y, int w, int h, int sx, int sy, int sw, int sh, bool bMirrorX, bool bMirrorY )
{
	Draw(m_pBitmap->GetSourceBitmap(),x,y,w,h,sx+m_pBitmap->GetX(),sy+m_pBitmap->GetY(),sw,sh, bMirrorX, bMirrorY);	
}
void CCDrawContext::Draw(int x, int y, int sx, int sy, int sw, int sh)
{
	if(m_pBitmap==NULL) return;
	int w = m_pBitmap->GetWidth();
	int h = m_pBitmap->GetHeight();
	Draw(x, y, w, h, sx, sy, sw, sh);
}

void CCDrawContext::Draw(sPoint &p)
{
	Draw(p.x, p.y);
}

void CCDrawContext::Draw(sRect& r)
{
	Draw(r.x, r.y, r.w, r.h);
}

void CCDrawContext::Draw(sRect& d, sRect& s)
{
	Draw(d.x, d.y, d.w, d.h, s.x, s.y, s.w, s.h);
}

void CCDrawContext::Draw(int x, int y, sRect& s)
{
	Draw(x, y, s.x, s.y, s.w, s.h);
}

void CCDrawContext::DrawEx(int tx1, int ty1, int tx2, int ty2, 
						  int tx3, int ty3, int tx4, int ty4)
{

}

void CCDrawContext::Draw(int x, int y, int w, int h, int sx, int sy, int sw, int sh)
{
	Draw(m_pBitmap->GetSourceBitmap(),x,y,w,h,sx+m_pBitmap->GetX(),sy+m_pBitmap->GetY(),sw,sh);
}

int CCGetLinePos(CCFont *pFont,const char* szText, int nWidth, bool bAutoNextLine, bool bColorSupport, int nLine,int nIndentation)
{
	int nInd = nIndentation >= nWidth ? 0 : nIndentation;

	int nStrLen = strlen(szText);
	if(nStrLen==0) return 0;

	int nX = 0;
	int nCurLine = 0;

	int nThisChar=0;	// 현재 글자의 시작 인덱스

	for(int i=0; i<nStrLen; i++){

		if(nCurLine==nLine) 
			return nThisChar;

		nThisChar = i;	// 현재 글자의 시작 인덱스

		char temp[3] = {0, 0, 0};
		temp[0] = szText[i];
		unsigned char c = temp[0];

		if(c=='\n'){	// Carriage Return
			nCurLine++;
			nThisChar++;
			continue;
		}

		if(bColorSupport)
		{
			if(c=='^' && ('0'<=szText[i+1]) && (szText[i+1]<='9'))
			{
				i++;
				continue;
			}
		}

		if(c>127){
			i++;
			if(i<nStrLen) temp[1] = szText[i];
		}

		if(bAutoNextLine){
			int w = pFont->GetWidth(temp);
			if(nX+w>nWidth){
				nCurLine++;
				nX = nIndentation;	// 둘째줄부터는 시작위치가 다르다.
			}
			nX += w;
		}
	}

	if(nCurLine==nLine) 
		return nThisChar;

	return nStrLen;
}

// 다음 줄의 시작 위치를 리턴한다
int CCGetNextLinePos(CCFont *pFont,const char* szText, int nWidth, bool bAutoNextLine, bool bColorSupport)
{
	return CCGetLinePos(pFont,szText,nWidth,bAutoNextLine,bColorSupport);
}

int CCGetWidth(CCFont *pFont,const char *szText,int nSize,bool bColorSupport)
{
	int nStrLen = min((int)strlen(szText),nSize);

	int nX = 0;

	for(int i=0; i<nStrLen; i++){

		char temp[3] = {0, 0, 0};
		temp[0] = szText[i];
		unsigned char c = temp[0];

		if(c=='\n'){	// Carriage Return
			return nX;
		}

		if(bColorSupport)
		{
			if(c=='^' && ('0'<=szText[i+1]) && (szText[i+1]<='9'))
			{
				i++;
				continue;
			}
		}

		if(c>127){
			i++;
			if(i<nStrLen) temp[1] = szText[i];
		}

		int w = pFont->GetWidth(temp);
		nX += w;
	}

	return nX;
}

int CCGetLineCount(CCFont *pFont,const char* szText, int nWidth, bool bAutoNextLine, bool bColorSupport,int nIndentation)
{
	int nLine = 0;
	int nLength = strlen(szText);

	int nCurPos=0;
	do {
		int nRealWidth = (nLine==0 ? nWidth : nWidth-nIndentation);
		int nOriginalCharCount = CCGetNextLinePos(pFont,szText+nCurPos,nRealWidth,bAutoNextLine,bColorSupport);
		if(nOriginalCharCount==0 && szText[nCurPos]!=0) return -1;
		nCurPos+=nOriginalCharCount;
		nLine++;
	} while(nCurPos<nLength);

	return nLine;
}

