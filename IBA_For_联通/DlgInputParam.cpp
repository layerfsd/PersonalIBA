// DlgInputParam.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "DlgInputParam.h"
#include ".\dlginputparam.h"


// CDlgInputParam 对话框

IMPLEMENT_DYNAMIC(CDlgInputParam, CIBADialog)
CDlgInputParam::CDlgInputParam(CWnd* pParent /*=NULL*/)
	: CIBADialog(CDlgInputParam::IDD, pParent)
	, m_strParam(_T(""))
	, m_strTitle(_T(""))
{
}

CDlgInputParam::~CDlgInputParam()
{
}

void CDlgInputParam::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strParam);
	DDX_Text(pDX, IDC_STATIC_TXT, m_strTitle);
}


BEGIN_MESSAGE_MAP(CDlgInputParam, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void CDlgInputParam::InitParam(CString strCaption,CString strTitle,CString strParam/*=_T("")*/)
{
	m_strCaption = strCaption;
	m_strTitle = strTitle;
	m_strParam = strParam;
}

// CDlgInputParam 消息处理程序

BOOL CDlgInputParam::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	SetWindowText(m_strCaption);

	UpdateData(FALSE);

	return TRUE;  
}

void CDlgInputParam::OnBnClickedOk()
{
	UpdateData();

	CDialog::OnOK();
}
