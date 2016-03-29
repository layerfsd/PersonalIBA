#pragma once

#include "IBADialog.h"

//
//class:CDlgPwd 开户时密码输入对话框
//
class CDlgPwd : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgPwd)
	DECLARE_MESSAGE_MAP()

public:
	CDlgPwd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPwd();

// 对话框数据
	enum { IDD = IDD_ND_REGISTER_PASSWORD };

protected:

	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnEnChangeEditPwd();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	void OnCancel();

private:
	CEdit m_edtPwd;
	CString m_strPwd;

public :

	CString GetPwd() const { return m_strPwd; }
};
