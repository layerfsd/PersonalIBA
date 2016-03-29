
#pragma once

class  L2_EXT_CLASS CThread 
{
public :
   
	CThread();

	virtual ~CThread();

	HANDLE GetHandle() const;

	DWORD GetId();

	void Join();

	void Wait() const;

	BOOL Wait(DWORD timeoutMillis) const;

	void Start(int nPriority = THREAD_PRIORITY_NORMAL);

	void Terminate(DWORD exitCode = 0);

	void Suspend();

	void Resume();	

	BOOL IsStillActive();


private :

	static UINT ThreadFunction(void *pV);

	CThread(const CThread &rhs);
	CThread &operator=(const CThread &rhs);

protected :

	virtual int Run() = 0;

	CWinThread* m_lpThread;
	
};

