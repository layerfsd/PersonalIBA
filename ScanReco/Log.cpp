// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#include "Log.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLog::CLog()
{
	m_fpLog = NULL;
	m_bLogTime = FALSE;
	m_bAutoNewline = FALSE;
}

CLog::~CLog()
{
	Close();
}

BOOL CLog::Open(const BOOL bLogTime, const BOOL bAutoNewline, const char *pFileName)
{
	if(m_fpLog != NULL)
	{
		Close();
	}

	char  cTemp[2048] = "\0";
	int iLen = 0;
	if(pFileName == NULL)
	{
		iLen = 0;
	}
	else
	{
		iLen = strlen(pFileName);
	}

	if(pFileName == NULL || iLen < 1)
	{
		char cPathName[MAX_PATH + 1] = "\0";
		if(GetModuleFileName(NULL, cPathName, MAX_PATH) > 0)
		{
			strncpy(cTemp, cPathName, 2048);
			char *p = strrchr(cTemp, '.');
			if(p != NULL)
			{
				strcpy(p, ".LOG");
			}
		}
	}
	else
	{
		strncpy(cTemp, pFileName, MAX_PATH);
	}


	m_fpLog = fopen(cTemp, "a");
	if(m_fpLog != NULL)
	{
		m_bAutoNewline = bAutoNewline;
		m_bLogTime = bLogTime;
		return TRUE;
	}

	return FALSE;
}

void CLog::Close()
{
	if(m_fpLog != NULL)
	{
		fclose(m_fpLog);
		m_fpLog = NULL;
		return;
	}
}

int CLog::Write(const char *format, ...)
{
	if(m_fpLog == NULL)
	{
		return -1;
	}
	if(m_bLogTime)
	{
		SYSTEMTIME stSYSTEMTIME;
		GetLocalTime(&stSYSTEMTIME);
		fprintf(m_fpLog, "[%04d-%02d-%02d %02d:%02d:%02d-%03d] ", stSYSTEMTIME.wYear, stSYSTEMTIME.wMonth, stSYSTEMTIME.wDay, stSYSTEMTIME.wHour, stSYSTEMTIME.wMinute, stSYSTEMTIME.wSecond, stSYSTEMTIME.wMilliseconds);
	}

	
	va_list arg_ptr;
	
	va_start(arg_ptr, format);
	int nWrittenBytes = vfprintf(m_fpLog, format, arg_ptr);
	va_end(arg_ptr);

	
	if(m_bAutoNewline)
	{
		fputs("\r\n", m_fpLog);
	}
	
	fflush(m_fpLog);
	return nWrittenBytes;
}

bool CLog::IsOpen()
{
	return (m_fpLog != NULL);
}
