// YB2IBADlg.h : 头文件
//

#pragma once

#include "ListenSocket.h"
#include "ClientSocket.h"

// CYB2IBADlg 对话框
class CYB2IBADlg : public CDialog
{
	DECLARE_MESSAGE_MAP()
// 构造
public:
	CYB2IBADlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_YB2IBA_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	
	virtual BOOL OnInitDialog();
	
	void WriteReg();

	BOOL SendMessageToIBA(UINT nType);

// 生成的消息映射函数
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg HRESULT OnSocketNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

// 实现
protected:

	HICON m_hIcon;

	CListenSocket m_ListenSocket;

	CClientSocket m_ClientSocket;

	CString m_strIdNumber;

	CString m_strPath;

public:
	afx_msg void OnNcDestroy();
};
