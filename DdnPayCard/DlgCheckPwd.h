#pragma once
#include "resource.h"


// CDlgCheckPwd 对话框

class CDlgCheckPwd : public CG2XDialog
{
	DECLARE_DYNAMIC(CDlgCheckPwd)

public:
	CDlgCheckPwd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCheckPwd();

// 对话框数据
	enum { IDD = IDD_DLG_CEHCKPWD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
