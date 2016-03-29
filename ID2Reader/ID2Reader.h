// ID2Reader.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号

// CID2ReaderApp:
// 有关此类的实现，请参阅 ID2Reader.cpp
//

class CID2ReaderApp : public CWinApp
{
public:
	CID2ReaderApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();

protected:


};

extern CID2ReaderApp theApp;
