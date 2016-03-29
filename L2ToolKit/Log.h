#pragma once

class L2_EXT_CLASS CLog
{
public :
	CLog(void);
	virtual ~CLog(void);

public :
	
	CString GetLogFileName() const;
	CString GetAppPath() const;

	void SetLogFileName(CString newVal);

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

	bool Write(LPCTSTR lpszMsg);
	bool Write(LPCTSTR lpszMsg, UINT eLogType);
	bool Write(const void* lpBuf, UINT nCount);
	bool Write(const void* lpBuf, UINT nCount, UINT eLogType);
	bool WriteFormat(LPCTSTR lpszMsg, ...);
	bool WriteFormat(UINT eLogType, LPCTSTR lpszMsg, ...);

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

	CCriticalSection m_cs;
	CFile m_LogFile;
};
