// ID2Monitor.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号


// CID2MonitorApp:
// 有关此类的实现，请参阅 ID2Monitor.cpp
//

class CID2MonitorApp : public CWinApp
{
public:
	CID2MonitorApp();

// 重写
	public:
	virtual BOOL InitInstance();

	virtual int ExitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CID2MonitorApp theApp;
