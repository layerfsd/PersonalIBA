#pragma once

#include "IBADialog.h"
// CDlgInputParam 对话框

class CDlgInputParam : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgInputParam)

public:
	CDlgInputParam(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgInputParam();

// 对话框数据
	enum { IDD = IDD_DLG_INPUT_PARAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	void InitParam(CString strCaption,CString strTitle,CString strParam=_T(""));
public:
	CString m_strParam; // 参数的值
	CString m_strTitle; // 标题
	CString m_strCaption; // 对话框的标题
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();


};
