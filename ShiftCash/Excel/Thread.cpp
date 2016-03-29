
#include "stdafx.h"
#include "Thread.h"


CThread::CThread()
: m_lpThread(0)
{

}
      
CThread::~CThread()
{
	Terminate(0);
}

HANDLE CThread::GetHandle() const
{
	if (m_lpThread != 0)
	{
		return m_lpThread->m_hThread;
	}

	return NULL;
}

void CThread::Start(int nPriority)
{
	m_lpThread = AfxBeginThread(ThreadFunction, (void*)this, nPriority, 0, CREATE_SUSPENDED);
	
	if (m_lpThread != 0)
	{
		//m_lpThread->m_bAutoDelete = FALSE;
		m_lpThread->ResumeThread();
	}
}

void CThread::Join()
{
	if (GetCurrentThreadId() != GetId())
	{
		Wait();
	}
}

void CThread::Wait() const
{
   if (!Wait(INFINITE))
   {
      //
   }
}

BOOL CThread::Wait(DWORD timeoutMillis) const
{
   BOOL ok = FALSE;

   DWORD result = WaitForSingleObject(GetHandle(), timeoutMillis);

   if (result == WAIT_TIMEOUT)
   {
      ok = FALSE;
   }
   else if (result == WAIT_OBJECT_0)
   {
      ok = TRUE;
   }
   
   return ok;
}

UINT CThread::ThreadFunction(void *pV)
{
	int result = 0;

	CThread* pThis = static_cast<CThread*>(pV);

	if (pThis != 0)
	{
		try
		{
			result = pThis->Run();
		}
		catch(...)
		{
			result = -1;
			//AfxMessageBox(_T("Thread Error!"));
		}
	}

	return result;
}

void CThread::Terminate( DWORD exitCode /* = 0 */)
{
	if (IsStillActive())
	{
		TerminateThread(GetHandle(), exitCode);		
		m_lpThread->Delete();
		m_lpThread = NULL;
	}

	m_lpThread = NULL;
}

BOOL CThread::IsStillActive()
{
	BOOL bRet = FALSE;

	if (m_lpThread != NULL) 
	{	
		DWORD dwExitCode = 0;

		GetExitCodeThread(GetHandle(), &dwExitCode);

		if (STILL_ACTIVE == dwExitCode)
		{
			bRet = TRUE;
		}
		else
		{
			m_lpThread = NULL;
		}
	}

	return bRet;
}

void CThread::Suspend()
{
	if (IsStillActive())
	{
		m_lpThread->SuspendThread();
	}
}

void CThread::Resume()
{
	if (IsStillActive())
	{
		m_lpThread->ResumeThread();
	}
}

DWORD CThread::GetId()
{
	if (IsStillActive())
	{
		return m_lpThread->m_nThreadID;
	}

	return NULL;
}


