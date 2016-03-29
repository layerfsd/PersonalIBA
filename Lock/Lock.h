// Lock.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号
#define WM_CHECKPASSWORD		(WM_APP + 354)

// CLockApp:
// 有关此类的实现，请参阅 Lock.cpp
//

class CLockApp : public CWinApp
{
public:
	CLockApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CLockApp theApp;
