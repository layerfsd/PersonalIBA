#pragma once
#include "IBADialog.h"

//
//class: CDlgCreditModify 修正充值金额对话框
//
class CDlgCreditModify : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgCreditModify)

public:
	CDlgCreditModify(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCreditModify();

// 对话框数据
	enum { IDD = IDD_DLG_CREDIT_MODIFY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CString m_strNewMoney;
	CString m_strCreditMoney;
	CBCGPMaskEdit m_editNewMoney;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
