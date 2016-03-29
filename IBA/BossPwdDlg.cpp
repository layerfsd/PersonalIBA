// BossPwdDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include ".\bosspwddlg.h"
#include "Encrypt.h"

// CDlgBossPwd 对话框

IMPLEMENT_DYNAMIC(CDlgBossPwd, CIBADialog)
CDlgBossPwd::CDlgBossPwd(CWnd* pParent /*=NULL*/)
: CIBADialog(CDlgBossPwd::IDD, pParent)
{
}

CDlgBossPwd::~CDlgBossPwd()
{
}

void CDlgBossPwd::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPwd);
}


BEGIN_MESSAGE_MAP(CDlgBossPwd, CIBADialog)

END_MESSAGE_MAP()


// CDlgBossPwd 消息处理程序

void CDlgBossPwd::OnOK()
{
	UpdateData();

	CString strPwdMD5;

	m_strPwd.Insert(0, _T("IBA"));

	CEncrypt::CalcMD5(m_strPwd, strPwdMD5);

	strPwdMD5.MakeUpper();
	
	CEncrypt::CalcMD5(strPwdMD5, strPwdMD5);

	strPwdMD5.MakeUpper();

	if (0 == strPwdMD5.CompareNoCase(CNetBarConfig::GetInstance()->GetBossPwd()))
	{
		CBCGPDialog::OnOK();
	}
	else
	{
		m_strPwd.Empty();
		UpdateData(FALSE);
		
		SetToolTipPos(IDC_EDIT_PASSWORD);
		ShowToolTip(IDS_PWDWRONG);

		GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
	}
}

BOOL CDlgBossPwd::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	SetEditFont();
	GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//-------------------------------------------------------------------------
// CDlgBossPwd 对话框

IMPLEMENT_DYNAMIC(CDlgBossPwdEx, CIBADialog)
CDlgBossPwdEx::CDlgBossPwdEx(CWnd* pParent /*=NULL*/)
: CIBADialog(CDlgBossPwdEx::IDD, pParent)
, m_strRemark(_T(""))
{
}

CDlgBossPwdEx::~CDlgBossPwdEx()
{
}

void CDlgBossPwdEx::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPwd);
}


BEGIN_MESSAGE_MAP(CDlgBossPwdEx, CIBADialog)

END_MESSAGE_MAP()


// CDlgBossPwd 消息处理程序

void CDlgBossPwdEx::OnOK()
{
	UpdateData();

	CString strPwdMD5;

	m_strPwd.Insert(0, _T("IBA"));

	CEncrypt::CalcMD5(m_strPwd, strPwdMD5);

	strPwdMD5.MakeUpper();

	CEncrypt::CalcMD5(strPwdMD5, strPwdMD5);

	strPwdMD5.MakeUpper();

	if (0 == strPwdMD5.CompareNoCase(CNetBarConfig::GetInstance()->GetBossPwd()))
	{
		CBCGPDialog::OnOK();
	}
	else
	{
		m_strPwd.Empty();
		UpdateData(FALSE);

		SetToolTipPos(IDC_EDIT_PASSWORD);
		ShowToolTip(IDS_PWDWRONG);

		GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
	}
}

BOOL CDlgBossPwdEx::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	if (!m_strRemark.IsEmpty())
	{
		GetDlgItem(IDC_STATIC_MODE)->SetWindowText(m_strRemark);
	}
	if (!m_strCaption.IsEmpty())
	{
		GetDlgItem(IDC_STATIC_CAPTION)->SetWindowText(m_strCaption);
	}

	SetEditFont();
	GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}