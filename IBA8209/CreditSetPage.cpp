// CreditSet.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "CreditSetPage.h"
#include ".\creditsetpage.h"


// CSetPageOfCredit 对话框

IMPLEMENT_DYNAMIC(CSetPageOfCredit, CIBAPage)
CSetPageOfCredit::CSetPageOfCredit()
: CIBAPage(CSetPageOfCredit::IDD)
, m_bAutoRefund(FALSE)
, m_nAutoRefundTime(0)
, m_bPopMsgBox(FALSE)
, m_nMaxCreditMoney(0)
{
	EnableVisualManagerStyle();
}

CSetPageOfCredit::~CSetPageOfCredit()
{
}

void CSetPageOfCredit::DoDataExchange(CDataExchange* pDX)
{
	CIBAPage::DoDataExchange(pDX);
	//DDX_Check(pDX, IDC_CHECK_REFUNDMENT, m_bAutoRefund);
	DDX_Text(pDX, IDC_EDIT_REFUNDTIME, m_nAutoRefundTime);
	DDX_Control(pDX, IDC_COMBO_RETURNACTION, m_cboReturnAction);
	DDX_Check(pDX, IDC_CHECK_POPMSGBOX, m_bPopMsgBox);
	DDX_Text(pDX, IDC_EDIT_MAX_MONEY, m_nMaxCreditMoney);
}


BEGIN_MESSAGE_MAP(CSetPageOfCredit, CIBAPage)
	ON_BN_CLICKED(IDC_CHECK_REFUNDMENT, OnBnClickedCheckRefundment)
END_MESSAGE_MAP()


// CSetPageOfCredit 消息处理程序

void CSetPageOfCredit::OnBnClickedCheckRefundment()
{
	UpdateData();

	if (m_bAutoRefund)
	{
		GetDlgItem(IDC_EDIT_REFUNDTIME)->SetFocus();
	} 
}

void CSetPageOfCredit::OnOK()
{
	UpdateData();

	// 2011/05/12-gxx: 网吧没有设置强制退款时间为0, 才需要保存时间的设置
	if (CNetBarConfig::GetInstance()->GetMustAutoReturn()!=1)
	{
		CIBAConfig::GetInstance()->SetRefundmentTime(m_nAutoRefundTime);
	}	

	// 回车动作
	if (m_cboReturnAction.GetCurSel() != -1)
	{
		CIBAConfig::GetInstance()->SetReturnAction(m_cboReturnAction.GetCurSel());
	}

	// 2012/03/05-8238-gxx:
	CIBAConfig::GetInstance()->SetMaxCreditMoney((INT)m_nMaxCreditMoney);

	//保存弹出信息
	CIBAConfig::GetInstance()->SetPopMsgBox(m_bPopMsgBox);

	CIBAPage::OnOK();
}

BOOL CSetPageOfCredit::OnInitDialog()
{
	CIBAPage::OnInitDialog();

	// 2011/04/27-gxx: 限制时间最多输入位数
	((CEdit*)GetDlgItem(IDC_EDIT_REFUNDTIME))->SetLimitText( CIBAConfig::MAX_TIME_LIMIT ); 

	//自动退款功能设置
	m_nAutoRefundTime = CIBAConfig::GetInstance()->GetRefundmentTime();
	
	// 2011/05/12-gxx: 如果设置了强制自动退款时间为0， 那么就禁用自动对款时间的编辑框
	if (CNetBarConfig::GetInstance()->GetMustAutoReturn()==1)
	{
		GetDlgItem(IDC_EDIT_REFUNDTIME)->EnableWindow(FALSE);
		m_nAutoRefundTime = 0;
	}

	
	//回车动作
	m_cboReturnAction.SetCurSel(CIBAConfig::GetInstance()->GetReturnAction());

	//是否弹出对话框
	m_bPopMsgBox = CIBAConfig::GetInstance()->GetPopMsgBox();

	// 2012/03/05-8238-gxx:
	m_nMaxCreditMoney = CIBAConfig::GetInstance()->GetMaxCreditMoney();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
