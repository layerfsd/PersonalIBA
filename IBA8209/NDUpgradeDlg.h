#pragma once

#include "NDDialog.h"
// CNDUpgradeDlg 对话框

class CNDUpgradeDlg : public CNDDialog
{
	DECLARE_DYNAMIC(CNDUpgradeDlg)
	DECLARE_MESSAGE_MAP()

public:

	CNDUpgradeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNDUpgradeDlg();

// 对话框数据
	enum { IDD = IDD_ND_UPGRADE };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	void OnOldCard(UINT nCardId, UINT nMemberId);
	void OnGetBalanceOK(NS_SERVLET::CBalanceMethod & Balance);
	afx_msg void OnBnClickedButtonUpgrade();

private:

	CString m_strMemberID;
	CString m_strNetBar;
	CString m_strDeposit;
	CString m_strBalanceW;
	CString m_strBalanceY;
	CString m_strBalanceZ;
	CString m_strMemberName;

	CString m_strReason;

	CBCGPComboBox m_cboUserClass;
};
