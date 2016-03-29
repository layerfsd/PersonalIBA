// StrProvider.h : StrProvider DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号


// CStrProviderApp
// 有关此类实现的信息，请参阅 StrProvider.cpp
//

class CStrProviderApp : public CWinApp
{
public:
	CStrProviderApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
BOOL __stdcall LoadStringContainer(const char* lpGrcFilePath);
void __stdcall GetStringA(UINT nID, char* pchOut, const unsigned int nSize);
void __stdcall GetStringW(UINT nID, wchar_t* pchOut, const unsigned int nSize);