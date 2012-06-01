#pragma once

#include "CCWidget.h"
#include "CCLookNFeel.h"

class CCMsgBox;
class CCButtonGroup;

#define CORE_CCBUTTON			"Button"

#define CCBTN_CLK_MSG		"click"	///< 버튼이 클릭될때 Listener에게 발생되는 메세지
#define CCBTN_RCLK_MSG		"rclick"///< R 버튼이 클릭될때 Listener에게 발생되는 메세지
#define CCBTN_DN_MSG			"down"	///< 버튼이 눌릴때
#define CCBTN_UP_MSG			"up"	///< 버튼이 띄어질때
#define CCBTN_RDN_MSG		"rdown"	///< R버튼이 눌릴때
#define CCBTN_RUP_MSG		"rup"	///< R버튼이 띄어질때
#define CCBTN_IN_MSG			"in"	///< 버튼 위에 커서가 올려 질때
#define CCBTN_OUT_MSG		"out"	///< 버튼 위에 커서가 빠질때 올려 질때


/// 버튼 타입
enum CCButtonType{
	CCBT_NORMAL = 0,		///< 일반 버튼
	CCBT_PUSH,			///< 푸쉬 버튼
	CCBT_PUSH2,
};

/// 디폴트 키 등록
enum CCButtonKeyAssigned{
	CCBKA_NONE = 0,		///< 아무키도 등록되어 있지 않은 버튼
	CCBKA_ENTER,			///< Enter 키 눌리면 실행되는 버튼
	CCBKA_ESC,			///< ESC 키 눌리면 실행되는 버튼
};

class CCButton;
/// Button의 Draw 코드가 있는 클래스, 이 클래스를 상속받아서 커스텀룩을 가질 수 있다.
class CCButtonLook{
protected:
	bool	m_bWireLook;
protected:
	virtual void OnDownDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual void OnUpDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual void OnOverDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual void OnDisableDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual void OnCheckBoxDraw(CCButton* pButton, CCDrawContext* pDC, bool bPushed){};
public:
	virtual void OnDrawText(CCButton* pButton, sRect& r, CCDrawContext* pDC);
	virtual void OnDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual sRect GetClientRect(CCButton* pButton, sRect& r);
	void SetWireLook(bool b) {	m_bWireLook = b;	}
	bool GetWireLook() const{	return m_bWireLook; }
public:
	CCButtonLook():m_bWireLook(false){}
};


/// 버튼 클래스
class CCButton : public CCWidget{
protected:
	bool		m_bMouseOver;
	bool		m_bLButtonDown;
	bool		m_bRButtonDown;
	sColor		m_TextColor;
	bool		m_bShowText;
	CCAlignmentMode	m_AlignmentMode;
	CCButtonType		m_Type;
	bool		m_bChecked;				///< CCBT_PUSH인 경우
	bool		m_bComboDropped;
	bool		m_bStretch;
	
	CCButtonGroup	*m_pButtonGroup;	///< 속해있는 그룹
	int				m_nIndexInGroup;	///< 같은 그룹내에서의 index

public:
	bool		m_bEnableEnter;			///< 포커스 있을때 Enter키로 버튼 누르기 활성화 ( Default : true )
	bool		m_bHighlight;			///< Highlight(&) 지원
	CCBitmap*	m_pIcon;				///< 아이콘
	CCButtonKeyAssigned	m_uKeyAssigned;	///< Key Assigned
	CCMsgBox*	m_pMsgBox;				///< 메세지 박스를 통한 확인 메세지
	string		m_strIDLConfirmText;	///< xml에 정의된 확인 메시지

	sPoint		m_ClickPos;
	sPoint		m_LDragStartClickPos;
	int			m_LDragVariationX;
	int			m_LDragVariationY;

private:
	//void DrawButton(CCDrawContext* pDC);
protected:
	//virtual void OnDraw(CCDrawContext* pDC);
	/// 마우스가 버튼 안에 들어올때 발생하는 이벤트 핸들러
	virtual void OnMouseIn(void);
	/// 마우스가 버튼 밖으로 빠져 나갈때 발생하는 이벤트 핸들러
	virtual void OnMouseOut(void);
	/// 버튼이 눌릴때 발생하는 이벤트 핸들러
	virtual void OnButtonDown(void);
	/// 버튼이 띄어질때 있을때 발생하는 이벤트 핸들러
	virtual void OnButtonUp(void);
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);

	/// 클릭되어졌을때
	virtual void OnButtonClick(void);	

	virtual bool OnShow(void);
	virtual void OnHide(void);
public:
	CCButton(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCButton(void);
	/// 버튼 텍스트 컬러 지정
	void SetTextColor(sColor color);
	/// 버튼 텍스트 컬러 얻기
	sColor GetTextColor(void);
	/// 버튼에 텍스트 보여주기
	void ShowText(bool bShow=true);
	virtual bool DefaultCommand(void);

	/// 버튼내 텍스트 정렬 얻기
	CCAlignmentMode GetAlignment(void);
	/// 버튼내 텍스트 정렬 지정
	CCAlignmentMode SetAlignment(CCAlignmentMode am);

	/// 버튼 타입 지정
	void SetType(CCButtonType t);
	/// 버튼 타입 얻기
	CCButtonType GetType(void);

	/// Push 버튼인 경우 체크
	void SetCheck(bool bCheck);
	/// Push 버튼인 경우 체크값 얻기
	bool GetCheck(void);

	/// 버튼이 눌려져 있는가? ( 마우스로 누르는 상태를 얻어낼 수 있다. )
	bool IsButtonDown(void);	// Async Button Down ?
	/// 마우스가 버튼 위에 올려져 있는가?
	bool IsMouseOver(void);

	/// Confirm Message Box
	void SetConfirmMessageBox(const char* szMessage);			/// 디폴트 확인 메시지
	void SetAlterableConfirmMessage(const char* szMessage);		/// 특수한 경우에 확인 메시지를 한동안 바꿔치기 할 수 있다
	void RestoreIDLConfirmMessage();							/// 확인 메시지를 바꿔치기한 쪽에서 책임지고 복원해줘야 한다~

	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage);

	int GetLDragVariationX() {
		return m_LDragVariationX;
	}

	int GetLDragVariationY() {
		return m_LDragVariationY;
	}

	DECLARE_LOOK(CCButtonLook)
	DECLARE_LOOK_CLIENT()

	void SetComboDropped(bool b) {
		m_bComboDropped = b;
	}
	bool GetComboDropped() const{
		return m_bComboDropped;
	}

	void SetStretch( bool b) {
		m_bStretch = b;
	}

	bool GetStretch( ) const {
		return m_bStretch;
	}

	void SetButtonGroup(CCButtonGroup *pGroup);

	int GetIndexInGroup() { 
		return m_nIndexInGroup; 
	}

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);

	virtual const char* GetClassName(void){ return CORE_CCBUTTON; }
};



// 버튼 그룹을 구현하는데 필요한 클래스
class CCButtonGroup {
	friend CCButton;
public:
	CCButtonGroup();
	~CCButtonGroup();

protected:
	int		m_nCount;
	CCButton *m_pPrevious;
};
