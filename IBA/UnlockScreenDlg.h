#pragma once

#include "IBADialog.h"

// CUnlockScreenDlg 对话框

class CUnlockScreenDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CUnlockScreenDlg)
	DECLARE_MESSAGE_MAP()

public:
	CUnlockScreenDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUnlockScreenDlg();

// 对话框数据
	enum { IDD = IDD_UNLOCKSCREENDLG };

	CString GetPwd() const { return m_strPwd; }

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	
	CString m_strPwd;

};
