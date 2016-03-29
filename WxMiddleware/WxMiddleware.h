// WxMiddleware.h : WxMiddleware DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号


// CWxMiddlewareApp
// 有关此类实现的信息，请参阅 WxMiddleware.cpp
//

class CWxMiddlewareApp : public CWinApp
{
public:
	CWxMiddlewareApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	static VOID CALLBACK TimerProc( HWND hwnd, /* handle to window */ UINT uMsg, /* WM_TIMER message */ UINT_PTR idEvent, /* timer identifier */ DWORD dwTime /* current system time */ );
protected:
	UINT m_nTimerTemp;
};
