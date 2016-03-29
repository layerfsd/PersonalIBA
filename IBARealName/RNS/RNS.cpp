// RNS.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "RNS.h"
#include "RNSDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRNSApp


// CRNSApp 构造

CRNSApp::CRNSApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CRNSApp 对象

CRNSApp theApp;


// CRNSApp 初始化

BOOL CRNSApp::InitInstance()
{
	CWinApp::InitInstance();

	ParseCommandLine(m_CommandLine);

	//CRNSDlg dlg;
	//m_pMainWnd = &dlg;
	//INT_PTR nResponse = dlg.DoModal();
	//if (nResponse == IDOK)
	//{
	//	// TODO: 在此放置处理何时用“确定”来关闭
	//	//对话框的代码
	//}
	//else if (nResponse == IDCANCEL)
	//{
	//	// TODO: 在此放置处理何时用“取消”来关闭
	//	//对话框的代码
	//}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}
