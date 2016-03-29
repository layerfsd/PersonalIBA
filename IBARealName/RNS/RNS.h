// RNS.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号
#include "commandlineinfo.h"

// CRNSApp:
// 有关此类的实现，请参阅 RNS.cpp
//

class CRNSApp : public CWinApp
{
public:
	CRNSApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现
	CCommandLine m_CommandLine;
};

extern CRNSApp theApp;
