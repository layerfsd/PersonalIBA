#pragma once
#include "PayVerifyDlg.h"


// CContainDlg 对话框

class CContainDlg : public CG2XDialog
{
	DECLARE_DYNAMIC(CContainDlg)

public:
	CContainDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CContainDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_CONTAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CPayVerifyDlg m_dlg;
	virtual BOOL OnInitDialog();
};
