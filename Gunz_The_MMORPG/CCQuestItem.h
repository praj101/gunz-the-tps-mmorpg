#ifndef _CCQUEST_ITEM_H
#define _CCQUEST_ITEM_H

#include "CCUID.h"
#include <map>
using std::map;

#include <mmsystem.h>

#include "CCDebug.h"
#include "CCQuestConst.h"
#include "CCZFileSystem.h"
#include "CCXml.h"
#include "CCSync.h"
#include "CCMatchGlobal.h"


class CCMatchObject;


#define QUEST_ITEM_FILE_NAME	"zquestitem.xml"

#define MQICTOK_ITEM		"ITEM"
#define MQICTOK_ID			"id"
#define MQICTOK_NAME		"name"
#define MQICTOK_TYPE		"type"
#define MQICTOK_DESC		"desc"
#define MQICTOK_UNIQUE		"unique"
#define MQICTOK_PRICE		"price"
#define MQICTOK_SECRIFICE	"secrifice"
#define MQICTOK_PARAM		"param"


class CCQuestMonsterBible
{
public :
	CCQuestMonsterBible() {}
	~CCQuestMonsterBible() {}

	void WriteMonsterInfo( int nMonsterBibleIndex );
	bool IsKnownMonster( const int nMonsterBibleIndex );
	
	inline const char MakeBit( const int nMonsterBibleIndex )
	{
		return 0x01 << (nMonsterBibleIndex % 8);
	}

	inline int operator [] (int nIndex ) const
	{
		return m_szData[ nIndex ];
	}

	void Clear() { memset( m_szData, 0, MAX_DB_MONSTERBIBLE_SIZE ); }
	const bool Copy( const char* pszData, const int nSize );
	const char* GetData() { return m_szData; }

private :
	char m_szData[ MAX_DB_MONSTERBIBLE_SIZE ];
};

#define MONSTER_BIBLE_SIZE sizeof(CCQuestMonsterBible)


struct CCQuestItemDesc
{
	unsigned long int	m_nItemID;
	char				m_szQuestItemName[ 32 ];
	int					m_nLevel;
	CCQuestItemType		m_nType;
	int					m_nPrice;
	bool				m_bUnique;
	bool				m_bSecrifice;
	char				m_szDesc[ 8192 ];
	int					m_nLifeTime;
	int					m_nParam;

	int GetSellBountyValue(int nCnt) { return int(m_nPrice * 0.25) * nCnt; }
};

class CCQuestItemDescManager : public map< int, CCQuestItemDesc* >
{
public :
	CCQuestItemDescManager();
	~CCQuestItemDescManager();

	static CCQuestItemDescManager& GetInst()
	{
		static CCQuestItemDescManager QuestItemDescManager;
		return QuestItemDescManager;
	}

	bool ReadXml( const char* szFileName );
	bool ReadXml( CCZFileSystem* pFileSystem, const char* szFileName );
	void ParseQuestItem( CCXmlElement& element );
	void Clear();

	CCQuestItemDesc* FindQItemDesc( const int nItemID );
	CCQuestItemDesc* FindMonserBibleDesc( const int nMonsterBibleIndex );

	inline bool IsQItemID( const int nQItemID )
	{
		if( (MINID_QITEM_LIMIT > nQItemID) || (MAXID_QITEM_LIMIT < nQItemID) )
			return false;
		return true;
	}

	inline bool IsMonsterBibleID( const int nQItemID )
	{
		if( (MINID_MONBIBLE_LIMIT > nQItemID) || (MAXID_MONBIBLE_LIMIT < nQItemID) )
			return false;
		return true;
	}
private :
	map< int, CCQuestItemDesc* >	m_MonsterBibleMgr;
};

#define GetQuestItemDescMgr() CCQuestItemDescManager::GetInst()

struct SimpleQuestItem
{
	unsigned long int	m_nItemID;
	unsigned int		m_nCount;
};

// client와 server와의 공통된 부분.
class CCBaseQuestItem
{
public: 
	CCBaseQuestItem() {}
	virtual ~CCBaseQuestItem() {}
};

// server에 특화된 부분.
// 퀘스트 아이템은 획득한 적이 있을 경우는 기본값 1을 Count에 설정을 함.
//   그렇게이 카운트가 0이 아닌 1부터 시작을 하기에 실질적인 수량보다 1이 많음.
//   현재 수량을 요구할시는 저장하고 있는 수량에 -1을 해서 반환을 해줘야 함.
class CCQuestItem : public CCBaseQuestItem
{
public:
	CCQuestItem() : CCBaseQuestItem(),  m_nCount( 0 ), m_pDesc( 0 ), m_bKnown( false )
	{
	}

	virtual ~CCQuestItem() 
	{
	}

	bool Create( const unsigned long int nItemID, const int nCount, CCQuestItemDesc* pDesc, bool bKnown=true );
	int Increase( const int nCount = 1 );
	int Decrease( const int nCount = 1 );

	unsigned long int	GetItemID()	{ return m_nItemID; }
	int GetCount()	{ return m_nCount; }
	bool IsKnown()	{ return m_bKnown; }	// 한번이라도 획득했었는지 여부
	CCQuestItemDesc* GetDesc();
	void SetDesc( CCQuestItemDesc* pDesc ) { m_pDesc = pDesc; }
	void SetItemID( unsigned long int nItemID )	{ m_nItemID = nItemID; }
	
// private:
	bool SetCount( int nCount, bool bKnown = true );
private :
	unsigned long int	m_nItemID;
	CCQuestItemDesc*		m_pDesc;
	int					m_nCount;			// 같은 종류의 아이템은 새로 생성하지 않고 수를 늘림.
	bool				m_bKnown;
};


// 게임중에 퀘스트 아이템을 등록하고 있는 클래스.
// 맵에 등록된 퀘스트 아이템은 적어도 한번은 획득한적이 있었던 아이템임.
// 개수가 1일경우는 획득한 적이 있던 아이템이지만 현재 가지고 있는 수량이 0이라는 뜻.
class CCQuestItemMap : public map< unsigned long int, CCQuestItem* >
{
public :
	CCQuestItemMap() : m_bDoneDbAccess( false )
	{
	}

	~CCQuestItemMap() {}


	void SetDBAccess( const bool bState )	{ m_bDoneDbAccess = bState; }
	bool IsDoneDbAccess()					{ return m_bDoneDbAccess; }

	virtual bool	CreateQuestItem( const unsigned long int nItemID, const int nCount, bool bKnown=true );
	void			Clear();
	void			Remove( const unsigned long int nItemID );
	CCQuestItem*		Find( const unsigned long int nItemID );
	void			Insert( unsigned long int nItemID, CCQuestItem* pQuestItem );
	
/*
	static CCUID UseUID()
	{
		m_csUIDGenerateLock.Lock();
			m_uidGenerate.Increase();	
		m_csUIDGenerateLock.Unlock();
		return m_uidGenerate;
	}
*/
private :
	// static CCUID				m_uidGenerate;
	// static CCCriticalSection	m_csUIDGenerateLock;
	bool					m_bDoneDbAccess;		// 디비에서 정보를 가져왔었는지 여부
};


// 유니크 아이템 획득시, 일정 시간 경과, 일정 횟수 이상 플레이.
// 유니크 아이템 획득은 바로 DB를 업데이 시킴.
class DBQuestCachingData
{
public :
	DBQuestCachingData() : m_dwLastUpdateTime( timeGetTime() ), m_nPlayCount( 0 ), m_bEnableUpdate( false ), m_nShopTradeCount( 0 ),
		m_pObject( 0 ), m_nRewardCount( 0 )
	{
	}

	~DBQuestCachingData() 
	{
	}

	bool IsRequestUpdate()
	{
		if( (MAX_PLAY_COUNT < m_nPlayCount) || (MAX_ELAPSE_TIME < GetUpdaetElapse()) ||
			(MAX_SHOP_TRADE_COUNT < m_nShopTradeCount) || (MAX_REWARD_COUNT < m_nRewardCount) ||
			m_bEnableUpdate )
			return m_bEnableUpdate = true;

		return m_bEnableUpdate = false;
	}

	bool IsRequestUpdateWhenLogout()
	{
		return ( (0 < (m_nShopTradeCount + m_nRewardCount)) || m_bEnableUpdate );
	}

	void IncreasePlayCount( const int nCount = 1 );
	void IncreaseShopTradeCount( const int nCount = 1 );
	void IncreaseRewardCount( const int nCount = 1 );
	bool CheckUniqueItem( CCQuestItem* pQuestItem );
	void SacrificeQuestItem() { m_bEnableUpdate = true; }
	void Reset();
	
	DWORD GetUpdaetElapse() 
	{
#ifdef _DEBUG
		char szTemp[ 100 ] = {0};
		DWORD t = timeGetTime();
		int a = t - m_dwLastUpdateTime;
		sprintf( szTemp, "Update Elapse %d %d\n", timeGetTime() - m_dwLastUpdateTime, a );
		cclog( szTemp );
#endif
		return timeGetTime() - m_dwLastUpdateTime; 
	}

	void SetEnableUpdateState( const bool bState )	{ m_bEnableUpdate = bState; }
	void SetCharObject( CCMatchObject* pObject )		{ m_pObject = pObject; }
	
	bool DoUpdateDBCharQuestItemInfo();

private :
	CCMatchObject*	m_pObject;				// DB업데이트때 데이터를 가져오기 위해서 저장해 놓은 포인터.
	DWORD			m_dwLastUpdateTime;		// 업데이트가 적용되면 같이 갱신됨. 
	int				m_nPlayCount;			// 게임횟수는 게임에 들어가서 하는 모든 행동에 관계없이 완료를 해야 1번 적용됨. 
	int				m_nShopTradeCount;		// 상점에서의 퀘스트 아이템 거래 횟수.
	bool			m_bEnableUpdate;		// 현재 상태. 업데이트가 가능하면 true임.
	int				m_nRewardCount;			// 현제까지 보상 받은 횟수.
};


inline bool IsQuestItemID(unsigned int nItemID)
{
	if ((MIN_QUESTITEM_ID <= nItemID) && (nItemID <= MAX_QUESTITEM_ID)) return true;
	return false;
}

#endif