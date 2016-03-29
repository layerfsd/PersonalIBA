// DlgCheckPwd.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgCheckPwd.h"
#include ".\dlgcheckpwd.h"


// CDlgCheckPwd 对话框

IMPLEMENT_DYNAMIC(CDlgCheckPwd, CG2XDialog)
CDlgCheckPwd::CDlgCheckPwd(CWnd* pParent /*=NULL*/)
	: CG2XDialog(CDlgCheckPwd::IDD, _T("请输入密码"), pParent)
{
}

CDlgCheckPwd::~CDlgCheckPwd()
{
}

void CDlgCheckPwd::DoDataExchange(CDataExchange* pDX)
{
	CG2XDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgCheckPwd, CG2XDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_GVWN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgCheckPwd 消息处理程序

void CDlgCheckPwd::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strPwd;
	GetDlgItemText(IDC_EDIT_PWD, strPwd);
	if(_T("delldodonew") == strPwd)
	{
		OnOK();
	}
	else
	{
		AfxMessageBox(_T("密码错误!"));
	}
}

BOOL CDlgCheckPwd::OnInitDialog()
{
	CG2XDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	GetCaptionBar()->GetSysBtnBar()->EraseID(SC_MAXIMIZE);
	GetCaptionBar()->GetSysBtnBar()->EraseID(SC_MINIMIZE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
