// UnlockScreenDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include ".\UnlockScreenDlg.h"


// CUnlockScreenDlg 对话框

IMPLEMENT_DYNAMIC(CUnlockScreenDlg, CIBADialog)
CUnlockScreenDlg::CUnlockScreenDlg(CWnd* pParent /*=NULL*/)
: CIBADialog(CUnlockScreenDlg::IDD, pParent)
{
}

CUnlockScreenDlg::~CUnlockScreenDlg()
{
}

void CUnlockScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPwd);
}


BEGIN_MESSAGE_MAP(CUnlockScreenDlg, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CUnlockScreenDlg 消息处理程序

BOOL CUnlockScreenDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	((CEdit*)GetDlgItem(IDC_EDIT_PASSWORD))->SetLimitText(30);
	((CEdit*)GetDlgItem(IDC_EDIT_PASSWORD))->SetPasswordChar('*');
	GetDlgItem(IDC_EDIT_PASSWORD)->SetFont(&m_Font);
	GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CUnlockScreenDlg::OnBnClickedOk()
{
	UpdateData();

	if (m_strPwd.IsEmpty())
	{
		ShowToolTip(IDS_INPUTERROR, IDC_EDIT_PASSWORD);
	}
	else
	{
		CBCGPDialog::OnOK();
	}
}

void CUnlockScreenDlg::OnBnClickedCancel()
{
	CBCGPDialog::OnCancel();
}
