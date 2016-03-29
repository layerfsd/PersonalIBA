#pragma once

#include "IBADialog.h"
#include "ColorGridCtrl.h"

// CStoreDlg 对话框

class CStoreDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CStoreDlg)
	DECLARE_MESSAGE_MAP()

public:
	CStoreDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStoreDlg();

// 对话框数据
	enum { IDD = IDD_DUTY_STORECHECK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	CBCGPGridCtrl*  m_lpCommodityGrid;

	CIBAGrid m_CommodityGrid;

	virtual BOOL OnInitDialog();

private:
	//填充按种类和价格分类数据
	void FillSortPriceGird();
	//填充按价格分类数据
	void FillPriceGird();
	// 初始化控件
	BOOL InitCtrl(void);

	BOOL InitSortGrid(void);

	BOOL InitPriceGrid(void);

	BOOL InitSortPriceGrid(void);
	// 初始化GRID
	BOOL SetGrid(UINT nType = 0);
	//填充按种类分类数据
	void FillSortGird();
	// 分类的类型
	CBCGPComboBox m_cboSortType;

public:
	afx_msg void OnCbnSelchangeComboSort();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonPrintcommodity();
};
