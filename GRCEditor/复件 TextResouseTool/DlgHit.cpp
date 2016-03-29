// DlgHit.cpp : 实现文件
//

#include "stdafx.h"
#include "TextResouseTool.h"
#include "DlgHit.h"
#include ".\dlghit.h"


// CDlgHit 对话框

IMPLEMENT_DYNAMIC(CDlgHit, CDialog)
CDlgHit::CDlgHit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHit::IDD, pParent)
	, m_bNextNotHit(FALSE)
{
}

CDlgHit::~CDlgHit()
{
}

void CDlgHit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_bNextNotHit);
}


BEGIN_MESSAGE_MAP(CDlgHit, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgHit 消息处理程序

BOOL CDlgHit::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	HICON hIcon = ::LoadIcon(NULL, IDI_INFORMATION);

	CStatic *pStcIcon = (CStatic*)GetDlgItem(IDC_STATIC_PIC);
	pStcIcon->SetIcon(hIcon);

	return TRUE;  
}

void CDlgHit::OnBnClickedOk()
{
	UpdateData();

	CGlobalParam::GetInstance()->SetIsNextHit(!m_bNextNotHit);
	CGlobalParam::GetInstance()->SetReplace(TRUE);

	OnOK();
}

void CDlgHit::OnBnClickedCancel()
{
	UpdateData();
	
	CGlobalParam::GetInstance()->SetIsNextHit(!m_bNextNotHit);
	CGlobalParam::GetInstance()->SetReplace(FALSE);

	OnCancel();
}
