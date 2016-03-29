// TextResouseTool.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "TextResouseTool.h"
#include "TextResouseToolDlg.h"

#include "GxxString.h"
#include ".\textresousetool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTextResouseToolApp

BEGIN_MESSAGE_MAP(CTextResouseToolApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTextResouseToolApp 构造

CTextResouseToolApp::CTextResouseToolApp()
{
	m_hAccelTable = NULL;
}


// 唯一的一个 CTextResouseToolApp 对象

CTextResouseToolApp theApp;


// CTextResouseToolApp 初始化

BOOL CTextResouseToolApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	CGxxString<TCHAR> str;
	TCHAR ch[1024] = {0};
	GetModuleFileName(m_hInstance, ch, 1024);
	str = ch;
	str.CutTail(_T('\\'));
	CString strDll = str + _T("\\StrProvider.dll");
	CString strGrc = str + _T("\\GrcEditor.grc");

	if (!CStrProviderLoader::GetInstance()->Loading(strDll, strGrc))
	{
		MessageBox(NULL,_T("GrcEditor.grc读取失败，程序无法启动"),_T("GrcEditor"),MB_OK|MB_ICONERROR);
		return FALSE;
	}

	AfxEnableControlContainer();

	m_hAccelTable = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CTextResouseToolDlg dlg;
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

BOOL CTextResouseToolApp::PreTranslateMessage(MSG* pMsg)
{
	if (m_hAccelTable) {
		if (::TranslateAccelerator(m_pMainWnd->GetSafeHwnd(), m_hAccelTable, pMsg)) {
			return(TRUE);
		}
	}

	return CWinApp::PreTranslateMessage(pMsg);
}

int CTextResouseToolApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}


//-------------------------------------------------------------------------

CGlobalParam::CGlobalParam()
{
	LoadParam();
}

void CGlobalParam::LoadParam()
{
	m_bIsNextHit = theApp.GetProfileInt(_T("UI"), _T("IsNextHit"), (int)TRUE); // 默认下一次提醒
	m_bReplace = theApp.GetProfileInt(_T("UI"), _T("Replace"), (int)TRUE); // 默认替换
	m_bMenuBefore = theApp.GetProfileInt(_T("UI"), _T("MenuBefore"), (int)TRUE); // 默认替换
	m_bCheckChange = theApp.GetProfileInt(_T("UI"), _T("CheckChange"), (int)TRUE); // 默认替换
	m_strFormatChars = theApp.GetProfileString(_T("UI"), _T("Formatchars"), _T("|;%;")); // 默认替换
	m_bForbidEditID = theApp.GetProfileInt(_T("UI"), _T("ForbidEditID"), (int)TRUE); // 默认替换
}

BOOL CGlobalParam::GetIsNextHit() const
{
	return m_bIsNextHit;
}

void CGlobalParam::SetIsNextHit(BOOL bVal)
{
	m_bIsNextHit = bVal;
	theApp.WriteProfileInt(_T("UI"), _T("IsNextHit"), bVal);
	
}

BOOL CGlobalParam::GetReplace() const
{	
	return m_bReplace;
}	

void CGlobalParam::SetReplace(BOOL bVal)
{
	m_bReplace = bVal;
	theApp.WriteProfileInt(_T("UI"), _T("Replace"), bVal);
}

void CGlobalParam::SetMenuBefore(BOOL bVal)
{
	m_bMenuBefore = bVal;
	theApp.WriteProfileInt(_T("UI"), _T("MenuBefore"), bVal);
}

BOOL CGlobalParam::GetMenuBefore() const
{
	return m_bMenuBefore;
}

BOOL CGlobalParam::GetCheckChange()
{
	return m_bCheckChange;
}

void CGlobalParam::SetCheckChange(BOOL bVal)
{
	m_bCheckChange = bVal;
	theApp.WriteProfileInt(_T("UI"), _T("CheckChange"), bVal); // 默认替换
}

CString CGlobalParam::GetFormatChars()
{
	return m_strFormatChars;
}

void CGlobalParam::SetFormatChars(CString strVal)
{
	m_strFormatChars = strVal;
	theApp.WriteProfileString(_T("UI"), _T("Formatchars"), strVal); // 默认替换
}

BOOL CGlobalParam::GetForbidEditID()
{
	return m_bForbidEditID;
}

void CGlobalParam::SetForbidEditID(BOOL bVal)
{
	m_bForbidEditID = bVal;
	theApp.GetProfileInt(_T("UI"), _T("ForbidEditID"), bVal); // 默认替换
}