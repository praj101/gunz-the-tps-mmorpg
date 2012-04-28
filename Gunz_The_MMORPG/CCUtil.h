#pragma once
#include <vector>
#include <string>

using std::string;
using std::vector;

// 10진수값에 대한 집합형
#define SetBitSet(sets, item)		(sets |= (1 << item))
#define ClearBitSet(sets, item)		(sets &= ~(1 << item))
#define CheckBitSet(sets, item)		(sets & (1 << item))

enum CCDateType
{
	CCDT_Y = 1,
	CCDT_YM,
	CCDT_YMD,
	CCDT_YMDH,
	CCDT_YMDHM,
};

const string MakeTimeString( const CCDateType DateType, SYSTEMTIME st );
const string MGetStrLocalTime( const CCDateType = CCDT_YMDHM );
const string MGetStrLocalTime( const unsigned short wDay, const unsigned short wHour, const unsigned short wMin, const CCDateType = CCDT_YMDHM );

bool SplitStrIP( const string& strIP, vector<BYTE>& vIP );
