#pragma once
#include "IBA.h"
#include "IBADialog.h"


// CDlgNotice 对话框

class CDlgNotice : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgNotice)

public:
	CDlgNotice(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgNotice();

	void SetParams(LPCTSTR lpTitle,LPCTSTR lpInfo,LPCTSTR lpLink,BOOL bShouldSign, BOOL bUpdate=FALSE);

// 对话框数据
	enum { IDD = IDD_DIALOG_NOTICE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	CString m_strTitle;
	CString m_strInfo;
	CString m_strLink;
	BOOL m_bShouldSign;

	CBCGPButton m_btn;

	BOOL m_bMouseAt;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnStnClickedStaticInfo();
	afx_msg void OnBnClickedOk();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
