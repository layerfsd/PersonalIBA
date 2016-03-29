// LockDlg.h : 头文件
//

#pragma once
#include "ImageEx.h"
#include "HoverButton.h"
#include "XPEdit.h"
#include "LockScreenDlg.h"
// CLockDlg 对话框
class CLockDlg : public CDialog
{
// 构造
public:
	CLockDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_LOCK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CFont m_Font;
	CFont m_Font2;

	CImageEx  m_ImageBg;
	CImageEx  m_ImageAdvert;

	CLockScreenDlg* m_plockScreendlg;

	CString m_strPassWord;
private:
	void CreateFont();
	void ReCaculateControl();

	BOOL CreateLockDlg();
public:
	CHoverButton m_LockBtn;
	CHoverButton m_CancleBtn;
	CXPEdit m_PassWordEdit;
	CXPEdit m_RPassWordEdit;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg LRESULT OnCheckPassword(WPARAM wParam, LPARAM lParam);

	afx_msg void OnDestroy();
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnClose();
};
