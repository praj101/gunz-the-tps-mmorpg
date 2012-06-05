#include "stdafx.h"
#include "CCIDLResource.h"
#include "Core.h"
#include "CCFrame.h"
#include "CCLabel.h"
#include "CCButton.h"
#include "CCEdit.h"
#include "CCListBox.h"
#include "CCPicture.h"
#include "CCScrollBar.h"
#include "CCSlider.h"
#include "CCGroup.h"
#include "CCComboBox.h"
#include "CCToolTip.h"
#include "CCBLabelLook.h"
#include "CCBButtonLook.h"
#include "CCBGroupLook.h"
#include "CCBFrameLook.h"
#include "CCBEditLook.h"
#include "CCBListBoxLook.h"
#include "CCBMultiColListBoxLook.h"
#include "CCBScrollBarLook.h"
#include "CCBSliderLook.h"
#include "CCBGroupLook.h"
#include "CCBitmap.h"
#include "CCPopupMenu.h"
#include "CCAnimation.h"
#include "CCCursor.h"
#include "CCBmButton.h"
#include "CCBmLabel.h"
#include "CCHotKey.h"
//#include "MActionKey.h"
#include "CCMsgBox.h"
#include "CCTextArea.h"
#include "CCBTextAreaLook.h"
#include "CCBSliderLook.h"
#include "CCTabCtrl.h"
#include "CCPanel.h"
#include "CCDebug.h"
#include "CCBTabCtrlLook.h"

CCIDLResource::CCIDLResource()
{
	m_pParent = Core::GetInstance()->GetMainFrame();
}
CCIDLResource::~CCIDLResource()
{
	Clear();
}

sPoint CCIDLResource::GetPoint(::CCXmlElement& element)
{
	sPoint point;
	element.GetChildContents(&point.x, "X");
	element.GetChildContents(&point.y, "Y");
	return point;
}
sRect  CCIDLResource::GetRect(::CCXmlElement& element)
{
	sRect rect;
	element.GetChildContents(&rect.w, "W");
	element.GetChildContents(&rect.h, "H");
	element.GetChildContents(&rect.x, "X");
	element.GetChildContents(&rect.y, "Y");
	return rect;
}
sSize  CCIDLResource::GetSize(::CCXmlElement& element)
{
	sSize size;
	element.GetChildContents(&size.w, "W");
	element.GetChildContents(&size.h, "H");
	return size;
}
sColor CCIDLResource::GetColor(::CCXmlElement& element)
{
	sColor color;

	int r = 0xff, g = 0xff, b = 0xff, a = 0xff;
	element.GetChildContents(&r, "R");
	element.GetChildContents(&g, "G");
	element.GetChildContents(&b, "B");
	element.GetChildContents(&a, "ALPHA");

	color.r = (unsigned char)r;
	color.g = (unsigned char)g;
	color.b = (unsigned char)b;
	color.a = (unsigned char)a;

	return color;
}
sAnchors CCIDLResource::GetAnchors(::CCXmlElement& element)
{
	sAnchors ret;

	element.GetChildContents(&ret.m_bLeft, "LEFT");
	element.GetChildContents(&ret.m_bTop, "TOP");
	element.GetChildContents(&ret.m_bRight, "RIGHT");
	element.GetChildContents(&ret.m_bBottom, "BOTTOM");

	return ret;
}

CCAlignmentMode CCIDLResource::GetAlignmentMode(::CCXmlElement& element)
{
	CCAlignmentMode am = CCD_NOTALIGN;

	char szValue[256] = "";

	if(element.GetChildContents(szValue, "HALIGN")==true){
		if(stricmp(szValue, "LEFT")==0) am |= CCD_LEFT;
		else if(stricmp(szValue, "CENTER")==0) am |= CCD_HCENTER;
		else if(stricmp(szValue, "RIGHT")==0) am |= CCD_RIGHT;
	}

	if(element.GetChildContents(szValue, "VALIGN")==true){
		if(stricmp(szValue, "TOP")==0) am |= CCD_TOP;
		else if(stricmp(szValue, "CENTER")==0) am |= CCD_VCENTER;
		else if(stricmp(szValue, "BOTTOM")==0) am |= CCD_BOTTOM;
	}

	return am;
}

void CCIDLResource::GetFrameBtn(CCFrameBtn* pFrameBtn, CCBFrameLook* pFrameLook, CCXmlElement& element)
{
	if (pFrameLook == NULL) return;

	char szTagName[256];
	::CCXmlElement childElement;

	int iCount = element.GetChildNodeCount();
	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, "BOUNDS"))
		{
			sRect rect = GetRect(childElement);
			pFrameBtn->m_Rect = rect;
		}
		else if (!strcmp(szTagName, "ANCHORS"))
		{
			pFrameBtn->m_Anchors = GetAnchors(childElement);
		}
	}

	pFrameBtn->m_bVisible = true;

//	pFrameBtn->m_pBitmap[0] = pFrameLook->m_pCloseButtonBitmaps[0];
//	pFrameBtn->m_pBitmap[1] = pFrameLook->m_pCloseButtonBitmaps[1];
}

void CCIDLResource::GetBmButtonBitmaps(CCBitmap** ppBitmaps, ::CCXmlElement& element)
{
	char szTagName[256];
	::CCXmlElement bitmapElement;

	int iCount = element.GetChildNodeCount();
	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		bitmapElement = element.GetChildNode(i);
		bitmapElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, "BITMAP"))
		{
			char szType[256];
			bitmapElement.GetAttribute(szType, "type", "");
			CCBitmap* pBitmap = GetBitmap(bitmapElement);

			if (pBitmap != NULL)
			{
				if (!strcmp(szType, "up"))
				{
					ppBitmaps[0] = pBitmap;
				}
				else if (!strcmp(szType, "down"))
				{
					ppBitmaps[1] = pBitmap;
				}
				else if (!strcmp(szType, "disable"))
				{
					ppBitmaps[2] = pBitmap;
				}
			}
		}
	}
}

CCBmButton* CCIDLResource::GetBmButton(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szTagName[1024];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	CCBmButton* pBmButton = (CCBmButton*)Core::GetInstance()->NewWidget(CORE_CCBMBUTTON, "", pParentWidget, pListener);
	InsertWidget(element, pBmButton);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (GetCommonWidgetProperty(pBmButton, childElement, szTagName)) continue;
		
		if (!strcmp(szTagName, "BUTTONLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBButtonLook*>::iterator itor = m_ButtonLookMap.find(szItem);
			if (itor != m_ButtonLookMap.end())
			{
				pBmButton->ChangeCustomLook((CCBButtonLook*)(*itor).second);
			}
		}

		if (!strcmp(szTagName, "BITMAP"))
		{
			char szType[256];
			childElement.GetAttribute(szType, "type", "");
			CCBitmap* pBitmap = GetBitmap(childElement);

			if (pBitmap != NULL)
			{
				if (!strcmp(szType, "up"))
				{
					pBmButton->SetUpBitmap(pBitmap);
				}
				else if (!strcmp(szType, "down"))
				{
					pBmButton->SetDownBitmap(pBitmap);
				}
				else if (!strcmp(szType, "disable"))
				{
					pBmButton->SetDisableBitmap(pBitmap);
				}
				else if (!strcmp(szType, "over"))
				{
					pBmButton->SetOverBitmap(pBitmap);
				}
			}
		}
		else if(!strcmp(szTagName, "CONFIRMMESSAGE"))
		{
			char szContents[256] = {0, };
			char szItem[256] = {0,};
			childElement.GetContents(szContents);
			TransText(szContents, szItem);
			pBmButton->SetConfirmMessageBox(szItem);
		}
		else if(!strcmp(szTagName, "CONFIRMLOOK")){
			if (pBmButton->m_pMsgBox != NULL)
			{
				char szItem[256];
				memset(szItem, 0, sizeof(szItem));
				childElement.GetContents(szItem);

				map<string, CCBFrameLook*>::iterator itor = m_FrameLookMap.find(szItem);
				if (itor != m_FrameLookMap.end())
				{
					CCBFrameLook* pFrameLook = (CCBFrameLook*)(*itor).second;
					pBmButton->m_pMsgBox->ChangeCustomLook(pFrameLook);
					pBmButton->m_pMsgBox->SetTitle(pFrameLook->m_szDefaultTitle);
				}
			}
		}
		else if( !strcmp(szTagName,"STRETCH"))
		{
			pBmButton->SetStretch(true);
		}
		else if( !strcmp(szTagName,"BMTEXTCOLOR"))
		{
			pBmButton->m_bTextColor = true;
			float r,g,b,a;
			childElement.GetAttribute( &r, "r", 0);
			childElement.GetAttribute( &g, "g", 0);
			childElement.GetAttribute( &b, "b", 0);
			childElement.GetAttribute( &a, "a", 255);
			pBmButton->m_BmTextColor = sColor( (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a);
		}
		else if(!strcmp(szTagName, "PUSHBUTTON")){
			pBmButton->SetType(CCBT_PUSH);
		}
		else if(!strcmp(szTagName, "SETCHECK")){
			pBmButton->SetCheck(true);
		}
		else if(!strcmp(szTagName, "GROUP")){
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCButtonGroup*>::iterator itor = m_ButtonGroupMap.find(szItem);

			CCButtonGroup *pButtonGroup;
			if(itor != m_ButtonGroupMap.end()) {
				pButtonGroup = itor->second;
			} else {
				pButtonGroup = new CCButtonGroup;
				m_ButtonGroupMap.insert(map<string, CCButtonGroup*>::value_type(szItem,pButtonGroup));
			}

			pBmButton->SetButtonGroup(pButtonGroup);
		}
	}

	return pBmButton;
}

CCBitmap* CCIDLResource::GetBitmap(::CCXmlElement& element)
{
	bool bSourceFound = false;
	char szFileName[256];

	memset(szFileName, 0, sizeof(szFileName));

	bool bBoundsFound = false;
	sRect rt;

	char szTagName[256];
	::CCXmlElement childElement;

	int iCount = element.GetChildNodeCount();
	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, "BOUNDS"))
		{
			bBoundsFound = true;
			rt = GetRect(childElement);
		}else
		if (!strcmp(szTagName, "SOURCE"))
		{
			bSourceFound = true;
			childElement.GetContents(szFileName);
		}
	}

	if(bSourceFound && bBoundsFound) {	// 부분 bitmap 이다

		if(szFileName[0]==0 || stricmp(szFileName,"NULL")==0) return NULL;
		CCBitmap *pBitmap = CCBitmapManager::Get(szFileName);
		if(pBitmap==NULL) {
			cclog("warning : bitmap %s not found.\n",szFileName);
			return NULL;
		}

		CCPartialBitmap *pNewBitmap = new CCPartialBitmap(pBitmap,rt);

		CCBitmapManager::Add(pNewBitmap);	// 나중에 지워주어야 한다

		return pNewBitmap;
	}

	// 그렇지 않으면 통 bitmap 이다

	element.GetContents(szFileName);
	if(szFileName[0]==0 || stricmp(szFileName,"NULL")==0) return NULL;

	CCBitmap *pBitmap = CCBitmapManager::Get(szFileName);
	if(pBitmap==NULL) {
		cclog("warning : bitmap %s not found.\n",szFileName);
	}

	return pBitmap;
}

// 부분 bitmap의 alias
CCBitmap* CCIDLResource::GetBitmapAlias(::CCXmlElement& element)
{
	bool bSourceFound = false;
	char szSourceFileName[256];

	memset(szSourceFileName, 0, sizeof(szSourceFileName));

	bool bBoundsFound = false;
	sRect rt;

	char szTagName[256];
	::CCXmlElement childElement;

	int iCount = element.GetChildNodeCount();
	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, "BOUNDS"))
		{
			bBoundsFound = true;
			rt = GetRect(childElement);
		}else
		if (!strcmp(szTagName, "SOURCE"))
		{
			bSourceFound = true;
			childElement.GetContents(szSourceFileName);
		}
	}

	char szAliasName[256];
	element.GetAttribute(szAliasName, "name");

	if(bSourceFound && bBoundsFound) {	// 부분 bitmap 이다

		if(szSourceFileName[0]==0 || stricmp(szSourceFileName,"NULL")==0) return NULL;
		CCBitmap *pBitmap = CCBitmapManager::Get(szSourceFileName);
		if(pBitmap==NULL) {
			cclog("warning : bitmap %s not found.\n",szSourceFileName);
			return NULL;
		}

		CCPartialBitmap *pNewBitmap = new CCPartialBitmap(pBitmap,rt);
		strcpy(pNewBitmap->m_szName,szAliasName);

		CCBitmapManager::Add(pNewBitmap);	// 나중에 지워주어야 한다

		return pNewBitmap;
	}

	return NULL;
}

void CCIDLResource::GetBitmaps(CCBitmap** ppBitmaps, CCXmlElement& element, const int nBitmapCount)
{
	char szTagName[256];
	::CCXmlElement bitmapElement;

	int iCount = element.GetChildNodeCount();
	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		bitmapElement = element.GetChildNode(i);
		bitmapElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, "BITMAP"))
		{
			int t = -1;
			bitmapElement.GetAttribute(&t, "type", -1);
			CCBitmap* pBitmap = GetBitmap(bitmapElement);

			if ((0 <= t) && (t < nBitmapCount))
			{
				ppBitmaps[t] = pBitmap;
			}
		}
	}
}

CCBGroupLook* CCIDLResource::GetGroupLook(::CCXmlElement& element)
{
	::CCXmlElement childElement, bitmapElement;
	char szTagName[256], szFontName[256];
	memset(szFontName, 0, sizeof(szFontName));
	bool bDefaultLook = false;

	CCBGroupLook* pGroupLook = new CCBGroupLook();

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, "FONT"))
		{
			childElement.GetContents(szFontName);			
		} 
		if (!strcmp(szTagName, "TEXTPOSITION"))
		{
			pGroupLook->m_TitlePosition = GetPoint(childElement);
		} 
		if (!strcmp(szTagName, "STRETCH"))
		{
			bool bStretch = true;
			childElement.GetContents(&bStretch);
			pGroupLook->m_bStretch = bStretch;
		} 
		else if (!strcmp(szTagName, "TEXTCOLOR"))
		{
			pGroupLook->m_FontColor = GetColor(childElement);
		} 
		else if (!strcmp(szTagName, "BITMAPS"))
		{
			GetBitmaps(pGroupLook->m_pFrameBitmaps, childElement, FRAME_BITMAP_COUNT);
		}
		else if (!strcmp(szTagName, "DEFAULT"))
		{
			childElement.GetContents(&bDefaultLook);
		}
	}
	pGroupLook->m_pFont = CCFontManager::Get(szFontName);

	// Default Look
	if(bDefaultLook==true) CCGroup::ChangeLook(pGroupLook);

	// FrameLook 등록
	char szItem[256];
	element.GetAttribute(szItem, IDL_ATTR_ITEM, "");
	if(!m_GroupLookMap.insert(map<string, CCBGroupLook*>::value_type(string(szItem), pGroupLook)).second)
		OutputDebugString("insert widget failed.\n");

	return pGroupLook;
}

CCBFrameLook* CCIDLResource::GetFrameLook(::CCXmlElement& element)
{
	::CCXmlElement childElement, bitmapElement;
	char szTagName[256], szFontName[256];
	memset(szFontName, 0, sizeof(szFontName));
	bool bDefaultLook = false;

	CCBFrameLook* pFrameLook = new CCBFrameLook();

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, "FONT"))
		{
			childElement.GetContents(szFontName);			
		} 
		if (!strcmp(szTagName, "TEXTPOSITION"))
		{
			pFrameLook->m_TitlePosition = GetPoint(childElement);
		} 
		if (!strcmp(szTagName, "STRETCH"))
		{
			bool bStretch = true;
			childElement.GetContents(&bStretch);
			pFrameLook->m_bStretch = bStretch;
		} 
		else if (!strcmp(szTagName, "TEXTCOLOR"))
		{
			pFrameLook->m_FontColor = GetColor(childElement);
		} 
		else if( !strcmp(szTagName,"BGCOLOR"))
		{
			pFrameLook->m_BGColor = GetColor(childElement);
		}
		else if (!strcmp(szTagName, "BITMAPS"))
		{
			GetBitmaps(pFrameLook->m_pFrameBitmaps, childElement, FRAME_BITMAP_COUNT);
		}
		else if (!strcmp(szTagName, "CLOSE"))
		{
			GetBmButtonBitmaps(pFrameLook->m_pCloseButtonBitmaps, childElement);
		}
		else if (!strcmp(szTagName, "MINIMIZE"))
		{
			GetBmButtonBitmaps(pFrameLook->m_pMinimizeButtonBitmaps, childElement);
		}
		else if (!strcmp(szTagName, "DEFAULT"))
		{
			childElement.GetContents(&bDefaultLook);
		}
		else if(!strcmp(szTagName,"CUSTOMLOOK"))
		{
			int iTempBuf;
			childElement.GetContents( &iTempBuf );
			pFrameLook->SetCustomLook( iTempBuf );
		}
		else if (!strcmp(szTagName, "DEFAULTTITLE"))
		{
			char szText[256];
			childElement.GetContents(szText);
			strcpy(pFrameLook->m_szDefaultTitle,szText);
		}
		else if (!strcmp(szTagName, "SCALABLE"))
		{
			pFrameLook->SetScaleEnable(true);
		} 
	}
	pFrameLook->m_pFont = CCFontManager::Get(szFontName);

	// Default Look
	if(bDefaultLook==true) CCFrame::ChangeLook(pFrameLook);

	// FrameLook 등록
	char szItem[256];
	element.GetAttribute(szItem, IDL_ATTR_ITEM, "");
	if(!m_FrameLookMap.insert(map<string, CCBFrameLook*>::value_type(string(szItem), pFrameLook)).second)
		OutputDebugString("insert widget failed.\n");

	return pFrameLook;
}


CCBTextAreaLook*	CCIDLResource::GetTextAreaLook(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szTagName[256], szFontName[256];
	memset(szFontName, 0, sizeof(szFontName));
	bool bDefaultLook = false;

	CCBTextAreaLook* pTextAreaLook = new CCBTextAreaLook();

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;
		if (!strcmp(szTagName, "FONT"))
		{
			childElement.GetContents(szFontName);			
		} 
		else if (!strcmp(szTagName, "BITMAPS"))
		{
			GetBitmaps(pTextAreaLook->m_pFrameBitmaps, childElement, 9);
		}
		else if (!strcmp(szTagName, "BGCOLOR"))
		{
			pTextAreaLook->SetBgColor(GetColor(childElement));
		}
		else if (!strcmp(szTagName, "DEFAULT"))
		{
			childElement.GetContents(&bDefaultLook);
		}
	}
	pTextAreaLook->m_pFont = CCFontManager::Get(szFontName);

	// Default Look
	if(bDefaultLook==true) CCTextArea::ChangeLook(pTextAreaLook);

	// TextAreaLook 등록
	char szItem[256];
	element.GetAttribute(szItem, IDL_ATTR_ITEM, "");
	if(!m_TextAreaLookMap.insert(map<string, CCBTextAreaLook*>::value_type(string(szItem), pTextAreaLook)).second)
		OutputDebugString("insert widget failed.\n");


	return pTextAreaLook;
}

CCBLabelLook* CCIDLResource::GetLabelLook(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szTagName[256], szFontName[256];
	memset(szFontName, 0, sizeof(szFontName));
	bool bDefaultLook = false;

	CCBLabelLook* pLabelLook = new CCBLabelLook();

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, "FONT"))
		{
			childElement.GetContents(szFontName);			
		} 
		else if (!strcmp(szTagName, "TEXTCOLOR"))
		{
			pLabelLook->m_FontColor = GetColor(childElement);
		} 
		else if (!strcmp(szTagName, "DEFAULT"))
		{
			childElement.GetContents(&bDefaultLook);
		}
	}
	pLabelLook->m_pFont = CCFontManager::Get(szFontName);

	// Default Look
	if(bDefaultLook==true) CCLabel::ChangeLook(pLabelLook);

	// LabelLook 등록
	char szItem[256];
	element.GetAttribute(szItem, IDL_ATTR_ITEM, "");
	if(!m_LabelLookMap.insert(map<string, CCBLabelLook*>::value_type(string(szItem), pLabelLook)).second)
		OutputDebugString("insert widget failed.\n");

	return pLabelLook;
}


CCBButtonLook* CCIDLResource::GetButtonLook(::CCXmlElement& element)
{
	::CCXmlElement childElement, bitmapElement;
	char szBuf[256], szFontName[256];
	memset(szFontName, 0, sizeof(szFontName));

	bool bDefaultLook = false;

	CCBButtonLook* pButtonLook = new CCBButtonLook();
	
	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (szBuf[0] == '#') continue;

		if (!strcmp(szBuf, "FONT"))
		{
			childElement.GetContents(szFontName);
		} 
		else if (!strcmp(szBuf, "TEXTCOLOR"))
		{
			pButtonLook->m_FontColor = GetColor(childElement);
		} 
		else if (!strcmp(szBuf, "TEXTDOWNCOLOR"))
		{
			pButtonLook->m_FontDownColor = GetColor(childElement);
		} 
		else if (!strcmp(szBuf, "TEXTDOWNOFFSET"))
		{
			pButtonLook->m_FontDownOffset = GetPoint(childElement);
		}
		else if (!strcmp(szBuf, "TEXTHIGHLIGHTCOLOR"))
		{
			pButtonLook->m_FontHighlightColor = GetColor(childElement);
		} 
		else if (!strcmp(szBuf, "TEXTDISABLECOLOR"))
		{
			pButtonLook->m_FontDisableColor = GetColor(childElement);
		} 
		else if (!strcmp(szBuf, "UP"))
		{
			GetBitmaps(pButtonLook->m_pUpBitmaps, childElement, 9);
		}
		else if (!strcmp(szBuf, "DOWN"))
		{
			GetBitmaps(pButtonLook->m_pDownBitmaps, childElement, 9);
		}
		else if (!strcmp(szBuf, "OVER"))
		{
			GetBitmaps(pButtonLook->m_pOverBitmaps, childElement, 9);
		}
		else if (!strcmp(szBuf, "FOCUS"))
		{
			GetBitmaps(pButtonLook->m_pFocusBitmaps, childElement, 4);
		}
		else if (!strcmp(szBuf, "DEFAULT"))
		{
			childElement.GetContents(&bDefaultLook);
		}
		else if (!strcmp(szBuf, "STRETCH"))
		{
			bool bStretch = true;
			childElement.GetContents(&bStretch);
			pButtonLook->m_bStretch = bStretch;
		} 
		else if(!strcmp(szBuf, "CUSTOMLOOK"))
		{
			pButtonLook->SetCustomLook(true);
		}
		else if(!strcmp(szBuf, "WIRELOOK"))
		{
			pButtonLook->SetWireLook(true);
		}
		else if (!strcmp(szBuf, "SCALABLE"))
		{
			pButtonLook->SetScaleEnable(true);
		} 
	}

	pButtonLook->m_pFont = CCFontManager::Get(szFontName);

	// Default Look
	if(bDefaultLook==true) CCButton::ChangeLook(pButtonLook);

	// ButtonLook 등록
	char szItem[256];
	element.GetAttribute(szItem, IDL_ATTR_ITEM, "");
	if(!m_ButtonLookMap.insert(map<string, CCBButtonLook*>::value_type(string(szItem), pButtonLook)).second)
		OutputDebugString("insert widget failed.\n");

	return pButtonLook;
}


CCBEditLook* CCIDLResource::GetEditLook(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szTagName[256];

	CCBEditLook* pEditLook = new CCBEditLook();

	int iCount = element.GetChildNodeCount();
	bool bDefaultLook = false;

	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;
		else if (!strcmp(szTagName, "FONT"))
		{
			char szFontName[256];
			childElement.GetContents(szFontName);
			pEditLook->m_pFont = CCFontManager::Get(szFontName);
		} 
		else if (!strcmp(szTagName, "BITMAPS"))
		{
			GetBitmaps(pEditLook->m_pFrameBitmaps, childElement, 9);
		}
		else if (!strcmp(szTagName, "DEFAULT"))
		{
			childElement.GetContents(&bDefaultLook);
		}
		else if(!strcmp(szTagName,"CUSTOMLOOK"))
		{
			pEditLook->SetCustomLook(true);
		}
	}

	// Default Look
	if(bDefaultLook==true){
		CCEdit::ChangeLook(pEditLook);
		CCHotKey::ChangeLook(pEditLook);
//		MActionKey::ChangeLook(pEditLook);
	}

	// EditLook 등록
	char szItem[256];
	element.GetAttribute(szItem, IDL_ATTR_ITEM, "");
	if(!m_EditLookMap.insert(map<string, CCBEditLook*>::value_type(string(szItem), pEditLook)).second)
		OutputDebugString("insert widget failed.\n");

	return pEditLook;
}

CCBListBoxLook* CCIDLResource::GetListBoxLook(::CCXmlElement& element, int nType)
{
	::CCXmlElement childElement;
	char szTagName[256];

	CCBListBoxLook* pListBoxLook = new CCBListBoxLook();

	bool bDefaultLook = false;

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		else if (!strcmp(szTagName, "FONT"))
		{
			char szFontName[256];
			childElement.GetContents(szFontName);
			pListBoxLook->m_pFont = CCFontManager::Get(szFontName);
		} 
		else if (!strcmp(szTagName, "BITMAPS")) 
		{
			GetBitmaps(pListBoxLook->m_pFrameBitmaps, childElement, 9);
		}
		else if (!strcmp(szTagName, "SELECTEDPLANECOLOR"))
		{
			pListBoxLook->m_SelectedPlaneColor = GetColor(childElement);
		} 
		else if (!strcmp(szTagName, "SELECTEDTEXTCOLOR"))
		{
			pListBoxLook->m_SelectedTextColor = GetColor(childElement);
		} 
		else if (!strcmp(szTagName, "UNFOCUSEDSELECTEDPLANECOLOR"))
		{
			pListBoxLook->m_UnfocusedSelectedPlaneColor = GetColor(childElement);
		} 
		else	if( !strcmp(szTagName, "ITEMTEXTMULTILINE"))
		{
			pListBoxLook->m_bItemTextMultiLine	= true;
		}
		else if( !strcmp(szTagName, "ITEMTEXTHCENTER"))
		{
			pListBoxLook->m_ItemTextAlignmentMode |= CCD_HCENTER;
		}
 		else if( !strcmp(szTagName, "ITEMTEXTVCENTER"))
		{
			pListBoxLook->m_ItemTextAlignmentMode |= CCD_VCENTER;
		}
		else if( !strcmp(szTagName, "ITECCBITMAP"))
		{
			pListBoxLook->m_pItemSlotBitmap	= GetBitmap( childElement );
		}
		else if (!strcmp(szTagName, "DEFAULT"))
		{
			childElement.GetContents(&bDefaultLook);
		}
	}

	// Default Look
	if(bDefaultLook==true){
		if(nType==0)	CCListBox::ChangeLook(pListBoxLook);
		else			CCComboListBox::ChangeLook(pListBoxLook);
	}

	// ListBoxLook 등록
	char szItem[256];
	element.GetAttribute(szItem, IDL_ATTR_ITEM, "");
	if(!m_ListBoxLookMap.insert(map<string, CCBListBoxLook*>::value_type(string(szItem), pListBoxLook)).second)
		OutputDebugString("insert widget failed.\n");

	return pListBoxLook;
}

CCBArrowLook* CCIDLResource::GetArrowLook(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szTagName[256];

	CCBArrowLook* pArrowLook = new CCBArrowLook();
	bool bDefaultLook = false;

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, "BITMAPS"))
		{
			GetBitmaps(pArrowLook->m_pArrowBitmaps, childElement, 8);
		}
		else if (!strcmp(szTagName, "DEFAULT"))
		{
			childElement.GetContents(&bDefaultLook);
		}
	}

	// Default Look
	if(bDefaultLook==true) CCArrow::ChangeLook(pArrowLook);

	// ArrowLook 등록
	char szItem[256];
	element.GetAttribute(szItem, IDL_ATTR_ITEM, "");
	if(!m_ArrowLookMap.insert(map<string, CCBArrowLook*>::value_type(string(szItem), pArrowLook)).second)
		OutputDebugString("insert widget failed.\n");

	return pArrowLook;
}

CCBThumbLook* CCIDLResource::GetThumbLook(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szTagName[256];

	bool bDefaultLook = false;

	CCBThumbLook* pThumbLook = new CCBThumbLook();

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, "VERTICAL"))
		{
			char szType[256];
			childElement.GetAttribute(szType, "type", "normal");
			if (!strcmp(szType, "normal"))
			{
				GetBitmaps(pThumbLook->m_pVBitmaps, childElement, 3);
			}
			else if (!strcmp(szType, "pressed"))
			{
				GetBitmaps(pThumbLook->m_pVPressedBitmaps, childElement, 3);
			}
		}
		else if (!strcmp(szTagName, "HORIZONTAL"))
		{
			char szType[256];
			childElement.GetAttribute(szType, "type", "normal");
			if (!strcmp(szType, "normal"))
			{
				GetBitmaps(pThumbLook->m_pHBitmaps, childElement, 3);
			}
			else if (!strcmp(szType, "pressed"))
			{
				GetBitmaps(pThumbLook->m_pHPressedBitmaps, childElement, 3);
			}
		}
		else if (!strcmp(szTagName, "DEFAULT"))
		{
			childElement.GetContents(&bDefaultLook);
		}
	}

	// Default Look
	if(bDefaultLook==true) CCThumb::ChangeLook(pThumbLook);

	// ThumbLook 등록
	char szItem[256];
	element.GetAttribute(szItem, IDL_ATTR_ITEM, "");
	if(!m_ThumbLookMap.insert(map<string, CCBThumbLook*>::value_type(string(szItem), pThumbLook)).second)
		OutputDebugString("insert widget failed.\n");

	return pThumbLook;
}

CCBScrollBarLook* CCIDLResource::GetScrollBarLook(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szTagName[256];

	bool bDefaultLook = false;

	CCBScrollBarLook* pScrollBarLook = new CCBScrollBarLook();

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, "VERTICAL"))
		{
			GetBitmaps(pScrollBarLook->m_pVFrameBitmaps, childElement, 3);
		}
		else if (!strcmp(szTagName, "HORIZONTAL"))
		{
			GetBitmaps(pScrollBarLook->m_pHFrameBitmaps, childElement, 3);
		}
		else if (!strcmp(szTagName, "DEFAULT"))
		{
			childElement.GetContents(&bDefaultLook);
		}
	}

	// Default Look
	if(bDefaultLook==true) CCScrollBar::ChangeLook(pScrollBarLook);

	// ScrollBarLook 등록
	char szItem[256];
	element.GetAttribute(szItem, IDL_ATTR_ITEM, "");
	if(!m_ScrollBarLookMap.insert(map<string, CCBScrollBarLook*>::value_type(string(szItem), pScrollBarLook)).second)
		OutputDebugString("insert widget failed.\n");

	return pScrollBarLook;
}

CCBSliderLook* CCIDLResource::GetSliderLook(::CCXmlElement& element)
{
	return NULL;
}

CCBSliderThumbLook* CCIDLResource::GetSliderThumbLook(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szTagName[256];

	bool bDefaultLook = false;

	CCBSliderThumbLook* pSliderThumbLook = new CCBSliderThumbLook();

	int iCount = element.GetChildNodeCount();
	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if(!strcmp(szTagName, "BITMAP") )
		{
			pSliderThumbLook->m_pBitmap = GetBitmap(childElement);
		}
		else if (!strcmp(szTagName, "DEFAULT"))
		{
			childElement.GetContents(&bDefaultLook);
		}
		// else if BITMAP...
	}

	// Default Look
	if(bDefaultLook==true) CCSliderThumb::ChangeLook((CCSliderThumbLook*)pSliderThumbLook);

	// ThumbLook 등록
	char szItem[256];
	element.GetAttribute(szItem, IDL_ATTR_ITEM, "");
	if(!m_ThumbLookMap.insert(map<string, CCBThumbLook*>::value_type(string(szItem), (CCBThumbLook*)pSliderThumbLook)).second)
		OutputDebugString("insert widget failed.\n");

	return pSliderThumbLook;
}

CCBTabCtrlLook* CCIDLResource::GetTabCtrlLook(::CCXmlElement& element)
{
	::CCXmlElement childElement, bitmapElement;
	char szTagName[256], szFontName[256];
	memset(szFontName, 0, sizeof(szFontName));
	bool bDefaultLook = false;

	CCBTabCtrlLook* pTabCtrlLook = new CCBTabCtrlLook();

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szTagName, 0, sizeof(szTagName));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, "BITMAPS"))
		{
			GetBitmaps(pTabCtrlLook->m_pFrameBitmaps, childElement, FRAME_BITMAP_COUNT);
		}
		else if (!strcmp(szTagName, "DEFAULT"))
		{
			childElement.GetContents(&bDefaultLook);
		}
	}

	// Default Look
	if(bDefaultLook==true) CCTabCtrl::ChangeLook(pTabCtrlLook);

	// CCTabCtrlLook 등록
	char szItem[256];
	element.GetAttribute(szItem, IDL_ATTR_ITEM, "");
	if(!m_TabCtrlLookMap.insert(map<string, CCBTabCtrlLook*>::value_type(string(szItem), pTabCtrlLook)).second)
		OutputDebugString("insert widget failed.\n");

	return pTabCtrlLook;
}

CCWidget* CCIDLResource::GetParentWidget(::CCXmlElement& element)
{
	char szBuf[4096];
	element.GetAttribute(szBuf, IDL_ATTR_PARENT, "");

	CCWidgetMMap::iterator itor;
	itor = m_WidgetMap.find(szBuf);
	if (itor != m_WidgetMap.end())
	{
		return (CCWidget*)(*itor).second;
	}
	else
	{
		return (CCWidget*)m_pParent;
	}
}

void CCIDLResource::InsertWidget(::CCXmlElement& element, CCWidget* pWidget)
{
	char szItem[256];
	element.GetAttribute(szItem, "item", "unnamed widget");
	strcpy(pWidget->m_szIDLName,szItem);
	m_WidgetMap.insert(CCWidgetMMap::value_type(string(szItem), pWidget));
}

bool CCIDLResource::GetCommonWidgetProperty(CCWidget* pWidget, CCXmlElement& element, const char* szTagName)
{
	if (szTagName[0] == '#') return true;

	bool bRet = false;
	if (!strcmp(szTagName, "BOUNDS"))
		{
			int w = CCGetWorkspaceWidth();
			int h = CCGetWorkspaceHeight();
			if(pWidget->GetParent()!=NULL){
				sRect r = pWidget->GetParent()->GetRect();
				w = r.w;
				h = r.h;
			}
			sRect rect = GetRect(element);
			// 만약 크기가 음수라면 현재 화면에서 최대 크기
			if(rect.w<0) rect.w = w-abs(rect.x);
			if(rect.h<0) rect.h = h-abs(rect.y);

			// 만약 위치가 CENTER_NUCCBER이면 정 중앙에서 시작
			// 만약 위치가 음수라면 좌측또는 하단으로 얼라인
			/*
			#define CENTER_NUCCBER	-10000
			if(rect.x<=CENTER_NUCCBER) rect.x = CCGetWorkspaceWidth()/2 + (rect.x-CENTER_NUCCBER);
			else if(rect.x<0) rect.x = w-rect.w+rect.x + 1;
			if(rect.y<=CENTER_NUCCBER) rect.y = CCGetWorkspaceHeight()/2 + (rect.y-CENTER_NUCCBER);
			else if(rect.y<0) rect.y = h-rect.h+rect.y + 1;
			*/

			pWidget->SetBounds(rect);
			pWidget->m_IDLRect = rect;
			bRet = true;
		}
	else if (!strcmp(szTagName, "TEXT"))
		{
			char szText[16384], szTar[16384];
			element.GetContents(szText);
			TransText(szText, szTar);
			pWidget->SetText(szTar);
			bRet = true;
		}
	else if (!strcmp(szTagName, "ANCHORS"))
		{	
			pWidget->m_Anchors = GetAnchors(element);
			bRet = true;
		}
	else if (!strcmp(szTagName, "CLIP"))
		{
			bool bClip = false;
			element.GetContents(&bClip);
			pWidget->SetClipByParent( bClip );
		}
	else if (!strcmp(szTagName, "TOOLTIP"))
		{
			char szContents[1024];
			char szToolTip[1024];
			element.GetContents(szContents);
			TransText(szContents, szToolTip);
			pWidget->AttachToolTip(szToolTip);
		}
	else if (!strcmp(szTagName, "ALIGN"))
		{
			pWidget->SetBoundsAlignment(GetAlignmentMode(element), -1, -1);
		}
	else if (!strcmp(szTagName, "VISIBLE"))
		{
			bool bValue = true;
			element.GetContents(&bValue);
			pWidget->Show(bValue);
		}
	else if (!strcmp(szTagName, "ENABLE"))
		{
			bool bValue = true;
			element.GetContents(&bValue);
			pWidget->Enable(bValue);
		}
	else if (!strcmp(szTagName, "FOCUSABLE")) 
		{
			bool bValue = true;
			element.GetContents(&bValue);
			pWidget->SetFocusEnable(bValue);
		}

	return bRet;
}

CCFrame*	CCIDLResource::GetFrame(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;
	CCBFrameLook* pFrameLook = NULL;

	pListener = pParentWidget = GetParentWidget(element);
//	CCFrame* pFrame = new CCFrame("", pParentWidget, pListener);
	CCFrame* pFrame = CreateFrame("", pParentWidget, pListener);
	InsertWidget(element, pFrame);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pFrame, childElement, szBuf)) continue;

		if (!strcmp(szBuf, "FRAMELOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBFrameLook*>::iterator itor = m_FrameLookMap.find(szItem);
			if (itor != m_FrameLookMap.end())
			{
				pFrameLook = (CCBFrameLook*)(*itor).second;
				pFrame->ChangeCustomLook(pFrameLook);
			}else {
				cclog("warning : FrameLook %s not found.\n",szItem);
			}
		}
		else if (!strcmp(szBuf, "CLOSE_BUTTON"))
		{
			GetFrameBtn(pFrame->GetCloseButton(), pFrameLook, childElement);
			/*
			GetFrameButton(pFrame->GetCloseButton(), childElement);
			pFrame->GetCloseButton()->SetUpBitmap(pFrameLook->m_pCloseButtonBitmaps[0]);
			pFrame->GetCloseButton()->SetDownBitmap(pFrameLook->m_pCloseButtonBitmaps[1]);
			pFrame->GetCloseButton()->SetDisableBitmap(pFrameLook->m_pCloseButtonBitmaps[2]);
			*/
//			pFrame->GetCloseButton()->Show(true);
		}
		else if (!strcmp(szBuf, "MINIMIZE_BUTTON"))
		{
			GetFrameBtn(pFrame->GetMinimizeButton(), pFrameLook, childElement);
			/*
			GetFrameButton(pFrame->GetMinimizeButton(), childElement);
			pFrame->GetMinimizeButton()->SetUpBitmap(pFrameLook->m_pMinimizeButtonBitmaps[0]);
			pFrame->GetMinimizeButton()->SetDownBitmap(pFrameLook->m_pMinimizeButtonBitmaps[1]);
			pFrame->GetMinimizeButton()->SetDisableBitmap(pFrameLook->m_pMinimizeButtonBitmaps[2]);
			*/
//			pFrame->GetMinimizeButton()->Show(true);
		}
		else if (!strcmp(szBuf, "TITLEBAR"))
		{
			bool bValue = true;
			childElement.GetContents(&bValue);
			pFrame->m_bTitleBar = bValue;
		}
		else if (!strcmp(szBuf, "RESIZABLE"))
		{
			bool bValue = true;
			childElement.GetContents(&bValue);
			pFrame->SetResizable(bValue);
		}
		else if (!strcmp(szBuf, "MINWIDTH"))
		{
			int nValue = 300;
			childElement.GetContents(&nValue);
			pFrame->m_iMinWidth = nValue;
		}
		else if (!strcmp(szBuf, "MINHEIGHT"))
		{
			int nValue = 300;
			childElement.GetContents(&nValue);
			pFrame->m_iMinHeight = nValue;
		}
		else if (!strcmp(szBuf, "MOVABLE"))
		{
			bool bValue = true;
			childElement.GetContents(&bValue);
			pFrame->SetMovable(bValue);
		}
	}


	return pFrame;
}

CCLabel* CCIDLResource::GetLabel(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	char szFontName[4096];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	CCLabel* pLabel = (CCLabel*)Core::GetInstance()->NewWidget(CORE_CCLABEL, "", pParentWidget, pListener);
	InsertWidget(element, pLabel);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pLabel, childElement, szBuf)) continue;

		if (!strcmp(szBuf, IDL_TEXTCOLOR))
		{
			sColor color = GetColor(childElement);
			pLabel->SetTextColor(color);
		}
		else if (!strcmp(szBuf, "LABELLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBLabelLook*>::iterator itor = m_LabelLookMap.find(szItem);
			if (itor != m_LabelLookMap.end())
			{
				pLabel->ChangeCustomLook((CCBLabelLook*)(*itor).second);
			}
		}
		else if(!strcmp(szBuf, "FONT"))
		{
			childElement.GetContents(szFontName);
			pLabel->SetFont( CCFontManager::Get(szFontName) );
		}
		else if(!strcmp(szBuf, "TEXTALIGN" ))
		{
			pLabel->SetAlignment( GetAlignmentMode(childElement));
		}

	}
	return pLabel;
}

CCButton* CCIDLResource::GetButton(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	CCButton* pButton = (CCButton*)Core::GetInstance()->NewWidget(CORE_CCBUTTON, "", pParentWidget, pListener);
	InsertWidget(element, pButton);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pButton, childElement, szBuf)) continue;

		if (!strcmp(szBuf, "BUTTONLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);
			
			map<string, CCBButtonLook*>::iterator itor = m_ButtonLookMap.find(szItem);
			if (itor != m_ButtonLookMap.end())
			{
				pButton->ChangeCustomLook((CCBButtonLook*)(*itor).second);
			}
		}
		else if(!strcmp(szBuf, "DEFAULTKEY")){
			char szItem[256] = {0,};
			childElement.GetContents(szItem);
			if(stricmp(szItem, "ENTER")==0){
				pButton->m_uKeyAssigned = CCBKA_ENTER;
			}
			else if(stricmp(szItem, "ESC")==0){
				pButton->m_uKeyAssigned = CCBKA_ESC;
			}
		}
		else if(!strcmp(szBuf, "CONFIRMMESSAGE")){

			char szContents[256] = {0, };
			char szItem[256] = {0,};
			childElement.GetContents(szContents);
			TransText(szContents, szItem);
			pButton->SetConfirmMessageBox(szItem);
		}
		else if(!strcmp(szBuf, "CONFIRMLOOK")){
			if (pButton->m_pMsgBox != NULL)
			{
				char szItem[256];
				memset(szItem, 0, sizeof(szItem));
				childElement.GetContents(szItem);

				map<string, CCBFrameLook*>::iterator itor = m_FrameLookMap.find(szItem);
				if (itor != m_FrameLookMap.end())
				{
					CCBFrameLook* pFrameLook = (CCBFrameLook*)(*itor).second;
					pButton->m_pMsgBox->ChangeCustomLook(pFrameLook);
					pButton->m_pMsgBox->SetTitle(pFrameLook->m_szDefaultTitle);
				}
			}
		}
		else if(!strcmp(szBuf, "PUSHBUTTON")){
			pButton->SetType(CCBT_PUSH);
		}
		else if(!strcmp(szBuf, "PUSHBUTTON2")){
			pButton->SetType(CCBT_PUSH2);
		}
		else if(!strcmp(szBuf, "SETCHECK")){
			pButton->SetCheck(true);
		}
		else if(!strcmp(szBuf, "GROUP")){
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCButtonGroup*>::iterator itor = m_ButtonGroupMap.find(szItem);
			
			CCButtonGroup *pButtonGroup;
			if(itor != m_ButtonGroupMap.end()) {
				pButtonGroup = itor->second;
			} else {
                pButtonGroup = new CCButtonGroup;
				m_ButtonGroupMap.insert(map<string, CCButtonGroup*>::value_type(szItem,pButtonGroup));
			}

			pButton->SetButtonGroup(pButtonGroup);
		}
	}

	return pButton;
}

CCEdit* CCIDLResource::GetEdit(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	CCEdit* pEdit = (CCEdit*)Core::GetInstance()->NewWidget(CORE_CCEDIT, "", pParentWidget, pListener);
	InsertWidget(element, pEdit);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pEdit, childElement, szBuf)) continue;


		if (!strcmp(szBuf, "EDITLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBEditLook*>::iterator itor = m_EditLookMap.find(szItem);
			if (itor != m_EditLookMap.end())
			{
				pEdit->ChangeCustomLook((CCBEditLook*)(*itor).second);
			}
		}
		else if (!strcmp(szBuf, "PASSWORD"))
		{
			bool bPassword = false;
			childElement.GetContents(&bPassword);
			if (bPassword == true)
			{
				pEdit->SetPasswordField(true);
			}
		}
		else if (!strcmp(szBuf, "NUCCBERONLY"))
		{
			bool bNumberOnly = false;
			childElement.GetContents(&bNumberOnly);
			if (bNumberOnly == true)
			{
				pEdit->SetNumberField(true);
			}
		}
		else if (!strcmp(szBuf, "MAXLENGTH"))
		{
			int nMaxLength;
			childElement.GetContents(&nMaxLength);
			pEdit->SetMaxLength(nMaxLength);
		}
	}

	return pEdit;
}

CCListBox* CCIDLResource::GetListBox(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;
	CCListBoxLook* pListBoxLook = NULL;

	pListener = pParentWidget = GetParentWidget(element);
	CCListBox* pListBox = (CCListBox*)Core::GetInstance()->NewWidget(CORE_CCLISTBOX, "", pParentWidget, pListener);	// RAONHAJE TODO : NewWidget에 Name 넘겨주도록 바꾸자.
	InsertWidget(element, pListBox);

	int iCount = element.GetChildNodeCount();

	char szItem[256];

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pListBox, childElement, szBuf)) continue;
		else if (!strcmp(szBuf, "LISTBOXLOOK"))
		{
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBListBoxLook*>::iterator itor = m_ListBoxLookMap.find(szItem);
			if (itor != m_ListBoxLookMap.end())
			{
				pListBoxLook = (CCBListBoxLook*)(*itor).second;
				pListBox->ChangeCustomLook(pListBoxLook);
			}
		}
		else if (!strcmp(szBuf, "SELECTED"))
		{
			bool bValue = true;
			childElement.GetContents(&bValue);
			pListBox->SetSelected(bValue);
		}
		else if( !strcmp(szBuf, "ITEMHEIGHT" ))
		{
			int iValue = -1;
			childElement.GetContents(&iValue);
			pListBox->SetItemHeight( iValue );
		}
		else if(!strcmp(szBuf, "FONT" ))
		{
			childElement.GetContents( szItem );
			pListBox->SetFont( CCFontManager::Get(szItem) );
		}
		else if(!strcmp(szBuf, "TEXTCOLOR" ))
		{
			pListBox->m_FontColor = GetColor( childElement );
		}
		else if(!strcmp(szBuf, "TEXTALIGN" ))
		{
			pListBox->m_FontAlign = GetAlignmentMode(childElement);
		}
		else if(!strcmp(szBuf, "NULLFRAME"))
		{
			pListBox->m_bNullFrame = true;
		}
		else if(!strcmp(szBuf, "MULTISELECT"))
		{
			pListBox->m_bMultiSelect = true;
		}
	}

	return pListBox;
}

CCPicture* CCIDLResource::GetPicture(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	CCPicture* pPicture = (CCPicture*)Core::GetInstance()->NewWidget(CORE_CCPICTURE, "", pParentWidget, pListener);
	InsertWidget(element, pPicture);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pPicture, childElement, szBuf)) continue;

		if (!strcmp(szBuf, "BITMAP"))
		{
			CCBitmap* pBitmap = GetBitmap(childElement);
			pPicture->SetBitmap(pBitmap);
		}
		else if (!strcmp(szBuf, "STRETCH"))
		{
			char ctemp[16];
			childElement.GetContents( ctemp );
			if( ctemp[0] == 'x' )
				pPicture->SetStretch(1);
			else if( ctemp[0] == 'y' )
				pPicture->SetStretch(2);
			else
				pPicture->SetStretch(3);
		}
		else if( !strcmp(szBuf, "DRAWMODE" ))
		{
			int mode;
			childElement.GetContents(&mode);
			DWORD cmode = pPicture->GetDrawMode();
			if( mode == 0 ) pPicture->SetDrawMode( cmode | CCBM_Normal );
			else if( mode == 1 ) pPicture->SetDrawMode( cmode | CCBM_FlipLR );
			else if( mode == 2 ) pPicture->SetDrawMode( cmode | CCBM_FlipUD );
			else if( mode == 3 ) pPicture->SetDrawMode( cmode | CCBM_RotL90 );
			else if( mode == 4 ) pPicture->SetDrawMode( cmode | CCBM_RotR90 );
		}
	}

	return pPicture;
}

CCScrollBar* CCIDLResource::GetScrollBar(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;
	CCScrollBarTypes ScrollBarType = CCSBT_VERTICAL;
	int nMin = 0, nMax = 0;

	element.GetAttribute(szBuf, "type", "v");
	if (!strcmp(szBuf, "h"))
	{
		ScrollBarType = CCSBT_HORIZONTAL;
	}

	pListener = pParentWidget = GetParentWidget(element);
	CCScrollBar* pScrollBar = (CCScrollBar*)Core::GetInstance()->NewWidget(CORE_CCSCROLLBAR, "", pParentWidget, pListener);
	pScrollBar->SetType(ScrollBarType);
	InsertWidget(element, pScrollBar);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pScrollBar, childElement, szBuf)) continue;

		if (!strcmp(szBuf, "MIN"))
		{
			childElement.GetContents(&nMin);
		}
		else if (!strcmp(szBuf, "MAX"))
		{
			childElement.GetContents(&nMax);
		}
		else if (!strcmp(szBuf, "VALUE"))
		{
			int nValue = 0;
			childElement.GetContents(&nValue);
			pScrollBar->SetValue(nValue);
		}
		else if (!strcmp(szBuf, "SCROLLBARLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBScrollBarLook*>::iterator itor = m_ScrollBarLookMap.find(szItem);
			if (itor != m_ScrollBarLookMap.end())
			{
				pScrollBar->ChangeCustomLook((CCBScrollBarLook*)(*itor).second);
			}
		}
		else if (!strcmp(szBuf, "ARROWLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBArrowLook*>::iterator itor = m_ArrowLookMap.find(szItem);
			if (itor != m_ArrowLookMap.end())
				pScrollBar->ChangeCustomArrowLook((CCBArrowLook*)(*itor).second);
		}else if (!strcmp(szBuf, "THUCCBLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBThumbLook*>::iterator itor = m_ThumbLookMap.find(szItem);
			if (itor != m_ThumbLookMap.end())
				pScrollBar->ChangeCustomThumbLook((*itor).second);
		}


	}
	pScrollBar->SetMinMax(nMin, nMax);

	return pScrollBar;
}

CCSlider* CCIDLResource::GetSlider(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;
	int nMin = 0, nMax = 0;

	pListener = pParentWidget = GetParentWidget(element);
	CCSlider* pSlider = (CCSlider*)Core::GetInstance()->NewWidget(CORE_CCSLIDER, "", pParentWidget, pListener);
	InsertWidget(element, pSlider);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pSlider, childElement, szBuf))
			continue;
		else if (!strcmp(szBuf, "MIN"))
		{
			childElement.GetContents(&nMin);
		}
		else if (!strcmp(szBuf, "MAX"))
		{
			childElement.GetContents(&nMax);
		}
		else if (!strcmp(szBuf, "VALUE"))
		{
			int nValue = 0;
			childElement.GetContents(&nValue);
			pSlider->SetValue(nValue);
		}
		else if (!strcmp(szBuf, "SLIDERLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBSliderLook*>::iterator itor = m_SliderLookMap.find(szItem);
			if (itor != m_SliderLookMap.end())
			{
				pSlider->ChangeCustomLook((CCBScrollBarLook*)(*itor).second);
			}
		}
		else if(!strcmp(szBuf, "ARROWLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBArrowLook*>::iterator itor = m_ArrowLookMap.find(szItem);
			if (itor != m_ArrowLookMap.end())
			{
				pSlider->m_pUp->ChangeCustomLook((CCBArrowLook*)(*itor).second);
				pSlider->m_pDown->ChangeCustomLook((CCBArrowLook*)(*itor).second);
			}
		}
		else if(!strcmp(szBuf, "THUCCBLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBThumbLook*>::iterator itor = m_ThumbLookMap.find(szItem);
			if (itor != m_ThumbLookMap.end())
			{
				pSlider->m_pThumb->ChangeCustomLook((CCBThumbLook*)(*itor).second);				
			}
		}
	}

	pSlider->SetMinMax(nMin, nMax);

	return pSlider;

}

CCGroup* CCIDLResource::GetGroup(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;
	CCBGroupLook* pGroupLook = NULL;

	pListener = pParentWidget = GetParentWidget(element);
	CCGroup* pGroup = (CCGroup*)Core::GetInstance()->NewWidget(CORE_CCGROUP, "", pParentWidget, pListener);
	InsertWidget(element, pGroup);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pGroup, childElement, szBuf)) continue;

		if (!strcmp(szBuf, "GROUPLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBGroupLook*>::iterator itor = m_GroupLookMap.find(szItem);
			if (itor != m_GroupLookMap.end())
			{
				pGroupLook = (CCBGroupLook*)(*itor).second;
				pGroup->ChangeCustomLook(pGroupLook);
			}
		}
	}

	return pGroup;
}

CCComboBox* CCIDLResource::GetComboBox(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	CCComboBox* pComboBox = (CCComboBox*)Core::GetInstance()->NewWidget(CORE_CCCMBBOX, "", pParentWidget, pListener);
	InsertWidget(element, pComboBox);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pComboBox, childElement, szBuf)) continue;

		if (!strcmp(szBuf, "BUTTONLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBButtonLook*>::iterator itor = m_ButtonLookMap.find(szItem);
			if (itor != m_ButtonLookMap.end())
			{
				pComboBox->ChangeCustomLook((CCBButtonLook*)(*itor).second);
			}
		}
		else if (!strcmp(szBuf, "LISTBOXLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBListBoxLook*>::iterator itor = m_ListBoxLookMap.find(szItem);
			if (itor != m_ListBoxLookMap.end())
			{
				pComboBox->m_pListBox->ChangeCustomLook((CCBListBoxLook*)(*itor).second);
			}
		}
		else if (!strcmp(szBuf, "LISTITEM"))
		{
			char szItem[4096], szContents[4096];
			childElement.GetContents(szContents);
			TransText(szContents, szItem);
			pComboBox->Add(szItem);

			bool bSelected = false;
			childElement.GetAttribute(&bSelected, "selected", false);
			if (bSelected)
			{
				pComboBox->SetSelIndex(pComboBox->GetCount()-1);
			}
		}
		else if(!strcmp(szBuf, "DROPSIZE"))
		{
			int nDropSize = 100;
			childElement.GetContents(&nDropSize);
			pComboBox->SetDropSize(nDropSize);
		}
		else if(!strcmp(szBuf, "COCCBOTYPE"))
		{
			int nType = 0;
			childElement.GetContents(&nType);
			pComboBox->SetComboType(nType);
		}
		else if(!strcmp(szBuf, "COCCBOFIRSTSIZE"))
		{
			int nComboSize = 0;
			childElement.GetContents(&nComboSize);
			pComboBox->SetNextComboBoxTypeSize(nComboSize);
		}
		else if(!strcmp(szBuf, "TEXTALIGN" ))
		{
			CCAlignmentMode mode = GetAlignmentMode( childElement );
 			pComboBox->m_pListBox->m_FontAlign = mode;
			pComboBox->SetAlignment(mode);
		}
		else if(!strcmp(szBuf, "FONT" ))
		{
			childElement.GetContents( szBuf );
			CCFont* pFont = CCFontManager::Get(szBuf);
			pComboBox->m_pListBox->	SetFont( pFont );
			pComboBox->SetFont( pFont );
		}
		else if(!strcmp(szBuf, "TEXTCOLOR" ))
		{
			sColor color = GetColor( childElement );
			pComboBox->m_pListBox->m_FontColor = color;
            pComboBox->SetTextColor( color );
		}
		else if( !strcmp(szBuf, "ITEMHEIGHT" ))
		{
			int iValue = -1;
			childElement.GetContents(&iValue);
			pComboBox->m_pListBox->SetItemHeight( iValue );
		}
		else if(!strcmp(szBuf,"DROPUNDER"))
		{
			bool bValue;
			childElement.GetContents(&bValue);
			pComboBox->m_bAutoDrop = false;
			pComboBox->m_bDropUnder = bValue;
		}
	}

	return pComboBox;
}

CCMenuItem* CCIDLResource::GetMenuItem(CCPopupMenu* pPopupMenu, CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szTagName[1024];
	CCMenuItem* pMenuItem = (CCMenuItem*)Core::GetInstance()->NewWidget(CORE_CCMENUITEM, "", NULL, NULL);
	pPopupMenu->AddMenuItem(pMenuItem);
	
	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;
		else if (!strcmp(szTagName, "TEXT"))
		{
			char szItem[1024];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);
			pMenuItem->SetText(szItem);
		}
		else if (!strcmp(szTagName, "SUBMENU"))
		{
			GetSubMenu(pMenuItem, childElement);
		}
	}

	return pMenuItem;	
}

CCPopupMenu* CCIDLResource::GetSubMenu(CCMenuItem* pParentMenuItem, ::CCXmlElement& element)
{
	CCPopupMenu* pSubMenu = pParentMenuItem->CreateSubMenu();
	::CCXmlElement childElement;
	char szTagName[1024];
	int iCount = element.GetChildNodeCount();
	for (int i = 0; i < iCount; i++)
	{
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;
		else if (!strcmp(szTagName, "MENUITEM"))
		{
			GetMenuItem(pSubMenu, childElement);
		}
	}

	return pSubMenu;
}

CCPopupMenu* CCIDLResource::GetPopupMenu(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);

	char szType[256];
	CCPopupMenuTypes t = CCPMT_VERTICAL;

	element.GetAttribute(szType, "type", "v");
	if (!strcmp(szType, "h")) t = CCPMT_HORIZONTAL;

	CCPopupMenu* pPopupMenu = (CCPopupMenu*)Core::GetInstance()->NewWidget(CORE_CCPOPUPMENU, "", pParentWidget, pListener);
	pPopupMenu->SetType(t);
	InsertWidget(element, pPopupMenu);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pPopupMenu, childElement, szBuf)) continue;

		else if (!strcmp(szBuf, "MENUITEM"))
		{
			GetMenuItem(pPopupMenu, childElement);
		}
	}

	pPopupMenu->Show(false);
	return pPopupMenu;
}

CCAniBitmap* CCIDLResource::GetAniBitmap(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];

	CCAniBitmap* pAniBitmap = new CCAniBitmap;

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (!strcmp(szBuf, "BITMAP"))
		{
			CCBitmap* pBitmap = GetBitmap(childElement);
			if(pBitmap)
				pAniBitmap->Add(pBitmap);
		}
		else if (!strcmp(szBuf, "DELAY"))
		{
			int nDelay = 0;
			childElement.GetContents(&nDelay);
			pAniBitmap->SetDelay(nDelay);
		}
	}

	char szItem[256];
	element.GetAttribute(szItem, IDL_ATTR_ITEM, "");
	if(!m_AniBitmapMap.insert(map<string, CCAniBitmap*>::value_type(string(szItem), pAniBitmap)).second)
		OutputDebugString("insert widget failed.\n");

	return pAniBitmap;
}

CCAnimation* CCIDLResource::GetAnimation(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	char szAniBitmap[256];
	CCAniBitmap* pAniBitmap = NULL;

	memset(szAniBitmap, 0, sizeof(szAniBitmap));
	CCWidget* pParentWidget = GetParentWidget(element);

	CCAnimation* pAnimation = (CCAnimation*)Core::GetInstance()->NewWidget(CORE_CCANIMATION, "", pParentWidget, NULL);
	InsertWidget(element, pAnimation);


	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pAnimation, childElement, szBuf)) continue;

		else if (!strcmp(szBuf, "ANIMTEMPLATE"))
		{
			childElement.GetContents(szAniBitmap);
		}
		else if (!strcmp(szBuf, "PLAYMODE"))
		{
			char szPlayMode[256];
			childElement.GetContents(szPlayMode);
			if (!strcmp(szPlayMode, "forwardonce"))
			{
				pAnimation->m_iPlayMode = CCPM_FORWARDONCE;
			}
			else if (!strcmp(szPlayMode, "forwardbackward"))
			{
				pAnimation->m_iPlayMode = CCPM_FORWARDNBACKWARD;
			}
			else if (!strcmp(szPlayMode, "repetition"))
			{
				pAnimation->m_iPlayMode = CCPM_REPETITION;
			}
			else if (!strcmp(szPlayMode, "stop"))
			{
				pAnimation->m_iPlayMode = CCPM_FORWARDONCE;
				pAnimation->m_bRunAnimation = false;
			}
		}
		else if (!strcmp(szBuf, "RUN"))
		{
			bool bValue = true;
			childElement.GetContents( &bValue);
			pAnimation->m_bRunAnimation = bValue;
		}
	}

	map<string, CCAniBitmap*>::iterator itor = m_AniBitmapMap.find(szAniBitmap);
	if (itor != m_AniBitmapMap.end())
	{
		pAniBitmap = ((CCAniBitmap*)(*itor).second);
		pAnimation->SetAniBitmap(pAniBitmap);
	}

	
	return pAnimation;
}

CCCursor* CCIDLResource::GetCursor(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	char szItem[256];

	element.GetAttribute(szItem, IDL_ATTR_ITEM, "");

	CCWidget* pParentWidget = GetParentWidget(element);
	CCCursor* pCursor = NULL;

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (!strcmp(szBuf, "ANIMTEMPLATE"))
		{
			char szAniBitmap[256];
			CCAniBitmap* pAniBitmap = NULL;
			childElement.GetContents(szAniBitmap);
			pAniBitmap = (m_AniBitmapMap.find(szAniBitmap))->second;
			if (pCursor == NULL) pCursor = new CCAniBitmapCursor(szItem, pAniBitmap);
		}
		else if (!strcmp(szBuf, "BITMAP"))
		{
			char szBitmap[256];
			CCBitmap* pBitmap = NULL;
			childElement.GetContents(szBitmap);
			pBitmap = CCBitmapManager::Get(szBitmap);
			if (pCursor == NULL) pCursor = new CCBitmapCursor(szItem, pBitmap);
		}
	}

	if (pCursor != NULL) 
	{
		if (pParentWidget!=NULL) pParentWidget->SetCursor(pCursor);
		else (CCWidget*)m_pParent->SetCursor(pCursor);

		CCCursorSystem::Add(pCursor);
	}
	return pCursor;
}

CCBmLabel* CCIDLResource::GetBmLabel(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[1024];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	CCBmLabel* pBmLabel = (CCBmLabel*)Core::GetInstance()->NewWidget(CORE_CCBMLABEL, "", pParentWidget, pListener);
	InsertWidget(element, pBmLabel);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pBmLabel, childElement, szBuf)) continue;
		else if (!strcmp(szBuf, "FONTSIZE"))
		{
			pBmLabel->SetCharSize(GetSize(childElement));
		}
		else if (!strcmp(szBuf, "BITMAP"))
		{
			CCBitmap* pBitmap = GetBitmap(childElement);

			if (pBitmap != NULL)
			{
				pBmLabel->SetLabelBitmap(pBitmap);
			}
		}
	}
	return pBmLabel;
}

CCFont* CCIDLResource::CreateFont(char* szAliasName, char* szFontName, int nHeight,
								bool bBold, bool bItalic, int nOutlineStyle, bool bAntialiasing, DWORD nColorArg1, DWORD nColorArg2)
{
	return NULL;
}

CCFont* CCIDLResource::GetFont(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	char szItem[256];
	char szName[256];
	int nHeight = 10;
	bool bBold = false;
	bool bItalic = false;
	int nOutlineStyle = 0;
	DWORD nColorArg1 = 0;
	DWORD nColorArg2 = 0;
	int a,r,g,b; a=r=g=b=0;
	bool bAntialiasing = false;

	element.GetAttribute(szItem, IDL_ATTR_ITEM, "");

	CCFont* pFont = NULL;

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (!strcmp(szBuf, "FONTSET"))
		{
			childElement.GetContents(szName);
//			char szAniBitmap[256];
//			CCAniBitmap* pAniBitmap = NULL;
//			childElement.GetContents(szAniBitmap);
//			if (pCursor == NULL) pCursor = new CCAniBitmapCursor(szItem, pAniBitmap);
		}
		else if (!strcmp(szBuf, "FONTHEIGHT"))
		{
			childElement.GetContents(&nHeight);
		}
		else if (!strcmp(szBuf, "BOLD"))
		{
			bBold = true;
		}
		else if (!strcmp(szBuf, "ITALIC"))
		{
			bItalic = true;
		}
		else if (!strcmp(szBuf, "OUTLINESTYLE"))
		{
			childElement.GetContents(&nOutlineStyle);
		}
		else if (!strcmp(szBuf, "ANTIALIASING"))
		{
			bAntialiasing = true;
		}
		else if (!strcmp(szBuf, "COLORARG1"))
		{
			childElement.GetContents((int*)&nColorArg1);
			childElement.GetChildContents(&a, "A");
			childElement.GetChildContents(&r, "R");
			childElement.GetChildContents(&g, "G");
			childElement.GetChildContents(&b, "B");
			nColorArg1 = CORE_ARGB(a,r,g,b);
		}
		else if (!strcmp(szBuf, "COLORARG2"))
		{
			childElement.GetContents((int*)&nColorArg2);
			childElement.GetChildContents(&a, "A");
			childElement.GetChildContents(&r, "R");
			childElement.GetChildContents(&g, "G");
			childElement.GetChildContents(&b, "B");
			nColorArg2 = CORE_ARGB(a,r,g,b);
		}
	}

	pFont = CreateFont(szItem, szName, nHeight, bBold, bItalic, nOutlineStyle, bAntialiasing, nColorArg1, nColorArg2);
	if (pFont != NULL) 
	{
		CCFontManager::Add(pFont);
	}
	return pFont;
}

CCHotKey* CCIDLResource::GetHotKey(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	CCHotKey* pHotKey = (CCHotKey*)Core::GetInstance()->NewWidget(CORE_CCHOTKEY, "", pParentWidget, pListener);
	InsertWidget(element, pHotKey);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pHotKey, childElement, szBuf)) continue;


		if (!strcmp(szBuf, "EDITLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBEditLook*>::iterator itor = m_EditLookMap.find(szItem);
			if (itor != m_EditLookMap.end())
			{
				pHotKey->ChangeCustomLook((CCBEditLook*)(*itor).second);
			}
		}

	}

	return pHotKey;
}

/*
MActionKey* CCIDLResource::GetActionKey(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	MActionKey* pActionKey = (MActionKey*)Core::GetInstance()->NewWidget(CORE_ACTIONKEY, "", pParentWidget, pListener);
	InsertWidget(element, pActionKey);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pActionKey, childElement, szBuf)) continue;


		if (!strcmp(szBuf, "EDITLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBEditLook*>::iterator itor = m_EditLookMap.find(szItem);
			if (itor != m_EditLookMap.end())
			{
				pActionKey->ChangeCustomLook((CCBEditLook*)(*itor).second);
			}
		}

	}

	return pActionKey;
}
*/

CCTextArea* CCIDLResource::GetTextArea(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	CCTextArea* pTextArea = (CCTextArea*)Core::GetInstance()->NewWidget(CORE_CCTEXTAREA, "", pParentWidget, pListener);
	InsertWidget(element, pTextArea);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pTextArea, childElement, szBuf)) continue;

		if (!strcmp(szBuf, "EDITABLE"))
		{
			bool bEditable = true;
			childElement.GetContents(&bEditable);
			pTextArea->SetEditable(bEditable);
		} 
		else if (!strcmp(szBuf, "RESIZABLE"))
		{
			bool bValue = true;
			childElement.GetContents(&bValue);
			pTextArea->SetResizable(bValue);
		}
		else if (!strcmp(szBuf, "TEXTOFFSET"))
		{
			pTextArea->SetTextOffset(GetPoint(childElement));
		} 
		else if (!strcmp(szBuf, "TEXTCOLOR"))
		{
			pTextArea->SetTextColor(GetColor(childElement));
		} 
		else if (!strcmp(szBuf, "TEXTAREALOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBTextAreaLook*>::iterator itor = m_TextAreaLookMap.find(szItem);
			if (itor != m_TextAreaLookMap.end())
			{
				CCBTextAreaLook* pLook = (CCBTextAreaLook*)(*itor).second;
				pTextArea->ChangeCustomLook(pLook);
				pTextArea->SetFont(pLook->m_pFont);
			}
		}
		else if (!strcmp(szBuf, "MAXLENGTH"))
		{
			int nMaxLength;
			childElement.GetContents(&nMaxLength);
			pTextArea->SetMaxLen(nMaxLength);
		}
		else if (!strcmp(szBuf, "INDENTATION"))
		{
			int nIndentation;
			childElement.GetContents(&nIndentation);
			pTextArea->SetIndentation(nIndentation);
		}
		else if (!strcmp(szBuf, "SCROLLBAR"))
		{
			bool bValue = true;
			childElement.GetContents(&bValue);
			pTextArea->SetScrollBarEnable(bValue);
		}
	}
	return pTextArea;
}

CCTabCtrl* CCIDLResource::GetTabCtrl(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	CCTabCtrl* pTabCtrl= (CCTabCtrl*)Core::GetInstance()->NewWidget(CORE_CCTABCTRL, "", pParentWidget, pListener);
	InsertWidget(element, pTabCtrl);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pTabCtrl, childElement, szBuf)) continue;

		if (!strcmp(szBuf, "TAB"))
		{
			CCWidget *pButton = NULL;
			CCWidget *pFrame = NULL;

			char szWidgetName[256];
			childElement.GetAttribute(szWidgetName, "button");
			pButton = FindWidget(szWidgetName);
			if(pButton != NULL && 
				(strcmp(pButton->GetClassName(),CORE_CCBUTTON)==0 ||
				strcmp(pButton->GetClassName(),CORE_CCBMBUTTON)==0))
			{
				childElement.GetAttribute(szWidgetName, "widget");
				pFrame = FindWidget(szWidgetName);
				pTabCtrl->Add((CCButton*)pButton,pFrame);
			}
		}
		else if (!strcmp(szBuf, "TABCONTROLLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBTabCtrlLook*>::iterator itor = m_TabCtrlLookMap.find(szItem);
			if (itor != m_TabCtrlLookMap.end())
			{
				pTabCtrl->ChangeCustomLook((CCBTabCtrlLook*)(*itor).second);
			}
		}
	}
	return pTabCtrl;
}

CCPanel* CCIDLResource::GetPanel(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
//	char szFontName[4096];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	CCPanel* pPanel = (CCPanel*)Core::GetInstance()->NewWidget(CORE_CCPANEL, "", pParentWidget, pListener);
	InsertWidget(element, pPanel);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pPanel, childElement, szBuf)) continue;

		if (!stricmp(szBuf, "BORDERSTYLE"))
		{
			char szBorderStyle[256];
			childElement.GetContents(szBorderStyle);
			if (!stricmp(szBorderStyle, "none")) pPanel->SetBorderStyle(CCBS_NONE);
			else if (!stricmp(szBorderStyle, "single")) pPanel->SetBorderStyle(CCBS_SINGLE);
		}
		else if(!stricmp(szBuf, "BORDERCOLOR"))
		{
			sColor color = GetColor(childElement);
			pPanel->SetBorderColor(color);
		}
		else if(!stricmp(szBuf, "BACKGROUND"))
		{
			sColor color = GetColor(childElement);
			pPanel->SetBackgroundColor(color);
		}
	}
	return pPanel;
}

void CCIDLResource::Parse(::CCXmlElement& element)
{
	char szTagName[256];
	element.GetTagName(szTagName);

	if (!stricmp(szTagName, "BUTTONLOOKTEMPLATE"))
	{
		GetButtonLook(element);
	}
	else if (!stricmp(szTagName, "GROUPLOOKTEMPLATE"))
	{
		GetGroupLook(element);
	}
	else if (!stricmp(szTagName, "FRAMELOOKTEMPLATE"))
	{
		GetFrameLook(element);
	}
	else if (!stricmp(szTagName, "LABELLOOKTEMPLATE"))
	{
		GetLabelLook(element);
	}
	else if (!stricmp(szTagName, "EDITLOOKTEMPLATE"))
	{
		GetEditLook(element);
	}
	else if (!stricmp(szTagName, "LISTBOXLOOKTEMPLATE"))
	{
		GetListBoxLook(element, 0);
	}
	else if (!stricmp(szTagName, "COCCBOLISTBOXLOOKTEMPLATE"))
	{
		GetListBoxLook(element, 1);
	}
	else if (!stricmp(szTagName, "ARROWLOOKTEMPLATE"))
	{
		GetArrowLook(element);
	}
	else if (!stricmp(szTagName, "THUCCBLOOKTEMPLATE"))
	{
		GetThumbLook(element);
	}
	else if (!stricmp(szTagName, "SCROLLBARLOOKTEMPLATE"))
	{
		GetScrollBarLook(element);
	}
	else if (!stricmp(szTagName, "SLIDERLOOKTEMPLATE"))
	{
		GetSliderLook(element);
	}
	else if (!stricmp(szTagName, "TEXTAREALOOKTEMPLATE"))
	{
		GetTextAreaLook(element);
	}
	else if( !stricmp(szTagName, "SLIDERTHUCCBLOOKTEMPLATE" ))
	{
		GetSliderThumbLook( element );
	}
	else if( !stricmp(szTagName, "TABCTRLLOOKTEMPLATE" ))
	{
		GetTabCtrlLook( element );
	}
	else if (!stricmp(szTagName, "FONTTEMPLATE"))
	{
		GetFont(element);
	}
	else if (!stricmp(szTagName, "FRAME"))
	{
		GetFrame(element);
	}
	else if (!stricmp(szTagName, "LABEL"))
	{
		GetLabel(element);
	}
	else if (!stricmp(szTagName, "BMLABEL"))
	{
		GetBmLabel(element);
	}
	else if (!stricmp(szTagName, "BUTTON"))
	{
		GetButton(element);
	}
	else if (!stricmp(szTagName, "BCCBUTTON"))
	{
		GetBmButton(element);
	}
	else if (!stricmp(szTagName, "EDIT"))
	{
		GetEdit(element);
	}
	else if (!stricmp(szTagName, "LISTBOX"))
	{
		GetListBox(element);
	}
	else if (!stricmp(szTagName, "PICTURE"))
	{
		GetPicture(element);
	}
	else if (!stricmp(szTagName, "SCROLLBAR"))
	{
		GetScrollBar(element);
	}
	else if (!stricmp(szTagName, "SLIDER"))
	{
		GetSlider(element);
	}
	else if (!stricmp(szTagName, "GROUP"))
	{
		GetGroup(element);
	}
	else if (!stricmp(szTagName, "COCCBOBOX"))
	{
		GetComboBox(element);
	}
	else if (!stricmp(szTagName, "POPUPMENU"))
	{
		GetPopupMenu(element);
	}
	else if (!stricmp(szTagName, "ANIMATIONTEMPLATE"))
	{
		GetAniBitmap(element);
	}
	else if (!stricmp(szTagName, "ANIMATION"))
	{
		GetAnimation(element);
	}
	else if (!stricmp(szTagName, "CURSOR"))
	{
		GetCursor(element);
	}
	else if (!stricmp(szTagName, "HOTKEY"))
	{
		GetHotKey(element);
	}
/*	else if (!stricmp(szTagName, "ACTIONKEY"))
	{
		GetActionKey(element);
	}	*/
	else if (!stricmp(szTagName, "TEXTAREA"))
	{
		GetTextArea(element);
	}
	else if (!stricmp(szTagName, "TABCONTROL"))
	{
		GetTabCtrl(element);
	}
	else if (!stricmp(szTagName, "PANEL"))
	{
		GetPanel(element);
	}
	else if (!stricmp(szTagName, "BITMAPALIAS"))
	{
		GetBitmapAlias(element);
	}
	else if (!stricmp(szTagName, "REBOUNDS"))
	{
		GetRebounds(element);
	}
}

bool CCIDLResource::LoadFromFile(char* szFileName, CCWidget* pParent,CCZFileSystem *pfs)
{
	m_pParent = pParent;
	if(m_pParent==NULL) m_pParent = Core::GetInstance()->GetMainFrame();

	CCXmlDocument	xmlDocument;
	::CCXmlElement		rootElement, childElement;
	char			szBuf[4096];

	xmlDocument.Create();

	if(!pfs)
	{
		if (!xmlDocument.LoadFromFile(szFileName)) 
		{
			xmlDocument.Destroy();
			return false;
		}
	}else
	{
		CCZFile mzf;
		if(!mzf.Open(szFileName,pfs))
			return false;

		char *buffer;
		buffer=new char[mzf.GetLength()+1];
		mzf.Read(buffer,mzf.GetLength());
		buffer[mzf.GetLength()]=0;

		if(!xmlDocument.LoadFromMemory(buffer))
		{
			delete buffer;
			return false;
		}
		delete buffer;
		mzf.Close();
	}

	rootElement = xmlDocument.GetDocumentElement();

	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = rootElement.GetChildNode(i);

		childElement.GetTagName(szBuf);
		if (szBuf[0] == '#') continue;
		else if (!strcmp(szBuf, "INCLUDE"))
		{
			char* pdest;
			pdest = strrchr(szFileName, '\\');
			if (pdest == NULL) pdest = strrchr(szFileName, '/');

			char szContents[256], szFileName2[256];
			childElement.GetContents(szContents);

			memset(szFileName2, 0, sizeof(szFileName2));
			if (pdest != NULL)
			{
				int t = pdest - szFileName + 1;
				strncpy(szFileName2, szFileName, t);
			}
			strcat(szFileName2, szContents);

			LoadFromFile(szFileName2, m_pParent, pfs);
		}

		Parse(childElement);
	}

	xmlDocument.Destroy();

	return true;
}

bool CCIDLResource::SaveToFile(const char* szFileName)
{
	CCXmlDocument	xmlDocument;
	::CCXmlElement		rootElement, childElement;
	if (!xmlDocument.Create()) return false;
	xmlDocument.CreateProcessingInstruction();
	rootElement = xmlDocument.CreateElement(IDL_ROOT);
	rootElement.AppendText("\n\t");
	xmlDocument.AppendChild(rootElement);

	for (CCWidgetMMap::iterator itor = m_WidgetMap.begin(); itor != m_WidgetMap.end(); ++itor)
	{
		CCWidget* pWidget = (*itor).second;
		if (!strcmp(pWidget->GetClassName(), CORE_CCLABEL))
		{
			SetLabel(rootElement, (CCLabel*)pWidget);
		}
	}

	rootElement.AppendText("\n");
	if (!xmlDocument.SaveToFile(szFileName)) 
	{
		xmlDocument.Destroy();
		return false;
	}
	xmlDocument.Destroy();
	return true;
}

void CCIDLResource::ClearLooks()
{
#define CLEAR(x) while(!x.empty()) { delete (*x.begin()).second; x.erase(x.begin()); }

	// 룩을 클리어하기 전에 위젯들이 디폴트 룩을 사용하도록 바꾼다 (언어변환으로 리소스 리로딩할때 문제 생김)
	CCButton::ChangeLook(NULL);
	CCListBox::ChangeLook(NULL);
	CCEdit::ChangeLook(NULL);
	CCFrame::ChangeLook(NULL);
	CCGroup::ChangeLook(NULL);
	CCEdit::ChangeLook(NULL);
	CCLabel::ChangeLook(NULL);
	CCListBox::ChangeLook(NULL);
	CCPanel::ChangeLook(NULL);
	CCPopupMenu::ChangeLook(NULL);
	CCThumb::ChangeLook(NULL);
	CCArrow::ChangeLook(NULL);
	CCScrollBar::ChangeLook(NULL);
	CCSliderThumb::ChangeLook(NULL);
	CCScrollBar::ChangeLook(NULL);
	CCTabCtrl::ChangeLook(NULL);
	CCTextArea::ChangeLook(NULL);


	CLEAR(m_LabelLookMap);
	CLEAR(m_ButtonLookMap);
	CLEAR(m_GroupLookMap);
	CLEAR(m_FrameLookMap);
	CLEAR(m_EditLookMap);
	CLEAR(m_ListBoxLookMap);
	CLEAR(m_ScrollBarLookMap);
	CLEAR(m_ArrowLookMap);
	CLEAR(m_ThumbLookMap);
//	CLEAR(m_SliderLookMap);
	CLEAR(m_AniBitmapMap);
//	CLEAR(m_WidgetMap)
	CLEAR(m_TabCtrlLookMap);

	CLEAR(m_TextAreaLookMap);
	CLEAR(m_ButtonGroupMap);
}

void CCIDLResource::Clear()
{
	ClearLooks();

	cclog("clear looks end.\n");

	m_WidgetMap.Clear();

	cclog("Widget map clear end.\n");

	CCCursorSystem::Destroy();

	cclog("desctroy cursor system end.\n");
}

void CCIDLResource::SetLabel(::CCXmlElement& element, CCLabel* pLabel)
{
	::CCXmlElement childElement, labelElement;
	labelElement = element.CreateChildElement(IDL_LABEL);

	sRect rect = pLabel->GetRect();
	SetRect(labelElement, &rect, "BOUNDS");
	pLabel->m_IDLRect = rect;

	sColor color = pLabel->GetTextColor();
	SetColor(labelElement, &color, "TEXTCOLOR");

	childElement = labelElement.CreateChildElement(IDL_TEXT);
	childElement.SetContents(pLabel->GetText());
}

void CCIDLResource::SetPoint(::CCXmlElement& element, sPoint* pPoint, const char* szTagName)
{
	::CCXmlElement pointElement, childElement;
	pointElement = element.CreateChildElement(szTagName);
	childElement = pointElement.CreateChildElement("X");
	childElement.SetContents(pPoint->x);
	childElement = pointElement.CreateChildElement("Y");
	childElement.SetContents(pPoint->y);
}

void CCIDLResource::SetRect(::CCXmlElement& element, sRect* pRect, const char* szTagName)
{
	::CCXmlElement rectElement, childElement;
	rectElement = element.CreateChildElement(szTagName);
	childElement = rectElement.CreateChildElement("X");
	childElement.SetContents(pRect->x);
	childElement = rectElement.CreateChildElement("Y");
	childElement.SetContents(pRect->y);
	childElement = rectElement.CreateChildElement("W");
	childElement.SetContents(pRect->w);
	childElement = rectElement.CreateChildElement("H");
	childElement.SetContents(pRect->h);
}

void CCIDLResource::SetSize(::CCXmlElement& element, sSize* pSize, const char* szTagName)
{
	::CCXmlElement sizeElement, childElement;
	sizeElement = element.CreateChildElement(szTagName);
	childElement = sizeElement.CreateChildElement("W");
	childElement.SetContents(pSize->w);
	childElement = sizeElement.CreateChildElement("H");
	childElement.SetContents(pSize->h);
}

void CCIDLResource::SetColor(::CCXmlElement& element, sColor* pColor, const char* szTagName)
{
	::CCXmlElement colorElement, childElement;
	colorElement = element.CreateChildElement(szTagName);
	childElement = colorElement.CreateChildElement("R");
	childElement.SetContents(pColor->r);
	childElement = colorElement.CreateChildElement("G");
	childElement.SetContents(pColor->g);
	childElement = colorElement.CreateChildElement("B");
	childElement.SetContents(pColor->b);
	childElement = colorElement.CreateChildElement("ALPHA");
	childElement.SetContents(pColor->a);
}

CCWidget* CCIDLResource::FindWidget(string szItem)
{
	CCWidgetMMap::iterator itor = m_WidgetMap.find(szItem);
	if (itor != m_WidgetMap.end())
	{
		return (CCWidget*)(*itor).second;
	}
	else
	{
		return NULL;
	}
}

void CCIDLResource::FindWidgets(CCWidgetList& widgetList, string szItem)
{
	pair<CCWidgetMMap::iterator, CCWidgetMMap::iterator> p = m_WidgetMap.equal_range(szItem);

	for (CCWidgetMMap::iterator itor = p.first; itor != p.second; ++itor)
	{
		CCWidget* pWidget = (*itor).second;
		widgetList.push_back(pWidget);
	}
}

CCFrame*	CCIDLResource::CreateFrame(const char* szName, CCWidget* pParent, CCListener* pListener)
{
	CCFrame* pFrame = (CCFrame*)Core::GetInstance()->NewWidget(CORE_CCFRAME, szName, pParent, pListener);
	return pFrame;
}

CCBFrameLook* CCIDLResource::FindFrameLook(string szItem)
{
	map<string, CCBFrameLook*>::iterator itor = m_FrameLookMap.find(szItem);
	if (itor != m_FrameLookMap.end())
	{
		return (*itor).second;
	}

	return NULL;
}

void CCIDLResource::InsertWidget(const char* pItemName, CCWidget* pWidget )
{
	m_WidgetMap.insert(CCWidgetMMap::value_type(string(pItemName), pWidget) );
}

void CCIDLResource::TransText(char* szSrc, char* szOut)
{
	strcpy(szOut, szSrc);
}

void CCIDLResource::GetRebounds(::CCXmlElement& element)
{
	char szItem[256] = {0, };

	::CCXmlElement childElement;

	element.GetAttribute(szItem, "item");
	sRect rt = GetRect(element);

	CCWidgetList widget_list;
	FindWidgets(widget_list, string(szItem));


/*
#ifdef _DEBUG
	if (widget_list.size() != 1)
	{
		_ASSERT(0);		// 위젯이 하나가 아니다.
	}
#endif
*/

	for (CCWidgetList::iterator itor = widget_list.begin(); itor != widget_list.end(); ++itor)
	{
		CCWidget* pWidget = (*itor);
		pWidget->SetBounds( rt);
		pWidget->m_IDLRect = rt;
	}
}