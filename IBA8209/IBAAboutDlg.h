#pragma once

#include "BCGPLabalButton.h"
// CIBAAboutDlg 对话框

class CIBAAboutDlg : public CStringLoaderDlg<CBCGPDialog>
{
	DECLARE_DYNAMIC(CIBAAboutDlg)
	DECLARE_MESSAGE_MAP()

public:
	CIBAAboutDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIBAAboutDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_ABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonDodonew();
	afx_msg void OnBnClickedButtonSunlike();
	afx_msg void OnBnClickedButtonDuduniu();
private:

	CBCGPLabelButton m_btnDodonew;

	CBCGPLabelButton m_btnSunlike;

	CBCGPLabelButton m_btnDodoniu;

	CString m_strVer;

};
