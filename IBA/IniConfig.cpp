#include "stdafx.h"
#include ".\iniconfig.h"

CIniConfig::CIniConfig(void)
{
}

CIniConfig::~CIniConfig(void)
{
}

void CIniConfig::ReadValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT& nRet, UINT nDefault /*= 0*/)
{
	nRet = GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, m_strIniFile);
}

void CIniConfig::ReadValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, UINT& nRet, UINT nDefault /*= 0*/)
{
	nRet = GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, m_strIniFile);
}

void CIniConfig::ReadValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, DWORD& nRet, UINT nDefault /*= 0*/)
{
	nRet = GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, m_strIniFile);
}

void CIniConfig::ReadValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, CString& strRet, LPCTSTR lpDefault)
{
	CString strTmp;

	GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, strTmp.GetBuffer(MAX_PATH), MAX_PATH, m_strIniFile);

	strTmp.ReleaseBuffer(MAX_PATH);

	strRet = strTmp.Trim();
}

void CIniConfig::SaveValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, const CString& strValue)
{
	WritePrivateProfileString(lpAppName, lpKeyName, strValue, m_strIniFile);
}

void CIniConfig::SaveValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, UINT nValue)
{
	CString strTmp;

	strTmp.Format(_T("%d"), nValue);

	WritePrivateProfileString(lpAppName, lpKeyName, strTmp, m_strIniFile);
}

void CIniConfig::SaveValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nValue)
{
	CString strTmp;

	strTmp.Format(_T("%d"), nValue);

	WritePrivateProfileString(lpAppName, lpKeyName, strTmp, m_strIniFile);
}

void CIniConfig::SaveValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, DWORD nValue)
{
	CString strTmp;

	strTmp.Format(_T("%d"), nValue);

	WritePrivateProfileString(lpAppName, lpKeyName, strTmp, m_strIniFile);
}

