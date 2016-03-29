
#pragma once
#include "IBADialog.h"

//
//class:CDlgBundTimeSelfDef 自定义包时对话框
//
class CDlgBundTimeSelfDef : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgBundTimeSelfDef)
	DECLARE_MESSAGE_MAP()

public:
	CDlgBundTimeSelfDef(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgBundTimeSelfDef();

	void SetMemberId(UINT val) { m_nMemberId = val; }
	
	void SetIsMember(BOOL bIsMember) { m_bIsMember = bIsMember; }
// 对话框数据
	enum { IDD = IDD_ND_BUNDTIME_SELFDEFINE };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnEndTimeChanged();
	afx_msg void OnEnChangeEditHour();
	afx_msg void OnEnChangeEditMin();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

private:
	// 开始时间控件
	CBCGPDateTimeCtrl m_btnStartTime;
	CBCGPDateTimeCtrl m_btnEndTime;

	// 初始化时间控件
	BOOL InitTimeCtrl(void);
	// 初始化时间控件的数据
	BOOL InitTimeDate(void);
	// 初始化控件
	void InitOtherCtrl(void);

	UINT m_nMemberId;
	
	CBCGPComboBox m_cboAccountType;
	
	CBCGPMaskEdit m_edtBundTimeMoney;
	// 区域
	CBCGPComboBox m_cboArea;
	
	CEdit m_edtHours;
	CEdit m_edtMinutes;

	BOOL m_bIsMember;
};
