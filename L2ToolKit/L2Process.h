#pragma once

class L2_EXT_CLASS CL2ProcessEnumerator
{
public :
	
	CL2ProcessEnumerator();
	virtual ~CL2ProcessEnumerator();
	
private:

	virtual BOOL OnEnuming(DWORD dwPID); //!< 返回成功则停止枚举

public :
	
	BOOL StartEnum();

};

class L2_EXT_CLASS CL2Process : protected CL2ProcessEnumerator
{
public :
	CL2Process(void);

	CL2Process(LPCTSTR lpszFileName);
	CL2Process(HANDLE  hProcess);
	CL2Process(DWORD   dwProcessId);
	CL2Process(HWND    hWnd);

	virtual ~CL2Process(void);

public :

	static BOOL CheckRunning(LPCTSTR lpszMutexName);
	static BOOL AdjustProcessTokenPrivilege();

public :
	
	void Attach(LPCTSTR lpszFileName);
	void Attach(HANDLE  hProcess);
	void Attach(DWORD   dwProcessId);
	void Attach(HWND    hWnd);

	BOOL Execute();
	BOOL Execute(LPCTSTR lpszCommandLine);
	BOOL Execute(int nCmdShow);
	BOOL Execute(LPCTSTR lpszDesk, LPCTSTR lpszCommandLine, int nCmdShow);
	BOOL Execute(HANDLE hToken);
	BOOL Execute(HANDLE hToken, LPCTSTR lpszDesk, LPCTSTR lpszCommandLine, int nCmdShow);
	
	void Wait();
	void Wait(DWORD dwMilliseconds);

	BOOL Terminate();

	BOOL IsRunning(void);

	BOOL CheckImageName(LPCTSTR lpszImageName);

	BOOL LimitCPU(const UINT nIndex = 0); //0 - 31

	DWORD GetPriorityClass();

	void SetPriorityClass(DWORD dwPriorityClass);

public :

	CString GetFileName() const;
	HANDLE  GetProcessHandle() const;
	DWORD   GetProcessId() const;
	HANDLE  GetProcessToken();
	COleDateTime GetCreationTime() const;

private :

	virtual BOOL OnEnuming(DWORD dwPID);

private :

	void CommnoConstruct();
	void GetProcessTimes();
	
protected :

	CString m_strFileName;

	HANDLE  m_hProcess;
	
	DWORD   m_dwProcessId;
	
	COleDateTime m_CreationTime;
	
};
