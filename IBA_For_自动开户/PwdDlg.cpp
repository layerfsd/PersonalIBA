// PwdDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "PwdDlg.h"

// CDlgPwd 对话框

IMPLEMENT_DYNAMIC(CDlgPwd, CIBADialog)
CDlgPwd::CDlgPwd(CWnd* pParent /*=NULL*/)
: CIBADialog(CDlgPwd::IDD, pParent)
{

}

CDlgPwd::~CDlgPwd()
{
}

void CDlgPwd::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PWD, m_edtPwd);
}

BEGIN_MESSAGE_MAP(CDlgPwd, CIBADialog)
	ON_EN_CHANGE(IDC_EDIT_PWD, OnEnChangeEditPwd)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgPwd 消息处理程序

void CDlgPwd::OnEnChangeEditPwd()
{
	UpdateData();

	m_edtPwd.GetWindowText(m_strPwd);

	GetDlgItem(IDOK)->EnableWindow(!m_strPwd.IsEmpty());
}

BOOL CDlgPwd::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	m_edtPwd.SetLimitText(8);

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgPwd::OnBnClickedOk()
{
	UpdateData();

	m_edtPwd.GetWindowText(m_strPwd);

	CBCGPDialog::OnOK();
}

void CDlgPwd::OnCancel()
{
	return ;
}