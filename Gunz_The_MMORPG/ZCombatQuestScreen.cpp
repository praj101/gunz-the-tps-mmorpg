#include "stdafx.h"
#include "ZCombatQuestScreen.h"
#include "ZCombatInterface.h"
#include "ZCharacter.h"
#include "ZMyCharacter.h"
#include "ZGame.h"
#include "ZCharacterManager.h"
#include "ZModule_QuestStatus.h"
#include "ZActor.h"

// Added
#include "ZGlobal.h"
// Added R349a
#include "ZMyInfo.h"
#include "ZGameClient.h"
// Added R350a
#include "ZStringResManager.h"

/////////////////////////////////////////////////////////////////////////////////
ZCombatQuestScreen::ZCombatQuestScreen()
{
	
}

ZCombatQuestScreen::~ZCombatQuestScreen()
{

}

// NPC 킬수를 기준으로 소팅
bool CompareQuestScreenCharacter(ZCharacter* a, ZCharacter* b) 
{
	ZModule_QuestStatus* pAMod = (ZModule_QuestStatus*)a->GetModule(ZMID_QUESTSTATUS);
	ZModule_QuestStatus* pBMod = (ZModule_QuestStatus*)b->GetModule(ZMID_QUESTSTATUS);

	if ((pAMod) && (pBMod))
	{
		if (pAMod->GetKills() < pBMod->GetKills()) return false;
	}

	return true;
}


void ZCombatQuestScreen::OnDraw(CCDrawContext* pDC)
{
	/////////////////////////////////////////////
	list<ZCharacter*>		SortedCharacterList;

	for(ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
		itor != ZGetGame()->m_CharacterManager.end(); ++itor)
	{
		ZCharacter* pCharacter = (*itor).second;
		if (!pCharacter->IsVisible()) continue;

		SortedCharacterList.push_back(pCharacter);
	}

	SortedCharacterList.sort(CompareQuestScreenCharacter);
/*
	int cnt = 0;
	for (list<ZCharacter*>::iterator itor = SortedCharacterList.begin(); itor != SortedCharacterList.end(); ++itor)
	{
		DrawPlayer(pDC, cnt, (*itor));
		cnt++;
	}
*/


	if ( ZGetQuest()->IsRoundClear())
	{
		// 운영자 hide는 제외
		bool bEventHide = false;
		if (ZGetMyInfo()->IsAdminGrade()) 
		{
			CCMatchObjCache* pCache = ZGetGameClient()->FindObjCache(ZGetMyUID());
			if (pCache && pCache->CheckFlag(CCTD_PlayerFlags_AdminHide)) bEventHide = true;
		}

		DWORD dwSec;
		DWORD dwCurrTime = timeGetTime();
		if ( ZGetQuest()->GetRemainedTime() < dwCurrTime)
			dwSec = 0;
		else
			dwSec = (ZGetQuest()->GetRemainedTime() - dwCurrTime) / 1000;

		char szSec[ 10];
		sprintf( szSec, "%d", dwSec);
		char szMsg[ 128];
		ZTransMsg( szMsg, MSG_GAME_NEXT_N_MIN_AFTER, 1, szSec);

		pDC->SetFont( CCFontManager::Get("FONTa10_O2Wht"));
		pDC->SetColor(sColor(0xFFFFFFFF));

		if (!bEventHide)
			TextRelative(pDC,400.f/800.f,500.f/600.f, szMsg, true);
	}
}


void ZCombatQuestScreen::DrawPlayer(CCDrawContext* pDC, int index, ZCharacter* pCharacter)
{
	CCFont *pFont = CCFontManager::Get("FONTa10b");
	pDC->SetFont( pFont );
	sColor color = sColor(0xFFFFFFFF);
	if (pCharacter->IsDie()) color = sColor(0xFF999999);
	else if (pCharacter == ZGetGame()->m_pMyCharacter) color = sColor(0xFFEEEE00);
	pDC->SetColor(color);

	char szMsg[128];
	float x, y;
	x = 10.0f / 800.0f;
	y = (200.0f/600.0f) + ((20.0f / 600.0f) * index);

	int screenx = x * CCGetWorkspaceWidth();
	int screeny = y * CCGetWorkspaceHeight();


	int nKills = 0;
	ZModule_QuestStatus* pMod = (ZModule_QuestStatus*)pCharacter->GetModule(ZMID_QUESTSTATUS);
	if (pMod)
	{
		nKills = pMod->GetKills();
	}

	sprintf(szMsg, "%d. %s / %d K.O.", index+1, pCharacter->GetProperty()->GetName(), nKills);
	pDC->Text(screenx, screeny, szMsg);

/*
	// hp, ap bar
	const int BAR_HEIGHT = 3;
	const int BAR_WIDTH = 80;
	int bar_hp_width = (int)(BAR_WIDTH * ((float)pCharacter->GetHP() / pCharacter->GetProperty()->fMaxHP));
	int bar_ap_width = (int)(BAR_WIDTH * ((float)pCharacter->GetAP() / pCharacter->GetProperty()->fMaxAP));

	color = sColor(0xFFD6290B);
	int bar_y = screeny + pFont->GetHeight()+2;
	pDC->SetColor(color);
	pDC->FillRectangle(screenx, bar_y, bar_hp_width, BAR_HEIGHT);
	pDC->Rectangle(screenx, bar_y, BAR_WIDTH, BAR_HEIGHT);

	color = sColor(0xFF3AAF3A);
	pDC->SetColor(color);
	bar_y += (BAR_HEIGHT + 2);
	pDC->FillRectangle(screenx, bar_y, bar_ap_width, BAR_HEIGHT);
	pDC->Rectangle(screenx, bar_y, BAR_WIDTH, BAR_HEIGHT);
*/
}