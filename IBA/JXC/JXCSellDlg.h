#pragma once

#include "..\IBADialog.h"
#include "JXCCommodityDlg.h"
#include "..\Servlet\RetailMethod.h"
// CJXCSellDlg 对话框

class CJXCSellDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CJXCSellDlg)
	DECLARE_MESSAGE_MAP()

public:
	CJXCSellDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CJXCSellDlg();

// 对话框数据
	enum { IDD = IDD_JXC_SELLDLG };

protected:

	CString m_strCommodityId;
	CString m_strCommodityName;
	CString m_strCommodityType;
	CString m_strCommodityPrice;
	CString m_strCommodityRemainQuantity;

	UINT m_nCommodityCount;

	CString m_strNetId;
	CString m_strPassword;

	CString m_strAllMoney;

	UINT m_nCardId;
	UINT m_nMemberId;
	
	NS_SERVLET::CSellRecordArray m_SellRecordArray;

	CBCGPComboBox m_cboPayType;
	CBCGPMaskEdit m_edtCount;

	CBCGPListCtrl m_CommodityList;

	CJXCCommodityDlg m_CommodityDlg;
	
	// 允许处理消息
	bool m_bAllowPretranslate;
public:

	void AddSellRecord(NS_SERVLET::CSellRecord& SellRecord);

protected:

	BOOL GetCardIdFromCenter();
	void InitListView();
	void ShowSellRecordInfo();
	BOOL CheckInput();
	void ClearData();
	void SaveRetailData(NS_SERVLET::CRetailMethod &Retail);
	BOOL QueryCommodity();

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();	
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonCommodity();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnCbnSelchangeComboPaytype();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnKillfocusEditPassword();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnEnKillfocusEditCount();
public:
	afx_msg void OnTimer(UINT nIDEvent);

	void PrintTicket(CString strAccount);
};
