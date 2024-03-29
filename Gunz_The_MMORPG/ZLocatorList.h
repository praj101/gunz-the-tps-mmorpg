#pragma once

#include <vector>
#include <string>

using std::vector;
using std::string;

class CCZFileSystem;

#define LOCATOR_PORT 8900

class ZLocatorList : public map< int, string >
{
public:
	ZLocatorList();
	virtual ~ZLocatorList();

	void AddIP( const char* szIP);

	void Clear() { m_LocatorIPList.clear(); }
	const int GetSize() const { return static_cast< int >( m_LocatorIPList.size() ); }
	const string& GetIPByPos( const int nPos ) { return m_LocatorIPList[ nPos ]; }

//	bool Init( CCZFileSystem* pFileSystem, const char* pszListFile );

	bool ParseLocatorList( ::CCXmlElement& element );

private :
//	bool LoadXML( CCZFileSystem* pFileSystem, const char* pszListFile );
	bool ParseLocator( ::CCXmlElement& element );

private :
	vector< string > m_LocatorIPList;
};