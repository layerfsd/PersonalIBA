// TestRealNameDlg.h : 头文件
//

#pragma once


// CTestRealNameDlg 对话框
class CTestRealNameDlg : public CDialog
{
// 构造

	DECLARE_MESSAGE_MAP()

public:
	CTestRealNameDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTREALNAME_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();


	CString m_strIDNumber;
	
	CString m_strName;

	CString m_strPeronId;

	CString m_strPwd;

	CString m_strIDType;

};
