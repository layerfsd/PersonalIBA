// IniFileManager.h: interface for the CIniFileManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILEMANAGER_H__7C8EF7DE_CB7B_4EEA_BC77_6CA00F491A43__INCLUDED_)
#define AFX_INIFILEMANAGER_H__7C8EF7DE_CB7B_4EEA_BC77_6CA00F491A43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIniFileManager  
{
public:
	CIniFileManager();
	virtual ~CIniFileManager();

public:
	BOOL GetModulePath(CString &strPath);
	UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault );
	CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = NULL );
	BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue );
	BOOL SetIniFilePath(LPTSTR lpPath = NULL);
	CString GetIniFilePath(void);
	BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue );

};

#endif // !defined(AFX_INIFILEMANAGER_H__7C8EF7DE_CB7B_4EEA_BC77_6CA00F491A43__INCLUDED_)
