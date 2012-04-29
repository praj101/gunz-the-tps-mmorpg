#pragma once
// Use local dipain /////////////////////////////////////////////////////////////////
//
// LOCALE_US		: International
// LOCALE_JAPAN		: Japan
// LOCALE_KOREA		: South Korea
// LOCALE_BRAZIL	: Brazil
// LOCALE_INDIA		: India
//
/////////////////////////////////////////////////////////////////////////////////////////
// According to the region define ///////////////////////////////////////////////////////////////////
#define LOCALE_US
#if (!defined(LOCALE_US)) && (!defined(LOCALE_JAPAN)) && (!defined(LOCALE_KOREA)) && (!defined(LOCALE_BRAZIL)) && (!defined(LOCALE_INDIA)) && (!defined(LOCALE_NHNUSA))

#define LOCALE_KOREA		//default value if no dipaindo South Korea

#endif // (!defined(LOCALE_US)) && (!defined(LOCALE_JAPAN)) && (!defined(LOCALE_KOREA))
