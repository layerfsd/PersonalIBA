#pragma once

class CIniConfig
{
public :

	CIniConfig(void);
	virtual ~CIniConfig(void);

	CString m_strIniFile;

	void ReadValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, CString& strRet, LPCTSTR lpDefault = NULL);
	void ReadValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT& nRet, UINT nDefault = 0);
	void ReadValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, UINT& nRet, UINT nDefault = 0);
	void ReadValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, DWORD& nRet, UINT nDefault = 0);

	void SaveValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, CString& strValue);
	void SaveValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nValue);
	void SaveValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, UINT nValue);
	void SaveValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, DWORD nValue);
};
