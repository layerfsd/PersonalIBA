#pragma once

#include "IBADialog.h"
#include "dal\IDTypeInfo.h"
// CReturnDlg 对话框

class CReturnDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CReturnDlg)
	DECLARE_MESSAGE_MAP()

public:
	CReturnDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CReturnDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_RETURN };

	void SetCheckOutInfo(CCheckOutinfo newVal) { m_CheckOutInfo = newVal; }

protected:

	void SetStaticFont();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	CCheckOutinfo m_CheckOutInfo;

	CString m_strUserName;

	CString m_strNetId;

	CString m_strMoney;

	CString m_strPersonalId;
};
