// DlgInputUserPwd.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "DlgInputUserPwd.h"


// CDlgInputUserPwd 对话框

IMPLEMENT_DYNAMIC(CDlgInputUserPwd, CIBADialog)
CDlgInputUserPwd::CDlgInputUserPwd(CWnd* pParent /*=NULL*/)
	: CIBADialog(CDlgInputUserPwd::IDD, pParent)
{
}

CDlgInputUserPwd::~CDlgInputUserPwd()
{
}

void CDlgInputUserPwd::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PWD, m_strPwd);
}


BEGIN_MESSAGE_MAP(CDlgInputUserPwd, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgInputUserPwd 消息处理程序

void CDlgInputUserPwd::OnBnClickedOk()
{
	UpdateData();
	CDialog::OnOK();
}
