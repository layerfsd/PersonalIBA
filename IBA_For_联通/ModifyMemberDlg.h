#pragma once

#include "IBADialog.h"
#include "NDDialog.h"
// CModifyMemberDlg 对话框

class CModifyMemberDlg : public CNDDialog
{
	DECLARE_DYNAMIC(CModifyMemberDlg)
	DECLARE_MESSAGE_MAP()
public:
	CModifyMemberDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CModifyMemberDlg();

// 对话框数据
	enum { IDD = IDD_ND_MODIFYMEMBER };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOldCard(UINT nCardId, UINT nMemberId);
	virtual void OnNewCard(UINT nCardId);
	virtual void ClearData();
	virtual void OnGetBalanceOK(NS_SERVLET::CBalanceMethod & Balance);
	afx_msg void OnBnClickedButtonModify();
private:

	CBCGPComboBox m_cboSex;
	CBCGPComboBox m_cboIDType;

	CString m_strPwd;

	CString m_strIdCardSN;

	CString m_strCardID;

	CString m_strName;

	CString m_strMemberID;

	CString m_strUserClass;

	CString m_strPersonalID;

	CString m_strNetBar;

	CString m_strAllBalance;

	//临时变量 
	
	CString m_strMemberName;

	UINT m_nNetbarID;

	CString m_strOldPersonalId;

};
