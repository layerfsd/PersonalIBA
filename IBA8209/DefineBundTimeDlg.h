
#pragma once

#include "IBADialog.h"

// 开始时使用到自定义包时

class CDefineBundleTimeDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CDefineBundleTimeDlg)
	DECLARE_MESSAGE_MAP()

public:
	CDefineBundleTimeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDefineBundleTimeDlg();

// 对话框数据
	enum { IDD  = IDD_ND_DEFINEBUNDTIME };

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

public:
	struct SelfDefBundleTime
	{
		UINT TimeId;       // 包时ID
		CString BeginTime; // 开始时间
		UINT TimePass;     // 包时时长
		UINT Amount;       // 包时金额
		UINT PcClass;      
		UINT AccountType;  

		BOOL bIsSelected;

	}m_BundleTimeInfo;

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
	
	CBCGPComboBox m_cboAccountType;
	
	CBCGPMaskEdit m_edtBundTimeMoney;
	// 区域
	CBCGPComboBox m_cboArea;
	
	CEdit m_edtHours;
	CEdit m_edtMinutes;
};
