#pragma once

#include "SimpleBrowser.h"
#include "resource.h"

class CBrowserDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(CBrowserDlg)
	DECLARE_MESSAGE_MAP()

public:
	CBrowserDlg(CWnd* pParent = NULL);
	virtual ~CBrowserDlg();

	enum { IDD = IDD_DIALOG_BROWSER };

protected :
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö
	virtual void OnOK();
	virtual void OnCancel();
    afx_msg	void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
	void OnDocumentComplete(NMHDR* pNMHDR, LRESULT* pResult);
	void HideBrowserBorder();

public:

	CSimpleBrowser m_WebBrowser;

	void Navigate(LPCTSTR URL, LPCTSTR postData = NULL, LPCTSTR headData = NULL);
};
