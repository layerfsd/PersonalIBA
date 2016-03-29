#pragma once


// CJXCPage 对话框

class CSetPageOfJXC : public CIBAPage
{
	DECLARE_DYNAMIC(CSetPageOfJXC)
	DECLARE_MESSAGE_MAP()

public:
	CSetPageOfJXC();
	virtual ~CSetPageOfJXC();

// 对话框数据
	enum { IDD = IDD_SET_JXC };

private:

	CBCGPComboBox m_cboStore;
	BOOL LoadStoreInfo();

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();
	virtual BOOL OnInitDialog();

public:
	BOOL m_bPayCash; // 是否现金支付
};
