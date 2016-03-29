#pragma once

#include "resource.h"
#include <objsafe.h>

// CPayCardHtmlDlg 对话框

class CPayVerifyDlg : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CPayVerifyDlg)
	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
	DECLARE_DISPATCH_MAP()

public:
	CPayVerifyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPayVerifyDlg();

// 对话框数据
	enum { IDD = IDD_PAY_DIALOG};

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();

	virtual BOOL IsExternalDispatchSafe();

	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);

public:

	CString m_strURL;

	CString m_strPostData;

	afx_msg void OnClose();

	void SplitLine(CString& strLine, CStringArray& strArray, TCHAR ch /*= '|'*/);

	BOOL PrintText(INT nFontSize, LPCTSTR lpszText);

	BOOL PcpNotify(INT nCmd, LPCTSTR lpszText);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
