#pragma once

#include "IBADialog.h"

// CDlgInputUserPwd 对话框

class CDlgInputUserPwd : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgInputUserPwd)

public:
	CDlgInputUserPwd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgInputUserPwd();

	CString GetPwd() { return m_strPwd; }
// 对话框数据
	enum { IDD = IDD_DIALOG5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedOk();
private:
	CString m_strPwd;
};
