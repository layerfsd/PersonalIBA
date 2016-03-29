// DdnPayCard.h : DdnPayCard DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号

#define WM_HOSTMSG	(WM_USER + 1)

// CDdnPayCardApp
// 有关此类实现的信息，请参阅 DdnPayCard.cpp
//

class CDdnPayCardApp : public CWinApp
{
public:
	CDdnPayCardApp();

// 重写
public:
	virtual BOOL InitInstance();

	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CDdnPayCardApp theApp;
extern bool g_bHasExit;