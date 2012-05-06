#pragma once

#include "Core4R2.h"
#include "RFrameWork.h"
#include "ZButton.h"
#include "ZMsgBox.h"
#include "ZActionKey.h"

_USING_NAMESPACE_REALSPACE2

class Core4Gunz : public Core{
public:
	virtual void Update(){
		RealSpace2::RFrame_Render();
		//RenderScene(NULL);
	}
	virtual CCBitmap* OpenBitmap(const char* szName){

		char aliasname[256];
		char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
		_splitpath(szName,drive,dir,fname,ext);
		sprintf(aliasname,"%s%s",fname,ext);

		CCBitmapR2* pNew = new CCBitmapR2;
		bool bRet = pNew->Create(aliasname, RGetDevice(), szName);
		if(bRet==false){
			delete pNew;
			return NULL;
		}
		return pNew;
	}
	virtual CCFont* OpenFont(const char* szName, int nHeight){
		CCFontR2* pNew = new CCFontR2;
		pNew->Create(szName, szName, nHeight);
		return pNew;
	}
	virtual CCWidget* NewWidget(const char* szClass, const char* szName, CCWidget* pParent, CCListener* pListener){
		if(strcmp(szClass, CORE_CCBUTTON)==0) return new ZButton(szName, pParent, pListener);
		else if( strcmp(szClass, CORE_CCBUTTON)==0) return new ZButton(szName, pParent, pListener);
		else if( strcmp(szClass, CCMSGBOX)==0) return new ZMsgBox(szName, pParent, pListener);
		else if( strcmp(szClass, CORE_ACTIONKEY)==0) return new ZActionKey(szName, pParent, pListener);
		return Core::NewWidget(szClass, szName, pParent, pListener);
	}

	virtual const char* GetActionKeyName(unsigned long int nKey);

	virtual void Draw(){
		Core::Draw();

		if (m_pDC && m_pMainFrame)
			m_pMainFrame->DrawAfterWidgets(m_pDC);
		
		//		sPoint p = CCEvent::GetMousePos();
		sPoint p = CCEvent::LatestPos;

		CCCursorSystem::Draw(GetDrawContext(), p.x, p.y);	// RAONHAJE Mouse Cursor SoftwareDraw
	}
};