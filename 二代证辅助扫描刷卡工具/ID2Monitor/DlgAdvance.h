#pragma once
#include "afxcmn.h"
#include "GxxListCtrl.h"


// CDlgAdvance 对话框

class CDlgAdvance : public CDialog
{
	DECLARE_DYNAMIC(CDlgAdvance)

public:
	CDlgAdvance(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAdvance();

// 对话框数据
	enum { IDD = IDD_DLG_ADVANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CGxxListCtrl m_listCtrl;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnAdd();
};
