// ReturnDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include ".\returndlg.h"


// CReturnDlg 对话框

IMPLEMENT_DYNAMIC(CReturnDlg, CIBADialog)
CReturnDlg::CReturnDlg(CWnd* pParent /*=NULL*/)
: CIBADialog(CReturnDlg::IDD, pParent)
{
}

CReturnDlg::~CReturnDlg()
{
}

void CReturnDlg::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_USERNAME, m_strUserName);
	DDX_Text(pDX, IDC_STATIC_NETID, m_strNetId);
	DDX_Text(pDX, IDC_STATIC_MONEY, m_strMoney);
	DDX_Text(pDX, IDC_STATIC_PERSONALID, m_strPersonalId);
}


BEGIN_MESSAGE_MAP(CReturnDlg, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CReturnDlg 消息处理程序

BOOL CReturnDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	SetStaticFont();
	
	GetDlgItem(IDC_STATIC_MONEY)->SetFont(&m_BigFont);

	m_strUserName = m_CheckOutInfo.GetUserName();
	m_strNetId = m_CheckOutInfo.GetSerialNum();
	m_strPersonalId = m_CheckOutInfo.GetPersonalId();

	UINT nTmp = CIBAHelpper::RoundMoney(m_CheckOutInfo.GetBalanceMoney(), CNetBarConfig::GetInstance()->GetRoundType());
	m_strMoney.Format(_T("%.2f 元"), nTmp / 100.0);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CReturnDlg::SetStaticFont()
{
	CWnd* pChildWnd = GetWindow(GW_CHILD);   

	while (pChildWnd != NULL)   
	{
		TCHAR szClassName[MAX_PATH] = {0};
		GetClassName(pChildWnd->GetSafeHwnd(), szClassName, MAX_PATH);

		if (lstrcmp(szClassName, _T("Static")) == 0)
		{
			if (pChildWnd->GetDlgCtrlID() != IDC_STATIC)
			{
				GetDlgItem(pChildWnd->GetDlgCtrlID())->SetFont(&m_NormalFont);
			}
		}

		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);  
	}
}

void CReturnDlg::OnBnClickedOk()
{
	CBCGPDialog::OnOK();
}

HBRUSH CReturnDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CIBADialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_MONEY)
		{
			pDC->SetTextColor(RGB(0, 0, 255));
		}	
	}

	return hbr;
}
