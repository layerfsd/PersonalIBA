// TestScanRecoDlg.h : header file
//

#if !defined(AFX_TESTSCANRECODLG_H__6DA4FFA3_4129_47AD_BF20_B526556C2362__INCLUDED_)
#define AFX_TESTSCANRECODLG_H__6DA4FFA3_4129_47AD_BF20_B526556C2362__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTestScanRecoDlg dialog


class CTestScanRecoDlg : public CDialog
{
// Construction
public:
	CTestScanRecoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTestScanRecoDlg)
	enum { IDD = IDD_TESTSCANRECO_DIALOG };
	CComboBox	m_objTypeID;
	int		m_iType;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestScanRecoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestScanRecoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtonOpen();
	afx_msg void OnButtonScan();
	afx_msg void OnButtonRecognise();
	afx_msg void OnButtonClose();
	afx_msg void OnButtonScanRecoAll();
	afx_msg void OnButtonReader();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void Fun(int idType);
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTSCANRECODLG_H__6DA4FFA3_4129_47AD_BF20_B526556C2362__INCLUDED_)
