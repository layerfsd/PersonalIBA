#pragma once

#include "Cashier.h"
#include "IBAFormView.h"

// COperationView 窗体视图

class COperationView : public CIBAFormView
{
	DECLARE_DYNCREATE(COperationView)
	DECLARE_MESSAGE_MAP()

protected:
	COperationView();           // 动态创建所使用的受保护的构造函数
	virtual ~COperationView();

public:
	enum { IDD = IDV_OPERATION };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void ClearData();
	void InitListView();

	void InitUserClassComboBox();
	void InitTimeControl();
	void InitData();

	//void QueryCredit(BOOL bReverse = FALSE);
	//void QueryRegister();
	//void QueryReturned();
	//void QueryCardCredit(BOOL bReverse = FALSE);
	//void QueryUpgrade();

	void StatUserCount(UINT nUserClass, CString& strClassName);
	void InitDetailData(CDetailDataMethod &DetailData);
	
	void QueryRegisterCenter();
	void QueryCreditCenter(BOOL bReverse = FALSE);
	void QueryReturnedCenter();
	void QueryCreditRevserCenter();
	void QueryCardCreditCenter();
	void QueryUpgradeCenter();

	void DoQuery();
	void CheckMoreRecord(CDetailDataMethod &DetailData);

	CBCGPListCtrl m_List;
	CBCGPComboBox m_cboOperationType;
	CBCGPDateTimeCtrl m_StartTime;
	CBCGPDateTimeCtrl m_EndTime;
	CBCGPComboBox m_cboUserClass;
	CBCGPComboBox m_cboCashier;

	CString m_strSerialNum;
	CString m_strPersonalID;
	CString m_strOperator;

	CUIntArray m_nUserCount;
	UINT m_nUnkonwUserClass;
	UINT m_nRecordCount;
	INT m_nAllMoney1;
	INT m_nAllMoney2;
	INT m_nAllMoney3;
	CString m_strResult;

	BOOL m_bSetDate;

	UINT m_nCurrPageNo;

public:

	virtual void OnInitialUpdate();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCbnSelchangeComboOperation();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNMClickListCommon(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonPrev();
	afx_msg void OnBnClickedButtonQuery();
};


