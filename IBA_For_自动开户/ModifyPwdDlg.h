#pragma once

#include "IBADialog.h"
#include ".\NDDialog.h"
// CModifyPwdDlg 对话框

class CModifyPwdDlg : public CNDDialog
{
	DECLARE_DYNAMIC(CModifyPwdDlg)
	DECLARE_MESSAGE_MAP()

public:
	CModifyPwdDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CModifyPwdDlg();

// 对话框数据
	enum { IDD = IDD_ND_MODIFYPWD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	virtual void OnOldCard(UINT nCardId, UINT nMemberId);
	virtual void OnNewCard(UINT nCardId);
	virtual void ClearData();
	virtual void OnGetBalanceOK(NS_SERVLET::CBalanceMethod & Balance);

	// 2011/07/22-8201-gxx: 
	virtual BOOL DoLocalModeQuery();
	virtual BOOL DoLocalModeOperate();


private :

	CString m_strPwd;

	CString m_strIdCardSN;

	CString m_strCardID;

	CString m_strMemberID;

	CString m_strNetBar;
public:
	afx_msg void OnTimer(UINT nIDEvent);
};
