// GCardID_RZX.h : GCardID_RZX DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号


// CGCardID_RZXApp
// 有关此类实现的信息，请参阅 GCardID_RZX.cpp
//

class CGCardID_RZXApp : public CWinApp
{
public:
	CGCardID_RZXApp();

// 重写
public:
	virtual BOOL InitInstance();

	virtual int ExitInstance();

};


