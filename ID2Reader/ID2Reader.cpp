// ID2Reader.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "ID2Reader.h"
#include "ID2ReaderDlg.h"
#include ".\id2reader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CID2ReaderApp

BEGIN_MESSAGE_MAP(CID2ReaderApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CID2ReaderApp 构造

CID2ReaderApp::CID2ReaderApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CID2ReaderApp 对象

CID2ReaderApp theApp;


// CID2ReaderApp 初始化

BOOL CID2ReaderApp::InitInstance()
{
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, _T("_ID2READER_MUTEX_"));
	if(hMutex == NULL)
		return FALSE;
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		return FALSE;
	}

	HANDLE hMutex2 = ::CreateMutex(NULL, FALSE, _T("_ID2READER_ALLOW_MUTEX_"));
	if(hMutex2 == NULL)
		return FALSE;

	if(GetLastError() != ERROR_ALREADY_EXISTS)
	{
		// 不允许运行
		return FALSE;
	}
	else
	{
		CloseHandle(hMutex2);
	}


	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	char chPath[512];
	::GetModuleFileName(NULL, chPath, 512);
	GXX_LOG_CREATE(CA2W(chPath), 3); // 创建日志环境，保留三天的日志

	GXX_LOG_HIT("-------------------------------------");
	GXX_LOG_HIT("- InitInstance");
	GXX_LOG_HIT("-------------------------------------");
	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	

	CID2ReaderDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用“确定”来关闭
		//对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}

int CID2ReaderApp::ExitInstance()
{
	GXX_LOG_HIT("DodoReader Exit!");

	return CWinApp::ExitInstance();
}


