// AddTerminalDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "AddTerminalDlg.h"
#include ".\addterminaldlg.h"
#include "ComputerInfo.h"
#include "Socket/LocalServer.h"


// CDlgAddTerm 对话框

IMPLEMENT_DYNAMIC(CDlgAddTerm, CIBADialog)
CDlgAddTerm::CDlgAddTerm(CWnd* pParent /*=NULL*/)
: CIBADialog(CDlgAddTerm::IDD, pParent)
,m_SeatList(NULL)
{
}

CDlgAddTerm::~CDlgAddTerm()
{
}

void CDlgAddTerm::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TREMINALIP, m_cboTerminalIP);
	DDX_Control(pDX, IDC_COMBO_TERMINALID, m_cboTerminalID);
	DDX_CBString(pDX, IDC_COMBO_TERMINALID, m_strTerminalID);
	DDX_CBString(pDX, IDC_COMBO_TREMINALIP, m_strTerminalIP);
}


BEGIN_MESSAGE_MAP(CDlgAddTerm, CIBADialog)
	ON_CBN_SELCHANGE(IDC_COMBO_TERMINALID, OnCbnSelchangeComboTerminalid)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CDlgAddTerm 消息处理程序

BOOL CDlgAddTerm::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	LoadComputeInfo();

	m_cboTerminalID.SetCurSel(0);
	OnCbnSelchangeComboTerminalid();
	m_cboTerminalID.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// 通过本地数据类载入所有的终端信息
void CDlgAddTerm::LoadComputeInfo(void)
{
	for (INT_PTR ii = 0; ii < CLocalServer::GetInstance()->ComputerList.GetCount(); ii++)
	{
		CComputerInfo info = CLocalServer::GetInstance()->ComputerList.GetComputer(ii);

		CString str  = info.GetTerminalID();
		if (str.IsEmpty())
		{
			continue;
		}

		INT_PTR jj = 0;
		for (jj = 0; jj < m_SeatList->GetSize(); jj++)
		{
			CSeat * tSeat = m_SeatList->GetAt(jj);
			if (str == tSeat->GetTerminalID())
			{
				break;
			}
		}

		if (jj >= m_SeatList->GetSize())
		{
			m_cboTerminalID.AddString(str);

			str = info.GetComputerIP();
			if (!str.IsEmpty())
			{
				m_cboTerminalIP.AddString(str);
			}
		}
	}

}

void CDlgAddTerm::OnCbnSelchangeComboTerminalid()
{
	UpdateData(TRUE);

	CString strTerminalID;
	int CurSelIndex = m_cboTerminalID.GetCurSel();
	m_cboTerminalID.GetLBText(CurSelIndex, strTerminalID);
	INT_PTR index = CLocalServer::GetInstance()->ComputerList.LookFor(0, strTerminalID);

	if (index >= 0)
	{
		CComputerInfo info = CLocalServer::GetInstance()->ComputerList.GetComputer(index);

		CString str = info.GetComputerIP();

		if (!str.IsEmpty())
		{
			m_cboTerminalIP.SelectString(-1, str);
		}
	}

}

void CDlgAddTerm::OnBnClickedOk()
{
	CBCGPDialog::OnOK();
	// TODO: 在此添加控件通知处理程序代码
}

void CDlgAddTerm::OnOK()
{
	CBCGPDialog::OnOK();
	// TODO: 在此添加控件通知处理程序代码
}

void CDlgAddTerm::OnBnClickedCancel()
{
	CBCGPDialog::OnCancel();
	// TODO: 在此添加控件通知处理程序代码
}

void CDlgAddTerm::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CBCGPDialog::OnClose();
}
