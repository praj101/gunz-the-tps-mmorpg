#include "stdafx.h"
#include "CCBButtonLook.h"
#include "CCColorTable.h"
#include "CCBitmapDrawer.h"
#include "Core.h"

CCBButtonLook::CCBButtonLook(void) : CCScalableLook()
{
	for(int i=0; i<9; i++){
		m_pUpBitmaps[i] = NULL;
		m_pDownBitmaps[i] = NULL;
		m_pOverBitmaps[i] = NULL;
	}
	for(int i=0; i<4; i++){
		m_pFocusBitmaps[i] = NULL;
	}

	m_pFont = NULL;
	m_FontColor = sColor(DEFCOLOR_CCBUTTON_TEXT);
	m_FontDownColor = sColor(DEFCOLOR_LIGHT);
	m_FontDisableColor = sColor(DEFCOLOR_DISABLE);
	m_FontHighlightColor = sColor(DEFCOLOR_PEN_HIGHLIGHT);
	m_bStretch = true;
	m_bCustomLook = false;

	m_FontDownOffset = sPoint(0,0);
}

void CCBButtonLook::DrawText(CCButton* pButton, CCDrawContext* pDC, sRect& r, const char* szText, CCAlignmentMode a)
{
	if(pButton->m_pIcon!=NULL){
		pDC->SetBitmap(pButton->m_pIcon);
		pDC->Draw(r.x, r.y+(r.h-pButton->m_pIcon->GetHeight())/2);

		r.x+=pButton->m_pIcon->GetWidth();
		r.w-=pButton->m_pIcon->GetWidth();
	}

	if(pButton->IsButtonDown())
	{
		r.x+=(int)(m_FontDownOffset.x * GetScale());
		r.y+=(int)(m_FontDownOffset.y * GetScale());
	}

	if(m_pFont!=NULL) pDC->SetFont(m_pFont);
	if(pButton->IsEnable()==true){
		if(pButton->IsButtonDown()==true) pDC->SetColor(m_FontDownColor);
		else if(pButton->IsMouseOver()) pDC->SetColor(m_FontDownColor);
		else if(pButton->GetType() == CCBT_PUSH2 && pButton->GetCheck()) pDC->SetColor(m_FontDownColor);
		else pDC->SetColor(m_FontColor);
	}
	else pDC->SetColor(m_FontDisableColor);
	if(pButton->m_bHighlight==true){
		sColor PrevHCol = pDC->SetHighlightColor(m_FontHighlightColor);
		pDC->TextWithHighlight(r, szText, a);
		pDC->SetHighlightColor(PrevHCol);
	}
	else{
		pDC->Text(r, szText, a);
	}
}

void CCBButtonLook::DrawFocus(CCDrawContext* pDC, sRect& r)
{
	if(m_pFocusBitmaps[0]==NULL) return;

	sRect cr = pDC->GetClipRect();
	sRect pcr = cr;
	cr.x -= 2;
	cr.y -= 2;
	cr.w += 4;
	cr.h += 4;
	pDC->SetClipRect(cr);

	pDC->SetBitmap(m_pFocusBitmaps[0]);
	pDC->Draw(r.x-2, r.y-2);
	pDC->SetBitmap(m_pFocusBitmaps[1]);
	pDC->Draw(r.x+r.w-m_pFocusBitmaps[1]->GetWidth()+2, r.y-2);
	pDC->SetBitmap(m_pFocusBitmaps[2]);
	pDC->Draw(r.x-2, r.y+r.h-m_pFocusBitmaps[1]->GetHeight()+2);
	pDC->SetBitmap(m_pFocusBitmaps[3]);
	pDC->Draw(r.x+r.w-m_pFocusBitmaps[1]->GetWidth()+2, r.y+r.h-m_pFocusBitmaps[1]->GetHeight()+2);

	pDC->SetClipRect(pcr);
}

void CCBButtonLook::OnDownDraw(CCButton* pButton, CCDrawContext* pDC)
{
	sRect r = pButton->GetInitialClientRect();
	if( GetCustomLook() )
	{
		DrawBitmapButtonCustom1( pDC, r, m_pDownBitmaps, true, m_bStretch );
		r = pButton->GetClientRect();
		r.x += 1;
		r.y += 1;
		r.w += 1;
		r.h += 1;
	}
	else
	{
		DrawBitmapFrame9(pDC, r, m_pDownBitmaps, m_bStretch, GetScale() );
		r = pButton->GetClientRect();
	}
	if(pButton->IsFocus()==true) DrawFocus(pDC, r);
    
	DrawText(pButton, pDC, r, pButton->m_szName, pButton->GetAlignment());
}

void CCBButtonLook::OnUpDraw(CCButton* pButton, CCDrawContext* pDC)
{
	if( pButton->GetType() == CCBT_PUSH2  && pButton->GetCheck() ) 
	{
		OnDownDraw( pButton, pDC );
		return;
	}

	sRect r = pButton->GetInitialClientRect();
	if( GetCustomLook() )
		DrawBitmapButtonCustom1( pDC, r, m_pUpBitmaps, false, m_bStretch );
	else
		DrawBitmapFrame9(pDC, r, m_pUpBitmaps, m_bStretch, GetScale() );
	if(pButton->IsFocus()==true) DrawFocus(pDC, r);

	DrawText(pButton, pDC, pButton->GetClientRect(), pButton->m_szName, pButton->GetAlignment());
}

void CCBButtonLook::OnOverDraw(CCButton* pButton, CCDrawContext* pDC)
{
	if( pButton->GetType() == CCBT_PUSH2 )
	{
		if( pButton->GetCheck() ) OnDownDraw( pButton, pDC );
		else OnUpDraw(pButton, pDC );
		return;
	}
	
	sRect r = pButton->GetInitialClientRect();
	if( GetCustomLook() )
		DrawBitmapButtonCustom1( pDC, r, m_pOverBitmaps, false, m_bStretch );
	else
		DrawBitmapFrame9(pDC, r, m_pOverBitmaps, m_bStretch, GetScale());
	if(pButton->IsFocus()==true) DrawFocus(pDC, r);

	DrawText(pButton, pDC, pButton->GetClientRect(), pButton->m_szName, pButton->GetAlignment());
}

void CCBButtonLook::OnDisableDraw(CCButton* pButton, CCDrawContext* pDC)
{
	OnUpDraw(pButton, pDC);
}

void CCBButtonLook::OnDraw( CCButton* pButton, CCDrawContext* pDC )
{
	if(GetWireLook())		// 위젯의 오른쪽에 붙는 방식일 경우에...
	{
		sRect rect = pButton->GetInitialClientRect();

		// 배경 그리고...
		pDC->SetColor(19,19,19,255);
		pDC->FillRectangle( rect);


		// 버튼이 그려질 크기를 구한다.
		sRect rectButtonBmp;
		rectButtonBmp.x = rect.x + rect.w - rect.h + 1;		// 버튼이 그려질 시작 위치를 구한다.
        rectButtonBmp.y = rect.y + 1;						// 테두리 두께(1 pixel)때문에 1 pixel 아래로 내려서 그린다.
		rectButtonBmp.w = rect.h - 2;						// 위젯의 높이를 기준으로 폭과 높이를 맞춘다.
		rectButtonBmp.h = rect.h - 2;						// 위젯의 높이를 기준으로 폭과 높이를 맞춘다.
		float fScale = (float)(rect.h - 2) / (float)m_pDownBitmaps[1]->GetHeight();

		// 버튼의 비트맵을 그린다
		if( pButton->GetComboDropped() )
		{
			HLineBitmap( pDC, rectButtonBmp.x, rectButtonBmp.y, rectButtonBmp.w, m_pDownBitmaps[2], false, fScale);
		}
		else
		{
			if( pButton->IsFocus() )
				HLineBitmap( pDC, rectButtonBmp.x, rectButtonBmp.y, rectButtonBmp.w, m_pDownBitmaps[0], false, fScale);
			else
				HLineBitmap( pDC, rectButtonBmp.x, rectButtonBmp.y, rectButtonBmp.w, m_pDownBitmaps[1], false, fScale);
		}
		
		sRect rectText = rect;
		rectText.w -= rect.h;
		pDC->SetColor(205,205,205,255);
	 	pDC->Text(rectText, pButton->m_szName, pButton->GetAlignment() );

		// 테두리 그리고...
		pDC->SetColor(205,205,205,255);
		pDC->Rectangle( rect);

	}
	else
		CCButtonLook::OnDraw( pButton, pDC );
}

#define CHECKBOX_SIZE  12	//800*600 기준으로 한 크기
void CCBButtonLook::OnCheckBoxDraw( CCButton* pButton, CCDrawContext* pDC, bool bPushed )
{
	// 현재 해상도라면 어느정도 크기로 그려야 하는지
	const int boxWidth = int(CHECKBOX_SIZE * CCGetWorkspaceWidth() / 800.f);
	const int boxHeight = int(CHECKBOX_SIZE * CCGetWorkspaceHeight() / 600.f);

	// 체크박스 그리기
	sRect r = pButton->GetInitialClientRect();
	int x = r.x + boxWidth;	// 약간의 여유분
	int y = r.y + (int)(r.h*0.5) -(int)(boxHeight*0.5);
	pDC->SetColor( 128, 128, 128, 255 );
	pDC->Rectangle( x, y, boxWidth, boxHeight );
	//체크버튼 그리기
	if(bPushed)
	{
		pDC->SetBitmap(m_pUpBitmaps[0]);
		pDC->Draw(x-(int)(boxWidth*0.5), y-(int)(boxHeight*0.5), boxWidth*1.8f, boxHeight*1.8f);
	}
	// 글씨쓰기
	r = pButton->GetClientRect();
	//	r.x += 2*CHECKBOX_SIZE;
	r.x += boxWidth;			// 동환이가 수정
	DrawText( pButton, pDC, r, pButton->m_szName, pButton->GetAlignment());
}

sRect CCBButtonLook::GetClientRect(CCButton* pButton, sRect& r)
{
	float fScale = GetScale();

	int al = (int)(fScale * GETWIDTH(m_pUpBitmaps[3]));
	int au = (int)(fScale * GETHEIGHT(m_pUpBitmaps[7]));
	int ar = (int)(fScale * GETWIDTH(m_pUpBitmaps[5]));
	int ab = (int)(fScale * GETHEIGHT(m_pUpBitmaps[1]));
	return sRect(r.x+al, r.y+au, r.w-(al+ar), r.h-(au+ab));
}

