
#include "stdafx.h"
#include "IBA.h"
#include "MainFrm.h"
#include "IBADoc.h"
#include "IBALoginDlg.h"
#include "IBAHelpper.h"
#include "MsgBoxDlg.h"
#include "DialogManager.h"
#include "RealName\IBARealName.h"
#include "dal\UpdateDBThread.h"
#include "localcashier.h"
#include <string>
#include "CurrentNetBarUser.h"
#include ".\iba.h"
#include "Process.h"
#include "WxMiddlewareImpl.h"
#include "LocalCenter\localcenter.h"
#include "..\wxmiddleware\WxLog.h"
#include "Mailer\base64.h"
#include "mailer\mailer.h"
#include <Dbghelp.h>
#include "IBALog.h"
#include "CrashShowDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CIBAApp


CIBAApp theApp;
DWORD CIBAGlobal::s_dwStartUpTickcount = 0;
HWND CIBAGlobal::m_hCurRegisterDlg = NULL;
HWND CIBAGlobal::m_hCurCreditDlg = NULL;

UINT CIBAApp::m_nTerminateSeconds = 10;

CIBAApp::CIBAApp() 
: CBCGPWorkspace(TRUE)
, m_hinstBCGCBRes(NULL) 
, m_lpCurCashier(NULL)
, m_lpIBAView(NULL)
, m_IBAInstance(_T("Global\\SUNLIKE_IBA")) //不可以改变的实例名称
, m_hModuleSAI(NULL), m_bInitAuditInterface(false)
, m_hRes(NULL)
{
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, _T("_IBA_SUNLIKE_MUTEX_"));
	if(hMutex == NULL)
		return ;
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//IBA_LOG(_T("已经运行一个实例"));
		TerminateProcess(GetCurrentProcess(), 0);
		
	}
	CIBAGlobal::s_dwStartUpTickcount = GetTickCount();
	//2016-0310 liyajun 设置全局未处理异常过滤函数,以捕获导致崩溃的异常
	::SetUnhandledExceptionFilter(/*(PTOP_LEVEL_EXCEPTION_FILTER)&*/TopLevelUnhandledExceptionFilter);
}

CString CIBAApp::GetWorkPath() const
{
	return m_strWorkPath;
}

CCashier* CIBAApp::GetCurCashier()
{
	if (m_lpCurCashier == NULL)
	{
		/*if (CNetBarConfig::GetInstance()->GetLocalMode())
		{
			m_lpCurCashier = new CLocalCashier;
		} 
		else*/
		{
			m_lpCurCashier = new CCashier;
		}
	}

	ASSERT(m_lpCurCashier);

	return m_lpCurCashier;
}

CIBARealName* CIBAApp::GetRealName()
{
	if (m_lpRealName == NULL)
	{
		m_lpRealName = new CIBARealName;
	}

	ASSERT(m_lpRealName);

	return m_lpRealName;
}

CDialogManager* CIBAApp::GetDialogManager()
{
	if (m_lpDialogManager == NULL)
	{
		m_lpDialogManager = new CDialogManager;
	}

	ASSERT(m_lpDialogManager);

	return m_lpDialogManager;
}

/////////////////////////////////////////////////////////////////////////////
// CIBAApp initialization

BOOL CIBAApp::InitInstance()
{
	UNIT_AUTO_TRACE("CIBAApp::InitInstance()--初始化实例");

	//2015-0723 liyajun 去掉显示内部测试版本
//#ifdef IBA_INNER_TESTING
//	AfxMessageBox(L"当前启动的IBA版本为内部测试版本");
//
//#endif

	if (!InitIBAEnv())
	{
		return FALSE;
	}
	
	InitIBA();
	UpdateIBA();

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CIBADoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CIBAView));
	AddDocTemplate(pDocTemplate);
	
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if (!ProcessShellCommand(cmdInfo)) return FALSE;

	IBA_TRACE("等待登录...");

	CBrokenNetwork::LoadLocalModeStatus();

	CIBALoginDlg dlg;
	if (IDOK != dlg.DoModal())
	{
		IBA_TRACE("登录失败!");

		return FALSE;
	}

	IBA_TRACE("登录成功.");

	OnLoginOK();

	if (!CLocalCenter::GetInstance()->InitLocalCenter())
	{
		AfxMessageBox(_T("初始化本地服务器失败!在网络不稳定的情况时,客户端扣费和上机会受到影响."));
	}

	CWxMiddlewareImpl::GetInstance()->LoadWxLibray();

	/**********************************/
	//TCHAR lpFilePath[MAX_PATH];
	//RtlZeroMemory(lpFilePath,MAX_PATH);
	//GetModuleFileName(NULL,lpFilePath,MAX_PATH);
	//::PathRemoveFileSpec(lpFilePath);
	//::PathAppend(lpFilePath,_T("Crash.txt"));

	//std::string sToUser = "liyajun917@hotmail.com"; 
	//std::string sFromUser = "liyajun9@126.com";
	//std::string sSubject = "TestMail#9";/*lpSubject;*/
	//std::string sContent = "Hello,world";/*lpContent;*/
	//std::string sServer = "smtp.126.com";	
	//std::string sFromPassword = "ralphlina1988";
	//CStringA strFilePath = CT2A(lpFilePath);
	//std::string sAttach = strFilePath.GetString();

	//jwsmtp::mailer mail(sToUser.c_str(),sFromUser.c_str(),sSubject.c_str(),sContent.c_str(),sServer.c_str(), jwsmtp::mailer::SMTP_PORT, false);

	//mail.username(sFromUser);
	//mail.password(sFromPassword);
	//mail.authtype(jwsmtp::mailer::PLAIN);	

	//mail.attach(sAttach);
	//mail.send();
	//std::string sCode = mail.response();
	//CStringA strCode_A = sCode.c_str();
	//CString strCode = CA2T(strCode_A);

	//::MessageBox(NULL, strCode, _T("Crash Report!"), MB_TOPMOST | MB_ICONSTOP);
	/************************************************************************/

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CIBAApp message handlers

int CIBAApp::ExitInstance() 
{
	SetTerminateProcess(1);

	if (CNetBarConfig::GetInstance()->GetMonitorAuditProxy() == 1 && CIBAConfig::GetInstance()->GetCashRegisterID() == 1)
	{
		CNetBarConfig::GetInstance()->SetAllowOpenUser(FALSE);
	}
	L2_SAFE_DELETE(m_lpCurCashier);
	L2_SAFE_DELETE(m_lpRealName);
	L2_SAFE_DELETE(m_lpDialogManager);

	//CleanState();

	if (m_hinstBCGCBRes != NULL)
	{
		FreeLibrary(m_hinstBCGCBRes);
	}
	if(m_hRes != NULL)
	{
		FreeLibrary(m_hRes);
	}
	
	BCGCBProCleanUp();

	CL2Process MyProcess;
	MyProcess.Attach(_T("IBAHotKey.exe"));
	MyProcess.Terminate();

	WCHAR wchName[1024] = {0};
	GetModuleFileName(theApp.m_hInstance, wchName,1024);
	int nRes = CWinApp::ExitInstance();

	//if (m_bAutoRestart)
	//{
	//	ShellExecuteW(NULL,_T("open"),wchName,0,NULL,SW_SHOW);	
	//	//WinExec(".\\IBABin\\NBX.exe", SW_HIDE);
	//}	

	CL2Process id2Reader;
	id2Reader.Attach(_T("ID2Reader.exe"));
	if (id2Reader.IsRunning())
	{
		id2Reader.Terminate();
	}
	
	UnInitSunlikeAuditInterface();

	return nRes;
}

void CIBAApp::PreLoadState()
{
	//
}

BOOL CIBAApp::LoadWindowPlacement(CRect& rectNormalPosition, int& nFflags, int& nShowCmd)
{
	BOOL bRet = CBCGPWorkspace::LoadWindowPlacement(rectNormalPosition, nFflags, nShowCmd);

	nShowCmd = SW_HIDE; //为了隐藏主窗口

	return bRet;
}

INT CIBAApp::IBAMsgBox(LPCTSTR lpszPrompt, UINT nType/* = MB_OK | MB_ICONEXCLAMATION*/, BOOL bFormat)
{
	CMsgBoxDlg dlg(AfxGetMainWnd());
	dlg.SetText(lpszPrompt, bFormat);
	dlg.SetType(nType);

	return dlg.DoModal();
}

// QSCADD 2013-6-17 14:49:03 显示我们自己的对话框但是实现和AfxMessageBox一样的效果--在最顶部
INT CIBAApp::IBAMsgBox2(LPCTSTR lpszPrompt, UINT nType/* = MB_OK | MB_ICONEXCLAMATION*/, BOOL bFormat)
{
	// disable windows for modal dialog
	EnableModeless(FALSE);
	HWND hWndTop;
	HWND hWnd = CWnd::GetSafeOwner_(NULL, &hWndTop);

	// WINBUG: re-enable the parent window, so that focus is restored 
	// correctly when the dialog is dismissed.
	if (hWnd != hWndTop)
		EnableWindow(hWnd, TRUE);

	CMsgBoxDlg dlg(CWnd::FromHandle(hWnd));
	dlg.SetText(lpszPrompt, bFormat);
	dlg.SetType(nType);
	
	int nResult = dlg.DoModal();
	// re-enable windows
	if (hWndTop != NULL)
		::EnableWindow(hWndTop, TRUE);
	EnableModeless(TRUE);

	return nResult;
}

INT CIBAApp::IBAMsgBox(UINT nResID, UINT nType /*= MB_OK | MB_ICONEXCLAMATION*/, BOOL bFormat)
{
	CIBAString strTmp;

	strTmp.LoadString(nResID);

	return IBAMsgBox(strTmp, nType, bFormat);
}

void CIBAApp::InitIBA()
{
	UNIT_AUTO_TRACE("CIBAApp::InitIBA()--");

	m_hinstBCGCBRes = LoadLibrary(_T("BCGCBProResChs.dll"));    // *** - language
	m_hRes = LoadLibrary(GetWorkPath() + _T("\\IBABin\\IBARes.dll"));

	if (m_hinstBCGCBRes != NULL)
	{
		BCGCBProSetResourceHandle(m_hinstBCGCBRes);
	}


	SetRegistryKey(_T("Sunlike"));
	SetRegistryBase(_T("BCGUI"));
	
	WriteProfileString(_T("Setting"), _T("WorkPath"), GetWorkPath());

	// Initialize all Managers for usage. They are automatically constructed
	// if not yet present
	InitContextMenuManager();
	InitKeyboardManager(); //快捷键
	
	NS_DAL::CUpdateDBThread UpdateDBThread;
	UpdateDBThread.Start();
	UpdateDBThread.Wait();

	CIBAConfig::GetInstance();
	CNetBarConfig::GetInstance();
	CIBAConfig::GetInstance()->LoadConfig();

	// QSCT1
	// 2014-4-25 qsc
	// 创建sfreg目录
	CString strSFREGPath = theApp.GetWorkPath() + _T("\\IBABin\\sfreg\\");
	CIBAHelpper::MakeSurePathExists(strSFREGPath);

	strSFREGPath = theApp.GetWorkPath() + _T("\\IBABin\\sfreg\\sfreg.exe");
	CFileVersionInfo fvi;
	if (!fvi.Create(strSFREGPath))
	{
		IBA_LOG0(_T("error: 读取sfreg.exe的版本号失败"));
	}
	else
	{
		IBA_LOG(_T("sfreg.exe version :%s"), fvi.GetProductVersion());
	}
}

void CIBAApp::UpdateIBA()
{
	UNIT_TRACE("CIBAApp::UpdateIBA()");

	WritePrivateProfileString(_T("INFO"), _T("IP"), CNetBarConfig::GetInstance()->GetLocalServerIP(), _T(".\\IBABin\\NBX.ini"));
	WritePrivateProfileString(_T("INFO"), _T("LOCATION"), _T("2"), _T(".\\IBABin\\NBX.ini"));
	WinExec(".\\IBABin\\NBX.exe", SW_HIDE);
	WinExec(".\\IBABin\\IBAHotKey.exe", SW_HIDE);
}

BOOL CIBAApp::InitIBAEnv()
{
	UNIT_AUTO_TRACE("CIBAApp::InitIBAEnv()--初始化IBA环境");

	CWinApp::InitInstance();

	if (!m_IBAInstance.CreateInstance())
	{	
		m_IBAInstance.ActiveMainWnd();
		return FALSE;
	}

	m_strWorkPath = CIBAConfig::GetAppPath();
	m_strWorkPath = m_strWorkPath.Left(m_strWorkPath.ReverseFind('\\'));
	VERIFY(SetCurrentDirectory(GetWorkPath()));

	CIBAHelpper::AddEnvironmentVariable(GetWorkPath() + _T("\\IBAScan"));
	CIBAHelpper::AddEnvironmentVariable(GetWorkPath() + _T("\\IBARealName"));

	//CIBALog::GetInstance(); //实例化日志

	InitCommonControls();

	AfxEnableControlContainer();

	if (!(AfxSocketInit() && AfxOleInit()))
	{
		IBA_LOG(_T("IBA Start Error!"));

		return FALSE;
	}

#ifndef __USING_INTER_RESOURCE
	// 2011/08/17-8201-gxx: 加载字符串资源
	CString strPathDll = m_strWorkPath + _T("\\IBABin\\StrProvider.dll");
	CString strPathGrc = m_strWorkPath + _T("\\IBAConfig\\IBA.grc");
	if (!CStrProviderLoader::GetInstance()->Loading(strPathDll, strPathGrc))
	{
		MessageBox(NULL,_T("Loading IBA.grc error!"),NULL,MB_OK|MB_ICONERROR);
		return FALSE;
	}
#endif
	
	return TRUE;
}

void CIBAApp::OnLoginOK()
{
	CString strTmp;
	m_pMainWnd->GetWindowText(strTmp);
	strTmp.Insert(0, _T(" "));
	free((void*)m_pszAppName);
	m_pszAppName = _tcsdup(strTmp);

	m_IBAInstance.SetMainWnd(m_pMainWnd->GetSafeHwnd());
	m_IBAInstance.UpdateInstanceData();
	
	if (CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		//取得收银台版本号
		theApp.GetCurCashier()->DoAssetStatus();
	}

	// 2014-4-28 - qsc 任子行实名 QSCT1
	//if(CNetBarConfig::GetInstance()->GetAuditType() == CIBAGlobal::emRealnameRZX)
	if(1 == CNetBarConfig::GetInstance()->GetDomainId())
	{
		if(!PathFileExists(theApp.GetWorkPath() + _T("\\IBABin\\AuditInterface.dll")))
		{
			IBA_LOG0(_T("AuditInterface.dll不存在，会导致无法上传注册信息到任子行！"));
			IBAMsgBox2(_T("AuditInterface.dll不存在，会导致无法上传注册信息到任子行！"));
		}
	}

	if(0 != CNetBarConfig::GetInstance()->GetAuditInterface())
	{
		InitSunlikeAuditInterface();	
	}
	
	((CMainFrame*)AfxGetMainWnd())->StartIBA();

}

void CIBAApp::UpdateToolbarLastID(const CString& str,BOOL bError)
{
	CWnd* pWnd = GetMainWnd();
	pWnd->SendMessage(WM_TOOLBAR_LABEL_TEXT,(WPARAM)(LPCTSTR)str,(LPARAM)bError);
}

CActiveMemberView* CIBAApp::GetActiveMemberView()
{
	CIBAView *pIBAView = theApp.GetIBAView();
	ASSERT(pIBAView);

	CActiveMemberView *pMemberView = (CActiveMemberView*)pIBAView->GetTabControl()
		.GetTabWnd(1);

	ASSERT(pMemberView);
	ASSERT( pMemberView->IsKindOf( RUNTIME_CLASS(CActiveMemberView) ));

	return pMemberView;
}

void CIBAApp::AutoRestart()
{
	CFile file;

	CString strWorkPath = theApp.GetWorkPath();
	CString strPath = strWorkPath;
	strPath += _T("\\IBATemp\\autoiba.bat");
	char chIBA[1024] = {0};
	GetModuleFileNameA(m_hInstance, chIBA, 1024);

	if (file.Open(strPath, CFile::modeCreate | CFile::modeWrite))
	{
		std::string str;
		str += "@echo off\n ping -n 1 127.0.0.1\n start ";
		str += chIBA;
		str += "\n";

		file.Write(str.c_str(), str.size());
		
		file.Close();

		CIBALog::GetInstance()->Write(_T("Auto retart IBA.exe"));

		ShellExecute(NULL, _T("open"), _T("cmd.exe"), _T("/C ") + strPath, NULL/*strWorkPath+_T("\\IBABin\\")*/, SW_HIDE);
		TerminateProcess(GetCurrentProcess(), 0);
	}
}

CString CIBAApp::GetLastNetID() const
{
	CString strNetID;

	if ( CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		strNetID = CCurrentNetBarUser::GetInstance()->GetNetId();
	}
	else
	{
		strNetID = ((CMainFrame*)theApp.GetMainWnd())->m_strLastIdInToolbar;
	}

	return strNetID;
}


BOOL CIBAApp::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	
#ifndef __USING_INTER_RESOURCE
	
	// 2011/08/22-8201-gxx:
	// 指定版本的文字资源来自于grc 
	//
	if (nCode == CN_UPDATE_COMMAND_UI)
	{
		if (nID == ID_LABEL)
		{
			return FALSE;
		}

		CCmdUI *pCmdUI = (CCmdUI*)pExtra;
		if (pCmdUI->m_pOther != NULL && m_pMainWnd != NULL && pCmdUI->m_pOther == &((CMainFrame*)m_pMainWnd)->m_wndToolBar)
		{
			// 不对工具栏做处理
		}
		else
		{
			pCmdUI->SetText(LOAD_STRING(nID));
		}
	}
#endif

	return __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//更改IBA状态为Restore(尝试连接)
/*************************************************
Function:       //UpdateIBAStatus
Description:    //更新网络状态显示
Input:          //BOOL bTryingConnection - 开关,默认为false.true - 设置为尝试连接,false - 根据当前IBA状态更新.
*************************************************/
void CIBAApp::UpdateIBAStatus(BOOL bTryingConnection/*=FALSE*/)
{
	if (bTryingConnection)
	{
		GetMainWnd()->SendMessage(WM_UPDATE_IBA_STATUS,3,0); //"正常模式|本地模式|恢复模式|尝试连接" 3 = 尝试连接模式
	}
	else
	{
		GetMainWnd()->SendMessage(WM_UPDATE_IBA_STATUS, CIBAGlobal::NetworkStatus,0);
	}
}

bool CIBAApp::InitSunlikeAuditInterface()
{
	CString strDllSunlikeAuditName = theApp.GetWorkPath() + _T("\\IBABin\\") + _T("SunlikeAudit.dll");
	if(PathFileExists(strDllSunlikeAuditName))
	{
		// 初始化前前关闭SunlikeAuditProxy.exe,不然的话，因为动态库里面初始化时，会有一个和SunlikeAuditProxy.exe
		// 一样的互斥量，会创建失败
		CProcess proxy;
		proxy.Attach(_T("SunlikeAuditProxy.exe"));
		proxy.Terminate(3000);		// 2014-7-28 - qsc 增加等待进程退出时间

		// 2014-7-24 - qsc
		// 删除希之光实名接口启动项
		LPITEMIDLIST pidlDesktop = NULL;
		HRESULT hr = SHGetSpecialFolderLocation(NULL, CSIDL_STARTUP, &pidlDesktop);
		if (SUCCEEDED(hr))
		{
			TCHAR szPath[256] = {0};
			hr = SHGetPathFromIDList(pidlDesktop, szPath);
			if (SUCCEEDED(hr))
			{
				CString strLnkPath;
				strLnkPath.Format(_T("%s%s"), szPath, _T("\\希之光实名接口.lnk"));
				if(PathFileExists(strLnkPath))
				{
					DeleteFile(strLnkPath);
				}
			}
		}	
		
		m_hModuleSAI = ::LoadLibrary(strDllSunlikeAuditName);
		if(m_hModuleSAI)
		{	
			typedef int (__cdecl* FNInit)();
			FNInit fnInit = (FNInit)GetProcAddress(m_hModuleSAI, "Init");
			if(fnInit)
			{
				if(0 == fnInit())
				{
					m_bInitAuditInterface = true;
					return true;
				}
				else
				{
					IBA_LOG(_T("fnInit 返回失败"), strDllSunlikeAuditName);
				}
			}
			else
			{
				IBA_LOG(_T("读取 %s 初始化函数失败"), strDllSunlikeAuditName);
			}
		}
		else
		{
			IBA_LOG(_T("读取 %s 失败"), strDllSunlikeAuditName);
		}
	}
	else
	{
		IBA_LOG(_T("文件 %s 不存在"), strDllSunlikeAuditName);
	}
	IBAMsgBox(_T("加载SunlikeAudit.dll失败，点击确定退出程序！"));
	//TerminateProcess(GetCurrentProcess(), 0);

	return false;
}

bool CIBAApp::UnInitSunlikeAuditInterface()
{
	bool bFlag = false;
	if(m_bInitAuditInterface)
	{
		if(m_hModuleSAI)
		{
			typedef int (__cdecl* FNUnInitQuick)();
			FNUnInitQuick fnUnInitQuick = (FNUnInitQuick)GetProcAddress(m_hModuleSAI, "UnInitQuick");
			if(fnUnInitQuick)
			{
				if(0 == fnUnInitQuick())
				{
					bFlag = true;
				}
			}
			FreeLibrary(m_hModuleSAI);
		}
	}
	return bFlag;
}

DWORD CIBAApp::ThreadTerminateProcess( LPVOID pParam )
{
	IBA_LOG0(_T("进程强制退出1"));
	Sleep(m_nTerminateSeconds * 1000);
	IBA_LOG0(_T("进程强制退出2"));
	TerminateProcess(GetCurrentProcess(), 0);
	return 0;
}

void CIBAApp::SetTerminateProcess( UINT nSeconds )
{
	m_nTerminateSeconds = nSeconds;
	::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadTerminateProcess, NULL, 0, 0);
}

//全局未处理异常函数
LONG WINAPI CIBAApp::TopLevelUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *pExcetpion)
{
	//载入dbghelp.dll
	HMODULE module = ::LoadLibrary(_T("dbghelp.dll"));
    if (module == NULL)
	{
		DWORD dwErrCode = GetLastError();
		IBA_LOG(_T("ExpFilter: Load dbghelp.dll failed!ErrCode:%d"),dwErrCode);
		return EXCEPTION_CONTINUE_SEARCH;
	}        

	//创建Dump文件
	//TCHAR lpFilePath[MAX_PATH];
	//RtlZeroMemory(lpFilePath,MAX_PATH);
	//GetModuleFileName(NULL,lpFilePath,MAX_PATH);
	//::PathRemoveFileSpec(lpFilePath);
	//::PathAppend(lpFilePath,_T("Crash.txt"));
	TCHAR lpFilePath[] = _T("c:\\Crash.txt");


	HANDLE hFile = ::CreateFile( lpFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		DWORD dwErrCode = GetLastError();
		IBA_LOG(_T("ExpFilter: Create dmpfile in disk C failed.ErrCode:%d"),dwErrCode);
		//如果打开失败,更改写入路径为d盘或e盘
		RtlZeroMemory(lpFilePath,sizeof(lpFilePath));
		_tcscpy(lpFilePath,_T("d:\\Crash.txt"));
		hFile = ::CreateFile( lpFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(INVALID_HANDLE_VALUE == hFile)
		{
			IBA_LOG(_T("ExpFilter: Create dmpfile in disk D failed."));	
			//如果打开失败,更改写入路径为d盘或e盘
			RtlZeroMemory(lpFilePath,sizeof(lpFilePath));
			_tcscpy(lpFilePath,_T("e:\\Crash.txt"));
			hFile = ::CreateFile( lpFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if(INVALID_HANDLE_VALUE == hFile)
			{
				IBA_LOG(_T("ExpFilter: Create dmpfile in disk E failed."));
				return EXCEPTION_CONTINUE_SEARCH;
			}
		}
		//return EXCEPTION_CONTINUE_SEARCH;
	}

	MINIDUMP_EXCEPTION_INFORMATION einfo;
	einfo.ThreadId = ::GetCurrentThreadId();//线程ID
	einfo.ExceptionPointers = pExcetpion;    //异常信息指针
	einfo.ClientPointers = FALSE;
	if(FALSE == ::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, &einfo, NULL, NULL))//写入dump文件
	{
		DWORD dwErrCode = GetLastError();
		::CloseHandle(hFile);
		IBA_LOG(_T("ExpFilter: Write dmpfile failed,ErrCode:%d"),dwErrCode);
		return EXCEPTION_CONTINUE_SEARCH;
	}
	::CloseHandle(hFile);

	//获取异常代码和异常地址
	PVOID dwExpAddress = pExcetpion->ExceptionRecord->ExceptionAddress;
	DWORD dwExpCode = pExcetpion->ExceptionRecord->ExceptionCode;
	char lpSubject[256];
	char lpContent[256];
	RtlZeroMemory(lpSubject,256);
	RtlZeroMemory(lpContent,256);
	UINT nNetbarId = CNetBarConfig::GetInstance()->GetNetBarId();
	CString strBuildTime = CIBALog::GetInstance()->GetAppBuildTime();
	CStringA strBuildTimeA = CT2A(strBuildTime);
	CString strVer = CIBALog::GetInstance()->GetAppVer();
	CStringA strVerA = CT2A(strVer);
	sprintf(lpSubject,"%d(%s-Build:%s)0x%08x--0x%08x",nNetbarId,strVerA,strBuildTimeA,dwExpCode,(DWORD)dwExpAddress);//邮件标题改为网吧ID+buildTime+ 异常代码+地址
	sprintf(lpContent,"NetbarId:%d\nDate:%s\nTime:%s\nExceptionCode:0x%08x\nExcetpionAddress:0x%08x\n",
		(int)2824,
		__DATE__,
		__TIME__,
		dwExpCode,
		(DWORD)dwExpAddress);

	std::string sToUser = "liyajun917@hotmail.com"; 
	std::string sFromUser = "liyajun9@126.com";
	std::string sSubject = lpSubject;//主题
	std::string sContent = lpContent;//内容
	std::string sServer = "smtp.126.com";	
	std::string sFromPassword = "ralphlina1988";
	CStringA strFilePath = CT2A(lpFilePath);
	std::string sAttach = strFilePath.GetString();

	jwsmtp::mailer mail(sToUser.c_str(),sFromUser.c_str(),sSubject.c_str(),sContent.c_str(),sServer.c_str(), 
		jwsmtp::mailer::SMTP_PORT, false);

	mail.username(sFromUser);
	mail.password(sFromPassword);
	mail.authtype(jwsmtp::mailer::PLAIN);	
	mail.attach(sAttach);
	mail.send();
	std::string sCode = mail.response();
	CStringA strCode_A = sCode.c_str();
	CString strCode = CA2T(strCode_A);

	CCrashShowDlg dlg;
	dlg.DoModal();
	IBA_LOG(_T("MailSend status:%s"),strCode);

	return EXCEPTION_EXECUTE_HANDLER;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
CIBAGlobal::emNetworkStatus CIBAGlobal::NetworkStatus = CIBAGlobal::emNetworkNormal;

CString CIBAGlobal::GetOperationName(int nOptType)
{
	//(NULL)|充值|退款|开户|换机|自动退款|会员结账|包时|退余额|冲正|修改充值|本地开户|本地用户退款|本地充值|本地退款|本地结账|包房结账|上报本地充值
	CIBAString strNames;
	CStringArray strArr;
	strNames.LoadString(IDS_OPT_TYPE_NAMES);
	CIBAHelpper::SplitLine(strNames,strArr);
	IBA_ASSERT2(strArr.GetCount()==emOpTypeCount,"操作类型字符串格式有改动");

	if (nOptType<0 || nOptType>=emOpTypeCount)
	{
		IBA_ASSERT2(0,"无效的操作类型");
		return _T("");
	}

	return strArr.GetAt(nOptType);

}


