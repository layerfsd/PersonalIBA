// DdnPayCard.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "DdnPayCard.h"
#include "PcpThread.h"
#include "DdnPcpInc.h"
#include "MD5.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	注意！
//
//		如果此 DLL 动态链接到 MFC
//		DLL，从此 DLL 导出并
//		调入 MFC 的任何函数在函数的最前面
//		都必须添加 AFX_MANAGE_STATE 宏。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CDdnPayCardApp

BEGIN_MESSAGE_MAP(CDdnPayCardApp, CWinApp)
END_MESSAGE_MAP()


// CDdnPayCardApp 构造

CDdnPayCardApp::CDdnPayCardApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CDdnPayCardApp 对象

CDdnPayCardApp theApp;


// CDdnPayCardApp 初始化

BOOL CDdnPayCardApp::InitInstance()
{
	CWinApp::InitInstance();

	
	WCHAR chPath[512] = {0};
	GetModuleFileNameW(this->m_hInstance,chPath,512);
	CStringW strFilew = chPath;
	strFilew = strFilew.Left(strFilew.ReverseFind(_T('\\')));
	strFilew += _T("\\CardLog\\");

	_wmkdir(strFilew);

	GXX_LOG_CREATE(strFilew);

	G2XStartup();

	return TRUE;
}

int CDdnPayCardApp::ExitInstance()
{
	G2XShutdown();
	return CWinApp::ExitInstance();
}
//******************************************************************************

//公共函数

LONG WINAPI DdnGetTime()
{
	time_t tNow = 0;

	time(&tNow);

	return (LONG)tNow;
}

void WINAPI DdnMD5String(LPCTSTR lpszString, LPTSTR lpszDest, DWORD dwLen)
{
	CString strMD5 = CMD5::MD5String(lpszString);

	lstrcpyn(lpszDest, strMD5.GetString(), min(dwLen, (DWORD)strMD5.GetLength()) + 1);
}

void WINAPI DdnMD5File(LPCTSTR lpszFileName, LPTSTR lpszDest, DWORD dwLen)
{
	CString strMD5 = CMD5::MD5File(lpszFileName);

	lstrcpyn(lpszDest, strMD5.GetString(), min(dwLen, (DWORD)strMD5.GetLength()) + 1);
}

//******************************************************************************

// 1.1版本的接口函数

BOOL WINAPI DdnInitializePcpA(LPCSTR lpszParams)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	GXX_LOG_HIT(_T("Call Fun: DdnInitializePcpA"));
	CPcpThread::CreatePcp(lpszParams);

	return TRUE;
}

BOOL WINAPI DdnInitializePcpW(LPCWSTR lpszParams)
{
	return DdnInitializePcpA(CW2A(lpszParams));
}

BOOL WINAPI DdnShowPcp()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	GXX_LOG_HIT(_T("Call Fun: DdnShowPcp"));
	return CPcpThread::ShowPcp();
}

BOOL WINAPI DdnHidePcp()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return CPcpThread::HidePcp();
}

BOOL WINAPI DdnTerminatePcp()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return CPcpThread::DestoryPcp();
}

//******************************************************************************

// 1.0版本的接口函数

void WINAPI CreatePayCardPlatformA(LPCSTR lpszParams)
{
	DdnInitializePcpA(lpszParams);

	Sleep(100);

	DdnShowPcp();
}

void WINAPI CreatePayCardPlatformW(LPCWSTR lpszParams)
{
	CreatePayCardPlatformA(CW2A(lpszParams));
}

void WINAPI DestroyPayCardPlatform()
{
	DdnTerminatePcp();
}

//******************************************************************************
