#pragma once

#include "..\IBADialog.h"
#include "..\dal\IBADAL.h"
#include "..\ColorGridCtrl.h"
//#include "jxcretailonlinedlg.h"

class CJXCSellDlg;
class CJXCExchangeDlg;
class CJXCPresentDlg;
class CJXCRetailOnlineDlg;

class CJXCCommodityDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CJXCCommodityDlg)
	DECLARE_MESSAGE_MAP()

public:
	CJXCCommodityDlg(CWnd* pParent = NULL, UINT nType = 0);   // 标准构造函数
	virtual ~CJXCCommodityDlg();

// 对话框数据
	enum { IDD = IDD_JXC_COMMODITY };
	
	CCommodityArray m_CommodityArray;
	void QueryCommodity(CCommodityQueryCondition & CommodityQueryCondition);

protected:

	static const UINT INVALID_COMMODITYID = 65535;

	INT m_nCount;

	INT m_nCurSel;
	CString m_strCommodityName;
	
	CBCGPMaskEdit m_edtCount;

	CIBAGrid m_wndGridTree;

	void InitGridTree();

	void FillGird();

	CJXCSellDlg* m_lpJXCSellDlg;
	CJXCExchangeDlg* m_lpJXCExchangeDlg;
	CJXCPresentDlg* m_lpJXCPresentDlg;
	CJXCRetailOnlineDlg* m_lpJXCRetailOnlineDlg;

	void AddSellRecord();

protected:

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnGridItemChanged(WPARAM wParam, LPARAM lParam);
};
