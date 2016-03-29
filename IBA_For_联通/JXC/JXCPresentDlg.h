#pragma once

#include "..\NDDialog.h"
#include "JXCCommodityDlg.h"
#include "..\Servlet\RetailMethod.h"

// CJXCPresentDlg 对话框

class CJXCPresentDlg : public CNDDialog
{
	DECLARE_DYNAMIC(CJXCPresentDlg)
	DECLARE_MESSAGE_MAP()

public:
	CJXCPresentDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CJXCPresentDlg();

	void AddSellRecord(NS_SERVLET::CSellRecord& SellRecord);
// 对话框数据
	enum { IDD = IDD_JXC_PRESENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonCommodity();
	afx_msg void OnBnClickedButtonAdd();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);

	void OnOldCard(UINT nCardId, UINT nMemberId);
	void OnGetBalanceOK(NS_SERVLET::CBalanceMethod & Balance);
	void ClearData();

private:
	// 允许处理消息
	bool m_bAllowPretranslate;

	CString m_strCommodityId;
	CString m_strCommodityName;
	CString m_strCommodityType;
	CString m_strCommodityPrice;
	CString m_strCommodityRemainQuantity;

	CString m_strPoint;

	UINT m_nCommodityCount;

	CString m_strAllMoney;

	NS_SERVLET::CSellRecordArray m_SellRecordArray;

	CBCGPMaskEdit m_edtCount;

	CBCGPListCtrl m_CommodityList;

	CJXCCommodityDlg m_CommodityDlg;

	void InitListView();
	void ShowSellRecordInfo();
	void SaveRetailData(NS_SERVLET::CRetailMethod &Retail);
	BOOL QueryCommodity();
	void ClearData2();

};
