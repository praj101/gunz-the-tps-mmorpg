#pragma once

#include "CCButton.h"
#include "CCListBox.h"

#define CCCOMBOBOX				"ComboBox"
#define CCCOMBOLISTBOX			"ComboListBox"
#define CCCMBBOX_CHANGED		"changed"

class MIDLResource;

/// ComboBox에서 드롭되는 ListBox
class CCComboListBox : public CCListBox{
protected:
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);
public:
	CCComboListBox(CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCComboListBox(void){}

	DECLARE_LOOK(CCListBoxLook)
	DECLARE_LOOK_CLIENT()


	virtual const char* GetClassName(void){ return CCCOMBOLISTBOX; }
};

class CCComboBox : public CCButton{
	friend MIDLResource;

	CCComboListBox*	m_pListBox;
	int			m_nDropHeight;
	CCListener*	m_pComboBoxListener;

	int			m_nComboType;
	int			m_nNextComboBoxTypeSize;

	bool		m_bAutoDrop;
	bool		m_bDropUnder;

protected:
	virtual bool OnCommand(CCWidget* pWindow, const char* szMessage);

public:
	CCComboBox(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCComboBox(void);

	void SetDropSize(int nHeight);

	void SetComboType(int nType) {
		m_nComboType = nType;
	}

	int GetComboType() {
		return m_nComboType;
	}

	void SetNextComboBoxTypeSize(int nSize) {
		m_nNextComboBoxTypeSize = nSize;
	}

	int GetNextComboBoxTypeSize() {
		return m_nNextComboBoxTypeSize;
	}

	void SetNextSel();
	void SetPrevSel();

	void Add(const char* szItem);
	void Add(MListItem* pItem);
	const char* GetString(int i);
	CCListItem* Get(int i);
	void Remove(int i);
	void RemoveAll(void);
	int GetCount(void);
	int GetSelIndex(void);
	bool SetSelIndex(int i);
	const char* GetSelItemString(void);
	CCListItem* GetSelItem(void);

	// Field Support
	void AddField(const char* szFieldName, int nTabSize) { m_pListBox->AddField(szFieldName, nTabSize); }
	void RemoveField(const char* szFieldName) { m_pListBox->RemoveField(szFieldName); }
	CCLISTFIELD* GetField(int i) { return m_pListBox->GetField(i); }
	int GetFieldCount(void) { return m_pListBox->GetFieldCount(); }

	bool IsVisibleHeader(void) { return m_pListBox->IsVisibleHeader(); }
	void SetVisibleHeader(bool bVisible) { m_pListBox->SetVisibleHeader(bVisible); }

	virtual void SetListener(CCListener* pListener);
	virtual CCListener* GetListener(void);

	void Sort(void);

	void SetListboxAlignment( CCAlignmentMode am)		{ m_pListBox->m_FontAlign = am; }


	virtual const char* GetClassName(void){ return CCCOMBOBOX; }

	virtual void OnReleaseFocus();

	void CloseComboBoxList();	

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);
};


