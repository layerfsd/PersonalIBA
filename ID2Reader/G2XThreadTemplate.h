#pragma once

/********************************************************************
CG2XThreadTemplate<TClass,TResult>
	使用线程模板，可以指定类成员函数为线程的执行函数
wirter: guoxuxing
date: 2011-4-22
*********************************************************************/


namespace G2XUtility{

class CG2XThread;

class CG2XThreadDelegate
{
	friend class CG2XThread;
public:
	CG2XThreadDelegate(){}
	virtual~CG2XThreadDelegate(){}

protected:
	virtual void didInitInstance(CG2XThread* pThread); //{}
	virtual int didExitInstance(CG2XThread* pThread); //{return 0;}
	virtual void didRun(CG2XThread* pThread); //{}
	virtual void didErrorReport(CG2XThread* pThread, const char* lpszError); //{}

	// 当需要判断是否可以安全退出线程时调用
	// 同时CG2XThreadDelegate的所有者需要重载didCanExitThread
	virtual bool didCanExitThread(CG2XThread* pThread); // { return pThread->m_bRequestSafeExit; }
};

class CG2XThread
{
	friend class CG2XThreadDelegate;
public:
	CG2XThread();
	virtual ~CG2XThread();

public:
	// 创建线程
	// pDelegate: 事件代理对象
	// bSuspend: true: 创建后暂停线程的执行
	//           false: 创建后线程立即执行
	// bMustSafeExit: 指示当对象销毁时,是否必须等待线程安全退出
	// nTimeOut: 安全退出时的等待时间
	bool Create(CG2XThreadDelegate* pDelegate, bool bSuspend=true, bool bMustSafeExit=false, int nTimeOut=-1);

	// 恢复运行
	bool Start();

	// 暂停线程的运行
	bool Pause();

	// 强制停止线程
	bool Stop(int nExitCode=0);

	// 安全退出, 该函数将一直等待线程安全结束才会返回
	// Run函数或者CG2XThreadDelegate::didRun函数中必须有调用CanSafeExit的代码，
	// 并且在CanSafeExit返回true时，退出run函数。
	// 否则SafeStop将一直等待到超时。
	bool SafeStop();

	bool CanSafeExit();

	// 通知线程需要安全退出了
	void SetRequestSafeExit();

protected:
	virtual void InitInstance();
	virtual unsigned ExitInstance();

protected:
	virtual void Run();
	virtual void ErrorReport(const char* lpszError);

private:
	static unsigned long ThreadEntry(void* pParam);

protected:
	void* m_hThread;
	void* m_hEventSafeStop;
	bool m_bMustSafeExit;
	int m_nSafeTimeOut;
	bool m_bActive;
	CG2XThreadDelegate* m_pDelegate;
	bool m_bRequestSafeExit; // 是否有请求安全退出的标志
};


template<class TClass,typename TResult=void>
class CG2XThreadTemplate
{
public:
	friend class TClass;
	CG2XThreadTemplate()
	{
		m_pObj = NULL;
		m_callback = 0;
	}
	CG2XThreadTemplate(TClass *pObj,TResult (TClass::*callback_fun)())
	{
		SetCallBack(callback_fun);
	}

	// 设置回调函数，必须为类成员函数
	void SetCallBack(TClass *pObj,TResult (TClass::*callback_fun)())
	{
		m_pObj = pObj;
		m_callback = callback_fun;
	}

protected:
	virtual int Run()
	{
		ASSERT(m_pObj);
		ASSERT(m_callback!=NULL);

		(m_pObj->*m_callback)();
		return 0;
	}
	virtual void Run()
	{
		ASSERT(m_pObj);
		ASSERT(m_callback!=NULL);

		(m_pObj->*m_callback)();
	}

private:
	TClass* m_pObj;
	TResult (TClass::*m_callback)();
};

}