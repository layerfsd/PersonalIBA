#pragma once

#include "IBADialog.h"

// CRealNameDeRegDlg 对话框

class CRealNameDeRegDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CRealNameDeRegDlg)
	DECLARE_MESSAGE_MAP()

public:
	CRealNameDeRegDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRealNameDeRegDlg();

// 对话框数据
	enum { IDD = IDD_RN_CARDDEREGISTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
public:
	afx_msg void OnBnClickedOk();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
private:
	// 临时卡号码
	CString m_strNetId;
	CEdit m_edtNetId;
};
