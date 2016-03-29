// WxMiddleware.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "WxMiddleware.h"

#include "ConnNotice.h"
#include "MainWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	注意！
//
//		如果此 DLL 动态链接到 MFC
//		DLL，从此 DLL 导出并
//		调入 MFC 的任何函数在函数的最前面
//		都必须添加 AFX_MANAGE_STATE 宏。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CWxMiddlewareApp

BEGIN_MESSAGE_MAP(CWxMiddlewareApp, CWinApp)
END_MESSAGE_MAP()


// CWxMiddlewareApp 构造

CWxMiddlewareApp::CWxMiddlewareApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_nTimerTemp = 0;
}


// 唯一的一个 CWxMiddlewareApp 对象

CWxMiddlewareApp theApp;
CMainWnd theMainWnd;


// CWxMiddlewareApp 初始化

BOOL CWxMiddlewareApp::InitInstance()
{
	CWinApp::InitInstance();

	//::MessageBox(NULL,_T("load dll!"), NULL, MB_OK);

	LPCTSTR lpszClassName=_T("IBATempTempWindow");
	WNDCLASS wcs = {0};
	wcs.lpszClassName = lpszClassName;
	wcs.cbClsExtra = 0;
	wcs.cbWndExtra = 0;
	wcs.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcs.hCursor = ::LoadCursor(NULL,IDC_ARROW);
	wcs.hIcon = NULL;
	wcs.hInstance = AfxGetInstanceHandle();
	wcs.lpfnWndProc = AfxWndProc;
	wcs.lpszMenuName = NULL;
	wcs.style = CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW;
	if (!AfxRegisterClass(&wcs))
		AfxThrowResourceException();

	theMainWnd.CreateEx(WS_EX_TOOLWINDOW,lpszClassName,NULL,0,CRect(0,0,0,0), NULL, 0 ,NULL);
	this->m_pMainWnd = &theMainWnd;

	m_nTimerTemp = SetTimer(NULL, 0, 10, TimerProc);

	return TRUE;
}

VOID CALLBACK CWxMiddlewareApp::TimerProc(
							   HWND hwnd,         // handle to window
							   UINT uMsg,         // WM_TIMER message
							   UINT_PTR idEvent,  // timer identifier
							   DWORD dwTime       // current system time
							   )
{
	if (idEvent == theApp.m_nTimerTemp)
	{
		if (CConnNotice::GetInstance()->GetWxMiddlewareDelegate() != NULL) 
		{
			KillTimer(NULL, idEvent);

			UINT d,n;
			CConnNotice::GetInstance()->GetWxMiddlewareDelegate()->getNetbarIdAndDomainId(&n, &d);

			CConnNotice::GetInstance()->ConnectNotice();
		}
	}
}

void WINAPI SetDelegate(CWxMiddlewareDelete* pDelegate)
{
	CConnNotice::GetInstance()->SetWxMiddlewareDelegate(pDelegate);
}

void WINAPI LoginStatus(LPCTSTR lpszTermId, int nStatus)
{
	CConnNotice::GetInstance()->SendClientLoginRet(lpszTermId, nStatus);
}

void WINAPI LogoutStatus(LPCTSTR lpszTermId, int nStatus)
{
	CConnNotice::GetInstance()->SendClientLogoutRet(lpszTermId, nStatus);
}

void WINAPI MessageStatus(int nStatus)
{
	CConnNotice::GetInstance()->SendClientMessageRet(nStatus);
}