// ShiftCashDlg.h : 头文件
//

#pragma once


// CShiftCashDlg 对话框
class CShiftCashDlg : public CDialog
{
// 构造
public:
	CShiftCashDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SHIFTCASH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnExportExcel();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CString m_strFilePath;
	afx_msg void OnBnClickedBtnExportOnkeyCost();
	afx_msg void OnBnClickedBtnExportCardopen();
	afx_msg void OnBnClickedBtnExportCashPay();
};
