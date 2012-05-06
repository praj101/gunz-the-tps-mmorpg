#ifndef ZMAPLISTBOX_H
#define ZMAPLISTBOX_H

#include "CCWidget.h"

class MListBox;
class CCZFileSystem;
class CCTextArea;

class ZMapListBox : public CCWidget{
protected:
	MListBox*	m_pListBox;
	CCBitmap*	m_pThumbnail;

protected:
	virtual void OnDraw(CCDrawContext* pDC);
	virtual bool OnShow();
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage);
public:
	ZMapListBox(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZMapListBox();

	void Refresh(CCZFileSystem* pFS);

	const char* GetSelItemString();

	void SetSelIndex(int i);
};

#endif
