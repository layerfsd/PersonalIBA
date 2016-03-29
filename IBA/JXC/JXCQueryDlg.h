#pragma once

#include "..\IBADialog.h"
#include "..\ColorGridCtrl.h"
// CJXCQueryDlg 对话框

class CJXCQueryDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CJXCQueryDlg)
	DECLARE_MESSAGE_MAP()

public:
	CJXCQueryDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CJXCQueryDlg();

// 对话框数据
	enum { IDD = IDD_JXC_QUERYDLG };

private:
	
	CBCGPComboBox m_cboCashier;
	CBCGPComboBox m_cboRetailType;
	CBCGPComboBox m_cboPayType;

	CBCGPDateTimeCtrl m_StartTime;
	CBCGPDateTimeCtrl m_EndTime;
	CIBAGrid m_wndGridTree;
	
	CRetailInfoArray m_RetailInfoArray;

	CString m_strCommodityId;
	CString m_strInfo;

protected:

	void InitGridTree();
	void FillGird();
	void InitTimeControl();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT nIDEvent);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void InitOperatorsCombo();
};
