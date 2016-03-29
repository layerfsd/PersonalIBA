// Lock.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Lock.h"
#include "LockDlg.h"
#include ".\lock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
char singleLockMutex[] = "2353FC39-9870-4cda-BB19-C7BBB7E60D3D";

// CLockApp{}


BEGIN_MESSAGE_MAP(CLockApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CLockApp 构造

CLockApp::CLockApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CLockApp 对象

CLockApp theApp;


// CLockApp 初始化

BOOL CLockApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	HANDLE m_hMutex = CreateMutex(NULL, FALSE, singleLockMutex);
	if (m_hMutex)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			return FALSE;

		}
	}
	else
	{

		return FALSE;
	}

	m_pMainWnd = new CLockDlg();

	((CLockDlg*)m_pMainWnd)->Create(CLockDlg::IDD);

	return TRUE;
}

int CLockApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
