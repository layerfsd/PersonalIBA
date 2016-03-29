#include "StdAfx.h"
#include "process.h"
#include <Psapi.h>
#include "kwindef.h"
#pragma comment(lib, "psapi.lib")
#include <tlhelp32.h>
CProcessEnumerator::CProcessEnumerator()
{

}

CProcessEnumerator::~CProcessEnumerator()
{

}

BOOL CProcessEnumerator::OnEnuming(DWORD dwPID)
{
	return TRUE; 
}

BOOL CProcessEnumerator::StartEnum()
{
	DWORD dwProcIds[1024] = {0}, cbNeeded = 0;

	if (!EnumProcesses(dwProcIds, sizeof(dwProcIds), &cbNeeded))
	{
		return FALSE;
	}

	for (int i = 0; i < (int)(cbNeeded / sizeof(DWORD)); i++)
	{
		if (dwProcIds[i] != NULL)
		{
			if (!OnEnuming(dwProcIds[i]))
			{
				break;
			}
		}
	}

	return TRUE;
}

void CProcessEnumerator::GetProcessArray( CArray<CString, CString> * processArray, ProcMode e_ProcMode )
{
	if (Toolhelp32Mode == e_ProcMode)
	{
		GetProcessToolhelp32(processArray);
	}
	else if (EnumProcessesMode == e_ProcMode)
	{
		GetProcessEnum(processArray);
	}
	else if (WTSEnumerateProcessMode == e_ProcMode)
	{
		GetProcessWTSEnum(processArray);
	}
	else if (QuerySystemInformationMode == e_ProcMode)
	{
		GetProcessQuerySystemInfor(processArray);
	}
}

void CProcessEnumerator::GetProcessToolhelp32( CArray<CString, CString> * processArray )
{
	PROCESSENTRY32 ProcessType;
	ProcessType.dwSize=sizeof(ProcessType);

	HANDLE processHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

	if(processHandle != INVALID_HANDLE_VALUE)
	{
		if(!Process32First(processHandle, &ProcessType))
		{
			return ;
		}
		else
		{
			CString strProcess(ProcessType.szExeFile);

			if (!strProcess.IsEmpty())
			{
				if (processArray)
				{
					processArray->Add(strProcess);
				}
				else
				{
					ProcessEachName(strProcess);
				}
			}

		}

		while(Process32Next(processHandle, &ProcessType))
		{
			CString strProcess(ProcessType.szExeFile);

			if (!strProcess.IsEmpty())
			{
				if (processArray)
				{
					processArray->Add(strProcess);
				}
				else
				{
					ProcessEachName(strProcess);
				}
			}

		}

	}

	if(processHandle!=NULL)
	{	
		CloseHandle(processHandle);
	}

}

void CProcessEnumerator::GetProcessEnum( CArray<CString, CString> * processArray )
{
	DWORD dwProcIds[1024] = {0}, cbNeeded = 0;

	if (EnumProcesses(dwProcIds, sizeof(dwProcIds), &cbNeeded))
	{
		for (int i = 0; i < (int)(cbNeeded / sizeof(DWORD)); i++)
		{
			if (dwProcIds[i] != NULL)
			{
				CString strFileName = _T("");

				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcIds[i]);

				if (hProcess != NULL)
				{	
					HMODULE hMods[1024] = {0}; DWORD cbNeeded = 0;

					if (EnumProcessModules(hProcess, hMods, sizeof(hMods) / sizeof(DWORD), &cbNeeded))
					{
						if (GetModuleFileNameEx(hProcess, hMods[0], strFileName.GetBuffer(MAX_PATH), MAX_PATH))
						{
							strFileName.ReleaseBuffer(MAX_PATH);
							strFileName.Trim();
							INT nstrLength = strFileName.GetLength() - 1;
							strFileName = strFileName.Right(nstrLength - strFileName.ReverseFind(_T('\\')));

							if (processArray)
							{
								processArray->Add(strFileName);
							}
							else
							{
								ProcessEachName(strFileName);
							}
						}

					}

				} 
			}
		}
	}
}

void CProcessEnumerator::GetProcessWTSEnum( CArray<CString, CString> * processArray )
{
	HMODULE hWtsApi32 = LoadLibrary(_T("WTSAPI32.DLL"));

	WTSENUMERATEPROCESSES wtsEnumProc = (WTSENUMERATEPROCESSES)GetProcAddress(hWtsApi32, "WTSEnumerateProcessesA");

	WTSFREEMEMORY  wtsFreeMemory = (WTSFREEMEMORY)GetProcAddress(hWtsApi32, "WTSFreeMemory");

	PWTS_PROCESS_INFO pProcInfo;
	DWORD dwCount = 0;

	if(!wtsEnumProc(NULL, 0, 1, &pProcInfo, &dwCount))
		return;
	for(DWORD i = 0; i < dwCount; i++)
	{

		CString strProcessName(pProcInfo[i].pProcessName);

		if (processArray)
		{
			processArray->Add(strProcessName);
		}
		else
		{
			ProcessEachName(strProcessName);
		}
	}

	wtsFreeMemory(pProcInfo);

}

void CProcessEnumerator::GetProcessQuerySystemInfor( CArray<CString, CString> * processArray )
{
		PVOID pProcInfo = NULL;
		DWORD dwInfoSize = 0x20000;
		PPROCESSINFO pProcessInfo;

		long ( __stdcall *NtQuerySystemInformation )( DWORD, PVOID, DWORD, DWORD );
	

		pProcInfo = (PVOID)(new byte[dwInfoSize]);
	
		NtQuerySystemInformation = (long(__stdcall*)(DWORD,PVOID,DWORD,DWORD))
	
			GetProcAddress( GetModuleHandle( _T("ntdll.dll") ),"NtQuerySystemInformation" );
	
		NtQuerySystemInformation(5, pProcInfo,dwInfoSize,0);
	
		pProcessInfo = (PPROCESSINFO)pProcInfo;
	
		do
		{
			if(pProcessInfo->dwOffset == 0)
			{
				break;
			}
	
			CString strProcessName(pProcessInfo->ProcessName.Buffer);
			
			if (processArray)
			{
				processArray->Add(strProcessName);
			}
			else
			{
				ProcessEachName(strProcessName);
			}

			pProcessInfo = (PPROCESSINFO)((byte*)pProcessInfo + pProcessInfo->dwOffset);
		}
		while(true);

}

INT CProcessEnumerator::ProcessEachName( CString strProcessName )
{

	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////


CProcess::CProcess(void)
: m_hProcess(NULL)
{
	CommnoConstruct();
}

CProcess::CProcess(LPCTSTR lpszFileName)
: m_hProcess(NULL)
{
	CommnoConstruct();
	m_strFileName = lpszFileName;
}

CProcess::CProcess(HANDLE hProcess)
: m_hProcess(NULL)
{
	Attach(hProcess);
}

CProcess::CProcess(DWORD dwProcessId)
: m_hProcess(NULL)
{
	Attach(dwProcessId);
}

CProcess::CProcess( HWND hWnd )
: m_hProcess(NULL)
{
	Attach(hWnd);
}

CProcess::~CProcess(void)
{
	CommnoConstruct();
}

void CProcess::CommnoConstruct()
{
	if (m_hProcess != NULL)
	{
		CloseHandle(m_hProcess);
	}

	m_strFileName.Empty();
	m_hProcess = NULL;
	m_dwProcessId = NULL;
}

void CProcess::Attach( LPCTSTR lpszFileName )
{
	CommnoConstruct();
	m_strFileName = lpszFileName;
	m_strFileName.Trim();
	m_strFileName.MakeLower();
	StartEnum();
}

void CProcess::Attach( HANDLE hProcess )
{
	CommnoConstruct();

	if (hProcess != NULL)
	{	
		HMODULE hMods[1024] = {0}; DWORD cbNeeded = 0;

		if (EnumProcessModules(hProcess, hMods, sizeof(hMods) / sizeof(DWORD), &cbNeeded))
		{
			if (GetModuleFileNameEx(hProcess, hMods[0], m_strFileName.GetBuffer(MAX_PATH), MAX_PATH))
			{
				m_strFileName.ReleaseBuffer(MAX_PATH);
				m_strFileName.Trim();
			}
		}

		m_hProcess = hProcess;
		m_dwProcessId = ::GetProcessId(m_hProcess);
	}
}

void CProcess::Attach( DWORD dwProcessId )
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

	Attach(hProcess);
}

void CProcess::Attach( HWND hWnd )
{
	DWORD dwProcessId = 0;

	GetWindowThreadProcessId(hWnd, &dwProcessId);

	Attach(dwProcessId);
}

BOOL CProcess::Execute()
{
	return Execute(NULL, NULL, NULL, SW_NORMAL);
}

BOOL CProcess::Execute( LPCTSTR lpszCommandLine )
{
	return Execute(NULL, NULL, lpszCommandLine, SW_NORMAL);
}

BOOL CProcess::Execute( int nCmdShow )
{
	return Execute(NULL, NULL, NULL, nCmdShow);
}

BOOL CProcess::Execute(LPCTSTR lpszDesk, LPCTSTR lpszCommandLine, int nCmdShow)
{
	return Execute(NULL, lpszDesk, lpszCommandLine, nCmdShow);
}

BOOL CProcess::Execute( HANDLE hToken )
{
	return Execute(hToken, NULL, NULL, SW_NORMAL);
}

BOOL CProcess::Execute(HANDLE hToken, LPCTSTR lpszDesk, LPCTSTR lpszCommandLine, int nCmdShow )
{
	BOOL bRet = FALSE;

	if (m_strFileName.IsEmpty())
	{
		return bRet;
	}

	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};

	si.cb = sizeof(si);
	si.dwFlags = STARTF_FORCEOFFFEEDBACK;
	si.wShowWindow = (WORD)nCmdShow; 
	si.lpDesktop = (LPTSTR)lpszDesk;

	TCHAR szCommandLine[2048] = {0};

	lstrcat(szCommandLine, m_strFileName);
	lstrcat(szCommandLine, _T(" "));
	lstrcat(szCommandLine, lpszCommandLine);

	if (hToken == NULL)
	{
		bRet = CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE, 0, 0, NULL, &si, &pi);
	}
	else
	{
		si.lpDesktop = _T("winsta0\\default");

		if (ImpersonateLoggedOnUser(hToken))
		{		
			bRet = CreateProcessAsUser(
				hToken,            // client's access token
				NULL,              // file to execute
				szCommandLine,     // command line
				NULL,              // pointer to process SECURITY_ATTRIBUTES
				NULL,              // pointer to thread SECURITY_ATTRIBUTES
				FALSE,             // handles are not inheritable
				NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,   // creation flags
				NULL,              // pointer to new environment block 
				NULL,              // name of current directory 
				&si,               // pointer to STARTUPINFO structure
				&pi                // receives information about new process
				); 			
			
			RevertToSelf();
		}
	}

	if (bRet)
	{
		if (m_hProcess != NULL)
		{
			CloseHandle(m_hProcess);
		}

		m_hProcess = pi.hProcess;
		m_dwProcessId = pi.dwProcessId;

		CloseHandle(pi.hThread);

	}

	return bRet;
}


BOOL CProcess::Terminate(DWORD dwMilliseconds)
{
	BOOL bRet = FALSE;

	if (m_hProcess != NULL)
	{
		bRet = TerminateProcess(m_hProcess, 0);
		
		if (dwMilliseconds > 0)
		{
			Wait(dwMilliseconds);
		}
		
		CloseHandle(m_hProcess);

		m_hProcess = NULL;
		m_dwProcessId = NULL;
	}

	return bRet;
}

void CProcess::Wait()
{
	Wait(INFINITE);
}

void CProcess::Wait(DWORD dwMilliseconds)
{
	if (m_hProcess != NULL)
	{
		WaitForSingleObject(m_hProcess, dwMilliseconds);
	}
}

BOOL CProcess::CheckRunning(LPCTSTR lpszMutexName)
{
	BOOL bRet = FALSE;

	HANDLE hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, lpszMutexName);
	
	if (hMutex != NULL)
	{
		CloseHandle(hMutex);

		bRet = TRUE;
	}
	else
	{
		if (GetLastError() == ERROR_ACCESS_DENIED) //没权限访问
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

BOOL CProcess::AdjustProcessTokenPrivilege()
{
	HANDLE hToken = NULL; 

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return FALSE;
	}

	TOKEN_PRIVILEGES tkp = {0}; 

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid))
	{
		CloseHandle(hToken);
		return FALSE;
	}

	tkp.PrivilegeCount = 1;

	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0); 

	if (GetLastError() != ERROR_SUCCESS)
	{
		CloseHandle(hToken);
		return FALSE;
	}

	CloseHandle(hToken);

	return TRUE;
}

CString CProcess::GetFileName() const
{
	return m_strFileName;
}

HANDLE CProcess::GetProcessHandle() const
{
	return m_hProcess;
}

DWORD CProcess::GetProcessId() const
{
	return m_dwProcessId;
}

BOOL CProcess::LimitCPU(const UINT nIndex)
{
	DWORD_PTR ulMask = (0x01 << nIndex) & 0xFFFFFFFF;

	return SetProcessAffinityMask(m_hProcess, ulMask);
}


DWORD CProcess::GetPriorityClass()
{
	if (m_hProcess != NULL)
	{
		return ::GetPriorityClass(m_hProcess);
	}

	return NULL;
}

void CProcess::SetPriorityClass( DWORD dwPriorityClass )
{
	if (m_hProcess != NULL)
	{
		::SetPriorityClass(m_hProcess, dwPriorityClass);
	}
}

HANDLE CProcess::GetProcessToken()
{
	HANDLE hToken = NULL;

	if (m_hProcess != NULL)
	{
		if (OpenProcessToken(m_hProcess, TOKEN_ALL_ACCESS, &hToken))
		{
			//
		}
	}

	return hToken;
}

BOOL CProcess::IsRunning(void)
{
	BOOL bRet = FALSE;

	if (m_hProcess != NULL)
	{	
		DWORD dwExitCode = 0;

		GetExitCodeProcess(m_hProcess, &dwExitCode);

		if (STILL_ACTIVE == dwExitCode)
		{
			bRet = TRUE;
		}
		else
		{
			m_hProcess = NULL;
			m_dwProcessId = NULL;
		}
	}

	return bRet;
}

BOOL CProcess::OnEnuming( DWORD dwPID )
{
	CProcess aProcess(dwPID);
	
	if (aProcess.CheckImageName(m_strFileName))
	{
		Attach(dwPID);
		return FALSE;
	}

	return TRUE;
}

BOOL CProcess::CheckImageName( LPCTSTR lpszImageName )
{
	CString strImageName(lpszImageName);
	
	strImageName.MakeLower();

	CString strTmp = GetFileName().MakeLower();

	if (strTmp.Find(strImageName) >= 0)
	{
		return TRUE;
	}

	return FALSE;
}

size_t CProcess::GetWorkingSetSize()
{
	PROCESS_MEMORY_COUNTERS pmc = {0};
	pmc.cb = sizeof PROCESS_MEMORY_COUNTERS;
	GetProcessMemoryInfo(m_hProcess, &pmc, sizeof PROCESS_MEMORY_COUNTERS);

	return pmc.WorkingSetSize;
}

//long CProcess::GetWorkingCPUSize()
//{
//	DWORD dwProcessID = GetProcessId();
//
//	long lCpuUse;
//	PVOID pProcInfo = NULL;
//	DWORD dwInfoSize = 0x20000;
//	PPROCESSINFO pProcessInfo;
//	DWORD dwWorkingSet;
//	long ( __stdcall *NtQuerySystemInformation )( DWORD, PVOID, DWORD, DWORD );
//
//
//	static __int64 LastTotalProcessCPUUsage = 0;
//	static __int64 LastCurrentProcessCPUUsage = 0;
//
//	int CurrentDelta;
//	int TotalDelta;
//
//	__int64 TotalProcessCPUUsage = 0;
//	__int64 CurrentProcessCPUUsage = 0;
//
//	/////////////////////////////////
//
//	pProcInfo = (PVOID)(new byte[dwInfoSize]);
//
//	NtQuerySystemInformation = (long(__stdcall*)(DWORD,PVOID,DWORD,DWORD))
//
//		GetProcAddress( GetModuleHandle( "ntdll.dll" ),"NtQuerySystemInformation" );
//
//	NtQuerySystemInformation(5, pProcInfo,dwInfoSize,0);
//
//	pProcessInfo = (PPROCESSINFO)pProcInfo;
//
//	do
//	{
//		TotalProcessCPUUsage += (__int64)pProcessInfo->KernelTime.QuadPart + (__int64)pProcessInfo->UserTime.QuadPart;
//
//		if(pProcessInfo->dwProcessID == dwProcessID)
//		{
//			dwWorkingSet = pProcessInfo->dwWorkingSet; 
//			CurrentProcessCPUUsage += (__int64)pProcessInfo->KernelTime.QuadPart + (__int64)pProcessInfo->UserTime.QuadPart;
//		}
//
//		/////////
//		if(pProcessInfo->dwOffset == 0)
//		{
//			break;
//		}
//
//		pProcessInfo = (PPROCESSINFO)((byte*)pProcessInfo + pProcessInfo->dwOffset);
//	}
//	while(true);
//
//	TotalDelta = TotalProcessCPUUsage - LastTotalProcessCPUUsage;
//	CurrentDelta = CurrentProcessCPUUsage - LastCurrentProcessCPUUsage;
//
//	if(TotalDelta != 0)
//		lCpuUse = 100 * CurrentDelta / TotalDelta;
//
//	LastTotalProcessCPUUsage = TotalProcessCPUUsage;
//	LastCurrentProcessCPUUsage = CurrentProcessCPUUsage;
//
//	delete[] pProcInfo;
//	return lCpuUse;
//
//}