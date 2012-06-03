#include "stdafx.h"
#include "ZShopEquipItemConfirm.h"
#include "ZShopEquipInterface.h"

// Added R347a
#include "ZGlobal.h"
// Added R349a
#include "ZStringResManager.h"
#include "ZMessages.h"
#include "CCPicture.h"
#include "CCEdit.h"
#include "ZIDLResource.h"
#include <sstream>

static ZIDLResource* GetIDLResource() {
	return ZGetGameInterface()->GetIDLResource();
}

void ZItemCountDlg::Open(ZITEMCOUNTDLG_MODE mode, const char* szItemName, CCBitmap* pIcon, int price, int nMin, int nMax, IItemCountDlgDoneHandler* pHandler )
{
	m_mode = mode;
	m_strItemName = szItemName;
	m_pItemIcon = pIcon;
	m_nPrice = price;
	m_nMin = nMin;
	m_nMax = nMax;
	m_pDoneHandler = pHandler;

	if (m_nMin > m_nMax || m_nMin < 0 || m_nMax < 0)
	{
		_ASSERT(0);
		m_nMin = m_nMax = 0;
	}

	m_nCurrCount = 1;

	CCFrame* pFrame = (CCFrame*)GetIDLResource()->FindWidget("TradeCountableItemFrame");
	CCLabel* pLabel1 = (CCLabel*)GetIDLResource()->FindWidget("TradeCountableItem_Message");
	CCLabel* pLabel2 = (CCLabel*)GetIDLResource()->FindWidget("TradeCountableItem_NullLabel");
	CCButton* pBtn = (CCButton*)GetIDLResource()->FindWidget("TradeCountableItem_Ok");

	switch (m_mode) {
		case ZICD_SELL:
			if (pFrame) pFrame->SetText(ZMsg(MSG_SHOPEQUIP_SELL_ITEM));
			if (pLabel1) pLabel1->SetText(ZMsg(MSG_SHOPEQUIP_SELL_COUNT_ASK));
			if (pLabel2) pLabel2->SetText(ZMsg(MSG_SHOPEQUIP_SELL_COUNT));
			if (pBtn) pBtn->SetText(ZMsg(MSG_SHOPEQUIP_SELL_BUTTON));
			break;
		case ZICD_BUY:
			if (pFrame) pFrame->SetText(ZMsg(MSG_SHOPEQUIP_BUY_ITEM));
			if (pLabel1) pLabel1->SetText(ZMsg(MSG_SHOPEQUIP_BUY_COUNT_ASK));
			if (pLabel2) pLabel2->SetText(ZMsg(MSG_SHOPEQUIP_BUY_COUNT));
			if (pBtn) pBtn->SetText(ZMsg(MSG_SHOPEQUIP_BUY_BUTTON));
			break;
		case ZICD_SENDACCOUNT:
			if (pFrame) pFrame->SetText(ZMsg(MSG_SHOPEQUIP_SEND_ITEM));
			if (pLabel1) pLabel1->SetText(ZMsg(MSG_SHOPEQUIP_SEND_COUNT_ASK));
			if (pLabel2) pLabel2->SetText(ZMsg(MSG_SHOPEQUIP_SEND_COUNT));
			if (pBtn) pBtn->SetText(ZMsg(MSG_SHOPEQUIP_SEND_BUTTON));
			break;
		case ZICD_BRINGACCOUNT:
			if (pFrame) pFrame->SetText(ZMsg(MSG_SHOPEQUIP_BRING_ITEM));
			if (pLabel1) pLabel1->SetText(ZMsg(MSG_SHOPEQUIP_BRING_COUNT_ASK));
			if (pLabel2) pLabel2->SetText(ZMsg(MSG_SHOPEQUIP_BRING_COUNT));
			if (pBtn) pBtn->SetText(ZMsg(MSG_SHOPEQUIP_BRING_BUTTON));
			break;
	}

	UpdateDlg();

	CCWidget* pWidget = GetIDLResource()->FindWidget( "TradeCountableItemFrame");
	if ( pWidget)
		pWidget->Show( true, true);

	pWidget = GetIDLResource()->FindWidget( "TradeCountableItem_CountNum");
	if ( pWidget)
		pWidget->SetFocus();
}

void ZItemCountDlg::Close()
{
	CCWidget* pWidget = GetIDLResource()->FindWidget( "TradeCountableItemFrame");
	if ( pWidget) pWidget->Show( false);
}

void ZItemCountDlg::UpdateDlg()
{
	char szText[ 256];

	CCPicture* pPicture = (CCPicture*)GetIDLResource()->FindWidget( "TradeCountableItem_ItemIcon");
	if ( pPicture && m_pItemIcon)
		pPicture->SetBitmap( m_pItemIcon);

	// 현재 수량이 올바른 범위내에 있는지 검사
	m_nCurrCount = max(m_nCurrCount, m_nMin);
	m_nCurrCount = min(m_nCurrCount, m_nMax);

	CCLabel* pLabel = (CCLabel*)GetIDLResource()->FindWidget( "TradeCountableItem_Calculate");
	if (pLabel)
	{
		if (m_mode == ZICD_SENDACCOUNT || m_mode == ZICD_BRINGACCOUNT)
		{
			sprintf( szText, "%s x %d", m_strItemName.c_str(), m_nCurrCount);
			pLabel->SetText( szText);
		}
		else if (m_mode == ZICD_SELL || m_mode == ZICD_BUY)
		{
			sprintf( szText, "%s (%d bounty) x %d", m_strItemName.c_str(), m_nPrice, m_nCurrCount);
			pLabel->SetText( szText);
		}
	}

	if (m_mode == ZICD_SELL || m_mode == ZICD_BUY)
	{
		pLabel = (CCLabel*)GetIDLResource()->FindWidget( "TradeCountableItem_Total");
		if (pLabel)
		{
			sprintf( szText, "= %d bounty", m_nPrice * m_nCurrCount);
			pLabel->SetText(szText);
		}
	}

	CCEdit* pEdit = (CCEdit*)GetIDLResource()->FindWidget( "TradeCountableItem_CountNum");
	if ( pEdit)
	{
		// 현재 에디트박스에 기입된 수량이 다르다면 갱신해준다
		const char* sz = pEdit->GetText();
		int count = atoi(sz);

		if (m_nCurrCount != count)
		{
			stringstream ss;
			ss << m_nCurrCount;
			pEdit->SetText( ss.str().c_str());
		}
	}
}

void ZItemCountDlg::AddCount( int n )
{
	m_nCurrCount += n;
	UpdateDlg();
}

void ZItemCountDlg::OnEditBoxChanged()
{
	CCEdit* pEdit = (CCEdit*)GetIDLResource()->FindWidget( "TradeCountableItem_CountNum");
	if ( pEdit)
	{
		const char* sz = pEdit->GetText();
		int count = atoi(sz);
		m_nCurrCount = count;
	}

	UpdateDlg();
}

void ZItemCountDlg::OnDlgDone()
{
	Close();

	if (m_pDoneHandler && m_nCurrCount > 0)
		m_pDoneHandler->OnDone(m_nCurrCount);
}

ZSimpleConfirmDlg::ZSimpleConfirmDlg()
{
	m_pMsgbox = (CCMsgBox*)Core::GetInstance()->NewWidget(CORE_CCMSGBOX, "", COre::GetInstance()->GetMainFrame(), this);
	m_pMsgbox->SetType(CCT_OKCANCEL);

	m_pDoneHandler = NULL;
}

ZSimpleConfirmDlg::~ZSimpleConfirmDlg()
{
	delete m_pMsgbox;
}

bool ZSimpleConfirmDlg::OnCommand(CCWidget* pWidget, const char* szMessage)
{
	if (pWidget == m_pMsgbox)
	{
		if (strcmp(szMessage, CCMSGBOX_OK)==0)
		{
			m_pMsgbox->Show(false);
			if (!m_pDoneHandler) { _ASSERT(0); return true; }
			m_pDoneHandler->OnDone(true);
		}
		else if (strcmp(szMessage, CCMSGBOX_CANCEL)==0)
		{
			m_pMsgbox->Show(false);
			if (!m_pDoneHandler) { _ASSERT(0); return true; }
			m_pDoneHandler->OnDone(false);
		}
	}
	return true;
}

void ZSimpleConfirmDlg::Open( const char* szMsg, ISimpleConfirmDlgDoneHandler* pHandler )
{
	m_pDoneHandler = pHandler;
	m_pMsgbox->SetText(szMsg);
	m_pMsgbox->Show(true, true);
}


ZSellCashItemConfirmDlg::ZSellCashItemConfirmDlg()
{
	m_pDoneHandler = NULL;
	m_nWaitActivatingOkBtnBeginTime = 0;
	m_bWaitActivatingOkBtn = false;
}

ZSellCashItemConfirmDlg::~ZSellCashItemConfirmDlg()
{

}

void ZSellCashItemConfirmDlg::Open(const char* szItemName, CCBitmap* pIcon, int price, int count, ISellCashItemConfirmDlgDoneHandler* pHandler)
{
	CCPicture* pPicture = (CCPicture*)GetIDLResource()->FindWidget("SellCashItemConfirmFrame_Thumbnail");
	if (pPicture)
		pPicture->SetBitmap(pIcon);

	CCLabel* pLabel = (CCLabel*)GetIDLResource()->FindWidget("SellCashItemConfirmFrame_ItemName");
	if (pLabel)
		pLabel->SetText(szItemName);

	char szPrice[256];
	sprintf(szPrice, "%d %s", price, ZMsg(MSG_CHARINFO_BOUNTY));
	pLabel = (CCLabel*)GetIDLResource()->FindWidget("SellCashItemConfirmFrame_Bounty");
	if (pLabel)
		pLabel->SetText(szPrice);

	CCTextArea* pTextArea = (CCTextArea*)GetIDLResource()->FindWidget("SellCashItemConfirmFrame_Warning");
	if (pTextArea)
		pTextArea->SetText( ZMsg(MSG_SHOPEQUIP_SELL_CASHITEM_CONFIRM));

	CCFrame* pFrame = (CCFrame*)GetIDLResource()->FindWidget("SellCashItemConfirmFrame");
	if (pFrame)
	{
		pFrame->Show(true, true);
	}

	m_pDoneHandler = pHandler;

	// 유저가 지금 팔고자 하는 캐쉬아이템을 확실히 인지할수 있는 시간을 주기 위해 OK 버튼을 몇초후 활성화시킨다
	CCButton* pButton = (CCButton*)GetIDLResource()->FindWidget("SellCashItemConfirmFrame_Sell");
	if (pButton)
	{
		pButton->Enable(false);
		m_bWaitActivatingOkBtn = true;
		m_nWaitActivatingOkBtnBeginTime = timeGetTime();
	}
}

void ZSellCashItemConfirmDlg::Update()
{
	if (!m_bWaitActivatingOkBtn) return;

	DWORD currTime = timeGetTime();
	if (currTime - m_nWaitActivatingOkBtnBeginTime > 3000)
	{
		m_bWaitActivatingOkBtn = false;

		CCButton* pButton = (CCButton*)GetIDLResource()->FindWidget("SellCashItemConfirmFrame_Sell");
		if (pButton)
			pButton->Enable(true);
	}
}

void ZSellCashItemConfirmDlg::Close()
{
	CCWidget* pWidget = GetIDLResource()->FindWidget( "SellCashItemConfirmFrame");
	if ( pWidget) pWidget->Show( false);
}

void ZSellCashItemConfirmDlg::OnOkButton()
{
	Close();

	if (m_pDoneHandler)
		m_pDoneHandler->OnConfirmSellCashItem(true);
}




void ZCashItemConfirmDlg::Open(CCBitmap* pItemIcon, ICashItemConfirmDlgDoneHandler* pHandler)
{/*
	CCPicture* pPicture = (CCPicture*)GetIDLResource()->FindWidget("BuyItemDetailFrame_Thumbnail");
	if (pPicture)
		pPicture->SetBitmap(pItemIcon);

	if (CCMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc( pListItem->GetItemID() ))
	{
		ZMyItemNode* pItemNode = ZGetMyInfo()->GetItemList()->GetItem( pListItem->GetUID() );
		SetupItemDescription(pItemDesc, "BuyItemDetailFrame_Desc", pItemNode, true);
	}

	//todok 캐쉬 보유량, 남을 캐쉬량 텍스트 세팅
	int myCash = ZGetMyInfo()->GetCash();
	char sz[256];
	CCLabel* pLabel = (CCLabel*)GetIDLResource()->FindWidget("BuyItemDetailFrame_MyCash");
	if (pLabel) {
		sprintf(sz, "%d", myCash);
		pLabel->SetText(sz);
	}
	
	pLabel = (CCLabel*)GetIDLResource()->FindWidget("BuyItemDetailFrame_RequiredCash");
	if (pLabel) {
		pLabel->SetText("500(test)");	//todok 임시코드
	}

	pLabel = (CCLabel*)GetIDLResource()->FindWidget("BuyItemDetailFrame_RemainCash");
	if (pLabel) {
		sprintf(sz, "%d", myCash - 500);	//todok 임시코드
		pLabel->SetText(sz);
	}

	// 콤보박스에 내용 넣기 (기간 혹은 묶음 수량)
	CCComboBox* pCombobox = (CCComboBox*)GetIDLResource()->FindWidget("BuyItemDetailFrame_DetailCombo");
	if (pCombobox)
	{
		pCombobox->RemoveAll();
		
		//todok 임시 코드
		int testDay[3] = { 7, 14, 30 };
		int testCash[3] = { 3000, 5000, 10000 };

		for (int i=0; i<3; ++i)
		{
			sprintf(sz, "%d일 : %d 캐쉬", testDay[i], testCash[i]);
			pCombobox->Add(sz);
		}
		pCombobox->SetSelIndex(0);
	}

	CCFrame* pFrame = (CCFrame*)GetIDLResource()->FindWidget("BuyItemDetailFrame");
	if (pFrame)
	{
		pFrame->Show(true, true);
	}
	*/
}