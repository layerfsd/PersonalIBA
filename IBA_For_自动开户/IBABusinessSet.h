#pragma once


//
//class:CSetPageOfRegister 收银台设置->开户设置
//
class CSetPageOfRegister : public CIBAPage
{
	DECLARE_DYNAMIC(CSetPageOfRegister)
	DECLARE_MESSAGE_MAP()

public:
	CSetPageOfRegister();
	virtual ~CSetPageOfRegister();

// 对话框数据
	enum { IDD = IDD_SET_REGISTER };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCbnSelchangeComboPasswordtype();
	afx_msg void OnCbnSelchangeComboPasswordtype2();
	
	CBCGPComboBox m_cboUserClass;
	CBCGPComboBox m_cboIDType;

	CBCGPComboBox m_cboPwdType;
	CString m_strDefPwd;

	CBCGPComboBox m_cboTempPwdType;
	CString m_strTempDefPwd;
	
	// 开户年龄限制
	CBCGPComboBox m_cboRegisterAge;
	// 开户金额
	CString	  m_strRegisterMoney;
	// 用户自带卡
	BOOL m_bHasCard;

	BOOL m_bCheckIDCardNumber;

};
