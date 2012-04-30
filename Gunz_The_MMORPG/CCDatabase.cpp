#include "StdAfx.h"
#include ".\mdatabase.h"

CCDatabase::CCDatabase(void) : m_fnLogCallback( 0 )
{
	m_strDSNConnect = "";

	m_dwOptions = 0;
	m_dwOptions |= CDatabase::noOdbcDialog;
	m_dwOptions |= CDatabase::useCursorLib;
}

CCDatabase::~CCDatabase(void)
{
}


bool CCDatabase::CheckOpen()
{
	bool ret = true;
	if (!m_DB.IsOpen())
	{
		ret = Connect(m_strDSNConnect);
		WriteLog( "CCDatabase::CheckOpen - Reconnect database\n" );
	}

	return ret;
}

CString CCDatabase::BuildDSNString(const CString strDSN, const CString strUserName, const CString strPassword)
{
	CString strDSNConnect =  _T("DSN=") + strDSN
					+ _T(";UID=") + strUserName
					+ _T(";PWD=") + strPassword;
	return strDSNConnect;
}

bool CCDatabase::Connect(CString strDSNConnect)
{
	if (m_DB.m_hdbc && m_DB.IsOpen()) m_DB.Close();

	m_strDSNConnect = strDSNConnect;

	BOOL bRet = FALSE;
	if (strDSNConnect.IsEmpty()) {
		try {
			bRet = m_DB.Open(NULL);
		} catch(CDBException* e) {
			char szLog[ 256 ] = {0,};
			_snprintf( szLog, 255, "CCDatabase::Connect - %s\n", e->m_strError );
			WriteLog( szLog );
		}
	} else {
		try {
			bRet = m_DB.OpenEx( strDSNConnect, m_dwOptions );
		} catch(CDBException* e) {
			char szLog[ 256 ] = {0,};
			_snprintf( szLog, 255, "CCDatabase::Connect - %s\n", e->m_strError );
			WriteLog( szLog );
		}
	}
	if (bRet == TRUE) {
		m_DB.SetQueryTimeout(60);
		return true;
	} else {
#ifdef _DEBUG
		OutputDebugString("DATABASE Error \n");
#endif
		return false;
	}
}

void CCDatabase::Disconnect()
{
	if (m_DB.IsOpen())
		m_DB.Close();
}


BOOL CCDatabase::IsOpen() const
{
	return m_DB.IsOpen();
}


void CCDatabase::ExecuteSQL( LPCTSTR lpszSQL )
{
	try
	{
		m_DB.ExecuteSQL( lpszSQL );
	}
	catch( ... )
	{
		throw;
	}
}


void CCDatabase::WriteLog( const string& strLog )
{
	if( 0 != m_fnLogCallback  )
	{
		m_fnLogCallback( strLog );
	}
}