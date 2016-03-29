#include "StdAfx.h"
#include "log.h"

#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include <ShlObj.h>
#pragma comment(lib, "shell32.lib")

#pragma comment(lib, "Version.lib")

CCommLog* CCommLog::m_Instance = NULL;

CCommLog* CCommLog::GetInstance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new CCommLog();
	}

	return m_Instance;
}
                                                                                                                                                      
CCommLog::CCommLog(void)
{
	m_LogOption = 1;
	m_WithThreadId = true;
	m_LastDay = 0;
	m_UseDayName = true;
	m_AutoDelLogFile = true;
	m_MaxLogFileNumber = 7;

	// ¼ÓÃÜÏà¹Ø
	m_pPSK = NULL;
#ifndef _DEBUG
	SetEncrypt(_T("dodonew"));
#endif

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	
	CString strPath(szPath);
	m_LogFileName = strPath + _T(".log");
	m_LogPath = strPath  + _T(".Logs\\");
}

CCommLog::~CCommLog(void)
{
	m_LogFile.Abort();
}

void CCommLog::WriteLogHead()
{
	Write(_T("*******************************************************"));	
	Write(_T("**                                                   **"));

	CString strVer(AfxGetApp()->m_pszAppName);
	strVer.AppendFormat(_T("(%s)"), GetAppVer());
	strVer.Replace(_T(", "), _T("."));

	CString strTmp(_T("**"));

	for (INT i = 0; i < (51 - strVer.GetLength()) / 2; i++)
	{
		strTmp.AppendChar(' ');
	}

	strTmp.Append(strVer);

	for (INT i = strTmp.GetLength(); i < 53; i++)
	{
		strTmp.AppendChar(' ');
	}

	strTmp.Append(_T("**"));

	Write(strTmp);

	Write(_T("**                                                   **"));
	Write(_T("*******************************************************"));
}


bool CCommLog::GetWithThreadId() const
{
	return m_WithThreadId;
}

CString CCommLog::GetLogFileName() const
{
	return m_LogFileName;
}

void CCommLog::SetLogFileName( CString newVal )
{	
	SetUseDayName(false);
	
	m_LogFileName = newVal;	
	
	OpenLogFile();
}

void CCommLog::SetWithThreadId( bool newVal )
{
	m_WithThreadId = newVal;
}

UINT CCommLog::GetLogOption() const
{
	return m_LogOption;
}

void CCommLog::SetLogOption( UINT newVal )
{
	m_LogOption = newVal;
}

bool CCommLog::LogBuf(const void* lpBuf, UINT nCount )
{
	if(IsNeedEncrypt())
	{
		Encrypt((BYTE*)lpBuf,nCount);
	}

	bool bRet = false;

	CSingleLock(&m_csLog, TRUE);

	try
	{
		if (CFile::hFileNull == m_LogFile.m_hFile)
		{
			OpenLogFile();
		}

		m_LogFile.Write(lpBuf, nCount);
		m_LogFile.Flush();
		bRet = true;
	}
	catch (CFileException* e)
	{
		//e->ReportError();
		e->Delete();
	}
	catch (CException* e)
	{
		//e->ReportError();
		e->Delete();
	}
	
	return bRet;
}

bool CCommLog::LogInfo( LPCTSTR lpszInfo )
{
	bool bRet = false;

#ifdef _UNICODE
	
	CStringA strInfo;
	strInfo = CW2A(lpszInfo);
	bRet = LogBuf(strInfo.GetString(), strInfo.GetLength());

#else

	bRet = LogBuf(lpszInfo, lstrlen(lpszInfo));

#endif

	return bRet;
}

bool CCommLog::Write( LPCTSTR lpszMsg)
{
	CString strMsg;

	MakeLogInfoHead(strMsg);

	strMsg.Append(lpszMsg);

	strMsg.Append(_T("\r\n"));

	bool bRet =  LogInfo(strMsg);

	strMsg.ReleaseBuffer();

	return bRet;
}

bool CCommLog::Write( LPCTSTR lpszMsg, UINT eLogType)
{
	if (!CheckLogOption(eLogType))
	{
		return false;
	}

	return Write(lpszMsg);
}

bool CCommLog::Write(const void* lpBuf, UINT nCount)
{
	CString strMsg;

	MakeLogInfoHead(strMsg);

	bool bRet = LogInfo(strMsg);

	bRet = LogBuf(lpBuf, nCount);

	strMsg.Empty();

	strMsg.Append(_T("\r\n"));

	bRet = LogInfo(strMsg);

	strMsg.ReleaseBuffer();

	return bRet;
}

bool CCommLog::Write(const void* lpBuf, UINT nCount, UINT eLogType)
{
	if (!CheckLogOption(eLogType))
	{
		return false;
	}

	return Write(lpBuf, nCount);
}

bool CCommLog::WriteFormat(LPCTSTR lpszMsg, ... )
{
	CString strMsg;
	
	MakeLogInfoHead(strMsg);

	va_list argList;
	va_start(argList, lpszMsg);
	strMsg.AppendFormatV(lpszMsg, argList);
	va_end( argList );

	strMsg.Append(_T("\r\n"));

	bool bRet = LogInfo(strMsg);

	strMsg.ReleaseBuffer();

	return bRet;
}

bool CCommLog::WriteFormat(UINT eLogType, LPCTSTR lpszMsg, ... )
{
	if (!CheckLogOption(eLogType))
	{
		return false;
	}

	CString strMsg;

	MakeLogInfoHead(strMsg);

	va_list argList;
	va_start(argList, lpszMsg);
	strMsg.AppendFormatV(lpszMsg, argList);
	va_end( argList );

	strMsg.Append(_T("\r\n"));

	bool bRet = LogInfo(strMsg);

	strMsg.ReleaseBuffer();

	return bRet;
}

void CCommLog::WriteLastError()
{
	DWORD dwError = GetLastError();
	
	CString strTmp;
	strTmp.Format(_T("GetLastError:%d - %s"), dwError, GetWinErrorMessage(dwError));
		
	Write(strTmp);
}

bool CCommLog::CheckLogOption(UINT eLogType )
{
	if (eLogType == 0) //DEBUG LOG
	{
#ifndef _DEBUG
		return false;
#else
		return true;
#endif
	}

	if (!(m_LogOption & eLogType))
	{
		return false;
	}

	return true;
}

void CCommLog::MakeLogInfoHead(CString& strMsg)
{
	SYSTEMTIME systime = {0};
	GetLocalTime(&systime);

	strMsg.Format(_T("%04d-%02d-%02d %02d:%02d:%02d.%03d"),
		systime.wYear, systime.wMonth, systime.wDay, systime.wHour,
		systime.wMinute, systime.wSecond, systime.wMilliseconds);

	if (GetWithThreadId())
	{
		strMsg.AppendFormat(_T(" [%04d]"), GetCurrentThreadId());
	}

	strMsg.Append(_T(" -> "));

	if (m_UseDayName && m_LastDay != systime.wDay)
	{
		m_LastDay = systime.wDay;
		ChangeLogFileName();
	}

}

void CCommLog::ChangeLogFileName()
{
	if (!PathFileExists(m_LogPath))
	{
		SHCreateDirectoryEx(NULL, m_LogPath, NULL);
	}

	CString strFileName = m_LogPath;
	
	SYSTEMTIME systime ={0};
	GetLocalTime(&systime);

	strFileName.AppendFormat(_T("%04d%02d%02d.log"), systime.wYear, systime.wMonth, systime.wDay);

	m_LogFileName = strFileName;
	OpenLogFile();

	if (GetAutoDelLogFile())
	{
		DelLogFiles();
	}
}

void CCommLog::OpenLogFile()
{
	CSingleLock(&m_csLog, TRUE);

	m_LogFile.Abort();

	CFileException ex;

	UINT nOpenFlag = CFile::modeWrite | CFile::shareDenyNone; 

	if (!PathFileExists(m_LogFileName))
	{
		nOpenFlag |= CFile::modeCreate;
	}

	if (!m_LogFile.Open(m_LogFileName, nOpenFlag, &ex))
	{
		//ex.ReportError();
		return;
	}
	
	m_LogFile.SeekToEnd();
}

CString CCommLog::GetLogPath() const
{
	return m_LogPath;
}

void CCommLog::SetLogPath( CString newVal )
{
	m_LogPath = newVal.Trim();
	
	if (m_LogPath.Right(1) != _T("\\"))
	{
		m_LogPath.Append(_T("\\"));
	}
}

bool CCommLog::GetUseDayName() const
{
	return m_UseDayName;
}

void CCommLog::SetUseDayName( bool newVal )
{
	m_LastDay = 0;
	m_UseDayName = newVal;
}

bool CCommLog::GetAutoDelLogFile() const
{
	return m_AutoDelLogFile;
}

void CCommLog::SetAutoDelLogFile( bool newVal )
{
	m_AutoDelLogFile = newVal;
}

UINT CCommLog::GetMaxLogFileNumber() const
{
	return m_MaxLogFileNumber;
}

void CCommLog::SetMaxLogFileNumber( UINT newVal )
{
	m_MaxLogFileNumber = newVal;
}

void CCommLog::DelLogFiles()
{
	COleDateTime dtNow = COleDateTime::GetCurrentTime();
	COleDateTimeSpan dts(m_MaxLogFileNumber, 0, 0, 0);
	dtNow -= dts;
	
	INT nFileMin = _tstoi(dtNow.Format(_T("%Y%m%d")));
	INT nFileTime = 0;

	CFileFind finder;
	
	CString strLogFileName(GetLogPath() + _T("*.log"));

	BOOL bResult = finder.FindFile(strLogFileName);
	
	while (bResult)
	{
		bResult = finder.FindNextFile();
		strLogFileName = finder.GetFileTitle();
		nFileTime = _tstoi(strLogFileName);
		
		if (nFileTime <= nFileMin)
		{
			DeleteFile(finder.GetFilePath());
		}
	}
	
	finder.Close();

}

CString CCommLog::GetAppPath()
{
	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString strPath(szPath);
	strPath = strPath.Left(strPath.ReverseFind(_T('\\')));
	return strPath;
}

CString CCommLog::GetAppVer()
{
	TCHAR szPath[MAX_PATH] = {0};
	
	GetModuleFileName(NULL, szPath, MAX_PATH);
	
	CString strTmp(szPath);

	DWORD zero = 0;

	DWORD verSize = ::GetFileVersionInfoSize(strTmp, &zero);

	if (verSize != 0)
	{
		CByteArray buffer;
		buffer.SetSize(verSize);

		if (::GetFileVersionInfo(strTmp, 0, verSize, buffer.GetData()))
		{
			LPTSTR pVersion = 0;
			UINT verLen = 0;

			if (::VerQueryValue(buffer.GetData(), 
				const_cast<LPTSTR>(_T("\\StringFileInfo\\080404b0\\FileVersion")), 
				(void**)&pVersion, &verLen))
			{
				strTmp = pVersion;
			}
			else if (::VerQueryValue(buffer.GetData(), 
				const_cast<LPTSTR>(_T("\\StringFileInfo\\080403a8\\FileVersion")), 
				(void**)&pVersion, &verLen))
			{
				strTmp = pVersion;
			}
			else
			{
				strTmp.Empty();
			}
		}
	}

	return strTmp;
}


CString CCommLog::GetWinErrorMessage(DWORD dwError)
{
	TCHAR szTmp[512] = {0};

	if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 
		0, dwError, 0, szTmp, 511, NULL))
	{
		/* if we fail, call ourself to find out why and return that error */
		return (GetWinErrorMessage(GetLastError()));  
	}

	CString strTmp(szTmp);
	
	strTmp.Remove('\r');
	strTmp.Remove('\n');

	return strTmp;
}



bool CCommLog::IsNeedEncrypt()
{
	if(NULL == m_pPSK || m_iPSKLen<=0 || strlen(m_pPSK) != m_iPSKLen)
		return FALSE;

	return TRUE;
}

void CCommLog::SetEncrypt(CString strPSK)
{
	if(strPSK == "") return;

	if(m_pPSK != NULL)
	{
		delete m_pPSK;
		m_pPSK = NULL;
	}

	m_iPSKLen = strPSK.GetLength();

	m_pPSK = new char[m_iPSKLen+1];

	memset(m_pPSK,0,m_iPSKLen+1);

	strcpy(m_pPSK, CT2A(strPSK));
}

void CCommLog::Encrypt(BYTE  *buffer,DWORD bufsize)
{

	if(NULL == buffer|| !IsNeedEncrypt()) return;

	DWORD num=0;

	while(num<bufsize)
	{	
		for(DWORD i=0;i<m_iPSKLen;i++)
		{
			buffer[num]^= m_pPSK[i];
		}

		num++;
	}

}
