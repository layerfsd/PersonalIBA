#pragma once
#include <afxtempl.h>
class CProcessEnumerator
{
protected :
	
	CProcessEnumerator();
	virtual ~CProcessEnumerator();

public:
	enum ProcMode
	{ 
		Toolhelp32Mode = 1,
		EnumProcessesMode = 2,
		WTSEnumerateProcessMode = 3,
		QuerySystemInformationMode = 4

	};
private:

	virtual BOOL OnEnuming(DWORD dwPID); //!< 返回成功则停止枚举

public :
	
	BOOL StartEnum();

	void GetProcessArray(CArray<CString, CString> * processArray, ProcMode e_ProcMode);

	void GetProcessToolhelp32(CArray<CString, CString> * processArray);

	void GetProcessEnum(CArray<CString, CString> * processArray);

	void GetProcessWTSEnum(CArray<CString, CString> * processArray);

	void GetProcessQuerySystemInfor(CArray<CString, CString> * processArray);

protected :
	virtual INT ProcessEachName(CString strProcessName);


};

template<class TClass>
class CProcessEnumeratorT : public CProcessEnumerator
{
public :

	typedef INT (TClass::*pfnProcessEachName)(CString strProcessName);

	CProcessEnumeratorT(TClass *pObj, pfnProcessEachName pProcessEachName)
		:m_pObj(pObj)
		,m_pfnProcessEachName(pProcessEachName)
	{

	}

protected :

	virtual INT ProcessEachName(CString strProcessName)
	{
		return (m_pObj->*m_pfnProcessEachName)(strProcessName);
	}

	TClass* m_pObj;

	pfnProcessEachName m_pfnProcessEachName;

private :

	CProcessEnumeratorT(const CProcessEnumeratorT &rhs);
	CProcessEnumeratorT &operator=(const CProcessEnumeratorT &rhs);
};


class CProcess : protected CProcessEnumerator
{
public :
	CProcess(void);

	CProcess(LPCTSTR lpszFileName);
	CProcess(HANDLE  hProcess);
	CProcess(DWORD   dwProcessId);
	CProcess(HWND    hWnd);

	virtual ~CProcess(void);

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

	BOOL Terminate(DWORD dwMilliseconds = 0);

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

	size_t GetWorkingSetSize();

	//long   GetWorkingCPUSize();
private :

	virtual BOOL OnEnuming(DWORD dwPID);

private :

	void CommnoConstruct();
	
protected :

	CString m_strFileName;

	HANDLE  m_hProcess;
	
	DWORD   m_dwProcessId;
	
};
