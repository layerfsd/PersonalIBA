// SEATSETDLG.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "SeatSetPage.h"
#include ".\seatsetpage.h"


// CSetPageOfSeat 对话框

IMPLEMENT_DYNAMIC(CSetPageOfSeat, CIBAPage)
CSetPageOfSeat::CSetPageOfSeat()
: CIBAPage(CSetPageOfSeat::IDD)
, m_strBitmapPath(_T(""))
{
}

CSetPageOfSeat::~CSetPageOfSeat()
{
}

void CSetPageOfSeat::DoDataExchange(CDataExchange* pDX)
{
	CIBAPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_BIYMAPPATH, m_strBitmapPath);
}


BEGIN_MESSAGE_MAP(CSetPageOfSeat, CIBAPage)
	ON_BN_CLICKED(IDC_BUTTON_BROWSER, OnBnClickedButtonBrowser)
END_MESSAGE_MAP()


// CSetPageOfSeat 消息处理程序

void CSetPageOfSeat::OnBnClickedButtonBrowser()
{
	CFileDialog dlg(TRUE, _T("bmp"), NULL, 0, _T("BMP Files (*.bmp)|*.bmp|JPG Files(*.jpg)|*.jpg|PNG Files(*.png)|*.png|"));
	if (dlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_BIYMAPPATH, dlg.GetPathName());
		CIBAConfig::GetInstance()->SetSeatBmpPath(dlg.GetPathName());
	}
}

BOOL CSetPageOfSeat::OnInitDialog()
{
	CIBAConfig::GetInstance()->LoadConfig();
	m_strBitmapPath = CIBAConfig::GetInstance()->GetSeatBmpPath();

	CIBAPage::OnInitDialog();//已经更新了数据到管理的变量

	GetDlgItem(IDC_EDIT_BIYMAPPATH)->SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSetPageOfSeat::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	UpdateData(TRUE);
	CIBAConfig::GetInstance()->SetSeatBmpPath(m_strBitmapPath);
	CIBAConfig::GetInstance()->SaveConfig();

	CIBAPage::OnOK();
}
