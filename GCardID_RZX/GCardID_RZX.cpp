// GCardID_RZX.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "GCardID_RZX.h"

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


// CGCardID_RZXApp 构造

CGCardID_RZXApp::CGCardID_RZXApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CGCardID_RZXApp 对象

CGCardID_RZXApp theApp;

// CGCardID_RZXApp 初始化

BOOL CGCardID_RZXApp::InitInstance()
{
	CWinApp::InitInstance();

	WCHAR ch[512] = {0};
	GetModuleFileNameW(this->m_hInstance, ch, 512);
	GXX_LOG_CREATE(ch, 1);

	GXX_LOG_HIT("InitInstance()");

	return TRUE;
}

int CGCardID_RZXApp::ExitInstance()
{
	GXX_LOG_HIT("ExitInstance()");
	return CWinApp::ExitInstance();
}