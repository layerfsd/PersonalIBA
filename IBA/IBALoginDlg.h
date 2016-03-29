#pragma once

#include "IBADialog.h"
#include "BCGPLabalButton.h"
#include "BCGPLinkStatic.h"
#include "..\L2ToolKit\GdipButton.h"

class CIBALoginDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CIBALoginDlg)
	DECLARE_MESSAGE_MAP()

public:
	CIBALoginDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIBALoginDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_LOGIN };
	
	void SetLoginType(UINT newVal);
	void myShowToolTip();
private:

	UINT m_nLoginType;

	CBCGPComboBox m_cboOperators;
	CString m_strPassword;

	CImage m_BKImage;

	CGdipButton  m_btnLogin;
	CGdipButton  m_btnCancel;
	CGdipButton  m_btnSet;
	CGdipButton  m_btnEnmergency;

	CBCGPLinkStatic m_btnAllCheckOut;

private:

	void InitOperatorsCombo();
	BOOL CheckInput(CString &strOperater);
	void LoadImage();

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonSet();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonEnmergency();
	LRESULT OnShowToolTip(WPARAM wParam, LPARAM lParam);
	
};
