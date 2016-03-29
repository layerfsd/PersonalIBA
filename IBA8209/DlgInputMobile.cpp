// DlgInputMobile.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "DlgInputMobile.h"
#include ".\dlginputmobile.h"


// CDlgInputMobile 对话框

IMPLEMENT_DYNAMIC(CDlgInputMobile, CIBADialog)
CDlgInputMobile::CDlgInputMobile(CWnd* pParent /*=NULL*/)
	: CIBADialog(CDlgInputMobile::IDD, pParent)
{
}

CDlgInputMobile::~CDlgInputMobile()
{
}

void CDlgInputMobile::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	CIBADialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgInputMobile, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgInputMobile 消息处理程序

void CDlgInputMobile::OnBnClickedOk()
{
	// 判断是否为有效的手机号码
	CString strNumbers;
	m_edit.GetWindowText(strNumbers);
	if (strNumbers.IsEmpty())
	{
		MessageBox(_T("请输入手机号码!"),_T("提示"),MB_OK|MB_ICONWARNING);
		m_edit.SetFocus();
		return;
	}
	if (strNumbers.GetLength() != 11)
	{
		MessageBox(_T("请输入有效的手机号码!"),_T("提示"),MB_OK|MB_ICONWARNING);
		m_edit.SetSel(0, strNumbers.GetLength());
		m_edit.SetFocus();
		return;
	}
	if (strNumbers.GetAt(0) != _T('1'))
	{
		MessageBox(_T("请输入有效的手机号码!"),_T("提示"),MB_OK|MB_ICONWARNING);
		m_edit.SetSel(0, strNumbers.GetLength());
		m_edit.SetFocus();
		return;
	}
	m_strPhoneNumber = strNumbers;
	CDialog::OnOK();
}

BOOL CDlgInputMobile::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	m_edit.SetLimitText(11);

	m_edit.SetFocus();

	return FALSE;  
}
