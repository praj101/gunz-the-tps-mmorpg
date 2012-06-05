#pragma once

#include "CCButton.h"
#include "CCListBox.h"

#define CORE_CCCMBBOX				"ComboBox"
#define CORE_CCCMBLISTBOX			"ComboListBox"
#define CCCMBBOX_CHANGED		"changed"

class CCIDLResource;

//ComboBox ListBox
class CCComboListBox : public CCListBox{
protected:
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);
public:
	CCComboListBox(CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCComboListBox(){}

	DECLARE_LOOK(CCListBoxLook)
	DECLARE_LOOK_CLIENT()


	virtual const char* GetClassName(){ return CORE_CCCMBLISTBOX; }
};

class CCComboBox : public CCButton{
	friend CCIDLResource;

	CCComboListBox*	m_pListBox;
	int			m_iDropHeight;
	CCListener*	m_pComboBoxListener;

	int			m_iComboType;
	int			m_iNextComboBoxTypeSize;

	bool		m_bAutoDrop;
	bool		m_bDropUnder;

protected:
	virtual bool OnCommand(CCWidget* pWindow, const char* szMessage);

public:
	CCComboBox(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCComboBox();

	void SetDropSize(int nHeight);

	void SetComboType(int nType) {
		m_iComboType = nType;
	}

	int GetComboType() {
		return m_iComboType;
	}

	void SetNextComboBoxTypeSize(int nSize) {
		m_iNextComboBoxTypeSize = nSize;
	}

	int GetNextComboBoxTypeSize() {
		return m_iNextComboBoxTypeSize;
	}

	void SetNextSel();
	void SetPrevSel();

	void Add(const char* szItem);
	void Add(CCListItem* pItem);
	const char* GetString(int i);
	CCListItem* Get(int i);
	void Remove(int i);
	void RemoveAll();
	int GetCount();
	int GetSelIndex();
	bool SetSelIndex(int i);
	const char* GetSelItemString();
	CCListItem* GetSelItem();

	//Field Support
	void AddField(const char* szFieldName, int nTabSize) { m_pListBox->AddField(szFieldName, nTabSize); }
	void RemoveField(const char* szFieldName) { m_pListBox->RemoveField(szFieldName); }
	CCLISTFIELD* GetField(int i) { return m_pListBox->GetField(i); }
	int GetFieldCount() { return m_pListBox->GetFieldCount(); }

	bool IsVisibleHeader() { return m_pListBox->IsVisibleHeader(); }
	void SetVisibleHeader(bool bVisible) { m_pListBox->SetVisibleHeader(bVisible); }

	virtual void SetListener(CCListener* pListener);
	virtual CCListener* GetListener();

	void Sort();

	void SetListboxAlignment( CCAlignmentMode am)		{ m_pListBox->m_FontAlign = am; }


	virtual const char* GetClassName(){ return CORE_CCCMBBOX; }

	virtual void OnReleaseFocus();

	void CloseComboBoxList();	

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);
};


