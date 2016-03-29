#include "stdafx.h"
#include "WxLog.h"

#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

CWxLog* CWxLog::GetInstance()
{
	static CWxLog wxLog;

	return &wxLog;
}

CWxLog::CWxLog(void)
{
	m_LogOption = 1;
	m_WithThreadId = true;
	m_LastDay = 0;
	m_UseDayName = true;
	m_MaxLogFileNumber = 7;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString strPath(szPath);
	strPath = strPath.Left(strPath.ReverseFind(_T('\\')));
	strPath += _T("\\WxLog");

	SetLogPath(strPath);
	SetMaxLogFileNumber(7);
	SetLogOption(255);
}

CWxLog::~CWxLog(void)
{
	m_LogFile.Abort();
}

bool CWxLog::GetWithThreadId() const
{
	return m_WithThreadId;
}

CString CWxLog::GetLogFileName() const
{
	return m_LogFileName;
}

void CWxLog::SetLogFileName( CString newVal )
{	
	SetUseDayName(false);

	m_LogFileName = newVal;	

	OpenLogFile();
}

void CWxLog::SetWithThreadId( bool newVal )
{
	m_WithThreadId = newVal;
}

UINT CWxLog::GetLogOption() const
{
	return m_LogOption;
}

void CWxLog::SetLogOption( UINT newVal )
{
	m_LogOption = newVal;
}

bool CWxLog::LogBuf(const void* lpBuf, UINT nCount )
{
	bool bRet = false;

	CSingleLock(&m_cs, TRUE);

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

bool CWxLog::LogInfo( LPCTSTR lpszInfo )
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

bool CWxLog::Write( LPCTSTR lpszMsg)
{
	CString strMsg;

	MakeLogInfoHead(strMsg);

	strMsg.Append(lpszMsg);

	strMsg.Append(_T("\r\n"));

	bool bRet =  LogInfo(strMsg);

	strMsg.ReleaseBuffer();

	return bRet;
}

bool CWxLog::Write( LPCTSTR lpszMsg, UINT eLogType)
{
	if (!CheckLogOption(eLogType))
	{
		return false;
	}

	return Write(lpszMsg);
}

bool CWxLog::Write(const void* lpBuf, UINT nCount)
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

bool CWxLog::Write(const void* lpBuf, UINT nCount, UINT eLogType)
{
	if (!CheckLogOption(eLogType))
	{
		return false;
	}

	return Write(lpBuf, nCount);
}

bool CWxLog::WriteFormat(LPCTSTR lpszMsg, ... )
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

bool CWxLog::WriteFormat(UINT eLogType, LPCTSTR lpszMsg, ... )
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

bool CWxLog::CheckLogOption(UINT eLogType )
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

void CWxLog::MakeLogInfoHead(CString& strMsg)
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

void CWxLog::ChangeLogFileName()
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

	Write(_T("***************************************"));
}

void CWxLog::OpenLogFile()
{
	CSingleLock(&m_cs, TRUE);

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

CString CWxLog::GetLogPath() const
{
	return m_LogPath;
}

void CWxLog::SetLogPath( CString newVal )
{
	m_LogPath = newVal.Trim();

	if (m_LogPath.Right(1) != _T("\\"))
	{
		m_LogPath.Append(_T("\\"));
	}
}