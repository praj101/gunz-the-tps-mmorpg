#include "stdafx.h"
#include "CCBaseLocale.h"

CCBaseLocale::CCBaseLocale()
{
	m_bIsComplete = false;
}

CCBaseLocale::~CCBaseLocale()
{

}

bool CCBaseLocale::Init(CCCountry nCountry)
{
	m_iCountry = nCountry;
	InitLanguageFromCountry();

	m_bIsComplete = OnInit();

	return m_bIsComplete;
}


// 국가값을 바탕으로 언어 세팅 - 이것은 국가가 추가될때마다 새로 넣어줘야 한다.
void CCBaseLocale::InitLanguageFromCountry()
{
	switch (m_iCountry)
	{
	case CCC_KOREA:		m_iLanguage = CCL_KOREAN;		break;
	case CCC_US:			m_iLanguage = CCL_ENGLISH;		break;
	case CCC_JAPAN:		m_iLanguage = CCL_JAPANESE;		break;
	case CCC_BRAZIL:		m_iLanguage = CCL_BRAZIL;		break;
	case CCC_INDIA:		m_iLanguage = CCL_INDIA;			break;
	case CCC_NHNUSA :	m_iLanguage = CCL_ENGLISH;		break;
	default:
		{
			// 해당 국가 없음 - 추가해주세요.
			_ASSERT(0);
		}
	};
}


const CCCountry GetCountryID( const char* pLanguage )
{
	ASSERT( (0 != pLanguage) && (0 < strlen(pLanguage)) );

	if( 0 == pLanguage )			return CCC_INVALID;
	if( 0 == strlen(pLanguage) )	return CCC_INVALID;

	if( 0 == stricmp("kor", pLanguage) )
		return CCC_KOREA;
	else if( 0 == stricmp("international", pLanguage) )
		return CCC_US;
	else if( 0 == stricmp("jpn", pLanguage) )
		return CCC_JAPAN;
	else if( 0 == stricmp("brz", pLanguage) )
		return CCC_BRAZIL;
	else if( 0 == stricmp("ind", pLanguage) )
		return CCC_INDIA;
	else if( 0 == stricmp("usa", pLanguage) )
		return CCC_NHNUSA;
	else
	{
		ASSERT( 0 );
	}

	return CCC_INVALID;
}


const CCLanguage GetLanguageID( const char* pLanguage )
{
	ASSERT( (0 != pLanguage) && (0 < strlen(pLanguage)) );

	if( 0 == pLanguage )		 return CCL_INVALID;
	if( 0 == strlen(pLanguage) ) return CCL_INVALID;

	if( 0 == stricmp("kor", pLanguage) )
		return CCL_KOREAN;
	else if( 0 == stricmp("usa", pLanguage) )
		return CCL_ENGLISH;
	else if( 0 == stricmp("jpn", pLanguage) )
		return CCL_JAPANESE;
	else if( 0 == stricmp("brz", pLanguage) )
		return CCL_BRAZIL;
	else if( 0 == stricmp("ind", pLanguage) )
		return CCL_INDIA;
	else if( 0 == stricmp("grm", pLanguage) )
		return CCL_GERMAN;
	else if( 0 == stricmp("spn", pLanguage) )
		return CCL_SPANISH;
	else
	{
		ASSERT( 0 );
	}
	
	return CCL_INVALID;
}