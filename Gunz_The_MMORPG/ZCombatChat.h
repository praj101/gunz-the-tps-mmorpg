#ifndef _ZCOMBATCHAT_H
#define _ZCOMBATCHAT_H

#include "ZInterface.h"

// Added
#include "CCEdit.h"

//class CCListBox;
class CCEdit;
class CCTextArea;

/////////////////
// ZTabPlayerList
#define ZPLB_ITEM_PICKPLAYER	"picked"
class ZTabPlayerList : public CCListBox {
protected:
	CCEdit*	m_pEditChat;

public:
	ZTabPlayerList(const char* szName, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	void SetChatControl(CCEdit* pEdit)	{ m_pEditChat = pEdit; }

	virtual bool OnShow();
	virtual void OnHide();
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);

	void OnPickPlayer();
};


class ZCombatChat
{
private:
protected:
	ZIDLResource*		m_pIDLResource;
	CCEdit*				m_pInputEdit;
	ZTabPlayerList*		m_pTabPlayerList;
	bool				m_bChatInputVisible;
	unsigned long int	m_nLastChattingMsgTime;
	bool				m_bTeamChat;
	bool				m_bShowOutput;

	void SetTeamChat(bool bVal)	{ m_bTeamChat = bVal; }
	void UpdateChattingBox();
	void ProcessChatMsg();
public:
	CCTextArea*			m_pChattingOutput;

	ZCombatChat();
	virtual ~ZCombatChat();
	bool Create( const char* szOutputTxtarea,bool bUsePlayerList);
	void Destroy();

	void Update();
	void EnableInput(bool bEnable, bool bToTeam=false);
	void OutputChatMsg(const char* szMsg);
	void OutputChatMsg(sColor color, const char* szMsg);

	void OnDraw(CCDrawContext* pDC);
	bool IsChat() { return m_bChatInputVisible; }
	bool IsTeamChat()	{ return m_bTeamChat; }
	bool IsShow()
	{
		if (m_pInputEdit == NULL) return false;
		return m_pInputEdit->IsVisible();
	}
	void SetFont( CCFont* pFont);

	void ShowOutput(bool bShow);
};




#endif