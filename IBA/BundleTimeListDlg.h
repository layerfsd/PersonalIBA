#pragma once

#include "IBADialog.h"
#include "Servlet\BundleTimeListMethod.h"

//
//class:CDlgBundleTimeList 包时信息列表对话框
//
class CDlgBundleTimeList : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgBundleTimeList)
	DECLARE_MESSAGE_MAP()
public:
	CDlgBundleTimeList(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgBundleTimeList();

// 对话框数据
	enum { IDD = IDD_BUNDLETINE_LISTDLG };

protected:
	
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMDblclkListBundletime(NMHDR *pNMHDR, LRESULT *pResult);
	
	void InitListView();

	void FillBundleTimeList();

	void InsertItem(INT nArrayOrder, INT nItem);

	CBCGPListCtrl m_BundleTimeList;

public:	

	UINT GetBundleTimeId() const { return m_nBundleTimeId; }
	UINT GetBundleTimeType() const { return m_nBundleTimeType; }
	UINT GetBundleTimeClass() const { return m_nBundleTimeClassId; }
	UINT GetBundleTimeMoney() const { return m_nBundleTimeMoney; }

	CString GetBundleTimeMoneyString() const;
	CString GetStartTime() const { return m_strStartTime; }
	CString GetEndTime() const { return m_strEndTime; }
	CString GetRemainTime() const { return m_strRemainTime; }// 包时的剩余时间
	
	//包时策略的选择方式
	void SetSelectStyle(UINT val) { m_nSelectStyle = val; }

	void SetUserClassId(UINT newVal) { m_nUserClassId = newVal; }

private:

	CString m_strStartTime;
	CString m_strEndTime;
	CString m_strRemainTime;// 包时的剩余时间

	UINT m_nBundleTimeId;
	UINT m_nBundleTimeType;  //包时消费类型
	UINT m_nBundleTimeClassId; //包时操作类型
	UINT m_nBundleTimeMoney;

	//包时策略的选择方式，0为普通方式（全部）， 1为在线方式（本时段区域）
	UINT m_nSelectStyle;

	//用户级别
	UINT m_nUserClassId;
};
