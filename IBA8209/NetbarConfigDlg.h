#pragma once

#include "IBADialog.h"
// CNetbarConfigDlg 对话框

class CNetbarConfigDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CNetbarConfigDlg)

public:
	CNetbarConfigDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNetbarConfigDlg();

// 对话框数据
	enum { IDD = IDD_NETBARCONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual BOOL OnInitDialog();
};
