#pragma once

#include "IBADialog.h"
#include "dal\IBADAL.h"
#include "ColorGridCtrl.h"
#include "Servlet\ShiftTotalMethod.h"

//
//class:CDlgIBADuty 上下班对话框
//
class CDlgIBADuty : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgIBADuty)
	DECLARE_MESSAGE_MAP()

public:
	CDlgIBADuty(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgIBADuty();

// 对话框数据
	enum { IDD = IDD_DUTY };

protected:
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonStat();
	afx_msg void OnEnChangeEditPayout();
	afx_msg void OnEnChangeEditNextreserve();
	afx_msg void OnEnChangeEditAll();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonPrint();
	afx_msg void OnBnClickedButtonJxcprint();
	afx_msg void OnEnChangeEditNextreverse();
	afx_msg void OnBnClickedButtonStorecheck();

private:

	void SetStaticFont();
	void InitContrls();
	void HideShiftTotalInfo();
	void GetEachMoney(INT& nPayout, INT& nRemainAmount, INT& nFetchAmount);
	void PrintTicket(CShiftCash &ShiftCash);
	void ShowShiftTotalInfo();
	UINT GetShiftType();
	void DoOnDuty();
	void DoOffDuty();
	INT  Round(double dsrc);
	CString GetShiftName(UINT nShiftType);

	//2011-03-16-gxx-清除下班后操作员的操作日志
	void DeleteOffDutyOperationFile();

private:

	INT m_nAllMoney;//现金总额
	INT m_nNextReserve;//下一班保留金额

	NS_SERVLET::CShiftTotalMethod m_ShiftTotalMethod;

	CString m_strPayout;//其他支出金额
	CString m_strFetchAmount;//取得的现金总额
	CString m_strDescription;//备注描述

	CEdit m_edtPayout; //必须为CEdit才能响应CHANGE
	// 下一班预留金
	CEdit m_edtNextReverse;
	// 留给下一班的金额
	CString m_strNextReverse;

	CString m_strOnDutyTime;

	INT m_nDutyIncome; // 班次收入
	INT m_nLocalCreditMoney;

private:

	CBCGPMaskEdit m_edtReverveAmount;//本班次备用金额
	CBCGPComboBox m_cboShift;//收银员转换

private:

	CIBAGrid m_wndGridTree;//网格控件
	void InitGridTree();
	void FillGird();
};
