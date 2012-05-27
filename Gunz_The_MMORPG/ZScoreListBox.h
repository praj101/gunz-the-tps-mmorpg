#ifndef _ZSCORELISTBOX_H
#define _ZSCORELISTBOX_H

#include <stdio.h>
#include "CCWidget.h"
#include "CCListBox.h"
#include "CCFrame.h"

class ZListItemScore;

class ZScoreListBox : public CCListBox
{
private:
protected:
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener)
	{
		return false;
	}
public:
	ZScoreListBox(const char* szName, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZScoreListBox() { }
};


class ZListItemScore : public CCListItem
{
protected:
	char	m_szTeam[256];
	char	m_szID[256];
	char	m_szState[256];
	int		m_nScore;
	int		m_nKills;
	int		m_nDeaths;
	int		m_nPing;
public:
	ZListItemScore(const char* szTeam, const char* szID, const char* szState, 
		int nScore, int nKills, int nDeaths, int nPing)
	{
		strcpy(m_szTeam, szTeam);
		strcpy(m_szID, szID);
		strcpy(m_szState, szState);
		m_nScore = nScore;
		m_nKills = nKills;
		m_nDeaths = nDeaths;
		m_nPing = nPing;
	}
	virtual const char* GetString() { return m_szID; }
	virtual const char* GetString(int i) 
	{
		static char szTemp[256];
		switch (i)
		{
		case 0:
			return m_szTeam;
			break;
		case 1:
			return m_szID;
			break;
		case 2:
			return m_szState;
			break;
		case 3:
			sprintf(szTemp, "%d", m_nScore);
			return szTemp;
			break;
		case 4:
			sprintf(szTemp, "%d", m_nKills);
			return szTemp;
			break;
		case 5:
			sprintf(szTemp, "%d", m_nDeaths);
			return szTemp;
			break;
		case 6:
			sprintf(szTemp, "%d", m_nPing);
			return szTemp;
			break;
		}

		return NULL;
	}
};

class ZScoreBoardFrame : public CCFrame
{
private:
protected:
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener)
	{
		return false;
	}
public:
	ZScoreBoardFrame(const char* szName, CCWidget* pParent=NULL, CCListener* pListener=NULL)
		: CCFrame(szName, pParent, pListener)
	{

	}
	virtual ~ZScoreBoardFrame() { }
};
#endif