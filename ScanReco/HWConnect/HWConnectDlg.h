// HWConnectDlg.h : header file
//
//{{AFX_INCLUDES()
#include "hwidcard.h"
//}}AFX_INCLUDES

#if !defined(AFX_HWCONNECTDLG_H__D483A56B_1379_4CD1_BBFC_40DFF0A983D0__INCLUDED_)
#define AFX_HWCONNECTDLG_H__D483A56B_1379_4CD1_BBFC_40DFF0A983D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CHWConnectDlg dialog

class CHWConnectDlg : public CDialog
{
// Construction
public:
	CHWConnectDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHWConnectDlg)
	enum { IDD = IDD_HWCONNECT_DIALOG };
	CComboBox	m_objIDTypeSelect;
	CProgressCtrl	m_objCScanProgress;
	CHWIDCard	m_objCHWConnect;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHWConnectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CHWConnectDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBUTTONCalibrate();
	afx_msg void OnBUTTONInit();
	afx_msg void OnBUTTONScan();
	afx_msg void OnOnScanFinishHwidcardctrl1(short btnType);
	afx_msg void OnOnScanBtnDownHwidcardctrl1(short btnType);
	afx_msg void OnOnProgressHwidcardctrl1(short Min, short Max, short Position);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnKillfocusComboIdtype();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_strScanImagePath;
	HANDLE m_hCompleteEvent;
	HANDLE m_hScanPicCompleteEvent;
	USHORT m_usIDType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HWCONNECTDLG_H__D483A56B_1379_4CD1_BBFC_40DFF0A983D0__INCLUDED_)
