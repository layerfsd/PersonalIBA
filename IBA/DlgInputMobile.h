#pragma once
#include "IBADialog.h"


// class:CDlgInputMobile 输入手机号码的对话框

class CDlgInputMobile : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgInputMobile)

public:
	CDlgInputMobile(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgInputMobile();

// 对话框数据
	enum { IDD = IDD_DLG_INPUT_MOBILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

public:
	CBCGPEdit m_edit;
	CString m_strPhoneNumber;
	virtual BOOL OnInitDialog();
};
