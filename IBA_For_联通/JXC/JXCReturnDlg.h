#pragma once

#include "..\IBADialog.h"
#include "..\dal\IBADAL.h"
#include "..\Servlet\RetailMethod.h"

class CJXCReturnDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CJXCReturnDlg)
	DECLARE_MESSAGE_MAP()

public:
	CJXCReturnDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CJXCReturnDlg();

// 对话框数据
	enum { IDD = IDD_JXC_RETURNDLG };

private:

	CString m_strCommodityId;

	CString m_strCommodityTd;

	CString m_strCommodityName;

	CString m_strCommodityType;

	CString m_strCommodityPrice;

	UINT m_nCommodityCount;

	CBCGPListCtrl m_CommodityList;

	NS_SERVLET::CSellRecordArray m_SellRecordArray;
	
	CRetailVoucher m_RetailVoucher;

	UINT m_nCardId;

	UINT m_nMemberId;

	CString m_strAllMomey;

	CString m_strVoucherNo;

protected:

	void ShowMoney();
	void InitListView();
	void ClearData();
	void SaveRetailData(NS_SERVLET::CRetailMethod &Retail);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonVoucherno();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	afx_msg void OnBnClickedButtonCommodity();
//	afx_msg void OnBnClickedButtonCommodity();
};
