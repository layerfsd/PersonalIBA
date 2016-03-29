#pragma once

// CSetPageOfMonitor2 对话框

class CSetPageOfMonitor2 : public CIBAPage
{
	DECLARE_DYNAMIC(CSetPageOfMonitor2)
	DECLARE_MESSAGE_MAP()

public:
	CSetPageOfMonitor2();   // 标准构造函数
	virtual ~CSetPageOfMonitor2();

// 对话框数据
	enum { IDD = IDD_SET_MONITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

private:

	BOOL m_bOpenAfterEnter;

	//余额不足
	CString m_strLittleMoney;

	// 用户级别组合框
	CBCGPComboBox m_cboUserType;
	// 用户级别的列表式颜色按钮
	CBCGPColorButton m_CPUserType;
	// 区域显示/隐藏列表框
	CCheckListBox m_ShowZoneList;

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCbnSelchangeComboUsertype();
	afx_msg void OnBnClickedButtonClusertype();

};
