#pragma once

#include "CCLocaleDefine.h"

//Country Code
enum CCCountry
{
	CCC_INVALID			= 0,
	CCC_KOREA			= 82,		//South Korea
	CCC_US				= 1,		//USA (International)
	CCC_JAPAN			= 81,		//Japanese
	CCC_BRAZIL			= 55,		//Brazil
	CCC_INDIA			= 91,		//??India

	//Take it from more than 10,000 CustomID.
	CCC_NHNUSA			= 10001		// NHN USA.
};

//
//#ifdef LOCALE_US
//	#define DEFAULT_COUNTRY			CCC_US
//#elif LOCALE_JAPAN
//	#define DEFAULT_COUNTRY			CCC_JAPAN
//#elif LOCALE_BRAZIL
//	#define DEFAULT_COUNTRY			CCC_BRAZIL
//#elif LOCALE_INDIA
//	#define DEFAULT_COUNTRY			CCC_INDIA
//#else
//	#define DEFAULT_COUNTRY			CCC_KOREA
//#endif
//

/* TODO: MLangageConf.h Must integrate with - bird */

//LANG_xxx defined in the winnt.h
enum CCLanguage
{
	CCL_INVALID				= 0x00,
	CCL_CHINESE				= LANG_CHINESE,					// Chinese
	CCL_CHINESE_TRADITIONAL	= SUBLANG_CHINESE_TRADITIONAL,	// Chinese thought
	CCL_KOREAN				= LANG_KOREAN,					// Korean (LANG_KOREAN the same value).
	CCL_ENGLISH				= LANG_ENGLISH,					// English (LANG_ENGLISH the same value).
	CCL_JAPANESE			= LANG_JAPANESE,				// Japanese (LANG_JAPANESE the same value).
	CCL_BRAZIL				= LANG_PORTUGUESE,				// Brazil (LANG_BRAZIL the same value).
	CCL_INDIA				= LANG_INDONESIAN,				// India (LANG_INDONESIAN the same value).
	CCL_GERMAN				= LANG_GERMAN,					// German (LANG_GERMAN the same value).
	CCL_SPANISH				= LANG_SPANISH,					// Spanish (LANG_SPANISH the same value).
};

//Localization-related top-level management classes
class CCBaseLocale
{
private:
	void InitLanguageFromCountry();
protected:
	CCCountry			m_nCountry;
	CCLanguage			m_nLanguage;

	bool				m_bIsComplete;

	virtual bool OnInit() = 0;
public:
	CCBaseLocale();
	virtual ~CCBaseLocale();
	bool Init(CCCountry nCountry);

	const CCCountry	GetCountry()		{ return m_nCountry; }
	const CCLanguage GetLanguage()		{ return m_nLanguage; }
	void SetLanguage(CCLanguage langID)	{ m_nLanguage = langID; }	//depending on the country to another language, no reply has been initialized

	const bool bIsComplete()			{ return m_bIsComplete; }
};

const CCCountry GetCountryID( const char* pCountry );
const CCLanguage GetLanguageID( const char* pLanguage );
