#pragma once

#include "WxMiddlewareDelegate.h"
#include <vector>
#include "common\ThreadTemplate.h"

class CWxMiddlewareImpl : public CWxMiddlewareDelete
{
public:
	static CWxMiddlewareImpl* GetInstance();

	void LoadWxLibray();

protected:
	CWxMiddlewareImpl();
	virtual ~CWxMiddlewareImpl();

protected:
	virtual void getNetbarIdAndDomainId(UINT * netbarId, UINT * domainId);
	virtual BOOL getMiddlewareCenterIPPort(TCHAR* ip, UINT * port);
	virtual BOOL checkin(LPCTSTR lpszNetId, LPCTSTR lpszTermId, LPCTSTR lpszPassword);
	virtual BOOL checkout(LPCTSTR lpszNetId, LPCTSTR lpszTermId);
	virtual void showmsg(LPCTSTR lpszTermId, LPCTSTR lpszUserID, LPCTSTR lpszPayAmount, LPCTSTR lpszGiveAmount, LPCTSTR lpszTime);

protected:
	void Run();

public:
	void CallSetDelegate();
	void CallLoginStatus(LPCTSTR lpszTermId, int nStatus);
	void CallLogoutStatus(LPCTSTR lpszTermId, int nStatus);
	void CallMessageStatus(int nStatus);

private:
	HMODULE m_hMod;
	//struct CheckinInfo
	//{
	//	CString strTermId;
	//	CString strNetId;
	//	UINT_PTR nTimerId;
	//};
	struct OperationInfo
	{
		CString strTermId;
		CString strNetId;
		INT nOperate;		// 0:checkin  1:checkout 2:message
		UINT_PTR nTimerId;
	};
	//std::vector<CheckinInfo> m_termsArray;
	std::vector<OperationInfo> m_termsArray;
	NS_COMMON::CThreadTemplate<CWxMiddlewareImpl> m_thread;

	typedef void (WINAPI* fnSetDelegateProc) (CWxMiddlewareDelete*);
	typedef void (WINAPI* fnLoginStatusProc) (LPCTSTR , int);
	typedef void (WINAPI* fnLogoutStatusProc) (LPCTSTR, int);
	typedef void (WINAPI* fnMessageStatusProc) (int);
	static VOID CALLBACK TimerProc( HWND hwnd, /* handle to window */ UINT uMsg, /* WM_TIMER message */ UINT_PTR idEvent, /* timer identifier */ DWORD dwTime /* current system time */ );
	fnSetDelegateProc m_procSetDelegate;
	fnLoginStatusProc m_procLoginStatus;
	fnLogoutStatusProc m_procLogoutStatus;
	fnMessageStatusProc m_procMessageStatus;
};