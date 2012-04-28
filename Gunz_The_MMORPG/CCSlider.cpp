#include "stdafx.h"
#include "CCSlider.h"
#include "CCColorTable.h"

void CCSliderThumbLook::OnDraw(CCSliderThumb* pThumb, CCDrawContext* pDC){
	pDC->SetColor(sColor(DEFCOLOR_CCBUTTON_LIGHTPLANE));
	pDC->FillRectangle(pThumb->GetClientRect());
}

sRect CCSliderThumbLook::GetClientRect(CCSliderThumb* pThumb, sRect& r){
	return r;
}

sSize CCSliderThumbLook::GetDefaultSize(CCSliderThumb* pThumb){
	return MSIZE(MSCROLLBAR_DEFAULT_WIDTH, MSCROLLBAR_DEFAULT_WIDTH);
}

IMPLEMENT_LOOK(CCSliderThumb, CCSliderThumbLook)

CCSliderThumb::CCSliderThumb(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCThumb(szName, pParent, pListener){
	LOOK_IN_CONSTRUCTOR()
}

sSize CCSliderThumb::GetDefaultSize(void){
	if(GetLook()!=NULL) return GetLook()->GetDefaultSize(this);
	return sSize(CCSCROLLBAR_DEFAULT_WIDTH, CCSCROLLBAR_DEFAULT_WIDTH);
}

IMPLEMENT_LOOK(CCSlider, CCScrollBarLook)

int CCSlider::GetThumbSize(void){
	int narrowWidth = (m_nScrollBarType==CCBT_VERTICAL) ? GetClientRect().w : GetClientRect().h;
	return narrowWidth;
}

void CCSlider::Initialize(void){
	delete m_pThumb;
	m_pThumb = new CCSliderThumb(NULL, this, this);
	sSize s = ((CCSliderThumb*)m_pThumb)->GetDefaultSize();
	m_pThumb->SetSize(s.w, s.h);
	m_pThumb->m_nDirection = CCBT_HORIZONTAL;
	LOOK_IN_CONSTRUCTOR()
}

CCSlider::CCSlider(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCScrollBar(szName, pParent, pListener, CCBT_HORIZONTAL){
	Initialize();
}

CCSlider::CCSlider(CCWidget* pParent, CCListener* pListener)
: CCScrollBar(pParent, pListener, MSBT_HORIZONTAL){
	Initialize();
}

CCSlider::~CCSlider(void){
}
