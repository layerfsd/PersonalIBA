#pragma once

#include "..\ColorGridCtrl.h"
#include "..\IBADialog.h"
#include "..\Dal\DALJXC.h"
// CJXCCommoditySelDlg 对话框

class CJXCCommoditySelDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CJXCCommoditySelDlg)
	DECLARE_MESSAGE_MAP()

public:
	CJXCCommoditySelDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CJXCCommoditySelDlg();

// 对话框数据
	enum { IDD = IDD_JXC_COMMODITYSEL };

protected:
	static const UINT INVALID_COMMODITYID = 65535;

	virtual void OnCancel();
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:

	static const UINT TimerIdOfSync = 1000;

	//商品当前选中的标号
	INT m_nCurSel;
	//树对象
	CIBAGrid m_wndGridTree;
	//初始化树
	void InitGridTree();
	//填充树
	void FillGird();

	void QueryCommodity();
	
	//取得当前选中商品的数据
	BOOL GetCurSelData(INT nCurSelIndex = INVALID_COMMODITYID);

public:
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonSyncserver();
	afx_msg void OnTimer(UINT nIDEvent);

private:
	
	//商品名称
	CString m_strCommodityName;
	//商品类型
	CString m_strCommodityType;
	//商品编码
	CString m_strCommodityCode;
	//商品单价（小单位）
	UINT m_nPrice;
	//商品库存数量
	UINT m_nDepositCount;
	//商品小单位
	CString m_strCommodityRetailUint;
	//商品大单位
	CString m_strCommodityStoreUint;
	//单位换算率
	UINT m_nUnitMutiple;

public:
	//商品队列
	CCommodityArray m_CommodityArray;

	CString GetCommodityName() const { return m_strCommodityName; }

	CString GetCommodityType() const { return m_strCommodityType; }

	void SetCommodityCode(CString val) { m_strCommodityCode = val; }
	CString GetCommodityCode() const { return m_strCommodityCode; }

	UINT GetPrice() const { return m_nPrice; }

	UINT GetDepositCount() const { return m_nDepositCount; }

	CString GetCommodityRetailUint() const { return m_strCommodityRetailUint; }

	CString GetCommodityStoreUint() const { return m_strCommodityStoreUint; }

	UINT GetUnitMutiple() const { return m_nUnitMutiple; }
};
