// DlgSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "TextResouseTool.h"
#include "DlgSetting.h"
#include ".\dlgsetting.h"

// CDlgSetting 对话框

IMPLEMENT_DYNAMIC(CDlgSetting, CDialog)
CDlgSetting::CDlgSetting(CWnd* pParent /*=NULL*/)
	: CStringLoaderDlg<CDialog>(CDlgSetting::IDD, pParent)
	, m_bReplace(FALSE)
	, m_bMenuBefore(FALSE)
	, m_bCheckChange(FALSE)
	, m_strFmtChars(_T(""))
	, m_bForbidEditID(FALSE)
{
}

CDlgSetting::~CDlgSetting()
{
}

void CDlgSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_REPLACE, m_bReplace);
	DDX_Check(pDX, IDC_CHECK_ORDER, m_bMenuBefore);
	DDX_Check(pDX, IDC_CHECK_CHANGE, m_bCheckChange);
	DDX_Text(pDX, IDC_EDIT_FMT_CHARS, m_strFmtChars);
	DDX_Check(pDX, IDC_CHECK_FORBID_ID, m_bForbidEditID);
}


BEGIN_MESSAGE_MAP(CDlgSetting, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_CHANGE, OnBnClickedCheckChange)
END_MESSAGE_MAP()


// CDlgSetting 消息处理程序

void CDlgSetting::OnBnClickedButton1()
{
	UpdateData();

	CGlobalParam::GetInstance()->SetReplace(m_bReplace);
	CGlobalParam::GetInstance()->SetMenuBefore(m_bMenuBefore);
	CGlobalParam::GetInstance()->SetCheckChange(m_bCheckChange);
	CGlobalParam::GetInstance()->SetFormatChars(m_strFmtChars);
	CGlobalParam::GetInstance()->SetForbidEditID(m_bForbidEditID);
}

void CDlgSetting::OnBnClickedOk()
{
	OnBnClickedButton1();
	OnOK();
}

BOOL CDlgSetting::OnInitDialog()
{
	CStringLoaderDlg<CDialog>::OnInitDialog();

	m_bReplace = CGlobalParam::GetInstance()->GetReplace();
	m_bMenuBefore = CGlobalParam::GetInstance()->GetMenuBefore();
	m_bCheckChange = CGlobalParam::GetInstance()->GetCheckChange();
	m_strFmtChars = CGlobalParam::GetInstance()->GetFormatChars();
	m_bForbidEditID = CGlobalParam::GetInstance()->GetForbidEditID();

	UpdateData(FALSE);

	return TRUE;  
}

void CDlgSetting::OnBnClickedCheckChange()
{
	UpdateData();

	if (m_bCheckChange)
	{
		GetDlgItem(IDC_EDIT_FMT_CHARS)->EnableWindow(TRUE);
	}
	else	
	{
		GetDlgItem(IDC_EDIT_FMT_CHARS)->EnableWindow(FALSE);
	}
}
