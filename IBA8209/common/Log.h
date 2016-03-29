#pragma once

#include <afxmt.h>
// 这个版本的Log是从别的地方的CLog改名字来的
// 因为原来动态库里面的CLog找不到源码
// 2014-4-8 - qsc 
class CCommLog
{
public :

	CCommLog(void);

	virtual ~CCommLog(void);

	static CCommLog* GetInstance();

	static CString GetAppPath();

	static CString GetAppVer();

	static CString GetWinErrorMessage(DWORD dwError);

private:
	
	static CCommLog* m_Instance;

public :
	
	void SetLogFileName(CString newVal);
	CString GetLogFileName() const;

	CString GetLogPath() const;
	void SetLogPath(CString newVal);

	UINT GetLogOption() const;
	void SetLogOption(UINT newVal);
	
	bool GetUseDayName() const;
	void SetUseDayName(bool newVal);

	bool GetWithThreadId() const;
	void SetWithThreadId(bool newVal);
	
	UINT GetMaxLogFileNumber() const;
	void SetMaxLogFileNumber(UINT newVal);

	bool GetAutoDelLogFile() const;
	void SetAutoDelLogFile(bool newVal);

public :

	virtual bool Write(LPCTSTR lpszMsg);
	virtual bool Write(LPCTSTR lpszMsg, UINT eLogType);
	virtual bool Write(const void* lpBuf, UINT nCount);
	virtual bool Write(const void* lpBuf, UINT nCount, UINT eLogType);
	virtual bool WriteFormat(LPCTSTR lpszMsg, ...);
	virtual bool WriteFormat(UINT eLogType, LPCTSTR lpszMsg, ...);

	void WriteLogHead();
	void WriteLastError();

protected :
	
	void MakeLogInfoHead(CString& strMsg);
	void ChangeLogFileName();
	void DelLogFiles();
	void OpenLogFile();
	
	bool CheckLogOption(UINT eLogType);
	bool LogInfo(LPCTSTR lpszInfo);
	bool LogBuf(const void* lpBuf, UINT nCount);
	
protected :

	CString m_LogFileName;
	CString m_LogPath;
	
	bool m_UseDayName;
	int  m_LastDay;

	UINT m_LogOption;
	bool m_WithThreadId;
	
	bool m_AutoDelLogFile;
	UINT m_MaxLogFileNumber;

	::CCriticalSection m_csLog;
	CFile m_LogFile;

private:
	// 加密相关
	CHAR *m_pPSK;
	DWORD  m_iPSKLen;//不包括结尾符

	void Encrypt(BYTE *buffer,DWORD bufsize);
	void SetEncrypt(CString strPSK);
	bool IsNeedEncrypt();
};
