// CrashShowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "CrashShowDlg.h"


// CCrashShowDlg 对话框

IMPLEMENT_DYNAMIC(CCrashShowDlg, CDialog)
CCrashShowDlg::CCrashShowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCrashShowDlg::IDD, pParent)
{
}

CCrashShowDlg::~CCrashShowDlg()
{
}

void CCrashShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCrashShowDlg, CDialog)
END_MESSAGE_MAP()


// CCrashShowDlg 消息处理程序
BOOL CCrashShowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString strMsg;
	strMsg = _T("糟糕！程序崩溃了...\n\n非常抱歉给您的体验带来困扰和不便，我们的开发人员将尽快收集信息并加紧修复...");
	GetDlgItem(IDC_CRASH_MESSAGE)->SetWindowText(strMsg);

	return TRUE;
}
