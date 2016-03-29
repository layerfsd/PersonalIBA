// PointExchangeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include ".\PointExchangeDlg.h"

// CNDPointExchangeDlg 对话框

IMPLEMENT_DYNAMIC(CNDPointExchangeDlg, CNDDialog)
CNDPointExchangeDlg::CNDPointExchangeDlg(CWnd* pParent /*=NULL*/)
: CNDDialog(CNDPointExchangeDlg::IDD, pParent)
, m_nCount(1), m_nExchangePoint(0)
{
}

CNDPointExchangeDlg::~CNDPointExchangeDlg()
{
}

void CNDPointExchangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CNDDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_USERNAME, m_strName);
	DDX_Text(pDX, IDC_STATIC_CLASS, m_strUserClass);
	DDX_Text(pDX, IDC_STATIC_POINT, m_strPoint);
	DDX_Text(pDX, IDC_STATIC_ALL, m_strAllMoney);
	DDX_Text(pDX, IDC_EDIT_PRESENTNAME, m_strPresentName);
	DDX_Text(pDX, IDC_EDIT_COUNT, m_nCount);
	DDX_Text(pDX, IDC_EDIT_EXCHANGEPOINT, m_nExchangePoint);
	DDX_Text(pDX, IDC_STATIC_NETBAR, m_strNetBar);
	DDX_Text(pDX, IDC_STATIC_PERSONALID, m_strPersonalID);
}


BEGIN_MESSAGE_MAP(CNDPointExchangeDlg, CNDDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CNDPointExchangeDlg 消息处理程序
BOOL CNDPointExchangeDlg::OnInitDialog()
{
	CNDDialog::OnInitDialog();

	SetEditFont();

	GetDlgItem(IDC_STATIC_POINT)->SetFont(&m_BigFont);
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFont(&m_BigFont);

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_COUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PRESENTNAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_EXCHANGEPOINT)->EnableWindow(FALSE);

	AddFocusEnter(IDC_EDIT_EXCHANGEPOINT, IDOK);
	AddFocusEnter(IDC_EDIT_COUNT, IDOK);
	AddFocusEnter(IDC_EDIT_PRESENTNAME, IDOK);

	return FALSE;  //如果重新设定焦点，则设为假
	// 异常: OCX 属性页应返回 FALSE
}

void CNDPointExchangeDlg::OnBnClickedOk()
{
	UpdateData();
	CWaitCursor Wait;

	CUsePointMethod UsePoint;
	UsePoint.SetMemberId(m_nMemberId);
	UsePoint.SetPoint(m_nExchangePoint);
	UsePoint.SetQuantity(m_nCount);
	UsePoint.SetArticle(m_strPresentName);
	theApp.GetCurCashier()->DoUsePoint(UsePoint);
	
	SetToolTipPos(IDOK);

	if (UsePoint.GetStatusCode() == 0)
	{
		ShowToolTip(L"积分兑换成功！");

		OnBnClickedButtonQuery();
	}
	else
	{
		CString strTmp;
		strTmp.Format(L"积分兑换失败！原因：%s", UsePoint.GetStatusMessage());
		ShowToolTip(strTmp, TRUE);
	}

	GetDlgItem(IDC_EDIT_EXCHANGEPOINT)->SetFocus();
}

void CNDPointExchangeDlg::OnOldCard(UINT nCardId, UINT nMemberId)
{
	QueryBalance(nMemberId, nCardId);

	UpdateData(FALSE);

	GetDlgItem(IDC_EDIT_COUNT)->EnableWindow(TRUE);

	GetDlgItem(IDC_EDIT_PRESENTNAME)->EnableWindow(TRUE);

	GetDlgItem(IDC_EDIT_EXCHANGEPOINT)->EnableWindow(TRUE);

	GetDlgItem(IDOK)->EnableWindow(TRUE);

	GetDlgItem(IDC_EDIT_PRESENTNAME)->SetFocus();
	
}

void CNDPointExchangeDlg::OnGetBalanceOK(NS_SERVLET::CBalanceMethod & Balance)
{
	m_strNetId = Balance.GetSerailNum();
	m_strName = Balance.GetUserName();
	m_strPoint.Format(_T("%d"), Balance.GetCurrentPoint());

	m_strUserClass = CNetBarConfig::GetInstance()->GetUserClassName(Balance.GetUserClass());

	//合计 = 无送账户+有送账户+赠送账户+预留上网费
	INT nAllMoney = Balance.GetCashBalance() + Balance.GetUsePresent() + Balance.GetCreditPresent() + Balance.GetOnlineAmount();
	m_strAllMoney.Format(_T("%.2f"), nAllMoney / 100.0);

	m_strPersonalID = Balance.GetPersonalId();
	m_strNetBar = CNetBarConfig::GetInstance()->GetNetBarName(Balance.GetOpenNetbarId());
}

void CNDPointExchangeDlg::ClearData()
{
	m_strName.Empty();
	m_strUserClass.Empty();
	m_strAllMoney.Empty();
	m_strPoint.Empty();
	m_strPersonalID.Empty();
	m_strNetBar.Empty();
	m_nCount = 1;
	m_strPresentName.Empty();
	m_nExchangePoint = 0;
}

HBRUSH CNDPointExchangeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CNDDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_POINT)
		{
			pDC->SetTextColor(RGB(0, 0, 255));
		}	
	}

	return hbr;
}
