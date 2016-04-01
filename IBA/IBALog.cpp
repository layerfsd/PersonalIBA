#include "stdafx.h"
#include "ibalog.h"
#include "IBAConfig.h"
#include "IBA.h"
#include "Encrypt.h"

SINGLETON_IMPLEMENT(CIBALog)

CIBALog::CIBALog(void)
{
	SetLogPath(theApp.GetWorkPath() + _T("\\IBALog"));
	SetAutoDelLogFile(true);
	SetMaxLogFileNumber(7);
	SetLogOption(CIBAConfig::GetInstance()->GetLogLevel());

	WriteLogHead();
}

CIBALog::~CIBALog(void)
{
}

void CIBALog::WriteLogHead()
{
	Write(_T("*******************************************************"));	
	Write(_T("**                                                   **"));

	CFileVersionInfo fvi;
	fvi.Create();

	CString strVer = fvi.GetProductVersion();
	
	strVer.Replace(_T(", "), _T("."));
	
	m_strVer = strVer;

	m_strComment = fvi.GetComments();

	CString strFilePath;
	::GetModuleFileName(NULL, strFilePath.GetBuffer(512), 512);
	//strFilePath.ReleaseBuffer(512);
	CFile file;
	CString strTmp1;
	file.Open(strFilePath, CFile::modeRead, NULL);
	if(INVALID_HANDLE_VALUE == file.m_hFile)
	{
		return;
	}

	long nLen = (long)file.GetLength();
	BYTE *pBuf = new BYTE[nLen];
	if (file.Read(pBuf, nLen) != nLen)
	{
		Write(_T("读取文件失败"));
	}

	char ch[33] = {0};
	CEncrypt::CalcMD5Bytes(pBuf, nLen, ch);
	m_strFileMD5Hash = CA2T(ch);

	WriteFormat(_T("**                  IBA(%s)                     **"), strVer);

	Write(_T("**                                                   **"));
	Write(_T("*******************************************************"));
	//输出Build时间
	WriteFormat(_T("Build Time: %s"),GetAppBuildTime());
	//Write(_T("LOG 2014-11-6 14:37:28"));
	// 2014-3-11 - qsc 内存泄漏了
	delete[] pBuf;
}

CString CIBALog::GetAppVer() const
{
	return m_strVer;
}

CString CIBALog::GetAppBuildTime() const
{
	return m_strComment;
}

CString CIBALog::GetFileHash() const
{
	return m_strFileMD5Hash;
}
