#pragma once

#include <afxmt.h>

class CWxLog
{
public:
	static CWxLog* GetInstance();

public:
	CString GetLogFileName() const;

	void SetLogFileName(CString newVal);

	CString GetLogPath() const;
	void SetLogPath(CString newVal);

	UINT GetLogOption() const;
	void SetLogOption(UINT newVal);

	bool GetUseDayName() const { return m_UseDayName; }
	void SetUseDayName(bool newVal) { m_LastDay = 0; m_UseDayName = newVal; }

	bool GetWithThreadId() const;
	void SetWithThreadId(bool newVal);

	UINT GetMaxLogFileNumber() const { return m_MaxLogFileNumber; }
	void SetMaxLogFileNumber(UINT newVal) { m_MaxLogFileNumber = newVal; }

public :

	bool Write(LPCTSTR lpszMsg);
	bool Write(LPCTSTR lpszMsg, UINT eLogType);
	bool Write(const void* lpBuf, UINT nCount);
	bool Write(const void* lpBuf, UINT nCount, UINT eLogType);
	bool WriteFormat(LPCTSTR lpszMsg, ...);
	bool WriteFormat(UINT eLogType, LPCTSTR lpszMsg, ...);

protected:
	void MakeLogInfoHead(CString& strMsg);
	void ChangeLogFileName();
	void OpenLogFile();

	bool CheckLogOption(UINT eLogType);
	bool LogInfo(LPCTSTR lpszInfo);
	bool LogBuf(const void* lpBuf, UINT nCount);

private:
	CWxLog();
	virtual ~CWxLog();

protected:
	CString m_LogFileName;
	CString m_LogPath;

	bool m_UseDayName;
	int  m_LastDay;

	UINT m_LogOption;
	bool m_WithThreadId;

	UINT m_MaxLogFileNumber;

	CCriticalSection m_cs;
	CFile m_LogFile;
};

#define WXM_LOG0 CWxLog::GetInstance()->Write
#define WXM_LOG	CWxLog::GetInstance()->WriteFormat