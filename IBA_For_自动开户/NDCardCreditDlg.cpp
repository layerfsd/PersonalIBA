// NDCardCreditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include "IBAHelpper.h"
#include "dal\IBADAL.h"
#include ".\ndcardcreditdlg.h"
#include "Socket\LocalServer.h"


// CNDCardCreditDlg 对话框

IMPLEMENT_DYNAMIC(CNDCardCreditDlg, CNDDialog)
CNDCardCreditDlg::CNDCardCreditDlg(CWnd* pParent /*=NULL*/)
: CNDDialog(CNDCardCreditDlg::IDD, pParent)
, m_strPoint(_T(""))
{
}

CNDCardCreditDlg::~CNDCardCreditDlg()
{
}

void CNDCardCreditDlg::DoDataExchange(CDataExchange* pDX)
{
	CNDDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_STATIC_USERNAME, m_strName);
	DDX_Text(pDX, IDC_STATIC_MEMBERID, m_strMemberID);
	DDX_Text(pDX, IDC_STATIC_NETBARID, m_strNetBar);
	DDX_Text(pDX, IDC_STATIC_PERSONALID, m_strPersonalID);
	DDX_Text(pDX, IDC_STATIC_CLASS, m_strUserClass);
	DDX_Text(pDX, IDC_STATIC_ALL, m_strAllBalance);
	DDX_Text(pDX, IDC_STATIC_YSCZ, m_strBalanceY);
	DDX_Text(pDX, IDC_STATIC_WSCZ, m_strBalanceW);
	DDX_Text(pDX, IDC_STATIC_ZSZH, m_strBalanceZ);
	DDX_Text(pDX, IDC_EDIT_ID, m_strCardId);
	DDX_Text(pDX, IDC_EDIT_CREDIT, m_strMoney);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPwd);

	DDX_Control(pDX, IDC_COMBO_IDTYPE, m_cboCardType);
	DDX_Text(pDX, IDC_STATIC_POINT, m_strPoint);
}


BEGIN_MESSAGE_MAP(CNDCardCreditDlg, CNDDialog)
	ON_BN_CLICKED(IDC_BUTTON_CREDIT, OnBnClickedButtonCredit)
	ON_BN_CLICKED(IDC_BUTTON_READCARD, OnBnClickedButtonReadcard)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CNDCardCreditDlg 消息处理程序

BOOL CNDCardCreditDlg::OnInitDialog()
{
	CNDDialog::OnInitDialog();

	//{ 2011/05/24-gxx: 自动查询
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

	SetEditFont();
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFont(&m_BigFont);
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();

	GetDlgItem(IDC_BUTTON_CREDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_READCARD)->EnableWindow(FALSE);

	m_cboCardType.SetFont(&m_Font);
	m_cboCardType.AddString(LOAD_STRING(IDS_VISUAL_CARD));  // 虚拟卡
	m_cboCardType.AddString(LOAD_STRING(IDS_REAL_CARD2));   // 真实卡
	m_cboCardType.SetCurSel(0);

	AddFocusEnter(IDC_EDIT_CREDIT, IDC_BUTTON_CREDIT);
	AddFocusEnter(IDC_EDIT_PASSWORD, IDC_BUTTON_CREDIT);
	AddFocusEnter(IDC_EDIT_ID, IDC_BUTTON_CREDIT);

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CNDCardCreditDlg::OnBnClickedButtonCredit()
{
	UpdateData();
	
	if (!CLocalServer::GetInstance()->ActiveMemberList.IsAllowCredit(m_nMemberId))
	{
		return;
	}


	if (m_strPwd.IsEmpty() || m_strCardId.IsEmpty())
	{
		return;
	}

	NS_SERVLET::CCardCreditMethod CardCredit;

	CString strTmp(_T("XN"));

	if (m_cboCardType.GetCurSel() == 1)
	{
		strTmp = (_T("ZS"));
	}

	CardCredit.SetCategory(strTmp);
	CardCredit.SetCardPwd(m_strPwd);
	CardCredit.SetMemberId(_ttoi(m_strMemberID));
	CardCredit.SetCardSerial(m_strCardId);

	double dblAmount = _tstof(m_strMoney);
	INT nTmp = dblAmount * 100;

	CardCredit.SetAmount(nTmp);

	theApp.GetCurCashier()->DoCardCredit(CardCredit);

	if (CardCredit.GetStatusCode() == 0)
	{
		m_CreditCard.DeleteCreditCard();

		{//写入数据库

			CCreditInfoDB CreditInfo;

			CreditInfo.SetCardSerial(m_strCardId);
			CreditInfo.SetCreditDate(CIBAHelpper::FormatCenterTime(CardCredit.GetTranTime()));
			CreditInfo.SetRefNo(CardCredit.GetRefNo());
			CreditInfo.SetCreditPresent(CardCredit.GetCreditPresent());
			CreditInfo.SetCreditAmount(nTmp);
			CreditInfo.SetOperator(theApp.GetCurCashier()->GetName());
			CreditInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
			CreditInfo.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
			CreditInfo.SetMemberId(_ttoi(m_strMemberID));
			CreditInfo.SetSerialNo(CardCredit.GetLocalSerialNo());
			CreditInfo.SetCassId(m_nUserClassID);
			CreditInfo.SetCategory(strTmp);
			
			CIBADAL::GetInstance()->AddCardCreditRecord(CreditInfo);

		}

		m_strPwd.Empty();
		m_strMoney.Empty();
		m_strCardId.Empty();
		UpdateData(FALSE);
		GetDlgItem(IDC_BUTTON_CREDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_READCARD)->EnableWindow(FALSE);

		OnBnClickedButtonQuery();
		
	}

	SetToolTipPos(IDC_BUTTON_CREDIT);
	ShowToolTip(CardCredit.GetStatusMessage(), CardCredit.GetStatusCode() != 0);
}

void CNDCardCreditDlg::OnBnClickedButtonReadcard()
{
	if (m_CreditCard.NextCreditCard())
	{
		if (m_CreditCard.IsCreditCardValid())
		{
			m_strCardId = m_CreditCard.GetCreditCardID();
			m_strPwd = m_CreditCard.GetCreditCardPassword();
			m_cboCardType.SetCurSel(m_CreditCard.GetCreditCardType());

			m_strMoney.Format(_T("%.2f"), m_CreditCard.GeCreditCardMoney() / 100.0);

			GetDlgItem(IDC_EDIT_CREDIT)->EnableWindow(m_CreditCard.GeCreditCardFun() == 99);
			UpdateData(FALSE);
		}
		else
		{
			m_CreditCard.DeleteCreditCard();
		}
	}
}

void CNDCardCreditDlg::OnOldCard(UINT nCardId, UINT nMemberId)
{
	m_strMemberID.Format(_T("%d"), nMemberId);
	
	QueryBalance(nMemberId, nCardId);

	GetDlgItem(IDC_EDIT_ID)->SetFocus();
	UpdateData(FALSE);
}

void CNDCardCreditDlg::OnNewCard(UINT nCardId, UINT nMemberId)
{
	GetDlgItem(IDC_EDIT_ID)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_CREDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_IDTYPE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
	CNDDialog::OnNewCard( nCardId );
}

void CNDCardCreditDlg::ClearData()
{
	m_strName.Empty();
	m_strMemberID.Empty();
	m_strNetBar.Empty();
	m_strPersonalID.Empty();
	m_strUserClass.Empty();
	m_strAllBalance.Empty();
	m_strBalanceY.Empty();
	m_strBalanceW.Empty();
	m_strBalanceZ.Empty();
	m_strPoint.Empty();
	m_strCardId.Empty();
	m_strMoney.Empty();
	m_strPwd.Empty();
	m_cboCardType.SetWindowText(_T(""));
	GetDlgItem(IDC_BUTTON_CREDIT)->EnableWindow(FALSE);

}

void CNDCardCreditDlg::OnGetBalanceOK(NS_SERVLET::CBalanceMethod & Balance)
{
	m_strMemberName = Balance.GetMemberName();
	m_strName = Balance.GetUserName();
	m_strPersonalID = Balance.GetPersonalId();
	m_nUserClassID = Balance.GetUserClass();
	m_strUserClass = CNetBarConfig::GetInstance()->GetUserClassName(Balance.GetUserClass());
	m_strNetBar = CNetBarConfig::GetInstance()->GetNetBarName(Balance.GetOpenNetbarId());

	double dblTmp = 0.0, dblTmp1 = 0.0, dblTmp2 = 0.0;

	//无送
	dblTmp = Balance.GetCashBalance();
	dblTmp2 = dblTmp / 100;
	m_strBalanceW.Format(_T("%.2f"), dblTmp2);

	dblTmp1 = dblTmp;

	//有送
	dblTmp = Balance.GetUsePresent();
	dblTmp2 = dblTmp / 100;
	m_strBalanceY.Format(_T("%.2f"), dblTmp2);

	dblTmp1 += dblTmp;

	//赠送
	dblTmp = Balance.GetCreditPresent();
	dblTmp2 = dblTmp / 100;
	m_strBalanceZ.Format(_T("%.2f"), dblTmp2);

	dblTmp1 += dblTmp;

	//上网预留金
	dblTmp = Balance.GetOnlineAmount();
	dblTmp2 = dblTmp / 100;
	
	dblTmp1 += dblTmp;

	//合计 = 无送账户+有送账户+赠送账户+预留上网费
	dblTmp2 = dblTmp1 / 100;
	m_strAllBalance.Format(_T("%.2f"), dblTmp2);

	//积分
	m_strPoint.Format(_T("%d"), Balance.GetCurrentPoint());

	GetDlgItem(IDC_BUTTON_CREDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_READCARD)->EnableWindow(TRUE);

}

void CNDCardCreditDlg::OnTimer(UINT nIDEvent)
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
