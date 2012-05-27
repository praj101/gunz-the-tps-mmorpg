#ifndef _ZCHANNELLISTITEM_H
#define _ZCHANNELLISTITEM_H


#include "CCListBox.h"
class ZChannelListItem : public CCListItem {
	CCUID			m_uidChannel;
	char			m_szChannelName[64];
	
	int				m_nChannelType;
	int				m_nChannelNumber;
	int				m_nPlayers,m_nMaxPlayers;

	char			m_szItemString[256];
public:
	ZChannelListItem(const CCUID& uid, const int nChannelNumber, const char* szChannelName, 
		const int nChannelType, const int nPlayers, const int nMaxPlayers) 
	{ 
		m_uidChannel = uid; strcpy(m_szChannelName, szChannelName); 
		m_nChannelType = nChannelType;
		m_nChannelNumber = nChannelNumber; 
		m_nPlayers = nPlayers;
		m_nMaxPlayers = nMaxPlayers;
	}
	virtual ~ZChannelListItem()			{}
	virtual const char* GetString()	{ 
		sprintf(m_szItemString,"%s (%d/%d)",m_szChannelName,m_nPlayers,m_nMaxPlayers);
		return m_szItemString; 
	}
	CCUID GetUID()						{ return m_uidChannel; }
	char* GetName()						{ return m_szChannelName; }
};




#endif