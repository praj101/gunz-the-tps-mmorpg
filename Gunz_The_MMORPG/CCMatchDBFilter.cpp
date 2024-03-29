#include "stdafx.h"
#include "CCMatchDBFilter.h"

#include <utility>


CCMatchDBFilter::CCMatchDBFilter()
{
}


CCMatchDBFilter::~CCMatchDBFilter()
{
}


string CCMatchDBFilter::Filtering( const string& str )
{
	static string strRemoveTok = "'";

	string strTmp = str;

	string::size_type pos;

	while( (pos = strTmp.find_first_of(strRemoveTok)) != string::npos )
		strTmp.erase( pos, 1 );

	return strTmp;
}