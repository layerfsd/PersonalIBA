#pragma once

// CSetPageOfDuty 对话框

class CSetPageOfDuty : public CIBAPage
{
	DECLARE_DYNAMIC(CSetPageOfDuty)
	DECLARE_MESSAGE_MAP()

public:
	CSetPageOfDuty();   // 标准构造函数
	virtual ~CSetPageOfDuty();

// 对话框数据
	enum { IDD = IDD_SET_ONOFFDUTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboShiftcount();

private:
	// 是否自动打印交班表
	BOOL m_bPrintShiftReport;
	// 是否使能动态备用金
	BOOL m_bDynamicReverseMoney;
	// 交班报表类型
	CBCGPComboBox m_cboShiftReportType;

	// 交班表类型
	int m_nShiftReportType;

	// 是否启用上交金额
	BOOL m_bIsFetchAmount;
	
	// 允许编辑其他支出项
	BOOL m_bEditPayout;

	BOOL m_bAllowNextDutyMoney;
	
	INT m_nShiftCount;
};
