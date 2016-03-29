#pragma once
#include "Servlet\ShiftTotalMethod.h"
#include "IBADialog.h"


// CDlgDutyLocal 对话框

class CDlgDutyLocal : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgDutyLocal)

public:
	CDlgDutyLocal(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDutyLocal();

// 对话框数据
	enum { IDD = IDD_DUTY_LOCAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strCardMoney;       // 充值卡金额
	CString m_strAccountShopping; // 账户购物
	CString m_strZSBConsume;      // 增收宝消费
	CString m_strZSBBalance;      // 增收宝余额
	CString m_strCreditMoney;     // 充值金额
	CString m_strReturnMoney;     // 退款金额
	CString m_strCashShopping;    // 现金购物
	CString m_strGoodsStock;      // 商品采购支出
	CString m_strOther;           // 其他支出
	CString m_strDeposit;         // 押金
	CString m_strDutyIncome;      // 班次收入
	CString m_strForwardReserve;  // 上一班预留
	CString m_strCashTotal;       // 现金总额

	CString m_strOnDutyTime;
	NS_SERVLET::CShiftTotalMethod m_ShiftTotalMethod;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();
};
