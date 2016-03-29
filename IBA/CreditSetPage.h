#pragma once

#include "IBADialog.h"

//
//class:CSetPageOfCredit 收银台设置->充值退款设置对话框
//
class CSetPageOfCredit : public CIBAPage
{
	DECLARE_DYNAMIC(CSetPageOfCredit)
	DECLARE_MESSAGE_MAP()

public:
	CSetPageOfCredit();   // 标准构造函数
	virtual ~CSetPageOfCredit();

// 对话框数据
	enum { IDD = IDD_SET_CREDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnClickedCheckRefundment();
	virtual void OnOK();
	virtual BOOL OnInitDialog();

private:
	// 使能自动退款
	BOOL m_bAutoRefund;
	// 自动退款时间
	UINT m_nAutoRefundTime;
	// 回车动作
	CBCGPComboBox m_cboReturnAction;
	// 是否弹出对话框
	BOOL m_bPopMsgBox;

	UINT m_nMaxCreditMoney;

};
