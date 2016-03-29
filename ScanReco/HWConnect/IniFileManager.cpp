// IniFileManager.cpp: implementation of the CIniFileManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFileManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniFileManager::CIniFileManager()
{

}

CIniFileManager::~CIniFileManager()
{

}

CString CIniFileManager::GetIniFilePath()
{
	CWinApp* pApp = AfxGetApp();
	return CString(pApp->m_pszProfileName);
}

BOOL CIniFileManager::SetIniFilePath(LPTSTR lpPath)
{
	try
	{
		CWinApp* pApp = AfxGetApp();
		
		if(lpPath == NULL)
		{
			char cIniFilePath[MAX_PATH] = "\0";
			GetModuleFileName(NULL, cIniFilePath, sizeof(cIniFilePath)-1);
			char *p = strrchr(cIniFilePath, '.');
			strcpy(p, ".INI");
			
			// First free the string allocated by MFC at CWinApp startup.
			// The string is allocated before InitInstance is called.
			free((void*)pApp->m_pszProfileName);
			// Change the name of the .ini file.
			// The CWinApp destructor will free the memory.
			pApp->m_pszProfileName = _tcsdup(cIniFilePath);
			
			return TRUE;
		}
		else
		{
			
			// First free the string allocated by MFC at CWinApp startup.
			// The string is allocated before InitInstance is called.
			free((void*)pApp->m_pszProfileName);
			// Change the name of the .ini file.
			// The CWinApp destructor will free the memory.
			pApp->m_pszProfileName = _tcsdup(lpPath);
			
			return TRUE;
		}
	}
	catch(...)
	{
	}
	
	return FALSE;
}

BOOL CIniFileManager::WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
{
	CWinApp* pApp = AfxGetApp();
	return pApp->WriteProfileString(lpszSection, lpszEntry, lpszValue);
}

BOOL CIniFileManager::WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue)
{
	CWinApp* pApp = AfxGetApp();
	return pApp->WriteProfileInt(lpszSection, lpszEntry, nValue);
}

CString CIniFileManager::GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
{
	CWinApp* pApp = AfxGetApp();
	return pApp->GetProfileString(lpszSection, lpszEntry, lpszDefault);
}

UINT CIniFileManager::GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
{
	CWinApp* pApp = AfxGetApp();
	return pApp->GetProfileInt(lpszSection, lpszEntry, nDefault);
}

BOOL CIniFileManager::GetModulePath(CString &strPath)
{
	char cBuf[MAX_PATH + 1] = "\0";
	if(GetModuleFileName(NULL, cBuf, MAX_PATH) > 0)
	{
		CString str = cBuf, strTemp;
		int n = str.ReverseFind('\\');
		if(n != -1)
		{
			str.SetAt(n + 1, '\0');
			strPath = str;

			return TRUE;
		}
		else
		{
			n = str.ReverseFind('/');
			if(n != -1)
			{
				str.SetAt(n + 1, '\0');
				strPath = str;
				
				return TRUE;
			}
			else
			{
			}
		}
	}

	return FALSE;
}
