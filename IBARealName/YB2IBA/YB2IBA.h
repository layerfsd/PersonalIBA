// YB2IBA.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号


// CYB2IBAApp:
// 有关此类的实现，请参阅 YB2IBA.cpp
//

class CYB2IBAApp : public CWinApp
{
public:
	CYB2IBAApp();

// 重写
public:
	virtual BOOL InitInstance();

	HANDLE m_hMutex;

// 实现
	BOOL CheckSingleInstance();
};

extern CYB2IBAApp theApp;
