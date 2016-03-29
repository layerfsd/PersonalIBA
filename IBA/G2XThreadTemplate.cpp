#include "stdafx.h"
#include "G2XThreadTemplate.h"
#include <Windows.h>
#include <objbase.h>

void CG2XThreadDelegate::didInitInstance(CG2XThread* pThread)
{

}

int CG2XThreadDelegate::didExitInstance(CG2XThread* pThread)
{
	return 0;
}

void CG2XThreadDelegate::didRun(CG2XThread* pThread)
{

}

void CG2XThreadDelegate::didErrorReport(CG2XThread* pThread, const char* lpszError)
{

}

bool CG2XThreadDelegate::didCanExitThread(CG2XThread* pThread)
{
	return pThread->m_bRequestSafeExit;
}


CG2XThread::CG2XThread()
{
	m_hThread = NULL;
	m_pDelegate = NULL;
	m_bActive = false;
	m_hEventSafeStop = NULL;
	m_nSafeTimeOut = INFINITE;
	m_bMustSafeExit = false;
}

CG2XThread::~CG2XThread()
{
	if (m_bMustSafeExit)
	{
		SafeStop();
	}
	else
	{
		Stop();
	}
}


bool CG2XThread::Create(CG2XThreadDelegate* pDelegate, bool bSuspend/*=true*/, bool bMustSafeExit/*=false*/, int nTimeOut/*=INFINITE*/)
{
	m_pDelegate = pDelegate;
	DWORD dwID = 0;
	m_hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CG2XThread::ThreadEntry, this, CREATE_SUSPENDED, &dwID);
	if(m_hThread == INVALID_HANDLE_VALUE || m_hThread == 0)
		return false;
	
	m_bMustSafeExit = bMustSafeExit;
	m_nSafeTimeOut = nTimeOut;
	m_hEventSafeStop = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!bSuspend)
	{
		return Start();
	}
	return true;
}

bool CG2XThread::Start()
{
	if(m_hThread == INVALID_HANDLE_VALUE || m_hThread == 0)
	{
		ErrorReport("call CG2XThread::Start() error: thread handle is invalid");
		return false;
	}
	return ResumeThread(m_hThread) != DWORD(-1);
}

bool CG2XThread::Pause()
{
	if(m_hThread == INVALID_HANDLE_VALUE || m_hThread == 0)
	{
		ErrorReport("call CG2XThread::Pause() error: thread handle is invalid");
		return false;
	}
	return SuspendThread(m_hThread) != DWORD(-1);
}

bool CG2XThread::Stop(int nExitCode)
{
	if(m_hThread == INVALID_HANDLE_VALUE || m_hThread == 0)
	{
		ErrorReport("call CG2XThread::Stop() error: thread handle is invalid");
		return false;
	}
	bool bRes = TerminateThread(m_hThread, (DWORD)nExitCode) == TRUE;
	m_hThread = NULL;
	if (m_hEventSafeStop && m_hEventSafeStop != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hEventSafeStop);
	}
	m_bRequestSafeExit = false;
	return bRes;
}

bool CG2XThread::SafeStop()
{
	if(m_hThread == INVALID_HANDLE_VALUE || m_hThread == 0)
	{
		ErrorReport("call CG2XThread::SafeStop() error: thread handle is invalid");
		return false;
	}
	SetRequestSafeExit();
	if (m_hEventSafeStop != INVALID_HANDLE_VALUE && m_hEventSafeStop != NULL)
	{
		WaitForSingleObject(m_hEventSafeStop, (DWORD)m_nSafeTimeOut);
	}
	m_hThread = NULL;
	m_hEventSafeStop = NULL;
	return true;
}

unsigned long CG2XThread::ThreadEntry(void* pParam)
{
	unsigned nRes = 0;
	//try
	//{
	CoInitialize(NULL);
	CG2XThread* pThread = (CG2XThread*) pParam;

	pThread->InitInstance();
	pThread->Run();
	nRes = pThread->ExitInstance();
	//}
	//catch(...)
	//{
	//	// do nothiing
	//}
		
	CoUninitialize();
	return nRes;
}	

void CG2XThread::InitInstance()
{
	if ( m_pDelegate )
	{
		m_pDelegate->didInitInstance(this);
	}
}

unsigned CG2XThread::ExitInstance()
{
	int nRes = 0;
	if( m_pDelegate )
	{
		nRes = m_pDelegate->didExitInstance(this);
	}
	if (m_hEventSafeStop && m_hEventSafeStop != INVALID_HANDLE_VALUE)
		SetEvent(m_hEventSafeStop); // ÈÃSafeStopº¯Êý·µ»Ø
	return nRes;
}

void CG2XThread::Run()
{
	if (m_pDelegate)
	{
		m_pDelegate->didRun(this);
	}
}

void CG2XThread::ErrorReport(const char* lpszError)
{
	if (m_pDelegate)
	{
		m_pDelegate->didErrorReport(this, lpszError);
	}
}

bool CG2XThread::CanSafeExit()
{
	if (m_pDelegate)
	{
		return m_pDelegate->didCanExitThread(this);
	}
	return false;
}


void CG2XThread::SetRequestSafeExit()
{
	m_bRequestSafeExit = true;
}