#include "stdafx.h"
#include "CCResource.h"
#include "CCWidget.h"
#include "CCToolTip.h"
#include "Core.h"
#include "CCButton.h"
#include "CCEdit.h"

inline bool CCIsSameWidgetClass(CCWidget* pWidget, const char* szClassName){
	if (strcmp(pWidget->GetClassName(), szClassName) == 0) return true;
	return false;
}

inline bool CCIsSameWidgetClass(CCWidget* pWidgetA, CCWidget* pWidgetB){
	if (strcmp(pWidgetA->GetClassName(), pWidgetB->GetClassName()) == 0) return true;
	return false;
}

CCWidget* CCWidget::m_pCapturedWidget = NULL;
CCWidget* CCWidget::m_pFocusedWidget = NULL;

static int g_nWidgetCount=0;

void CCWidget::MakeLocalEvent(CCEvent* pLoalEvent, const CCEvent* pEvent){
	memcpy(pLoalEvent, pEvent, sizeof(CCEvent));

	sRect sr = GetScreenRect();
	pLoalEvent->sPos.x -= sr.x;
	pLoalEvent->sPos.y -= sr.y;
}

bool CCWidget::EventResize(CCEvent* pEvent){
	CCEvent LocalEvent;
	MakeLocalEvent(&LocalEvent, pEvent);

	// Widget Resize
#define RESIZER_SIZE	4
	if(m_bResizable==true){
		sPoint sp = pEvent->sPos;
		CCWidget* pParent = GetParent();
		if(pParent==NULL) return false;

		if(pEvent->iMessage==CCWM_LBUTTONDOWN){
			m_iResizeSide = 0;
			if(LocalEvent.sPos.y>=-RESIZER_SIZE && LocalEvent.sPos.y<=RESIZER_SIZE){
				if(LocalEvent.sPos.x>=-RESIZER_SIZE && LocalEvent.sPos.x<=m_Rect.w+RESIZER_SIZE)
					m_iResizeSide |= 1;
			}
			if(LocalEvent.sPos.x>=m_Rect.w-RESIZER_SIZE && LocalEvent.sPos.x<=m_Rect.w+RESIZER_SIZE){
				if(LocalEvent.sPos.y>=-RESIZER_SIZE && LocalEvent.sPos.y<=m_Rect.h+RESIZER_SIZE)
					m_iResizeSide |= 2;
			}
			if(LocalEvent.sPos.y>=m_Rect.h-RESIZER_SIZE && LocalEvent.sPos.y<=m_Rect.h+RESIZER_SIZE){
				if(LocalEvent.sPos.x>=-RESIZER_SIZE && LocalEvent.sPos.x<=m_Rect.w+RESIZER_SIZE)
					m_iResizeSide |= 4;
			}
			if(LocalEvent.sPos.x>=-RESIZER_SIZE && LocalEvent.sPos.x<=RESIZER_SIZE){
				if(LocalEvent.sPos.y>=-RESIZER_SIZE && LocalEvent.sPos.y<=m_Rect.h+RESIZER_SIZE)
					m_iResizeSide |= 8;
			}
			if(m_iResizeSide!=0){
				SetCapture();
				//
				return true;
			}
		}
		if(m_iResizeSide!=0 && pEvent->iMessage==CCWM_LBUTTONUP){
			ReleaseCapture();
			m_iResizeSide = 0;
			return true;
		}
		if(pEvent->iMessage==CCWM_MOUSEMOVE){
			if(m_iResizeSide!=0){
				sPoint pp = CCScreenToClient(pParent, sp);
				sPoint tp = CCScreenToClient(this, sp);
				sRect r = m_Rect;
				if(m_iResizeSide&1){
					r.h += (r.y - pp.y);
					r.y = pp.y;
				}
				if(m_iResizeSide&2){
					r.w = tp.x;
				}
				if(m_iResizeSide&4){
					r.h = tp.y;
				}
				if(m_iResizeSide&8){
					r.w += (r.x - pp.x);
					r.x = pp.x;
				}
				SetPosition(r.x, r.y);
				if(r.w<m_iMinWidth) r.w = m_iMinWidth;
				if(r.h<m_iMinHeight) r.h = m_iMinHeight;
				SetSize(r.w, r.h);
				return true;
			}
		}
	}

	return false;
}


void CCWidget::InsertChild(CCWidget* pWidget){
	m_Children.InsertBefore(pWidget);
	pWidget->m_pParent = this;
}

void CCWidget::AddChild(CCWidget* pWidget){
	m_Children.Add(pWidget);
	pWidget->m_pParent = this;
}

void CCWidget::RemoveChild(CCWidget* pWidget){
	for(int i=0; i<m_Children.GetCount(); i++){
		CCWidget* pCurWnd = m_Children.Get(i);
		if(pCurWnd==pWidget){
			pWidget->m_pParent = NULL;
			m_Children.Delete(i);
			return;
		}
	}
}

void CCWidget::AddExclusive(CCWidget* pWidget){
	m_Exclusive.Add(pWidget);
}

bool CCWidget::RemoveExclusive(CCWidget* pWidget){
	for(int i=0; i<m_Exclusive.GetCount(); i++){
		CCWidget* pThis = m_Exclusive.Get(i);
		if(pThis==pWidget){
			m_Exclusive.Delete(i);
			return true;
		}
	}
	return false;
}

CCWidget* CCWidget::GetLatestExclusive(){
	if(m_Exclusive.GetCount()>0) return m_Exclusive.Get(m_Exclusive.GetCount()-1);
	return NULL;
}

CCWidget::CCWidget(const char* szName, CCWidget* pParent, CCListener* pListener){
	g_nWidgetCount++;
	m_iID = g_nWidgetCount;

	if(szName==NULL) m_szName[0] = NULL;
	else{
		if(strlen(szName)<CCWIDGET_NAME_LENGTH){
			strcpy(m_szName, szName);
		}
		else{
			memcpy(m_szName, szName, CCWIDGET_NAME_LENGTH-4);
			m_szName[CCWIDGET_NAME_LENGTH-4] = '.';
			m_szName[CCWIDGET_NAME_LENGTH-3] = '.';
			m_szName[CCWIDGET_NAME_LENGTH-2] = '.';
			m_szName[CCWIDGET_NAME_LENGTH-1] = 0;
		}
	}

	// Default Region
	m_Rect.x = 0;
	m_Rect.y = 0;
	m_Rect.w = 100;
	m_Rect.h = 100;

	m_pParent = pParent;
	if(m_pParent!=NULL) m_pParent->AddChild(this);

	m_pListener = pListener;

	m_pCursor = NULL;		// NULL Cursor is Default Cursor
	m_pFont = NULL;			// NULL Font is Default Font

	m_bVisible = true;
	m_bEnable = true;
	m_bFocusEnable = false;	// Default Focus Disabled

	m_pToolTip = NULL;

	SetAccelerator(0);

	m_bZOrderChangable = false;
	m_bResizable = false;
	m_iResizeSide = 0;

	m_bClipByParent = true;

	m_iOpacity = 255;

	m_bEnableDesignerMode = true;
	m_iDragWidget = 0;
	m_bModifiedByDesigner = false;
	m_bAddedByDesigner = false;
	m_BoundsAlignment = CCD_NOTALIGN;

	m_iMinWidth = 10;
	m_iMinHeight = 10;

	m_bIsListBox = false;

	m_iDebugType = 0;
	m_bEventAcceleratorCall = false;

	m_IDLRect = sRect(-1,-1,-1,-1);

}

CCWidget::~CCWidget(){
	ReleaseExclusive();

	for(int i=0; i<m_Children.GetCount(); i++){
		CCWidget* pWidget = m_Children.Get(i);
		pWidget->m_pParent = NULL;
	}

	if(m_pParent!=NULL) m_pParent->RemoveChild(this);
	if(CCWidget::m_pCapturedWidget==this) CCWidget::m_pCapturedWidget = NULL;
	if(CCWidget::m_pFocusedWidget==this) CCWidget::m_pFocusedWidget = NULL;
	DetachToolTip();
}

void CCWidget::OnRun(){
}

void CCWidget::OnDraw(CCDrawContext* pDC){
	// Draw Nothing
	pDC->SetColor(sColor(196, 196, 196));
	pDC->FillRectangle(GetInitialClientRect());
}

bool CCWidget::OnEvent(CCEvent* pEvent, CCListener* pListener){
	return false;
}

void CCWidget::Run(){
	OnRun();

	for(int i=0; i<m_Children.GetCount(); i++){
		CCWidget* pCurWnd = m_Children.Get(i);
		pCurWnd->Run();
	}
}
#include "CCDebug.h"

void CCWidget::Draw(CCDrawContext* pDC){
	if(m_bVisible==false) return;

	unsigned char nLastOpacity;
	nLastOpacity = pDC->GetOpacity();

	sRect sr = GetScreenRect();
	pDC->SetOrigin(sPoint(sr.x, sr.y));

	if(m_pFont!=NULL) pDC->SetFont(m_pFont);
	else pDC->SetFont(CCFontManager::Get(NULL));

	pDC->SetOpacity((unsigned char)(nLastOpacity * (float)(m_iOpacity / 255.0f)));
	if(!IsEnable())
		pDC->SetOpacity((unsigned char)(pDC->GetOpacity()*0.70));	

	bool bIntersect = true;
	sRect rectScreen(0, 0, CCGetWorkspaceWidth()-1, CCGetWorkspaceHeight()-1);
	sRect PrevClipRect;
	if(GetParent()!=NULL) {
		sRect parentClipRect = CCClientToScreen(GetParent(), GetParent()->GetClientRect());
		bIntersect = rectScreen.Intersect(&PrevClipRect,parentClipRect);
	}else
		PrevClipRect = rectScreen;

	sRect CurrClipRect = GetScreenRect();
	sRect IntersectClipRect;

	if(m_bClipByParent==true){
		if(PrevClipRect.Intersect(&IntersectClipRect, CurrClipRect)==true){
			sRect test = IntersectClipRect;
			if(IntersectClipRect.w>0 && IntersectClipRect.h>0) {
				pDC->SetClipRect(IntersectClipRect);
				OnDraw(pDC);
			}
		}
	}
	else{
		pDC->SetClipRect(CurrClipRect);
		OnDraw(pDC);
	}

	for(int i=0; i<m_Children.GetCount(); i++){
		CCWidget* pCurWnd = m_Children.Get(i);
		if(pCurWnd==GetLatestExclusive()) continue;
		if(pCurWnd != NULL ) pCurWnd->Draw(pDC);
	}
	if(GetLatestExclusive()!=NULL) 
		GetLatestExclusive()->Draw(pDC);

	pDC->SetOpacity(nLastOpacity);
}

void CCWidget::DrawAfterWidgets( CCDrawContext* pDC ){
	for(int i=0; i<m_Children.GetCount(); i++){
		CCWidget* pCurWnd = m_Children.Get(i);
		if(pCurWnd != NULL ) pCurWnd->DrawAfterWidgets(pDC);
	}
}

void CCWidget::Redraw(){
	Core::GetInstance()->Update();
}

bool CCWidget::Event(CCEvent* pEvent){
	if(m_bVisible==false) return false;

	sRect r = GetRect();
	r.x = r.y = 0;

	if(pEvent->iMessage==CCWM_LBUTTONDOWN)
		int k=0;

	CCEvent LocalEvent;
	MakeLocalEvent(&LocalEvent, pEvent);

	if(m_iDebugType==2){
		if(pEvent->iMessage==CCWM_LBUTTONDOWN )
			int k=0;
	}

	if(r.InPoint(LocalEvent.sPos)==true && (CCWidget::m_pCapturedWidget==NULL || CCWidget::m_pCapturedWidget==this) && IsVisible()==true){
		if(m_pCursor!=NULL) CCCursorSystem::Set(m_pCursor);
		else CCCursorSystem::Set(CCCURSOR_ARROW);	

		if(m_pToolTip!=NULL){
			SetZOrder(CC_TOP);
			m_pToolTip->Show(true);
		}
	}
	else{
		if(m_pToolTip!=NULL){
			m_pToolTip->Show(false);
		}
	}

	if(m_bEnable==false) return false;

	if(GetLatestExclusive()!=NULL){
		if(GetLatestExclusive()->Event(pEvent)==true) return true;
		if(GetLatestExclusive()!=NULL) return false;
	}

	if(pEvent->iMessage==CCWM_LBUTTONDOWN){
		if ( r.InPoint(LocalEvent.sPos)==true){
			SetFocus();
 			if((m_bFocusEnable)&&(m_bZOrderChangable==true)) SetZOrder(CC_TOP);
		}
		else	// ReleaseFocus
		{
//			ReleaseFocus();
//			OnReleaseFocus();
//			ReleaseCapture();
		}
	}

	if(GetLatestExclusive()==NULL){
		for(int i=m_Children.GetCount()-1; i>=0; i--){
			CCWidget* pCurWnd = m_Children.Get(i);
			if(pCurWnd->Event(pEvent)==true) {
				return true;
			}
		}
	}

	if(EventResize(pEvent)==true) return true;

	if(pEvent->iMessage==CCWM_HOTKEY){
		if(OnHotKey(pEvent->uKey)==true) return true;
	}

	if(pEvent->iMessage==CCWM_CHAR || pEvent->iMessage==CCWM_KEYDOWN || pEvent->iMessage==CCWM_IMECOMPOSE ||
		pEvent->iMessage==CCWM_ACTIONKEYDOWN || pEvent->iMessage==CCWM_ACTIONKEYUP || pEvent->iMessage==CCWM_ACTIONPRESSED || pEvent->iMessage==CCWM_ACTIONRELEASED){
		if (IsFocus()) {
			if(OnEvent(&LocalEvent, GetListener())==true) 
				return true;

			// Tab Key
			if(pEvent->iMessage==CCWM_KEYDOWN && pEvent->uKey==VK_TAB){
				if(OnTab(!pEvent->GetShiftState())==true) 
					return true;
			}
		}
	}
	else{
		if((CCWidget::m_pCapturedWidget==NULL || CCWidget::m_pCapturedWidget==this))
			if(OnEvent(&LocalEvent, GetListener())==true) 
				return true;
	}

	if(pEvent->iMessage==CCWM_LBUTTONUP) ReleaseCapture();

	return false;
}

bool CCWidget::EventAccelerator(CCEvent* pEvent){
	if(m_bVisible==false) return false;	
	if(m_bEnable==false) return false;

	if(!(pEvent->iMessage==CCWM_KEYDOWN || pEvent->iMessage==CCWM_SYSKEYDOWN)) return false;

	if(pEvent->iMessage==CCWM_KEYDOWN)
		if(CCWidget::m_pFocusedWidget!=NULL)
			if(strcmp(CCWidget::m_pFocusedWidget->GetClassName(), CORE_CCEDIT)==0) return false;

	if(GetLatestExclusive()!=NULL){
		if(GetLatestExclusive()->EventAccelerator(pEvent)==true) return true;
		if(GetLatestExclusive()!=NULL) return false;
	}

	if(GetLatestExclusive()==NULL){
		for(int i=m_Children.GetCount()-1; i>=0; i--){
			CCWidget* pCurWnd = m_Children.Get(i);
			if(pCurWnd->EventAccelerator(pEvent)==true) return true;
		}
	}

	// Accelerator
	char szKey[2] = {(char)pEvent->uKey, 0};
	if(m_iAccelerator==szKey[0]) {

		m_bEventAcceleratorCall = true;

		if(DefaultCommand()==true) {
			m_bEventAcceleratorCall = false;
			return true;
		}
		m_bEventAcceleratorCall = false;
	}

	return false;
}

bool CCWidget::EventDefaultKey(CCEvent* pEvent){
	if(m_bVisible==false) return false;
	if(m_bEnable==false) return false;

	if(!(pEvent->iMessage==CCWM_KEYDOWN))return false;

	if(GetLatestExclusive()!=NULL){
		if(GetLatestExclusive()->EventDefaultKey(pEvent)==true) return true;
		if(GetLatestExclusive()!=NULL) return false;
	}

	if(GetLatestExclusive()==NULL){
		for(int i=m_Children.GetCount()-1; i>=0; i--){
			CCWidget* pCurWnd = m_Children.Get(i);
			if(pCurWnd->EventDefaultKey(pEvent)==true) return true;
		}
	}

	if(strcmp(GetClassName(), CORE_CCBUTTON)!=0) return false;

	CCButton* pButton = (CCButton*)this;
	if((pButton->m_uKeyAssigned==CCBKA_ENTER && pEvent->uKey==VK_RETURN) ||
		(pButton->m_uKeyAssigned==CCBKA_ESC && pEvent->uKey==VK_ESCAPE))
		if(DefaultCommand()==true) return true;

	return false;
}

bool CCWidget::OnShow(){
	return true;
}

void CCWidget::OnHide(){
}

void CCWidget::OnSize(int w, int h){
	if (m_pToolTip)
		m_pToolTip->SetBounds();
}

void CCWidget::OnShow(bool bVisible){
	for(int i=m_Children.GetCount()-1; i>=0; i--){
		CCWidget* pCurWnd = m_Children.Get(i);
		if(pCurWnd->m_bVisible==true) pCurWnd->OnShow(bVisible);
	}

	if(bVisible==true) OnShow();
	else OnHide();

	if(bVisible==false && CCWidget::m_pFocusedWidget==this) ReleaseFocus();
}

bool CCWidget::OnTab(bool bForward){
	CCWidget* pParent = GetParent();
	if(pParent==NULL) return false;

	if(m_pParent->GetLatestExclusive()==this) return false;

	int nThisIndex = pParent->GetChildIndex(this);
	if(nThisIndex<0) return false;

	for(int i=0; i<pParent->GetChildCount(); i++){
		int nIndex = 0;
		if(bForward==true) nIndex = (nThisIndex+i+1) % pParent->GetChildCount();
		else nIndex = (nThisIndex+pParent->GetChildCount()-1-i) % pParent->GetChildCount();

		CCWidget* pSibling = pParent->GetChild( nIndex );
		if(pSibling->IsFocusEnable()==true && pSibling!=this && pSibling->IsVisible()==true && pSibling->IsEnable()==true){
			pSibling->SetFocus();
			return true;
		}
	}

	return false;
}

bool CCWidget::OnDrop(CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString){
	return false;
}

void CCWidget::ResizeChildrenByAnchor(int w, int h){
	for(int i=0; i<m_Children.GetCount(); i++){
		CCWidget* pChild = m_Children.Get(i);
		if(pChild->GetBoundsAlignment()!=0){
			pChild->SetBoundsAlignment(pChild->GetBoundsAlignment(), w, h);
			continue;
		}

		sRect r = pChild->m_Rect;
		if(pChild->m_Anchors.m_bLeft==true && pChild->m_Anchors.m_bRight==true){
			r.w += (w-m_Rect.w);
		}
		else if(pChild->m_Anchors.m_bRight==true){
			r.x += (w-m_Rect.w);
		}
		if(pChild->m_Anchors.m_bTop==true && pChild->m_Anchors.m_bBottom==true){
			r.h += (h-m_Rect.h);
		}
		else if(pChild->m_Anchors.m_bBottom==true){
			r.y += (h-m_Rect.h);
		}
		pChild->SetBounds(r);
	}
}

void CCWidget::Show(bool bVisible, bool bModal){
	if(m_bVisible==bVisible){
		if(bModal==true){
			if(m_pParent!=NULL && m_pParent->GetLatestExclusive()==this)
				return;
		}
		else return;
	}

	m_bVisible = bVisible;

	if(bVisible==true && bModal==true) SetExclusive();
	else if(bVisible==false) {
		ReleaseExclusive();
		if(CCWidget::m_pFocusedWidget==this) ReleaseFocus();
	}

	OnShow(bVisible);
}

void CCWidget::Enable(bool bEnable){
	m_bEnable = bEnable;
}

bool CCWidget::IsVisible(){
	return m_bVisible;
}

bool CCWidget::IsEnable(){
	return m_bEnable;
}

void CCWidget::SetResizable(bool bEnable){
	m_bResizable = bEnable;
}

bool CCWidget::IsResizable(){
	return m_bResizable;
}

void CCWidget::SetListener(CCListener* pListener){
	m_pListener = pListener;
}

CCListener* CCWidget::GetListener(){
	return m_pListener;
}

int CCWidget::GetID(){
	return m_iID;
}

void CCWidget::SetID(int nID){
	m_iID = nID;
}

void CCWidget::SetText(const char* szText){
	if(szText==NULL) return;
	_ASSERT(strlen(szText)<CCWIDGET_NAME_LENGTH);
	if (strlen(szText) >= CCWIDGET_NAME_LENGTH) return;

	strcpy(m_szName, szText);
	if(m_pToolTip!=NULL) if(m_pToolTip->IsUseParentName()==true) m_pToolTip->SetBounds();

	SetLabelAccelerator();
}

const char* CCWidget::GetText(){
	return m_szName;
}

void CCWidget::SetCapture(){
	CCWidget::m_pCapturedWidget = this;
}

void CCWidget::ReleaseCapture(){
	CCWidget::m_pCapturedWidget = NULL;
}

void CCWidget::SetFocusEnable(bool bEnable){
	m_bFocusEnable = bEnable;
}

bool CCWidget::IsFocusEnable(){
	return m_bFocusEnable;
}

void CCWidget::SetFocus(){
	if(m_bFocusEnable==false) 
		return;

	CCWidget* pExDes = FindExclusiveDescendant();
	if(pExDes!=NULL) 
		return;

	if(CCWidget::m_pFocusedWidget==this) return;

	if(CCWidget::m_pFocusedWidget!=NULL) CCWidget::m_pFocusedWidget->OnReleaseFocus();

	CCWidget::m_pFocusedWidget = this;
	OnSetFocus();
}

void CCWidget::ReleaseFocus(){
	if(CCWidget::m_pFocusedWidget==this) OnReleaseFocus();
	CCWidget::m_pFocusedWidget = NULL;
}

bool CCWidget::IsFocus(){
	if(CCWidget::m_pFocusedWidget==this) return true;
	return false;
}

CCWidget* CCWidget::GetParent(){
	return m_pParent;
}

int CCWidget::GetChildCount(){
	return m_Children.GetCount();
}

CCWidget* CCWidget::GetChild(int i){
	return m_Children.Get(i);
}

int CCWidget::GetChildIndex(CCWidget* pWidget){
	for(int i=0; i<m_Children.GetCount(); i++){
		CCWidget* pChild = m_Children.Get(i);
		if(pChild==pWidget) return i;
	}

	return -1;
}

void CCWidget::SetExclusive(){
	if(m_pParent!=NULL){
		m_pParent->AddExclusive(this);
		SetFocus();
	}
}

void CCWidget::ReleaseExclusive(){
	if(m_pParent!=NULL)
		m_pParent->RemoveExclusive(this);
}

CCCursor* CCWidget::SetCursor(CCCursor* pCursor){
	CCCursor* pTemp = m_pCursor;
	m_pCursor = pCursor;
	return pTemp;
}

CCCursor* CCWidget::GetCursor(){
	return m_pCursor;
}

CCFont* CCWidget::SetFont(CCFont* pFont){
	CCFont* pTemp = m_pFont;
	m_pFont = pFont;
	return pTemp;
}

CCFont* CCWidget::GetFont(){
	if(m_pFont==NULL) return CCFontManager::Get(NULL);
	else return m_pFont;
}

void CCWidget::SetSize(int w, int h){
	if(w<0) w = 1;
	if(h<0) h = 1;

	ResizeChildrenByAnchor(w, h);

	m_Rect.w = w;
	m_Rect.h = h;

	OnSize(w, h);
}

void CCWidget::SetSize(sSize& s){
	SetSize(s.w, s.h);
}


void CCWidget::SetPosition(int x, int y){
	sPoint p;

	m_Rect.x = x;
	m_Rect.y = y;
}

void CCWidget::SetPosition(sPoint& p){
	SetPosition(p.x, p.y);
}

void CCWidget::SetBounds(sRect& r){
	SetBounds(r.x, r.y, r.w, r.h);
}

void CCWidget::SetBounds(int x, int y, int w, int h){
	SetPosition(x, y);

	if(w<0) w = 1;
	if(h<0) h = 1;

	SetSize(w, h);
}

sPoint CCWidget::GetPosition(){
	return sPoint(m_Rect.x, m_Rect.y);
}

sRect CCWidget::GetRect(){
	return m_Rect;
}

sRect CCWidget::GetIDLRect(){
	return m_IDLRect;
}

void CCWidget::GetBoundsAlignmentPosition(sPoint* p, CCAlignmentMode am, int w, int h){
	p->x = m_Rect.x;
	p->y = m_Rect.y;

	sSize s(w, h);
	if(w==-1){
		s.w = CCGetWorkspaceWidth();
		if(GetParent()!=NULL) s.w = GetParent()->m_Rect.w;
	}
	if(h==-1){
		s.h = CCGetWorkspaceHeight();
		if(GetParent()!=NULL) s.h = GetParent()->m_Rect.h;
	}

	if(m_BoundsAlignment&CCD_LEFT) p->x = 0;
	else if(m_BoundsAlignment&CCD_RIGHT) p->x = s.w-m_Rect.w;
	else if(m_BoundsAlignment&CCD_HCENTER) p->x = (s.w-m_Rect.w)/2;

	if(m_BoundsAlignment&CCD_LEFT) p->y = 0;
	else if(m_BoundsAlignment&CCD_BOTTOM) p->y = s.h-m_Rect.h;
	else if(m_BoundsAlignment&CCD_VCENTER) p->y = (s.h-m_Rect.h)/2;
}

void CCWidget::SetBoundsAlignment(CCAlignmentMode am, int w, int h){
	m_BoundsAlignment = am;

	sPoint p;
	GetBoundsAlignmentPosition(&p, m_BoundsAlignment, w, h);

	m_Rect.x = p.x;
	m_Rect.y = p.y;
}

CCAlignmentMode CCWidget::GetBoundsAlignment(){
	return m_BoundsAlignment;
}


sRect CCWidget::GetScreenRect(){
	if(m_pParent!=NULL){
		sRect sr = m_pParent->GetScreenRect();
		sRect r = m_Rect;
		r.Offset(sr.x, sr.y);
		return r;
	}

	return m_Rect;
}

void CCWidget::AttachToolTip(const char* szToolTipString){
	DetachToolTip();
	m_pToolTip = new CCToolTip(szToolTipString, this);
}

void CCWidget::AttachToolTip(CCToolTip* pToolTip){
	DetachToolTip();
	m_pToolTip = pToolTip;
}

void CCWidget::DetachToolTip(){
	if(m_pToolTip!=NULL) delete m_pToolTip;
	m_pToolTip = NULL;
}

CCToolTip* CCWidget::GetToolTip(){
	return m_pToolTip;
}

void CCWidget::SetAccelerator(int a){
	if(a==0) m_iAccelerator = -1;
	else m_iAccelerator = a;
}

void CCWidget::SetLabelAccelerator(){
	SetAccelerator(GetLabelAccelerator());
}

char CCWidget::GetLabelAccelerator(){
	char szAnd[2] = {GetAndChar(m_szName), 0};
	_strupr(szAnd);
	if(szAnd[0]==0) return -1;
	return szAnd[0];
}

char CCWidget::GetToolTipAccelerator(){
	if(m_pToolTip!=NULL){
		char szAnd[2] = {GetAndChar(m_pToolTip->m_szName), 0};
		_strupr(szAnd);
		if(szAnd[0]==0) return -1;
		return szAnd[0];
	}
	else{
		return -1;
	}
}

sRect CCWidget::GetClientRect(){
	return sRect(0, 0, m_Rect.w, m_Rect.h);
}

sRect CCWidget::GetInitialClientRect(){
	return CCWidget::GetClientRect();
}

void CCWidget::SetZOrder(CCZOrder z){
	if(m_pParent==NULL) return;

	CCWidget* pParent = m_pParent;
	pParent->RemoveChild(this);

	switch(z){
	case CC_TOP:
		pParent->AddChild(this);
		break;
	case CC_BOTTOM:
		pParent->InsertChild(this);
		break;
	}
}

CCWidget* CCWidget::FindExclusiveDescendant(){
	if(m_Exclusive.GetCount()>0) return m_Exclusive.Get(m_Exclusive.GetCount()-1);

	for(int i=0; i<m_Children.GetCount(); i++){
		CCWidget* pChild = m_Children.Get(i);
		CCWidget* pExDes = pChild->FindExclusiveDescendant();
		if(pExDes!=NULL) return pExDes;
	}

	return NULL;
}

bool CCWidget::IsExclusive(CCWidget* pWidget){
	for(int i=0; i<m_Exclusive.GetCount(); i++){
		CCWidget* pThis = m_Exclusive.Get(i);
		if(pThis==pWidget) return true;
	}
	return false;
}


CCWidget* CCWidget::Find(sPoint& p){
	if(IsVisible()==false) return NULL;

	for(int i=0; i<m_Children.GetCount(); i++){
		CCWidget* pChild = m_Children.Get(m_Children.GetCount()-i-1);
		CCWidget* pFind = pChild->Find(p);
		if(pFind!=NULL) return pFind;
	}

	if(GetScreenRect().InPoint(p)==true)
		return this;

	return NULL;
}

CCWidget* CCWidget::FindDropAble(sPoint& p){
	if(IsVisible()==false) return NULL;

	for(int i=0; i<m_Children.GetCount(); i++){
		CCWidget* pChild = m_Children.Get(m_Children.GetCount()-i-1);
		CCWidget* pFind = pChild->FindDropAble(p);
		if(pFind!=NULL) return pFind;
	}

	if(GetScreenRect().InPoint(p)==true)
		if(m_bFocusEnable==true)
			return this;

	return NULL;
}


bool CCWidget::Drop(CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString){
	return OnDrop(pSender, pBitmap, szString, szItemString);
}

void CCWidget::GetHierarchicalName(char* szName){
	if(m_pParent!=NULL){
		m_pParent->GetHierarchicalName(szName);
		strcat(szName, "/");
		strcat(szName, m_szName);
	}
	else{
		strcpy(szName, m_szName);
	}
}

CCWidget* CCWidget::FindWidgetByHierarchicalName(const char* szName){
	char szHierachicalName[2048];
	GetHierarchicalName(szHierachicalName);
	if(strcmp(szName, szHierachicalName)==0){
		return this;
	}

	for(int i=0; i<GetChildCount(); i++){
		CCWidget* pFind = GetChild(i)->FindWidgetByHierarchicalName(szName);
		if(pFind!=NULL) return pFind;
	}

	return NULL;
}

bool CCWidget::IsMsg(const char* szMsg1, const char* szMsg2){
	char szTmp1[256];
	char szTmp2[256];
	strcpy(szTmp1, szMsg1);
	strcpy(szTmp2, szMsg2);
	_strupr(szTmp1);
	_strupr(szTmp2);
	if(strcmp(szTmp1, szTmp2)==0) return true;
	return false;
}

void* CCWidget::Query(const char* szQuery){
	return NULL;
}

void CCWidget::SetOpacity(unsigned char nOpacity){
	m_iOpacity = nOpacity;
}

void CCWidget::MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight){
	if(GetIDLRect().w>0 && GetIDLRect().h>0)	
	{
		m_Rect.x = int(m_IDLRect.x * byIDLWidth +0.5f);
		m_Rect.w = int(m_IDLRect.w * byIDLWidth +0.5f);
		m_Rect.y = int(m_IDLRect.y * byIDLHeight +0.5f);
		m_Rect.h = int(m_IDLRect.h * byIDLHeight +0.5f);
	}
	else 
	{
		m_Rect.x = int(m_Rect.x * byCurrWidth +0.5f);
		m_Rect.w = int(m_Rect.w * byCurrWidth +0.5f);
		m_Rect.y = int(m_Rect.y * byCurrHeight +0.5f);
		m_Rect.h = int(m_Rect.h * byCurrHeight +0.5f);
	}

	SetBoundsAlignment( GetBoundsAlignment(), -1, -1);

	int n = GetChildCount();
	for( int i = 0; i < n; ++i)
	{
		GetChild(i)->MultiplySize(byIDLWidth, byIDLHeight, byCurrWidth, byCurrHeight);
	}
}

unsigned char CCWidget::GetOpacity(){
	return m_iOpacity;
}

int GetAndPos(const char* szText){
	if(szText==NULL) return -1;
	int nLen = strlen(szText);
	for(int i=0; i<nLen; i++){
		if(szText[i]=='&') return i;
	}
	return -1;
}

char GetAndChar(const char* szText){
	int nLen = GetAndPos(szText);
	if(nLen==-1) return 0;
	return szText[nLen+1];
}

int RemoveAnd(char* szText){
	if(szText==NULL) return -1;
	int nPos = GetAndPos(szText);
	if(nPos==-1) return -1;
	int nLen = strlen(szText);
	for(int i=0; i<(nLen-nPos); i++){
		szText[nPos+i] = szText[nPos+i+1];
	}
	return nPos;
}

int RemoveAnd(char* szRemovedText, const char* szText){
	if(szText==NULL) return -1;
	strcpy(szRemovedText, szText);
	return RemoveAnd(szRemovedText);
}

int RemoveAnd(char* szRemovedFrontText, char* cUnderLineChar, char* szRemovedBackText, const char* szText){
	if(szText==NULL) return -1;
	int nPos = GetAndPos(szText);
	if(nPos==-1) return -1;
	memcpy(szRemovedFrontText, szText, nPos);
	szRemovedFrontText[nPos] = 0;
	*cUnderLineChar = szText[nPos+1];
	strcpy(szRemovedBackText, szText+nPos+2);
	return nPos;
}

sPoint CCClientToScreen(CCWidget* pWidget, sPoint& p){
	sRect r = pWidget->GetScreenRect();
	return sPoint(p.x+r.x, p.y+r.y);
}

sPoint CCScreenToClient(CCWidget* pWidget, sPoint& p){
	sRect r = pWidget->GetScreenRect();
	return sPoint(p.x-r.x, p.y-r.y);
}

sRect CCClientToScreen(CCWidget* pWidget, sRect& p){
	sRect r = pWidget->GetScreenRect();
	return sRect(p.x+r.x, p.y+r.y, p.w, p.h);
}

sRect CCScreenToClient(CCWidget* pWidget, sRect& p){
	sRect r = pWidget->GetScreenRect();
	return sRect(p.x-r.x, p.y-r.y, p.w, p.h);
}

sPoint GetCursorPosition(){
	POINT p;
	GetCursorPos(&p);
	sPoint mp;
	mp.x = p.x;
	mp.y = p.y;
	return mp;
}