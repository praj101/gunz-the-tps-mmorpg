#pragma once

#define CCDRAGOBJECT_STRING_LENGTH	256

class CCBitmap;

class CCDragObject{
	int			m_iID;
	char		m_szString[CCDRAGOBJECT_STRING_LENGTH];
	CCBitmap*	m_pBitmap;

public:
	CCDragObject(int nID, const char* szString, CCBitmap* pBitmap);
	virtual ~CCDragObject();

	int GetID(){
		return m_iID;
	}
	const char* GetString(){
		return m_szString;
	}
	CCBitmap* GetBitmap(){
		return m_pBitmap;
	}
};