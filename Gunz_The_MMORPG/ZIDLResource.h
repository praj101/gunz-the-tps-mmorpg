#ifndef ZIDLRESOURCE_H
#define ZIDLRESOURCE_H

#include "CCIDLResource.h"
#include "ZFilePath.h"


class ZFrame;
class ZMapListBox;
class ZScoreListBox;
class ZScoreBoardFrame;
class ZMeshView;
class ZMeshViewList;
class ZCharacterView;
class ZCharacterViewList;
class ZShopEquipListbox;
class ZStageInfoBox;
class ZItemSlotView;
class ZRoomListBox;
class ZPlayerListBox;
class ZCanvas;
class ZPlayerSelectListBox;
class ZBmNumLabel;
class ZClanListBox;
class ZDuelTournamentRankingListBox;
class ZServerView;
class ZActionKey;

class ZIDLResource : public CCIDLResource
{
private:
protected:
	ZMapListBox* GetMapListBox(CCXmlElement& element);
	ZScoreBoardFrame* GetScoreBoardFrame(CCXmlElement& element);
	ZScoreListBox* GetScoreListBox(CCXmlElement& element);
	ZMeshView* GetMeshView(CCXmlElement& element);
	ZMeshViewList* GetMeshViewList(CCXmlElement& element);
	ZCharacterView* GetCharacterView(CCXmlElement& element);
	ZCharacterViewList* GetCharacterViewList(CCXmlElement& element);
	ZShopEquipListbox* GetEquipmentListBox(CCXmlElement& element);
	ZStageInfoBox* GetStageInfoBox(CCXmlElement& element);
	ZItemSlotView* GetItemSlot(CCXmlElement& element);
	ZRoomListBox* GetRoomListBox(CCXmlElement& element);
	ZPlayerListBox* GetPlayerListBox(CCXmlElement& element);
	ZCanvas* GetCanvas(CCXmlElement& element);
	ZPlayerSelectListBox* GetPlayerSelectListBox(CCXmlElement& element);
	ZBmNumLabel *GetBmNumLabel(CCXmlElement& element);
	ZClanListBox* GetClanListBox( CCXmlElement& element );
	ZDuelTournamentRankingListBox* GetDuelTournamentRankingListBox( CCXmlElement& element );
	ZServerView* GetServerView(CCXmlElement& element);
	ZActionKey* GetActionKey(CCXmlElement& element);

	virtual void TransText(char* szSrc, char* szOut);		// 국제화를 위한 함수
	virtual void Parse(CCXmlElement& element);
	virtual CCFrame*	CreateFrame(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual CCFont* CreateFont(char* szAliasName, char* szFontName, int nHeight
		              ,bool bBold = false, bool bItalic = false, int nOutlineStyle = 0, bool bAntialiasing = false, DWORD nColorArg1=0, DWORD nColorArg2=0);
public:
	ZIDLResource();
	virtual ~ZIDLResource();
};

void ZGetInterfaceSkinPath(char* pOutPath, const char* szSkinName);
void ZGetInterfaceSkinPathSubLanguage(char* pOutPath, const char* szSkinName);	// 현재 선택된 언어를 위한 인터페이스 비트맵 경로

#endif