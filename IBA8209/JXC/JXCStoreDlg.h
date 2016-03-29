#pragma once
#include "..\IBADialog.h"
#include "JXCCommodityDlg.h"
#include "jxccommodityseldlg.h"
#include "..\Servlet\storeinmethod.h"

// CJXCStoreDlg 对话框

class CJXCStoreDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CJXCStoreDlg)
	DECLARE_MESSAGE_MAP()

public:
	CJXCStoreDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CJXCStoreDlg();

// 对话框数据
	enum { IDD = IDD_JXC_STORE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonCommodity();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedOk();
//	afx_msg void OnEnChangeEditCount();
	afx_msg void OnEnChangeEditPrice();
	afx_msg void OnBnClickedButtonSyncserver();
//	afx_msg void OnEnChangeEditCount2();
	afx_msg void OnEnKillfocusEditCount();
	afx_msg void OnEnKillfocusEditCount2();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnEnUpdateEditCount2();
	afx_msg void OnEnUpdateEditCount();
private:
	//编号
	CString m_strCommodityId;
	//商品名字
	CString m_strCommodityName;
	//商品类型
	CString m_strCommodityType;
	//商品库存量
	CString m_strCommodityRemainQuantity;
	//////////////////////////大小单位/////////////////////////
	// 商品小单位，如果是双单位，就是大单位，平时显示
	CString m_strCommodityUnit;
	// 入库数量（小单位），如果是双单位，就是大单位，平时显示
	INT m_nStoreCount;
	// 数量,如果是双单位，就是小单位，平时隐藏
	INT  m_nStoreCount2;
	//采购单价（备份数据）
	INT m_nPrice;
	// 采购单价（捆绑），因为要显示2位小数
	CString m_strCommodityPrice;
	// 商品总价,因为要显示小数，所以采用字串
	CString m_strCommodityAmount;
	// 单位,如果是双单位，就是小单位，平时隐藏
	CString m_strCommodityUnit2;
	// 单价的单位
	CString m_strPriceUnit;
	//汇率字串
	CString m_strUnitMutiple;
	//汇率
	INT m_nUnitMutiple;
	/////////////////////////////////////

	void ClearData();
	void InitListView();
	void MakeStoreInRecord(NS_SERVLET::CStoreInRecordArray &StoreInRecordArray);
	BOOL AddRecord(NS_SERVLET::CStoreInRecordArray &StoreInRecordArray, INT i);
	BOOL QueryCommodity();
	void GetDataFromDlg(CJXCCommoditySelDlg &CommodityDlg);
	BOOL CheckPopedom(LPCTSTR lpszModuleName);
	
private://控件变量
	//入库商品数量
	//CEdit m_edtStoreCount;
	// 采购单价（上限）
	CEdit m_edtPrice;
	//入库商品列表
	CBCGPListCtrl m_CommodityList;
	// 商品编号
	CEdit m_edtCommodityId;
	// 入库数量
	//CEdit m_edtStoreCount2;

private:
	// 允许处理消息
	bool m_bAllowPretranslate;

	CString m_strResult;

	CString m_strStoreCount2;
	CString m_strStoreCount;
};
