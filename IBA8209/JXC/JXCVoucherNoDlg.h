#pragma once

#include "..\IBADialog.h"
#include "..\dal\IBADAL.h"
#include "..\ColorGridCtrl.h"
#include "..\Servlet\RetailMethod.h"
// CJXCVoucherNoDlg 对话框

class CJXCVoucherNoDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CJXCVoucherNoDlg)
	DECLARE_MESSAGE_MAP()

public:
	CJXCVoucherNoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CJXCVoucherNoDlg();

// 对话框数据
	enum { IDD = IDD_JXC_VOUCHERNO };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	void InitGridTree();
	void FillGird();

	INT m_nCurSel;

	CString m_strVoucherNo;

	CRetailInfoArray m_RetailInfoArray;
	
	NS_SERVLET::CSellRecordArray m_SellRecordArray;

	CRetailVoucher m_RetailVoucher;

	CIBAGrid m_wndGridTree;

public:

	CRetailVoucher GetRetailVoucher();

	void GetSellRecordArray(NS_SERVLET::CSellRecordArray& newVal);
};
