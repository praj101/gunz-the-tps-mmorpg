#ifndef _SACRIFICE_QUEST_ITEM_TABLE
#define _SACRIFICE_QUEST_ITEM_TABLE


#include "CCQuestConst.h"

class CCQuestSacrificeSlot;


#define SACRIFICE_TABLE_XML "SacrificeTable.xml"

/* CCQuestConst.h로 이동.
#define MSQITRES_NOR  1	// 특별시나리오에 해당하는 희생아이템만 없고, 일반시나리오에 대한 희생 아이템만 있는 상황.
#define MSQITRES_SPC  2	// 읿나 시나리오 아이템과 특별시나리오에 해당하는 희생아이템이 있음.
#define MSQITRES_INV  3	// 해당 QL에대한 희생아이템 정보 테이블이 없음. 이경우는 맞지 않는 희생 아이템이 올려져 있을경우.
#define MSQITRES_DUP  4 // 양쪽 슬롯에 특별 시나리오용 희생 아이템이 올려져 있음.
#define MSQITRES_EMP  5 // 양쪽 슬롯이 모두 비어 있음. 이 상태는 QL값을1로 해줘야 함.
#define MSQITRES_ERR -1	// 에러... 테이블에서 해당 QL을 찾을수 없음. QL = 0 or QL값이 현제 구성된 MAX QL보다 클경우.
*/


#define CCSQITC_ITEM		"ITEM"
#define CCSQITC_MAP		"map"
#define CCSQITC_QL		"ql"
#define CCSQITC_DIID		"default_item_id"
#define CCSQITC_SIID1	"special_item_id1"
#define CCSQITC_SIID2	"special_item_id2"
#define CCSQITC_SIGNPC	"significant_npc"
#define CCSQITC_SDC		"sdc"
#define CCSQITC_SID		"ScenarioID"



class CCSacrificeQItemInfo
{
	friend class CCSacrificeQItemTable;

public :
	unsigned long	GetDefQItemID()		{ return m_nDefaultQItemID; }
	unsigned long	GetSpeQItemID1()	{ return m_nSpecialQItemID1; }
	unsigned long	GetSpeQItemID2()	{ return m_nSpecialQItemID2; }
	const char*		GetMap()			{ return m_szMap; }
	int				GetSigNPCID()		{ return m_nSignificantNPCID; }
	float			GetSDC()			{ return m_fSDC; }
	int				GetQL()				{ return m_nQL; }
	int				GetScenarioID()		{ return m_nScenarioID; }
	
private :
	CCSacrificeQItemInfo() : m_nDefaultQItemID( 0 ), m_nSpecialQItemID1( 0 ), m_nSpecialQItemID2( 0 ), m_nSignificantNPCID( 0 ), m_fSDC( 0.0f ) {}
	CCSacrificeQItemInfo( unsigned long nDfQItemID, unsigned long nSpecIID1, unsigned long nSpecIID2, const int nSigNPCID, const float fSdc ) :
		m_nDefaultQItemID( nDfQItemID ), m_nSpecialQItemID1( nSpecIID1 ), m_nSpecialQItemID2( nSpecIID2 ), 
		m_nSignificantNPCID( nSigNPCID ), m_fSDC( fSdc ) {}

	unsigned long	m_nDefaultQItemID;
	unsigned long	m_nSpecialQItemID1;
	unsigned long	m_nSpecialQItemID2;
	char			m_szMap[ 24 ];
	int				m_nSignificantNPCID;
	float			m_fSDC;
	int				m_nQL;
	int				m_nScenarioID;
};


// 각레벨에 필요한 희생 아이템 테이블.
class CCSacrificeQItemTable : private multimap< int, CCSacrificeQItemInfo >
{
public :
	CCSacrificeQItemTable() {}
	~CCSacrificeQItemTable() {}

	static CCSacrificeQItemTable& GetInst()
	{
		static CCSacrificeQItemTable SacrificeQItemTable;
		return SacrificeQItemTable;
	}

	int	 FindSacriQItemInfo( const int nQL, CCQuestSacrificeSlot* pSacrificeSlot, int& outResultQL );
	bool ReadXML( const char* pszFileName );
	bool ReadXml( CCZFileSystem* pFileSystem, const char* szFileName );

	CCSacrificeQItemInfo* GetResultTable() { return m_pResultTable; }

	bool TestInitTable();

private :
	void ParseTable( ::CCXmlElement& element );

	CCSacrificeQItemInfo* m_pResultTable;	// CheckInvalidSlot에의해 해당하는 테이블이 검색되면 여기에 셋팅됨.
};


inline CCSacrificeQItemTable* GetSacriQItemTable()
{
	return &(CCSacrificeQItemTable::GetInst());
}



#endif ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////