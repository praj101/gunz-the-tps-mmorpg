#include "stdafx.h"

#include "ZApplication.h"
#include "ZMapListBox.h"
#include "CCListBox.h"
#include "CCZFileSystem.h"
#include "Core4R2.h"
#include "CCTextArea.h"
#include "ZChannelRule.h"

// Added R347a
#include "ZGlobal.h"
// Added R349a
#include "Core.h"

#define PREVIEW_W	200

void ZMapListBox::OnDraw(CCDrawContext* pDC)
{
	if(m_pThumbnail!=NULL){
		sRect r = GetClientRect();
		pDC->SetBitmap(m_pThumbnail);
		pDC->Draw(r.x+r.w-PREVIEW_W+10, r.y);
	}
}

bool ZMapListBox::OnShow()
{
	Refresh(ZApplication::GetFileSystem());

	if(m_pThumbnail!=NULL){
		delete m_pThumbnail;
		m_pThumbnail = NULL;
	}

	// 처음 아이템을 가리킨다.
	SetSelIndex(0);

	m_pListBox->SetFocus();

	return true;
}

bool ZMapListBox::OnCommand(CCWidget* pWidget, const char* szMessage)
{
	if(pWidget==m_pListBox && strcmp(szMessage, CCLB_ITEM_SEL)==0){
		SetSelIndex(-1);
		return true;
	}
	else if(pWidget==m_pListBox && strcmp(szMessage, CCLB_ITEM_DBLCLK)==0){
		if(GetListener()!=NULL)return GetListener()->OnCommand(pWidget, szMessage);
	}
	return false;
}

ZMapListBox::ZMapListBox(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCWidget(szName, pParent, pListener)
{
	SetBounds(0, 0, 300, 200);

	sRect r = GetClientRect();

	m_pListBox = new CCListBox("MapList", this, this);
	m_pListBox->SetBounds(r.x, r.y, r.w-PREVIEW_W-10, r.h-r.y);
	m_pListBox->m_Anchors.m_bLeft = true;
	m_pListBox->m_Anchors.m_bTop = true;
	m_pListBox->m_Anchors.m_bRight = true;
	m_pListBox->m_Anchors.m_bBottom = true;

	m_pThumbnail = NULL;
}

ZMapListBox::~ZMapListBox()
{
	if(m_pThumbnail!=NULL){
		delete m_pThumbnail;
		m_pThumbnail = NULL;
	}
	delete m_pListBox;
}

void ZMapListBox::Refresh(CCZFileSystem* pFS)
{
	m_pListBox->RemoveAll();

#define EXT	".rs"
#define MAPDIRECTORY "Maps/"

	CCChannelRule* pRule = ZGetChannelRuleMgr()->GetCurrentRule();
	if (pRule == NULL) {
		cclog("ZMapListBox::Refresh() > No Current ChannelRule \n");
		return;
	}

	int nExtLen = (int)strlen(EXT);
	for(int i=0; i<pFS->GetFileCount(); i++){
		const char* szFileName = pFS->GetFileName(i);
		const CCZFILEDESC* desc = pFS->GetFileDesc(i);
		int nLen = (int)strlen(szFileName);

		if( strnicmp(desc->m_szFileName,MAPDIRECTORY,strlen(MAPDIRECTORY))==0 &&
			nLen>nExtLen && stricmp(szFileName+nLen-nExtLen, EXT)==0 )
		{
			char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
			_splitpath(szFileName,drive,dir,fname,ext);

			if (ZIsLaunchDevelop()) 
			{
				m_pListBox->Add(fname);
//			} else {
//				if (pRule->CheckMap(fname))
//					m_pListBox->Add(fname);
			}
		}
	}
}

const char* ZMapListBox::GetSelItemString()
{
	return m_pListBox->GetSelItemString();
}

void ZMapListBox::SetSelIndex(int i)
{
	if(i>=0 && i<m_pListBox->GetCount()) m_pListBox->SetSelIndex(i);

	if(m_pThumbnail!=NULL){
		delete m_pThumbnail;
		m_pThumbnail = NULL;
	}

	const char* szName = m_pListBox->GetSelItemString();
	if(szName==NULL) return;
	char szThumbnail[256];
	sprintf(szThumbnail, "%s.rs.bmp", szName);

	m_pThumbnail = Core::GetInstance()->OpenBitmap(szThumbnail);
}
