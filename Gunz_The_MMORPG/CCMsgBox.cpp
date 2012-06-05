#include "stdafx.h"
#include "CCMsgBox.h"
#include "CCStringTable.h"
#include "Core.h"

int charinstr(char* sz, char c)
{
	int len = strlen(sz);
	for(int i=0; i<len; i++){
		if(sz[i]==c) return i;
	}
	return -1;
}

/*
void CCMsgBox::OnDrawText(CCDrawContext* pDC)
{
	//CCFrameDrawer* pFD = GetFrameDrawer();

	pDC->SetColor(sColor(255, 255, 255));

	char* pStr = m_szMessage;
	int nLen = charinstr(pStr, '\n');
	int nY = m_Rect.y+10;
	if(nLen<0){
		//pFD->Text(pDC, sPoint(m_Rect.x+10, m_Rect.y+10), pStr, MFDTS_NORMAL, false, &m_Rect);
		pDC->Text(m_Rect.x+10, m_Rect.y+10, pStr);
	}
	else{
		do{
			char temp[MWIDGET_NAME_LENGTH];
			memcpy(temp, pStr, nLen);
			temp[nLen] = 0;
			//pFD->Text(pDC, sPoint(m_Rect.x+10, nY), temp, MFDTS_NORMAL, false, &m_Rect);
			pDC->Text(m_Rect.x+10, nY, temp);
			pStr += (nLen+1);
			nLen = charinstr(pStr, '\n');
			nY += GetFont()->GetHeight();
		}while(nLen>0);
	}
}

void CCMsgBox::OnDraw(CCDrawContext* pDC)
{
	OnDrawText(pDC);
}
*/



#define CCMSGBOX_W	CONVERT800(400)
#define CCMSGBOX_X	(CCGetWorkspaceWidth()/2-CCMSGBOX_W/2)
#define CCMSGBOX_H	CONVERT600(120)
#define CCMSGBOX_Y	(CCGetWorkspaceHeight()/2-CCMSGBOX_H/2)

//#define CCMSGBOX_OK_W	80
//#define CCMSGBOX_OK_H	32
#define CCMSGBOX_OK_W	CONVERT800(80)
#define CCMSGBOX_OK_H	CONVERT600(32)

// 예전 디파인 값
/*#define CCMSGBOX_W	400
#define CCMSGBOX_X	(CCGetWorkspaceWidth()/2-CCMSGBOX_W/2)
#define CCMSGBOX_H	140
#define CCMSGBOX_Y	(CCGetWorkspaceHeight()/2-CCMSGBOX_H/2)

#define CCMSGBOX_OK_W	80
#define CCMSGBOX_OK_H	32
*/
bool CCMsgBox::OnShow(void)
{
  	AdjustSize();
    
	if(m_pOK!=NULL && m_pOK->IsVisible()==true) 
		m_pOK->SetFocus();
	else if(m_pCancel!=NULL) m_pCancel->SetFocus();

	return true;
}

void CCMsgBox::OnSize(int w, int h)
{
#if 0 //예전 코드
 	sRect r = GetInitialClientRect();
	m_pMessage->SetBounds(sRect(r.x+19, r.y+40, r.w-38, r.h-70));
	int nOKOffset = 0;
	if(m_iType==CCT_OKCANCEL || m_iType==CCT_YESNO) nOKOffset = CCMSGBOX_OK_W+5;
	if(m_pOK!=NULL) m_pOK->SetBounds(sRect(r.x+r.w-CCMSGBOX_OK_W-nOKOffset - 19, r.y+r.h-CCMSGBOX_OK_H-20, CCMSGBOX_OK_W, CCMSGBOX_OK_H));
	if(m_pCancel!=NULL) m_pCancel->SetBounds(sRect(r.x+r.w-CCMSGBOX_OK_W - 19, r.y+r.h-CCMSGBOX_OK_H-20, CCMSGBOX_OK_W, CCMSGBOX_OK_H));

#else

	// 해상도 변경에 따라 메시지 박스의 크기가 조정되도록 변경

	sRect r = GetInitialClientRect();
	/*r.x = CONVERT800(r.x);
	r.w = CONVERT800(r.w);
	r.y = CONVERT600(r.y);
	r.h = CONVERT600(r.h);*/

	m_pMessage->SetBounds(sRect(
		r.x + CONVERT800(19),
		r.y + CONVERT600(40),
		r.w - CONVERT800(38),
		r.h - CONVERT600(70)));

	int nOKOffset = 0;
	if(m_iType==CCT_OKCANCEL || m_iType==CCT_YESNO) 
		nOKOffset = CCMSGBOX_OK_W+ CONVERT800(5);

	if(m_pOK!=NULL) m_pOK->SetBounds(sRect(
		r.x+r.w-CCMSGBOX_OK_W-nOKOffset - CONVERT800(19), 
		r.y+r.h-CCMSGBOX_OK_H-CONVERT600(20), 
		CCMSGBOX_OK_W, 
		CCMSGBOX_OK_H));
	
	if(m_pCancel!=NULL) m_pCancel->SetBounds(sRect(
		r.x+r.w-CCMSGBOX_OK_W - CONVERT800(19), 
		r.y+r.h-CCMSGBOX_OK_H-CONVERT600(20), 
		CCMSGBOX_OK_W, 
		CCMSGBOX_OK_H));
#endif
}

void CCMsgBox::MultiplySize( float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight )
{
	// OnSize에서 화면 해상도에 맞게 재설정하고 있으므로 특별히 배율을 적용할 필요가 없다
	AdjustSize();
}

CCMsgBox::CCMsgBox(const char* szMessage, CCWidget* pParent, CCListener* pListener, CCMsgBoxType nType)
: CCFrame(CCGetString(MSID_MESSAGE), pParent, (pListener==NULL)?pParent:pListener)
{
//	m_pMessage = new CCLabel(szMessage, this, this);
	m_pMessage = new CCTextArea(255, szMessage, this, this);
	m_pMessage->SetEditable(false);

	m_pOK = NULL;
	m_pCancel = NULL;

	if(nType!=CCT_NOTDECIDED)
		SetType(nType);

	m_bResizable = false;

	AdjustSize();

	Show(false);
}

CCMsgBox::~CCMsgBox(void)
{
	delete m_pMessage;
	if(m_pOK!=NULL) delete m_pOK;
	if(m_pCancel!=NULL) delete m_pCancel;
}

void CCMsgBox::SetType(CCMsgBoxType nType)
{
	m_iType = nType;

	if(m_pOK) delete m_pOK;
	if(m_pCancel) delete m_pCancel;

	if(nType!=CCT_CANCEL){
		//m_pOK = new CCButton(CCGetString(MSID_OK), this, this);
		m_pOK = (CCButton*)Core::GetInstance()->NewWidget(CORE_CCBUTTON, CCGetString(MSID_OK), this, this );
		m_pOK->SetLabelAccelerator();
		m_pOK->m_uKeyAssigned = CCBKA_ENTER;
	}
	if(nType==CCT_YESNO) m_pOK->SetText(CCGetString(MSID_YES));

	if(nType==CCT_OKCANCEL || nType==CCT_YESNO || nType==CCT_CANCEL){
		//m_pCancel = new CCButton(CCGetString(MSID_CANCEL), this, this);
		m_pCancel = (CCButton*)Core::GetInstance()->NewWidget(CORE_CCBUTTON, CCGetString(MSID_CANCEL), this, this );
		if(nType==CCT_YESNO) m_pCancel->SetText(CCGetString(MSID_NO));
		m_pCancel->SetAccelerator(27);
		m_pCancel->m_uKeyAssigned = CCBKA_ESC;
	}
	CCMsgBox::OnSize(CCMSGBOX_W, CCMSGBOX_H);
}

bool CCMsgBox::OnCommand(CCWidget* pWindow, const char* szMessage)
{
	CCListener* pListener = GetListener();
	if(pListener==NULL) return false;

	if(pWindow==m_pOK && strcmp(szMessage, CCBTN_CLK_MSG)==0){
		if(m_iType==CCT_YESNO) pListener->OnCommand(this, CCMSGBOX_YES);
		else pListener->OnCommand(this, CCMSGBOX_OK);
		return true;
	}
	else if(pWindow==m_pCancel && strcmp(szMessage, CCBTN_CLK_MSG)==0){
		if(m_iType==CCT_YESNO) pListener->OnCommand(this, CCMSGBOX_NO);
		else pListener->OnCommand(this, CCMSGBOX_CANCEL);
		return true;
	}

	return false;
}

void CCMsgBox::SetTitle(const char* szTitle)
{
	strcpy(m_szName, szTitle);
}

const char* CCMsgBox::GetTitle(void)
{
	return m_szName;
}

void CCMsgBox::SetMessage(const char* szMessage)
{
	m_pMessage->SetText(szMessage);
	
	AdjustSize();
}

const char* CCMsgBox::GetMessage(void)
{
	static char buffer[1024];
	buffer[0]=0;
	if(m_pMessage->GetText(buffer,sizeof(buffer)))
		return buffer;
	return buffer;
}

void CCMsgBox::SetText(const char* szText)
{
	SetMessage(szText);
}

const char* CCMsgBox::GetText(void)
{
	return GetMessage();
}

void CCMsgBox::AdjustSize()
{
	int nLineCount = CCGetLineCount(m_pMessage->GetFont(),GetMessage(),CCMSGBOX_W);

	SetBounds(sRect(CCMSGBOX_X, CCMSGBOX_Y, CCMSGBOX_W, CCMSGBOX_H + nLineCount*m_pMessage->GetFont()->GetHeight()));
}