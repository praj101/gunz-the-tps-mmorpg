#include "stdafx.h"

#include "ZCharacterView.h"
#include "RealSpace2.h"
#include "CCMatchObject.h"
#include "ZApplication.h"
#include "ZMyInfo.h"
#include "ZGameClient.h"

// Added
#include "ZGameInterface.h"

extern bool Enable_Cloth;
static rvector force = rvector( 15, 0, 0 );
static float maxForce = 10;


char* szItemNames[MMCIP_END] = 
{
		"머리",
		"가슴",
		"손",
		"다리",
		"발",
		"왼쪽손가락",
		"오른쪽손가락",
		"근접무기",
		"주무기",
		"보조무기",
		"아이템1",
		"아이템2",
};

unsigned long int GetVisualWeaponID(unsigned long int nMeleeItemID, unsigned long int nPrimaryItemID,
									unsigned long int nSecondaryItemID, unsigned long int nCustom1ItemID,
									unsigned long int nCustom2ItemID)
{
	if (nPrimaryItemID != 0) return nPrimaryItemID;
	else if (nSecondaryItemID != 0) return nSecondaryItemID;
	else if (nMeleeItemID != 0) return nMeleeItemID;
	else if (nCustom1ItemID != 0) return nCustom1ItemID;
	else if (nCustom2ItemID != 0) return nCustom2ItemID;

	return 0;
}


void ZCharacterView::RepositionItemSlots()
{
	sRect r = GetClientRect();

#define ARMORLIST_TOP_SPACE	10
#define ARMORLIST_BOTTOM_SPACE	50
#define LEFTITEM_COUNT	5
	int nItemHeight = (r.h-ARMORLIST_TOP_SPACE-ARMORLIST_BOTTOM_SPACE)/LEFTITEM_COUNT;
	int nItemWidth = nItemHeight;

#define ITEM_SPACE	6
	for(int i=0; i<LEFTITEM_COUNT; i++){
		sRect ir(r.x+r.w-nItemWidth-ARMORLIST_TOP_SPACE, r.y+10+nItemHeight*i, nItemWidth, nItemHeight);
		ir.w-=ITEM_SPACE;
		ir.h-=ITEM_SPACE;
		m_ItemSlots[i].m_Rect = ir;
	}

	for(int i=0; i<MMCIP_END-LEFTITEM_COUNT; i++){
		sRect ir(r.x+nItemWidth*(i<3?i:i-3)+ARMORLIST_TOP_SPACE, r.y+r.h-nItemHeight*(i<3?2:1), nItemWidth, nItemHeight);
		ir.w-=ITEM_SPACE;
		ir.h-=ITEM_SPACE;
		m_ItemSlots[i+LEFTITEM_COUNT].m_Rect = ir;
	}
}

void ZCharacterView::OnSize(int w, int h)
{
	RepositionItemSlots();
}

void ZCharacterView::OnDraw(CCDrawContext* pDC)
{
	DWORD dwCurTime = timeGetTime();
	if ( !m_bLButtonDown && m_bAutoRotate && m_bEnableRotate)
		RotateRight( (float)( dwCurTime - m_dwTime) / 20.0f);

	m_dwTime = dwCurTime;


	if( !Enable_Cloth )
	{
		m_pTVisualMesh.m_pVisualMesh->DestroyCloth();
	}
	else if( m_pTVisualMesh.m_pVisualMesh->isChestClothMesh() )
	{
		srand( timeGetTime());
		int rint = rand() % 10;
		force.x += rint - 7;
		force.x = min(max( force.x, 5 ), maxForce * 0.3 );
		srand( timeGetTime());
		rint = rand() % (int)(maxForce*0.3);
		force.y += rint - 4;	
		force.y = min(max( force.y, 0 ), maxForce );
 		force.z = -90;

		m_pTVisualMesh.m_pVisualMesh->SetClothForce( force );

//		if( m_pTVisualMesh.m_pVisualMesh->GetClothMeshNodeRender() )
//			m_pTVisualMesh.m_pVisualMesh->SetClothMeshNodeRender(false);
	}

	//bool bGame = ZGetGame() ? true:false;	//< 어차피 이건 UI니까 게임중이건 아니건 bGame=false로 계산
	bool bGame = false;
	m_pTVisualMesh.m_pVisualMesh->SetClothValue(bGame,0.f); // cloth

	ZMeshView::OnDraw(pDC);

	if( m_pTVisualMesh.m_pVisualMesh->isChestClothMesh() )
	{
//		m_pTVisualMesh.m_pVisualMesh->UpdateCloth();
//		m_pTVisualMesh.m_pVisualMesh->RenderCloth();
	}

	if (GetDrawInfo() == false) 
		return;

	sRect r; 
	/*= GetClientRect();

	if (ZApplication::GetGameInterface()->GetState() == GUNZ_STAGE)
	{
		if (ZGetGameClient()->GetMatchStageSetting()->IsTeamPlay())
		{
			sRect tr(66, 12, 16, 16);
			if(m_Info.nTeam== CCMT_RED){		// Rea Team
				pDC->SetColor(255, 0, 0);
				pDC->FillRectangle(tr);
			}
			else if(m_Info.nTeam==CCMT_BLUE){	// Blue Team
				pDC->SetColor(0, 0, 255);
				pDC->FillRectangle(tr);
			}
		}
		
		if (m_Info.bMaster == true) 
		{
			pDC->SetColor(171, 245, 167);
			pDC->Text(r, "Master", CCAM_VCENTER|CCD_HCENTER);
		}
		else 
		{
			switch (m_Info.nStageState)
			{
			case MOSS_NONREADY:
				{

				}
				break;
			case MOSS_READY:
				{
					pDC->SetColor(255, 255, 255);
					pDC->Text(r, "Ready", CCAM_VCENTER|CCD_HCENTER);
				}
				break;
			case MOSS_SHOP:
				{
					pDC->SetColor(252, 231, 122);
					pDC->Text(r, "Shop", CCAM_VCENTER|CCD_HCENTER);
				}
				break;
			case MOSS_EQUIPMENT:
				{
					pDC->SetColor(252, 231, 122);
					pDC->Text(r, "Equipment", CCAM_VCENTER|CCD_HCENTER);
				}
				break;
			}
		}
	}

#define TEXTAREA_HEIGHT	14	// 텍스트가 위치할 아랫쪽 크기

	// 이름을 쓴다.
	r.y = r.h-TEXTAREA_HEIGHT;
	r.h = TEXTAREA_HEIGHT;
	pDC->SetColor(255, 255, 255);
	pDC->Text(r, GetText(), CCD_HCENTER);

	// 레벨을 쓴다.
	r.y -= TEXTAREA_HEIGHT;
	char szLevel[16];
	sprintf(szLevel, "Lv.%d", m_Info.nLevel);
	pDC->SetColor(255, 125, 125);
	pDC->Text(r, szLevel, CCD_HCENTER);

	if(m_Info.bFireWall==true){
		CCBitmap* pBitmap = CCBitmapManager::Get("firewall.png");
		pDC->SetBitmap(pBitmap);
		pDC->Draw(0, 0);
	}
	if(m_Info.bNAT==true){
		CCBitmap* pBitmap = CCBitmapManager::Get("nat.png");
		pDC->SetBitmap(pBitmap);
		pDC->Draw(16, 0);
	}
//*/
/* 	r = GetInitialClientRect();
	pDC->SetColor( 61, 61, 61 );
  	pDC->FillRectangle(r.x, r.y+r.h-22, r.w, 22 );

	pDC->SetColor( 128, 128, 128 );
	pDC->Rectangle(r);
	
	pDC->SetColor( 255,255,255);
 	pDC->Text( sRect( r.x+27, r.y + r.h-22, r.w, 22 ), GetText(), CCAM_VCENTER );

	CCBitmap* pBitmap = CCBitmapManager::Get("level_marker.png");
	if(pBitmap != NULL ) pDC->SetBitmap( pBitmap );
	pDC->Draw( r.x + r.w - 40, r.y + r.h - 20, 16, 16 );

	pDC->SetColor( 255, 255, 255 );
	char szLevel[16];
	sprintf( szLevel, "%2d", m_Info.nLevel );
 	pDC->Text( sRect(r.x + r.w - 30, r.y + r.h - 22, 30, 22 ), szLevel );
*/	// 동환이가 막음
}

bool ZCharacterView::IsDropable(CCWidget* pSender)
{
	return false;
}

bool ZCharacterView::OnDrop(CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString)
{
	return false;
}

ZCharacterView::ZCharacterView(const char* szName, CCWidget* pParent, CCListener* pListener)
: ZMeshView(szName, pParent, pListener)
{
	// ZOrder변경되는 문제로 툴팁은 우선 보여주지 않는다.
//	AttachToolTip(szName);
//	m_pTVisualMesh.SetVisualMesh();

	SetDrawInfo(false);

	m_fDist = 235.0f;
	m_fMaxDist = 400.0f;
	m_fMinDist = 120.0f;

	m_Eye = rvector(0.0f, 92, -m_fDist);
	m_At = rvector(0, 92, 0);
	m_Up = rvector(0, 1, 0);

	memset(&m_Info, 0, sizeof(ZCharacterViewInfo));

	m_bVisibleEquipment = false;
	m_nVisualWeaponParts = MMCIP_PRIMARY;


	m_bAutoRotate = false;
	m_dwTime = timeGetTime();


	for(int i=0; i<MMCIP_END; i++)
	{
		m_ItemSlots[i].m_nItemID = 0;
	}

	RepositionItemSlots();

	//jintriple3 메모리 프록시..
	m_Info.m_pMnTeam = new CCProtectValue<int>;
	m_Info.m_pMnTeam->Set_MakeCrc(0);
}

ZCharacterView::~ZCharacterView()
{
	delete m_Info.m_pMnTeam;
}

void ZCharacterView::InitCharParts(ZCharacterView* pCharView, CCMatchCharItemParts nVisualWeaponParts)
{
	if(pCharView) {

		unsigned long int itemids[MMCIP_END];
		for (int i = 0; i < MMCIP_END; i++) itemids[i] = pCharView->m_ItemSlots[i].m_nItemID;

		InitCharParts(pCharView->m_Info.nSex, pCharView->m_Info.nHair, pCharView->m_Info.nFace,
						itemids, nVisualWeaponParts);
	}
}

void ZCharacterView::InitCharParts(CCMatchSex nSex, unsigned int nHair, unsigned int nFace, 
				   const unsigned long int* nEquipItemIDs, CCMatchCharItemParts nVisualWeaponParts)
{
	RMesh* pMesh = NULL;
	if (nSex == MMS_MALE) {
		pMesh = ZGetMeshMgr()->Get("heroman1");
	}
	else {
		pMesh = ZGetMeshMgr()->Get("herowoman1");
	}

	if (m_pTVisualMesh.GetVMesh(false)) {
		m_pTVisualMesh.Create();
		m_pTVisualMesh.GetVMesh()->Create(pMesh);
	}

	// 정보 저장
	m_Info.nSex = nSex;
	m_Info.nHair = nHair;
	m_Info.nFace = nFace;

	for (int i = 0; i < MMCIP_END; i++)
	{
		m_ItemSlots[i].m_nItemID = nEquipItemIDs[i];
	}

	unsigned long int nWeaponID = 0;

	if (nVisualWeaponParts == MMCIP_PRIMARY) {
		nWeaponID= GetVisualWeaponID(nEquipItemIDs[MMCIP_MELEE], nEquipItemIDs[MMCIP_PRIMARY]
		, nEquipItemIDs[MMCIP_SECONDARY], nEquipItemIDs[MMCIP_CUSTOM1], nEquipItemIDs[MMCIP_CUSTOM2]);
	}
	else {
		nWeaponID = nEquipItemIDs[nVisualWeaponParts];
	}

	if (m_pTVisualMesh.GetVMesh())
	{
		ZChangeCharParts( m_pTVisualMesh.GetVMesh() , nSex, nHair, nFace, nEquipItemIDs );
		ZChangeCharWeaponMesh( m_pTVisualMesh.GetVMesh(), nWeaponID);

		//들고 있는 무기에 따라서 연결..
		m_pTVisualMesh.GetVMesh()->SetAnimation("login_idle");
		m_pTVisualMesh.GetVMesh()->SetCheckViewFrustum(false);
	}

	if( Enable_Cloth )
	{
		m_pTVisualMesh.m_pVisualMesh->ChangeChestCloth(0.9f,1);
	}
}


void ZCharacterView::SetParts(CCMatchCharItemParts nParts, unsigned int nItemID)
{
	CCMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pItemDesc == NULL) return;
	if (IsSuitableItemSlot(pItemDesc->m_nSlot, nParts) == false) return;

	// 성별 제한 조건만 체크한다.
	if (pItemDesc->m_nResSex.Ref() != -1)
	{
		if (pItemDesc->m_nResSex.Ref() != int(ZGetMyInfo()->GetSex())) return;
	}


	m_ItemSlots[nParts].m_nItemID = nItemID;

	if (nParts == MMCIP_PRIMARY)
	{
		ZChangeCharWeaponMesh(m_pTVisualMesh.GetVMesh(), m_ItemSlots[MMCIP_PRIMARY].m_nItemID);
	}
	else
	{
		unsigned long int itemids[MMCIP_END];
		for (int i = 0; i < MMCIP_END; i++) itemids[i] = m_ItemSlots[i].m_nItemID;
		ZChangeCharParts(m_pTVisualMesh.GetVMesh(), m_Info.nSex, m_Info.nHair, m_Info.nFace, itemids);
	}

	if( Enable_Cloth )
		m_pTVisualMesh.m_pVisualMesh->ChangeChestCloth(0.9f,1);
}

void ZCharacterView::ChangeVisualWeaponParts(CCMatchCharItemParts nVisualWeaponParts)
{
	if (m_pTVisualMesh.GetVMesh() == NULL) return;

	unsigned long int itemids[MMCIP_END];
	for (int i = 0; i < MMCIP_END; i++) itemids[i] = m_ItemSlots[i].m_nItemID;

	unsigned long int nWeaponID;

	if (nVisualWeaponParts == MMCIP_PRIMARY)
	{
		nWeaponID= GetVisualWeaponID(itemids[MMCIP_MELEE],
			itemids[MMCIP_PRIMARY], itemids[MMCIP_SECONDARY],
			itemids[MMCIP_CUSTOM1], itemids[MMCIP_CUSTOM2]);
	}
	else
	{
		nWeaponID = itemids[nVisualWeaponParts];
	}

	ZChangeCharWeaponMesh(m_pTVisualMesh.GetVMesh(), nWeaponID);

	//들고 있는 무기에 따라서 연결..

	m_pTVisualMesh.GetVMesh()->SetAnimation("login_idle");
	m_pTVisualMesh.GetVMesh()->SetCheckViewFrustum(false);

	m_nVisualWeaponParts = nVisualWeaponParts;
}

void ZCharacterView::SetSelectMyCharacter()
{
	ZMyInfo* pmi = ZGetMyInfo();
	ZMyItemList* pil = ZGetMyInfo()->GetItemList();

	unsigned long int nWeaponID = GetVisualWeaponID(pil->GetEquipedItemID(MMCIP_MELEE),
		pil->GetEquipedItemID(MMCIP_PRIMARY), pil->GetEquipedItemID(MMCIP_SECONDARY),
		pil->GetEquipedItemID(MMCIP_CUSTOM1), pil->GetEquipedItemID(MMCIP_CUSTOM2));

	unsigned long int nItemids[MMCIP_END];

	if(pmi) 
	{
		for (int i = 0; i < MMCIP_END; i++)
		{
			nItemids[i] = pil->GetEquipedItemID(CCMatchCharItemParts(i));
		}

		InitCharParts(pmi->GetSex(), pmi->GetHair(), pmi->GetFace(), nItemids);
	}
}

void ZCharacterView::OnInvalidate()
{
	if(m_pTVisualMesh.m_pVisualMesh) 
		m_pTVisualMesh.m_pVisualMesh->OnInvalidate();
}
void ZCharacterView::OnRestore()
{
	if(m_pTVisualMesh.m_pVisualMesh) 
		m_pTVisualMesh.m_pVisualMesh->OnRestore();
}

bool GetUserInfoUID(CCUID uid,sColor& _color,char* sp_name,CCMatchUserGradeID& gid);

void ZCharacterView::SetCharacter( CCUID uid  )
{
	m_Info.UID = uid;
	
	CCMatchObjCacheMap* pObjCacheMap = ZGetGameClient()->GetObjCacheMap();
	for(CCMatchObjCacheMap::iterator itor = pObjCacheMap->begin(); itor != pObjCacheMap->end(); ++itor)
	{
		CCMatchObjCache* pObj = (*itor).second;
 		if( pObj->GetUID() == m_Info.UID )
		{
			InitCharParts( pObj->GetCostume()->nSex, pObj->GetCostume()->nHair, pObj->GetCostume()->nFace, 
				pObj->GetCostume()->nEquipedItemID );

			sColor _color;
			char sp_name[255];
			CCMatchUserGradeID gid;

			if(GetUserInfoUID(m_Info.UID,_color,sp_name,gid)) {//특별한 유저인 경우
				SetText( sp_name );
				m_Info.nLevel = 0;//pObj->GetLevel();
			}
			else {
				SetText( pObj->GetName() );
				m_Info.nLevel = pObj->GetLevel();
			}

			SetDrawInfo( true );
		}
	}	
}