// ID2Monitor.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "ID2Monitor.h"
#include "ID2MonitorDlg.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CID2MonitorApp

BEGIN_MESSAGE_MAP(CID2MonitorApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CID2MonitorApp 构造

CID2MonitorApp::CID2MonitorApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CID2MonitorApp 对象

CID2MonitorApp theApp;


// CID2MonitorApp 初始化

BOOL CID2MonitorApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	CString strPath;
	GetModuleFileName(NULL,strPath.GetBuffer(255),255);
	strPath.ReleaseBuffer();

	strPath = strPath.Left(strPath.ReverseFind(_T('\\')));

	CString strEvnPath = strPath;

	strPath += _T("\\ID2MonitorLog\\");

	_mkdir(strPath);
	GXX_LOG_CREATE(CT2W(strPath),7);

	GXX_LOG_HIT("InitInstance------------ID2MonitorV1.0.0.25");

	CString strTmp;

	strTmp.GetEnvironmentVariable(_T("Path"));

	if (strTmp.Right(1) != _T(";"))
	{
		strTmp.AppendChar(';');
	}

	strTmp.Append(strEvnPath+"\\Scan");

	if (SetEnvironmentVariable(_T("Path"), strTmp))
	{
		GXX_LOG_HIT("设置环境变量成功");
	}
	else
	{
		GXX_LOG_HIT("设置环境变量失败!");
	}

	

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CID2MonitorDlg dlg;
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

int CID2MonitorApp::ExitInstance()
{
	GXX_LOG_HIT("ExitInstance-------------");

	return CWinApp::ExitInstance();
}