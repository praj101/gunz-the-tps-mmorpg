#ifndef ZSERVERVIEW
#define ZSERVERVIEW

#include "ZPrerequisites.h"
#include "CCWidget.h"
#include <list>

using namespace RealSpace2;



// ServerInfo
struct ServerInfo
{
	char	szName[ 32];
	char	szAddress[ 32];
	int		nPort;
	int		nType;
	int		nNumOfUser;
	int		nCapacity;
	bool	bIsLive;
	int		nRow;
	int		nCol;
	sRect	rRect;		// 서버리스트 선택을 화면 픽셀 클릭으로 변경
	char	szAgentIP[ 32];
};
typedef list<ServerInfo*>	SERVERLIST;

#define AGENTMAXCOUNT	20

// Class ZServerView
class ZServerView : public CCWidget
{
protected:
	SERVERLIST		m_cServerList;
	int				m_nSelectNum;
	int				m_nTextOffset;
	int				m_nSelBoxSizeX;
	int				m_nSelBoxSizeY;
	int				m_nSelBoxSizeW;

	CCBitmap*		m_pPingBitmap[6];
	DWORD			m_dAgentPing[AGENTMAXCOUNT];

protected:
	virtual void OnDraw( CCDrawContext* pDC);
	virtual bool OnEvent( CCEvent* pEvent, CCListener* pListener);

	void ServerListDraw(CCDrawContext* pDC, sRect rectBox, ServerInfo* pServerInfo, bool bSelected);
	void IconDraw(CCDrawContext* pDC, sRect rectBox, int nType, bool bSelected);
	void PingImageDraw(CCDrawContext* pDC, sRect rectBox, int nImageCount, bool bIsLive);

public:
	ZServerView( const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZServerView(void);


	void ClearServerList( void);
	bool AddServer( char* szName, char* szAddress, int nPort, int nType, int nNumOfUser, int nCapacity, bool IsLive, char* szAgentIP="", int nRow=0, int nCol=0);
	void SetAgentPing(DWORD nIP, DWORD dTimeStamp);

	ServerInfo* GetSelectedServer();
	int GetCurrSel();
	int GetCurrSel2()						{ return m_nSelectNum; }
	void SetCurrSel( int nNumber);
	bool IsSelected()						{ return ( (GetCurrSel() > -1) ? true : false); }
	void SetTextOffset( int nOffset)		{ m_nTextOffset = nOffset; }

	void SetSelBoxSize( int x, int y)		{ m_nSelBoxSizeX = x;  m_nSelBoxSizeY = y; }
	void SetSelBoxWidth( int width)			{ m_nSelBoxSizeW = width; }

	SERVERLIST GetServerList()				{ return m_cServerList; }

#define MINT_SERVERVIEW	"ServerView"
	virtual const char* GetClassName(void){ return MINT_SERVERVIEW; }

};

#endif
