#pragma once

#include "NDDialog.h"

// CNDPointExchangeDlg 对话框

class CNDPointExchangeDlg : public CNDDialog
{
	DECLARE_DYNAMIC(CNDPointExchangeDlg)
	DECLARE_MESSAGE_MAP()

public:
	CNDPointExchangeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNDPointExchangeDlg();

// 对话框数据
	enum { IDD = IDD_JXC_POINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog(); 
	virtual void OnOldCard(UINT nCardId, UINT nMemberId);
	virtual void OnGetBalanceOK(NS_SERVLET::CBalanceMethod & Balance);
	virtual void ClearData();
	afx_msg void OnBnClickedOk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

private:

	// 积分总额
	CString m_strPoint;
	// 账户总额
	CString m_strAllMoney;
	// 总需积分
	UINT m_nExchangePoint;
	// 礼品名称
	CString m_strPresentName;
	// 密码
	CEdit m_edtPassWord;
	// 兑换数量
	UINT m_nCount;

	CString m_strNetBar;

};
