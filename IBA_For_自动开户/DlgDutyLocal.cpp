// DlgDutyLocal.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "DlgDutyLocal.h"
#include ".\dlgdutylocal.h"
#include "Cashier.h"
#include "ActiveMemberList.h"
#include "Socket\LocalServer.h"


// CDlgDutyLocal 对话框

IMPLEMENT_DYNAMIC(CDlgDutyLocal, CIBADialog)
CDlgDutyLocal::CDlgDutyLocal(CWnd* pParent /*=NULL*/)
	: CIBADialog(CDlgDutyLocal::IDD, pParent)
	, m_strCardMoney(_T(""))
	, m_strAccountShopping(_T(""))
	, m_strZSBConsume(_T(""))
	, m_strZSBBalance(_T(""))
	, m_strCreditMoney(_T(""))
	, m_strReturnMoney(_T(""))
	, m_strCashShopping(_T(""))
	, m_strGoodsStock(_T(""))
	, m_strOther(_T(""))
	, m_strDeposit(_T(""))
	, m_strDutyIncome(_T(""))
	, m_strForwardReserve(_T(""))
	, m_strCashTotal(_T(""))
{
}

CDlgDutyLocal::~CDlgDutyLocal()
{
}

void CDlgDutyLocal::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_CREDITCARD, m_strCardMoney);
	DDX_Text(pDX, IDC_STATIC_NETBARPAY, m_strAccountShopping);
	DDX_Text(pDX, IDC_STATIC_ZSB_CONSUME, m_strZSBConsume);
	DDX_Text(pDX, IDC_STATIC_ZSB_BALANCE, m_strZSBBalance);
	DDX_Text(pDX, IDC_STATIC_CREDIT, m_strCreditMoney);
	DDX_Text(pDX, IDC_STATIC_RETURNED, m_strReturnMoney);
	DDX_Text(pDX, IDC_STATIC_CASHPAY, m_strCashShopping);
	DDX_Text(pDX, IDC_STATIC_STOREIN, m_strGoodsStock);
	DDX_Text(pDX, IDC_STATIC_OTHER, m_strOther);
	DDX_Text(pDX, IDC_STATIC_DEPOSIT, m_strDeposit);
	DDX_Text(pDX, IDC_STATIC_NETCASH, m_strDutyIncome);
	DDX_Text(pDX, IDC_STATIC_RESERVE, m_strForwardReserve);
	DDX_Text(pDX, IDC_STATIC_ALL, m_strCashTotal);
}


BEGIN_MESSAGE_MAP(CDlgDutyLocal, CIBADialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgDutyLocal 消息处理程序

BOOL CDlgDutyLocal::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	if (!theApp.GetCurCashier()->IsOnDuty())
	{
		IBA_ASSERT2(0,"恢复模式交接班信息的查看必须要再上班状态");
		return FALSE;
	}

	// 初始化控件
	{
		CWnd* pChildWnd = GetWindow(GW_CHILD);   

		while (pChildWnd != NULL)   
		{
			TCHAR szClassName[MAX_PATH] = {0};
			GetClassName(pChildWnd->GetSafeHwnd(), szClassName, MAX_PATH);
		
			if (lstrcmp(szClassName, _T("Static")) == 0)
			{
				/*if (pChildWnd->GetDlgCtrlID() != IDC_STATIC && 
					pChildWnd->GetDlgCtrlID() != IDC_STATIC2_HANDLEIN && 
					pChildWnd->GetDlgCtrlID() != IDC_STATIC2_NEXTRESERVE)*/
				{
					//GetDlgItem(pChildWnd->GetDlgCtrlID())->SetFont(&m_FontSmall);
				}
			}
			pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);  
		}
	}
	//GetDlgItem(IDC_STATIC_CREDITCARD)->SetFont(&m_Font);
	GetDlgItem(IDC_STATIC_ALL)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_SELFDUTY)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_REMARK)->SetFont(&m_Font);

	m_strOnDutyTime = theApp.GetCurCashier()->GetOnDutyTime();

	CLastTranInfo LastTranInfo;//最后交易方法
	LastTranInfo.Load();//载入注册表信息

	m_ShiftTotalMethod.SetLastRefNo(LastTranInfo.GetLastRefNo());//最后一次交易的操作号
	m_ShiftTotalMethod.SetLastTranTime(LastTranInfo.GetLastTranTime());//最后一次交易的时间
	m_ShiftTotalMethod.SetLastTranType(LastTranInfo.GetLastTranType());//最后一次交易的类型

	theApp.GetCurCashier()->DoShiftTotal(m_ShiftTotalMethod);//

	if (m_ShiftTotalMethod.GetStatusCode() == 0)
	{		
		UINT nCasherID = CIBAConfig::GetInstance()->GetCashRegisterID();
		INT nCreditMoney = m_ShiftTotalMethod.GetCreditAmount();
		nCreditMoney += CLocalServer::GetInstance()->ActiveMemberList.GetLocalTotalMoney(nCasherID);

		m_strCreditMoney.Format(_T("%.2f"), nCreditMoney/100.0);
		m_strCashShopping.Format(_T("%.2f"), m_ShiftTotalMethod.GetCashPay()/100.0);
		m_strReturnMoney.Format(_T("%.2f"), m_ShiftTotalMethod.GetReturnedAmount()/100.0);
		m_strCardMoney.Format(_T("%.2f"), m_ShiftTotalMethod.GetCreditCard()/100.0);
		m_strZSBConsume.Format(_T("%.2f"), m_ShiftTotalMethod.GetZSBConsume()/100.0);
		m_strZSBBalance.Format(_T("%.2f"), m_ShiftTotalMethod.GetZSBBalance()/100.0);
		m_strAccountShopping.Format(_T("%.2f"), m_ShiftTotalMethod.GetNetBarPay()/100.0);
		m_strGoodsStock.Format(_T("%.2f"), m_ShiftTotalMethod.GetStoreInAmount()/100.0);
		m_strDeposit.Format(_T("%.2f"), m_ShiftTotalMethod.GetDeposit()/100.0);
		m_strForwardReserve.Format(_T("%.2f"), m_ShiftTotalMethod.GetReserveAmount()/100.0);
		m_strOther = _T("0.00");

		INT nAllMoney = 0;
		nAllMoney += nCreditMoney;
		nAllMoney -= m_ShiftTotalMethod.GetReturnedAmount();
		nAllMoney += m_ShiftTotalMethod.GetCashPay();
		nAllMoney += m_ShiftTotalMethod.GetDeposit();
		nAllMoney -= m_ShiftTotalMethod.GetStoreInAmount();
		nAllMoney -= 0;
		nAllMoney += m_ShiftTotalMethod.GetCostExpense(); // +工本费
		m_strDutyIncome.Format(_T("%.2f"),nAllMoney/100.0);
		nAllMoney += m_ShiftTotalMethod.GetReserveAmount();
		m_strCashTotal.Format(_T("%.2f"),nAllMoney/100.0);

		
		UpdateData(FALSE);
	}
	else
	{
		ShowToolTip(m_ShiftTotalMethod.GetStatusMessage(), TRUE);
	}
	


	return TRUE;  
}

HBRUSH CDlgDutyLocal::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CIBADialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch(pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC_CREDITCARD:
	case IDC_STATIC_NETBARPAY:
	case IDC_STATIC_ZSB_CONSUME:
	case IDC_STATIC_ZSB_BALANCE:
	case IDC_STATIC_CREDIT:
	case IDC_STATIC_RETURNED:
	case IDC_STATIC_CASHPAY:
	case IDC_STATIC_STOREIN:
	case IDC_STATIC_OTHER:
	case IDC_STATIC_DEPOSIT:
	case IDC_STATIC_NETCASH:
	case IDC_STATIC_RESERVE:
	case IDC_STATIC_ALL:
		pDC->SetTextColor(RGB(0,0,255));
		break;
	}
	
	return hbr;
}

void CDlgDutyLocal::OnBnClickedOk()
{
	CBCGPDialog::OnOK();
}
