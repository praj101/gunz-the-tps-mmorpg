#pragma once

#include "CCLocaleDefine.h"

// 국가 코드
enum CCCountry
{
	CCC_INVALID			= 0,
	CCC_KOREA			= 82,		// 한국
	CCC_US				= 1,		// 미국(인터네셔날)
	CCC_JAPAN			= 81,		// 일본
	CCC_BRAZIL			= 55,		// 브라질
	CCC_INDIA			= 91,		// 인도

	// 10000이상부턴 CustomID.
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

/* TODO: MLangageConf.h 와 통합해야함 - bird */

// LANG_xxx는 winnt.h에 정의되어 있음. - by SungE.
enum CCLanguage
{
	CCL_INVALID				= 0x00,
	CCL_CHINESE				= LANG_CHINESE,					// 중국어
	CCL_CHINESE_TRADITIONAL	= SUBLANG_CHINESE_TRADITIONAL,	// 중국어 같은데 -  _-)a;; 
	CCL_KOREAN				= LANG_KOREAN,					// 한국어 (LANG_KOREAN값과 동일)
	CCL_ENGLISH				= LANG_ENGLISH,					// 영어 (LANG_ENGLISH값과 동일)
	CCL_JAPANESE			= LANG_JAPANESE,				// 일본어 (LANG_JAPANESE값과 동일)
	CCL_BRAZIL				= LANG_PORTUGUESE,				// 브라질 (LANG_BRAZIL값과 동일)
	CCL_INDIA				= LANG_INDONESIAN,				// 인도 (LANG_INDONESIAN값과 동일)
	CCL_GERMAN				= LANG_GERMAN,					// 독일어 (LANG_GERMAN값과 동일)
	CCL_SPANISH				= LANG_SPANISH,					// 스페인어 (LANG_SPANISH값과 동일)
};

/// 지역화 관련 최상위 관리 클래스 
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
	void SetLanguage(CCLanguage langID)	{ m_nLanguage = langID; }	// 국가에 따라 초기화된 언어를 다른 것으로 바꾸고 싶을때

	const bool bIsComplete()			{ return m_bIsComplete; }
};

const CCCountry GetCountryID( const char* pCountry );
const CCLanguage GetLanguageID( const char* pLanguage );
