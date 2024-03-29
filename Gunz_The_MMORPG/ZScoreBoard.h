#ifndef _ZSCOREBOARD_H
#define _ZSCOREBOARD_H

#include "ZInterface.h"
#include "ZCharacterManager.h"


class CCFrame;
class ZFrame;
class ZScoreListBox;

class ZScoreBoard : public ZInterface
{
private:
protected:
	CCFrame*				m_pFrame;
	ZScoreListBox*		m_pListBox;
	ZCharacterManager*	m_pCharacterManager;
	ZIDLResource*		m_pIDLResource;
	virtual bool OnShow();
	virtual void OnHide();

	void Refresh();
	void Clear();
	void Add(const char* szTeam, const char* szID, const char* szState, 
		int nScore, int nKills, int nDeaths, int nPing);
public:
	ZScoreBoard(ZCharacterManager* pCharacterManager, ZIDLResource* pIDLResource,
		        const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZScoreBoard();
	virtual bool OnCreate();
	virtual void OnDestroy();

	virtual void OnDraw(CCDrawContext* pDC);
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);

	void Update();
};


#endif