
#pragma once

#include "..\Servlet\RetailMethod.h"
#include "..\Servlet\BalanceMethod.h"
#include "..\NDDialog.h"
#include "JXCCommodityDlg.h"

// CJXCRetailOnlineDlg 对话框
class CJXCRetailOnlineDlg : public CNDDialog
{
	DECLARE_DYNAMIC(CJXCRetailOnlineDlg)
	DECLARE_MESSAGE_MAP()

public:
	CJXCRetailOnlineDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CJXCRetailOnlineDlg();

// 对话框数据
	enum { IDD = IDD_JXC_RETAILONLINE };

	void AddSellRecord(NS_SERVLET::CSellRecord& SellRecord);

	void InitUserData(CString strTermId, CString strNetId);

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonCommodity();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonQuery();
	afx_msg void OnTimer(UINT nIDEvent);

	void OnOldCard(UINT nCardId, UINT nMemberId);
	void OnGetBalanceOK(NS_SERVLET::CBalanceMethod & Balance);
	void ClearData();

private:
	// 允许处理消息
	bool m_bAllowPretranslate;
	/////////////商品信息////////////
	CString m_strCommodityId;
	CString m_strCommodityName;
	CString m_strCommodityType;
	CString m_strCommodityPrice;
	CString m_strCommodityRemainQuantity;
	UINT m_nCommodityCount;

	CString m_strTermId;
	
	//待结账金额
	CString m_strAllMoney;
	//查询中间变量

	UINT m_bOnline;//是否在线的标志
	//////////////控件变量////////////
	NS_SERVLET::CSellRecordArray m_SellRecordArray;

	CBCGPMaskEdit m_edtCount;

	CBCGPListCtrl m_CommodityList;

	CJXCCommodityDlg m_CommodityDlg;

	void InitListView();
	void ShowSellRecordInfo();
	void SaveRetailData(NS_SERVLET::CRetailOnlineMethod &RetailOnline);
	BOOL QueryCommodity();
	void ClearData2();

	void PrintTicket(CString strAccount);
};
