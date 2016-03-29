#pragma once

#include "NDDialog.h"
#include "ColorGridCtrl.h"

// CNDCheckInDlg 对话框

class CNDCheckInDlg : public CNDDialog
{
	DECLARE_DYNAMIC(CNDCheckInDlg)
	DECLARE_MESSAGE_MAP()

public:
	CNDCheckInDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNDCheckInDlg();

// 对话框数据
	enum { IDD = IDD_ND_CHECKINDLG };

protected:
	
	virtual void OnGetBalanceOK(CBalanceMethod & Balance);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
	afx_msg void OnBnClickedOk();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonRefresh();

	virtual BOOL DoLocalModeQuery();

	BOOL m_bCheckIn;

	CString m_strPwd;

	BOOL m_bPowerOn;

	CIBAGrid m_wndGridTree;

	void InitGridTree();
	void FillGridTree();

	void CheckoutOldComputer();
	BOOL CheckTakeUp(CString strTermId);



public:
	
	BOOL GetCheckIn() const { return m_bCheckIn; }
	void SetCheckIn(BOOL newVal) { m_bCheckIn = newVal; }

	void SetNetId(CString strNetId);
	
};
