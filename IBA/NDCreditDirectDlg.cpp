//NDCreditDirectDlg.cpp : 实现文件

#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include "IBAHelpper.h"
#include ".\NDCreditDirectDlg.h"
#include "Socket\LocalServer.h"

// CModifyPwdDlg 对话框

IMPLEMENT_DYNAMIC(CNDCreditDirectDlg, CNDDialog)

CNDCreditDirectDlg::CNDCreditDirectDlg(CWnd* pParent /*=NULL*/)
: CNDDialog(CNDCreditDirectDlg::IDD, pParent)
{

}

CNDCreditDirectDlg::~CNDCreditDirectDlg()
{
}

void CNDCreditDirectDlg::DoDataExchange(CDataExchange* pDX)
{
	CNDDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_CREDIT, m_edtCreditMoney);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT, m_cboAccount);
	DDX_Control(pDX, IDC_COMBO_KIND, m_cboCreditKind);

	DDX_Text(pDX, IDC_STATIC_USERNAME, m_strName);
	DDX_Text(pDX, IDC_STATIC_NETBAR, m_strNetBar);
	DDX_Text(pDX, IDC_STATIC_PERSONALID, m_strPersonalID);
	DDX_Text(pDX, IDC_STATIC_CLASS, m_strUserClass);
	DDX_Text(pDX, IDC_STATIC_YSCZ, m_strBalanceY);
	DDX_Text(pDX, IDC_STATIC_WSCZ, m_strBalanceW);
	DDX_Text(pDX, IDC_STATIC_ZSZH, m_strBalanceZ);

	DDX_Text(pDX, IDC_STATIC_ONLINEAMOUNT, m_strAllBalance);

}


BEGIN_MESSAGE_MAP(CNDCreditDirectDlg, CNDDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CModifyPwdDlg 消息处理程序

void CNDCreditDirectDlg::OnBnClickedOk()
{
	UpdateData();

	if (CLocalServer::GetInstance()->ActiveMemberList.GetUserStateByNetId(m_strNetId) ==
		CIBAGlobal::emUserStateLocal)
	{
		CBCGPDialog::OnOK();
	}
	
	CString strTmp;
	m_edtCreditMoney.GetWindowText(strTmp);
	INT nCreditMoney = (INT)(_tstof(strTmp) * 100);
	
	if (nCreditMoney > 999999)
	{
		ShowToolTip(IDS_MAXMONEY, IDC_EDIT_CREDIT, TRUE);
		return;
	}

	CWaitCursor WaitCursor;

	CCreditDirectMethod CreditDirect;

	CreditDirect.SetMemberId(m_nMemberId);
	CreditDirect.SetCreditMode(m_cboCreditKind.GetCurSel());
	CreditDirect.SetAmount(nCreditMoney);
	CreditDirect.SetAccountType(m_cboAccount.GetCurSel() + 1);

	theApp.GetCurCashier()->DoCreditDirect(CreditDirect);

	if (CreditDirect.GetStatusCode() == 0)
	{
		m_edtCreditMoney.SetWindowText(_T(""));

		OnBnClickedButtonQuery();

		ShowToolTip(IDS_OPERATEOK, IDOK, FALSE);
	}
	else
	{
		SetToolTipPos(IDOK);
		ShowToolTip(CreditDirect.GetStatusMessage(), TRUE);
	}
}

BOOL CNDCreditDirectDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	//{ 2011/05/24-gxx: 自动查询在刷卡及扫描证件的情况下采用
	if (m_bAutoQuery)
	{
		SetTimer(TimeIdOfAutoQuery, 100, NULL);//只查询一次
	}
	else
	{
		CWnd* pWnd = GetDlgItem(IDC_EDIT_IDCARDSNO);
		ASSERT(pWnd);
		if (pWnd)
		{
			pWnd->SetFocus();
		}
	}
	//}

	m_edtCreditMoney.SetFont(&m_BigFont);
	m_edtCreditMoney.DisableMask();
	m_edtCreditMoney.SetValidChars(_T("0123456789."));
	m_edtCreditMoney.SetLimitText(6);

	m_cboAccount.SetCurSel(0);
	m_cboCreditKind.SetCurSel(1); //默认减钱

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();

	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFont(&m_BigFont);

	AddFocusEnter(IDC_EDIT_IDCARDSNO, IDC_BUTTON_QUERY);
	AddFocusEnter(IDC_EDIT_CREDIT, IDOK);

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CNDCreditDirectDlg::ClearData()
{
	m_strName.Empty();
	m_strNetBar.Empty();
	m_strPersonalID.Empty();
	m_strUserClass.Empty();
	m_strAllBalance.Empty();

	GetDlgItem(IDOK)->EnableWindow(FALSE);

}


void CNDCreditDirectDlg::OnGetBalanceOK(CBalanceMethod & Balance)
{
	CNDDialog::OnGetBalanceOK(Balance);

	m_strName = Balance.GetUserName();
	m_strPersonalID = Balance.GetPersonalId();
	m_strUserClass = CNetBarConfig::GetInstance()->GetUserClassName(Balance.GetUserClass());
	m_strNetBar = CNetBarConfig::GetInstance()->GetNetBarName(Balance.GetOpenNetbarId());

	m_strBalanceW.Format(_T("%.2f"), Balance.GetCashBalance() / 100.0);//无送
	m_strBalanceY.Format(_T("%.2f"), Balance.GetUsePresent() / 100.0);//有送
	m_strBalanceZ.Format(_T("%.2f"), Balance.GetCreditPresent() / 100.0);//赠送

	GetDlgItem(IDOK)->EnableWindow(TRUE);

	m_edtCreditMoney.SetFocus();
}

HBRUSH CNDCreditDirectDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CNDDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_ALL)
		{
			pDC->SetTextColor(RGB(0, 0, 255));
		}
	}
	else if (CTLCOLOR_EDIT == nCtlColor)
	{
		if (pWnd->GetDlgCtrlID() == IDC_EDIT_CREDIT)
		{
			pDC->SetTextColor(RGB(255, 0, 0));
		}		
	}

	return hbr;
}

void CNDCreditDirectDlg::OnTimer(UINT nIDEvent)
{
	if (TimeIdOfAutoQuery == nIDEvent)
	{
		KillTimer(nIDEvent);

		if (m_bAutoQuery)
		{
			m_bAutoQuery = FALSE;
			OnBnClickedButtonQuery();
		}
	}

	CNDDialog::OnTimer(nIDEvent);
}

BOOL CNDCreditDirectDlg::DoLocalModeQuery()
{
	CNDDialog::DoLocalModeQuery();
	theApp.IBAMsgBox(_T("不允许本地用户进行账户直充"));
	return FALSE;
}