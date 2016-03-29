// YB2IBA.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "YB2IBA.h"
#include "YB2IBADlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CYB2IBAApp 构造

CYB2IBAApp::CYB2IBAApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CYB2IBAApp 对象

CYB2IBAApp theApp;

BOOL CYB2IBAApp::CheckSingleInstance()
{
	const TCHAR singleMutex[] = _T("IBAREALNAME_YB2IBA");

	m_hMutex = ::CreateMutex(NULL, FALSE, singleMutex);
	
	if (m_hMutex != NULL)
	{
		if (::GetLastError() == ERROR_ALREADY_EXISTS)
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

// CYB2IBAApp 初始化

BOOL CYB2IBAApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!CheckSingleInstance())
	{
		return FALSE;
	}

	CString strCmdLine(m_lpCmdLine);
	
	if (0 != strCmdLine.CompareNoCase(_T("YBWW")))
	{
		return FALSE;
	}
	 
	AfxSocketInit();

	CYB2IBADlg* pDlg = new CYB2IBADlg;
	pDlg->Create(CYB2IBADlg::IDD);
	pDlg->ShowWindow(SW_HIDE);

	m_pMainWnd = pDlg;

	return TRUE;
}
