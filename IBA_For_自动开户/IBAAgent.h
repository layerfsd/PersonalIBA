#pragma once

#include "BCGPLabalButton.h"
//
//class: CDlgIBAAgent 代理对话框
//
class CDlgIBAAgent : public CStringLoaderDlg<CBCGPDialog>
{
	DECLARE_DYNAMIC(CDlgIBAAgent)
	DECLARE_MESSAGE_MAP()

public:
	CDlgIBAAgent(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgIBAAgent();

// 对话框数据
	enum { IDD = IDD_DIALOG_AGENT };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonEmail();
	afx_msg void OnBnClickedButtonBbs();
	afx_msg void OnBnClickedButtonOnline();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

private:
	// 初始化参数，赋予值
	void InitData();
	// 代理名称
	CString m_strAgentName;
	// 手机号码
	CString m_strMobilePhone;
	// 固定电话
	CString m_strRegularTel;

	CBCGPLabelButton m_btnEmail;

	CBCGPLabelButton m_btnBBS;

	CBCGPLabelButton m_btnOnline;

	CFont m_font;
	
};
