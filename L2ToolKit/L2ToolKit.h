// L2ToolKit.h : L2ToolKit DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号


// CL2ToolKitApp
// 有关此类实现的信息，请参阅 L2ToolKit.cpp
//

class CL2ToolKitApp : public CWinApp
{
public:
	CL2ToolKitApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
