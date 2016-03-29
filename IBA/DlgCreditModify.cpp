// DlgCreditModify.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "DlgCreditModify.h"
#include ".\dlgcreditmodify.h"


// CDlgCreditModify 对话框

IMPLEMENT_DYNAMIC(CDlgCreditModify, CIBADialog)
CDlgCreditModify::CDlgCreditModify(CWnd* pParent /*=NULL*/)
	: CIBADialog(CDlgCreditModify::IDD, pParent)
{
	m_strCreditMoney = _T("0.0");
	m_strNewMoney = _T("0.0");
}

CDlgCreditModify::~CDlgCreditModify()
{
}

void CDlgCreditModify::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_STATIC_CURRENT, m_strCreditMoney);
	DDX_Text(pDX, IDC_EDIT_REAL, m_strNewMoney);
	DDX_Control(pDX, IDC_EDIT_REAL, m_editNewMoney);
	CIBADialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgCreditModify, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgCreditModify 消息处理程序

BOOL CDlgCreditModify::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	UpdateData(FALSE);

	m_editNewMoney.SetLimitText(6);
	m_editNewMoney.SetValidChars(_T("0.123456789"));

	m_editNewMoney.SetFocus();
	m_editNewMoney.SetSel(0,m_strNewMoney.GetLength());

	return FALSE;  
}

void CDlgCreditModify::OnBnClickedOk()
{
	UpdateData(TRUE);

	double dOld = _tstof(m_strCreditMoney);
	double dNew = _tstof(m_strNewMoney);

	if (dNew<0.01)
	{
		theApp.IBAMsgBox(_T("请输入有效的修正金额"));
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_REAL);
		pEdit->SetFocus();
		pEdit->SetSel(0,m_strNewMoney.GetLength());

		return;
	}
	if (dNew >= dOld)
	{
		theApp.IBAMsgBox(_T("修正金额必须小于误充时的金额"));
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_REAL);
		pEdit->SetFocus();
		pEdit->SetSel(0,m_strNewMoney.GetLength());
		return;
	}

	CDialog::OnOK();
}
