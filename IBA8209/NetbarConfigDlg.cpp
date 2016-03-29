// NetbarConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include ".\NetbarConfigDlg.h"


// CNetbarConfigDlg 对话框

IMPLEMENT_DYNAMIC(CNetbarConfigDlg, CIBADialog)
CNetbarConfigDlg::CNetbarConfigDlg(CWnd* pParent /*=NULL*/)
: CIBADialog(CNetbarConfigDlg::IDD, pParent)
{
}

CNetbarConfigDlg::~CNetbarConfigDlg()
{
}

void CNetbarConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNetbarConfigDlg, CIBADialog)
END_MESSAGE_MAP()


// CNetbarConfigDlg 消息处理程序

void CNetbarConfigDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	CIBADialog::OnOK();
}

BOOL CNetbarConfigDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
