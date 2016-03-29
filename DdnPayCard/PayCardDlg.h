#pragma once

#include "resource.h"
#include "WebBrowser.h"

// CPayCardDlg 对话框

class CPayCardDlg : public CG2XDialog, public CWebBrowserEventHandler
{
	DECLARE_DYNAMIC(CPayCardDlg)
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()

public:
	CPayCardDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPayCardDlg();

// 对话框数据
	enum { IDD = IDD_PAYCARD_DIALOG };

	HICON m_hIcon;

	BOOL AnalyzeParamString(LPCTSTR lpszParams);

	void OpenHomePage(BOOL bCashWay,BOOL bOpened);

	//CString EncodeURL(LPCTSTR lpszUrl);

private:
	CString m_strCashID; // 收银台ID

	CString m_strCasherPwd; // 收银员密码

	CString m_strNetbarId;

	CString m_strDomainId;

	CString m_strCashier;

	CString m_strDutyId;

	CString m_strDdnServer;


private:
	CString m_strURLMain;
	CString m_strURLBakup;

	CWebBrowser* m_lpWebBrowser;

	BOOL CreateWebBrower();

	CString GetHomePageURL();

	CString GetLocalIP();

public:
	BOOL m_bOldNetbar; // 是否为老的收银台

protected:

	void EnableRemoteInput(BOOL bEnable);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnCancel();
	virtual void OnOK();
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	virtual void WebBrowser_BeforeNavigate2(CWebBrowser& Sender, LPCTSTR lpszURL, DWORD nFlags,
		LPCTSTR lpszTargetFrameName, CByteArray& baPostedData,
		LPCTSTR lpszHeaders, BOOL* pbCancel);

	virtual HRESULT WebBrowser_GetExternal(CWebBrowser& Sender, LPDISPATCH *lppDispatch);

	BOOL PrintText(INT nFontSize, LPCTSTR lpszText);
	void SplitLine(CString& strLine, CStringArray& strArray, TCHAR ch /*= '|'*/);
	
public:
	afx_msg void OnPaint();
};
