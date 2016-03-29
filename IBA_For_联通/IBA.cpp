
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CIBAApp


CIBAApp theApp;


CIBAApp::CIBAApp() 
: CBCGPWorkspace(TRUE)
, m_hinstBCGCBRes(NULL) 
, m_lpCurCashier(NULL)
, m_lpIBAView(NULL)
, m_IBAInstance(_T("Global\\SUNLIKE_IBA")) //不可以改变的实例名称
{
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

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CIBAApp message handlers

int CIBAApp::ExitInstance() 
{
	L2_SAFE_DELETE(m_lpCurCashier);
	L2_SAFE_DELETE(m_lpRealName);
	L2_SAFE_DELETE(m_lpDialogManager);

	//CleanState();

	if (m_hinstBCGCBRes != NULL)
	{
		FreeLibrary(m_hinstBCGCBRes);
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
		CIBALog::GetInstance()->Write(_T("IBA Start Error!"));

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

void CIBAApp::UpdateIBAStatus(BOOL bTryingConnection/*=FALSE*/)
{
	if (bTryingConnection)
	{
		GetMainWnd()->SendMessage(WM_UPDATE_IBA_STATUS,3,0);
	}
	else
	{
		GetMainWnd()->SendMessage(WM_UPDATE_IBA_STATUS, CIBAGlobal::NetworkStatus,0);
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
CIBAGlobal::emNetworkStatus CIBAGlobal::NetworkStatus = CIBAGlobal::emNetworkNormal;

CString CIBAGlobal::GetOperationName(int nOptType)
{
	//(NULL)|充值|退款|开户|换机|自动退款|会员结账|包时|退余额|充值冲正|充值修正|本地开户|本地用户退款|本地充值|断网退款|断网结账
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