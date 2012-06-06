#include "stdafx.h"
#include "CCBmButton.h"

#define MDEPRECATED
#ifdef MDEPRECATED

void CCBmButton::OnDownDraw(CCDrawContext* pDC)
{
	/*
	if(m_pDownBitmap==NULL) CCButton::OnDownDraw(pDC);	// Bitmap이 없으면 text로 표시
	else{
		pDC->SetBitmap(m_pDownBitmap);
		pDC->Draw(m_Rect.x, m_Rect.y);
	}
	*/
	
	if(m_pDownBitmap!=NULL){
		pDC->SetBitmap(m_pDownBitmap);
		sRect r = GetClientRect();
		if( m_bStretch )
			pDC->Draw(r.x-2, r.y-2, r.w, r.h );
		else
			pDC->Draw(r.x-2, r.y-2);
	}
}

void CCBmButton::OnUpDraw(CCDrawContext* pDC)
{
	/*
	if(m_pUpBitmap==NULL) CCButton::OnUpDraw(pDC);	// Bitmap이 없으면 text로 표시
	else{
		pDC->SetBitmap(m_pUpBitmap);
		pDC->Draw(m_Rect.x, m_Rect.y);
	}
	*/
	if(m_pUpBitmap!=NULL){
		pDC->SetBitmap(m_pUpBitmap);
		sRect r = GetClientRect();
		if( m_bStretch )
			pDC->Draw(r.x-2, r.y-2, r.w, r.h );
		else
			pDC->Draw(r.x-2, r.y-2);
	}
}

void CCBmButton::OnOverDraw(CCDrawContext* pDC)
{
	/*
	if(m_pUpBitmap==NULL) CCButton::OnOverDraw(pDC);	// Bitmap이 없으면 text로 표시
	else{
		// 현재는 UpBitmap으로..
		pDC->SetBitmap(m_pUpBitmap);
		pDC->Draw(m_Rect.x, m_Rect.y);
	}
	*/
	if(m_pUpBitmap!=NULL){
		pDC->SetBitmap(m_pOverBitmap);
		sRect r = GetClientRect();
		if( m_bStretch )
			pDC->Draw(r.x-2, r.y-2, r.w, r.h );
		else
			pDC->Draw(r.x-2, r.y-2);
	}
}

void CCBmButton::OnDisableDraw(CCDrawContext* pDC)
{
	if(m_pDisableBitmap!=NULL){
		pDC->SetBitmap(m_pDisableBitmap);
		sRect r = GetClientRect();
		if( m_bStretch )
			pDC->Draw(r.x-2, r.y-2, r.w, r.h );
		else
			pDC->Draw(r.x-2, r.y-2);
	}
	else{
		if(m_pUpBitmap!=NULL){
			pDC->SetBitmap(m_pUpBitmap);
			sRect r = GetClientRect();
			if( m_bStretch )
				pDC->Draw(r.x-2, r.y-2, r.w, r.h );
			else
				pDC->Draw(r.x-2, r.y-2);
		}
	}
}

void CCBmButton::OnDraw(CCDrawContext* pDC)
{
	if(IsEnable()==false){
		OnDisableDraw(pDC);
	}
	else if((GetType()==CCBT_NORMAL && IsButtonDown()==true) 
		|| (GetType()==CCBT_PUSH && GetCheck())) {
		OnDownDraw(pDC);
	}
	else if(IsMouseOver()==true){
		OnOverDraw(pDC);
	}
	else{
		OnUpDraw(pDC);
	}

	// 글씨
	if( m_bTextColor )
	{
		pDC->SetColor( m_BmTextColor );
		pDC->Text(GetClientRect(), m_szName, GetAlignment());
	}
	else
        GetLook()->OnDrawText( this, this->GetClientRect(), pDC );
}

CCBmButton::CCBmButton(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCButton(szName, pParent, pListener)
{
	m_pUpBitmap = NULL;
	m_pDownBitmap = NULL;
	m_pDisableBitmap = NULL;
	m_pOverBitmap = NULL;
	m_bStretch = false;
	m_bTextColor	= false;
}

void CCBmButton::SetUpBitmap(CCBitmap* pBitmap)
{
 	m_pUpBitmap = pBitmap;
	if( !m_bStretch && m_pUpBitmap!=NULL)
		SetSize(m_pUpBitmap->GetWidth(), m_pUpBitmap->GetHeight());
}

void CCBmButton::SetDownBitmap(CCBitmap* pBitmap)
{
	m_pDownBitmap = pBitmap;
	if(!m_bStretch && m_pDownBitmap!=NULL && m_pUpBitmap==NULL)
		SetSize(m_pDownBitmap->GetWidth(), m_pDownBitmap->GetHeight());
}

void CCBmButton::SetDisableBitmap(CCBitmap* pBitmap)
{
	m_pDisableBitmap = pBitmap;
	if(!m_bStretch && m_pDisableBitmap!=NULL && m_pDownBitmap==NULL && m_pUpBitmap==NULL)
		SetSize(m_pDisableBitmap->GetWidth(), m_pDisableBitmap->GetHeight());
}

void CCBmButton::SetOverBitmap(CCBitmap* pBitmap)
{
	m_pOverBitmap = pBitmap;
}

#endif