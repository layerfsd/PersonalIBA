// DlgRNZdCrc.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "DlgRNZdCrc.h"
#include ".\dlgrnzdcrc.h"


// CDlgRNZdCrc 对话框

IMPLEMENT_DYNAMIC(CDlgRNZdCrc, CIBADialog)
CDlgRNZdCrc::CDlgRNZdCrc(CWnd* pParent /*=NULL*/)
	: CIBADialog(CDlgRNZdCrc::IDD, pParent)
	, m_dwCRC(0)
{
}

CDlgRNZdCrc::~CDlgRNZdCrc()
{
}

void CDlgRNZdCrc::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRNZdCrc, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgRNZdCrc 消息处理程序

void CDlgRNZdCrc::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	// 2014-8-27 - qsc
	m_dwCRC = GetDlgItemInt(IDC_EDIT_CRC);
	CBCGPDialog::OnOK();
}
