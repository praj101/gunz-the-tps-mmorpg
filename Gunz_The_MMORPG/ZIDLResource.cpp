//#define _INDEPTH_DEBUG_

#include "stdafx.h"

#include "ZIDLResource.h"
#include "winsock2.h"
//#include "ZGame.h"
#include "Core4R2.h"
#include "RealSpace2.h"
#include "Core.h"
#include "CCBScrollBarLook.h"
#include "CCBMultiColListBoxLook.h"

#include "ZFrame.h"
//#include "ZMapListBox.h"
//#include "ZScoreListBox.h"
//#include "ZMeshView.h"
//#include "ZMeshViewList.h"
//#include "ZCharacterView.h"
//#include "ZCharacterViewList.h"
//#include "ZShopEquipListbox.h"
#include "ZStageInfoBox.h"
//#include "ZItemSlotView.h"
//#include "ZRoomListBox.h"
//#include "ZPlayerListBox.h"
#include "ZCanvas.h"
//#include "ZPlayerSelectListBox.h"
#include "ZBmNumLabel.h"
//#include "ZClanListBox.h"
//#include "ZDuelTournamentRankingListBox.h"
//#include "ZServerView.h"
#include "ZStringResManager.h"
#include "ZActionKey.h"
#include "ZConfiguration.h"

ZIDLResource::ZIDLResource()
{

}
ZIDLResource::~ZIDLResource()
{

}

ZCanvas*		ZIDLResource::GetCanvas(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	ZCanvas* pCanvas = new ZCanvas("", pParentWidget, pListener);
	InsertWidget(element, pCanvas);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		GetCommonWidgetProperty(pCanvas, childElement, szBuf);
	}

	return pCanvas;
}

ZMapListBox*	ZIDLResource::GetMapListBox(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
//	ZMapListBox* pListBox = new ZMapListBox("", pParentWidget, pListener);
//	InsertWidget(element, pListBox);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

//		GetCommonWidgetProperty(pListBox, childElement, szBuf);
	}

	return NULL;
}

ZScoreBoardFrame* ZIDLResource::GetScoreBoardFrame(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;
	CCBFrameLook* pFrameLook = NULL;

	pListener = pParentWidget = GetParentWidget(element);
//	ZScoreBoardFrame* pFrame = new ZScoreBoardFrame("", pParentWidget, pListener);
//	InsertWidget(element, pFrame);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

//		if (GetCommonWidgetProperty(pFrame, childElement, szBuf)) continue;

		if (!strcmp(szBuf, "FRAMELOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBFrameLook*>::iterator itor = m_FrameLookMap.find(szItem);
			if (itor != m_FrameLookMap.end())
			{
				pFrameLook = (CCBFrameLook*)(*itor).second;
//				pFrame->ChangeCustomLook((CCFrameLook*)pFrameLook);
			}
		}
	}


//	return pFrame;
	return NULL;
}


ZScoreListBox*	ZIDLResource::GetScoreListBox(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
//	ZScoreListBox* pListBox = new ZScoreListBox("", pParentWidget, pListener);
//	InsertWidget(element, pListBox);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

//		GetCommonWidgetProperty(pListBox, childElement, szBuf);
	}
//	return pListBox;
	return NULL;
}


ZMeshView*		ZIDLResource::GetMeshView(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];

	CCWidget* pParentWidget = GetParentWidget(element);
//	ZMeshView* pWidget = new ZMeshView("", pParentWidget, pParentWidget);
//	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

//		GetCommonWidgetProperty(pWidget, childElement, szBuf);

		if (!strcmp(szBuf, "BUTTONLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBButtonLook*>::iterator itor = m_ButtonLookMap.find(szItem);
			if (itor != m_ButtonLookMap.end())
			{
//				pWidget->ChangeCustomLook((CCButtonLook*)(*itor).second);
			}
//			pWidget->SetLook(true);
		}
	}

//	return pWidget;
	return NULL;
}

ZMeshViewList*	ZIDLResource::GetMeshViewList(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];

	CCWidget* pParentWidget = GetParentWidget(element);
//	ZMeshViewList* pWidget = new ZMeshViewList("", pParentWidget, pParentWidget);
//	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

//		if(GetCommonWidgetProperty(pWidget, childElement, szBuf)) continue;

		if(!strcmp(szBuf, "ITEMWIDTH")){
			int nWidth = 80;
			childElement.GetContents(&nWidth);
//			pWidget->SetItemWidth(nWidth);
		}

	}

//	return pWidget;
	return NULL;
}

ZCharacterView* ZIDLResource::GetCharacterView(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];

	CCWidget* pParentWidget = GetParentWidget(element);
//	ZCharacterView* pWidget = new ZCharacterView("", pParentWidget, pParentWidget);
//	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

//		GetCommonWidgetProperty(pWidget, childElement, szBuf);

		if(!strcmp(szBuf, "VISIBLEEQUIPMENT")){
			bool bVisibleEquipment = false;
			childElement.GetContents(&bVisibleEquipment);
//			pWidget->SetVisibleEquipment(bVisibleEquipment);
		}
	}

//	return pWidget;
	return NULL;
}

ZCharacterViewList* ZIDLResource::GetCharacterViewList(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];

	CCWidget* pParentWidget = GetParentWidget(element);
//	ZCharacterViewList* pWidget = new ZCharacterViewList("", pParentWidget, pParentWidget);
//	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

//		if(GetCommonWidgetProperty(pWidget, childElement, szBuf)) continue;

		if(!strcmp(szBuf, "ITEMWIDTH")){
			int nWidth = 80;
			childElement.GetContents(&nWidth);
//			pWidget->SetItemWidth(nWidth);
		}
	}

//	return pWidget;
	return NULL;
}

ZStageInfoBox*	ZIDLResource::GetStageInfoBox(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];

	CCWidget* pParentWidget = GetParentWidget(element);
	ZStageInfoBox* pWidget = new ZStageInfoBox("", pParentWidget, pParentWidget);
	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pWidget, childElement, szBuf)) continue;
		else if (!strcmp(szBuf, "STAGEINFOITEMLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, CCBListBoxLook*>::iterator itor = m_ListBoxLookMap.find(szItem);
			if (itor != m_ListBoxLookMap.end())
			{
				CCBListBoxLook* pListBoxLook = NULL;
				pListBoxLook = (*itor).second;
//				pWidget->ChangeCustomLook(pListBoxLook);
				pWidget->SetLook(pListBoxLook);
			}
		}
	}

	return pWidget;
}

ZClanListBox*	ZIDLResource::GetClanListBox( ::CCXmlElement& element )
{
	::CCXmlElement childElement;
	char szBuf[4096];
//	char szAttr[4096];

	CCWidget* pParentWidget = GetParentWidget(element);
//	ZClanListBox* pWidget = new ZClanListBox("", pParentWidget, pParentWidget);
//	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

//		if(GetCommonWidgetProperty(pWidget, childElement, szBuf)) continue;
	}
//	return pWidget;
	return NULL;
}

ZDuelTournamentRankingListBox* ZIDLResource::GetDuelTournamentRankingListBox( ::CCXmlElement& element )
{
	::CCXmlElement childElement;
	char szBuf[4096];
	//	char szAttr[4096];

	CCWidget* pParentWidget = GetParentWidget(element);
//	ZDuelTournamentRankingListBox* pWidget = new ZDuelTournamentRankingListBox("", pParentWidget, pParentWidget);
//	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

//		if(GetCommonWidgetProperty(pWidget, childElement, szBuf)) continue;
	}
//	return pWidget;
	return NULL;
}

ZServerView*	ZIDLResource::GetServerView(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[ 4096];

	CCWidget* pParentWidget = GetParentWidget( element);
//	ZServerView* pWidget = new ZServerView( "", pParentWidget, pParentWidget);
//	InsertWidget( element, pWidget);

	int iCount = element.GetChildNodeCount();
	for (int i = 0;  i < iCount;  i++)
	{
		memset(szBuf, 0, sizeof( szBuf));
		childElement = element.GetChildNode( i);
		childElement.GetTagName( szBuf);

//		GetCommonWidgetProperty( pWidget, childElement, szBuf);

//		if ( !strcmp( szBuf, "NAME"))
//		{
//			char szName[ 25];
//			childElement.GetContents( szName);
//			pWidget->SetName( szName);
//		}
	}

//	return pWidget;
	return NULL;
}

ZPlayerSelectListBox* ZIDLResource::GetPlayerSelectListBox(::CCXmlElement& element)
{
/*	::CCXmlElement childElement;
	char szBuf[4096];
//	char szAttr[4096];

	CCWidget* pParentWidget = GetParentWidget(element);
	ZPlayerSelectListBox* pWidget = new ZPlayerSelectListBox("SelectPlayer", pParentWidget, pParentWidget);
	pWidget->SetListener(pWidget);
	InsertWidget(element, pWidget);

	int iCount = element.GetChildNodeCount();

	bool bMode1 = false;

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if(GetCommonWidgetProperty(pWidget, childElement, szBuf)) continue;
		else if(!strcmp(szBuf, "MULTISELECT"))
		{
			pWidget->m_bMultiSelect = true;
		}
	}*/
	return NULL;
//	return pWidget;
}

ZBmNumLabel*	ZIDLResource::GetBmNumLabel(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[1024];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	ZBmNumLabel* pBmLabel = new ZBmNumLabel(CORE_ZBMNUMLABEL, pParentWidget, pListener);
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

ZActionKey*		ZIDLResource::GetActionKey(::CCXmlElement& element)
{
	::CCXmlElement childElement;
	char szBuf[4096];
	CCWidget* pParentWidget;	CCListener* pListener;

	pListener = pParentWidget = GetParentWidget(element);
	ZActionKey* pActionKey = (ZActionKey*)Core::GetInstance()->NewWidget(CORE_ACTIONKEY, "", pParentWidget, pListener);
	InsertWidget(element, pActionKey);

	int iCount = element.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		memset(szBuf, 0, sizeof(szBuf));
		childElement = element.GetChildNode(i);
		childElement.GetTagName(szBuf);

		if (GetCommonWidgetProperty(pActionKey, childElement, szBuf)) continue;

		/*
		if (!strcmp(szBuf, "EDITLOOK"))
		{
			char szItem[256];
			memset(szItem, 0, sizeof(szItem));
			childElement.GetContents(szItem);

			map<string, MBEditLook*>::iterator itor = m_EditLookMap.find(szItem);
			if (itor != m_EditLookMap.end())
			{
				pActionKey->ChangeCustomLook((MBEditLook*)(*itor).second);
			}
		}
		*/

	}

	return pActionKey;
}

void			ZIDLResource::Parse(::CCXmlElement& element)
{
	CCIDLResource::Parse(element);

	char szTagName[256];
	element.GetTagName(szTagName);

	// Gunz Customized Widgets...
	if (!strcmp(szTagName, "MAPLISTBOX"))
	{
		GetMapListBox(element);
	}
	else if (!strcmp(szTagName, "CANVAS"))
	{
		GetCanvas(element);
	}
	else if (!strcmp(szTagName, "SCORELISTBOX"))
	{
		GetScoreListBox(element);
	}
	else if (!strcmp(szTagName, "SCOREBOARD"))
	{
		GetScoreBoardFrame(element);
	}
	else if (!strcmp(szTagName, "MESHVIEW"))
	{
		GetMeshView(element);
	}
	else if (!strcmp(szTagName, "MESHVIEWLIST"))
	{
		GetMeshViewList(element);
	}
	else if (!strcmp(szTagName, "CHARACTERVIEW"))
	{
		GetCharacterView(element);
	}
	else if (!strcmp(szTagName, "CHARACTERVIEWLIST"))
	{
		GetCharacterViewList(element);
	}
	else if (!strcmp(szTagName, "EQUIPMENTLISTBOX"))	// 상점 리뉴얼하기 전에 사용하던 리스트위젯
	{
//		GetEquipmentListBox(element);
	}
	else if(!strcmp(szTagName, "STAGEINFOBOX"))
	{
		GetStageInfoBox(element);
	}
	else if (!strcmp(szTagName, "ITEMSLOT"))
	{
//		GetItemSlot(element);
	}
	else if (!strcmp(szTagName, "ROOMLIST"))
	{
//		GetRoomListBox(element);
	}
	else if(!strcmp(szTagName,"PLAYERLISTBOX"))
	{
#ifdef _INDEPTH_DEBUG_
		cclog("ZIDLResource::Parse() [%s]\n", szTagName);
#endif
//		GetPlayerListBox(element);
	}
	else if(!strcmp(szTagName,"PLAYERSELECTLISTBOX"))
	{
		GetPlayerSelectListBox(element);
	}
	else if(!strcmp(szTagName,"ZBMNUMLABEL"))
	{
		GetBmNumLabel(element);
	}
	else if (!strcmp(szTagName, "CLANLIST"))
	{
		GetClanListBox(element);
	}
	else if (!strcmp(szTagName, "DUELTOURNAMENTRANKINGLIST"))
	{
		GetDuelTournamentRankingListBox(element);
	}
	else if (!strcmp(szTagName, "SERVERVIEW"))
	{
		GetServerView(element);
	}
	else if (!strcmp(szTagName,"ACTIONKEY"))
	{
		GetActionKey(element);
	}
}

CCFrame*		ZIDLResource::CreateFrame(const char* szName, CCWidget* pParent, CCListener* pListener)
{
	CCFrame* pFrame = new ZFrame(szName, pParent, pListener);
	return pFrame;
}

CCFont*			ZIDLResource::CreateFont(char* szAliasName, char* szFontName, int nHeight, 
								bool bBold, bool bItalic, int nOutlineStyle, bool bAntialiasing, DWORD nColorArg1, DWORD nColorArg2)
{
	CCFontR2* pNew = new CCFontR2;
	pNew->Create(szAliasName, szFontName, nHeight, 1.0f, bBold, bItalic, nOutlineStyle, -1, bAntialiasing, nColorArg1, nColorArg2);
	return pNew;
}

void			ZIDLResource::TransText(char* szSrc, char* szOut)
{
	strcpy(szOut, ZGetStringResManager()->GetStringFromXml(szSrc));

	if (!strnicmp(szOut, "STR:", 4))
	{
		cclog("%s , %s\n", szSrc, szOut);
	}
}

void ZGetInterfaceSkinPath(char* pOutPath, const char* szSkinName)
{
	sprintf(pOutPath, "%s%s/", PATH_INTERFACE, szSkinName);
}

void ZGetInterfaceSkinPathSubLanguage(char* pOutPath, const char* szSkinName)
{
	// 대체 언어용 UI이미지는 interface/###/skinname 의 폴더에 넣도록 한다
	// 예) interface/default, interface/lodable의 독일어용 경로는 interface/grm/default, interface/grm/lodable이 된다
	const char* szLanguage = ZGetConfiguration()->GetSelectedLanguage();
	sprintf(pOutPath, "%s%s/%s/", PATH_INTERFACE, szLanguage, szSkinName);
}


