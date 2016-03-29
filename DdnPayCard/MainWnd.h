#pragma once

#include "PayCardDlg.h"
#include "MDataXReceiver.h"

// CMainWnd

class CMainWnd : public CWnd
{
	DECLARE_DYNAMIC(CMainWnd)
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()

public:
	CMainWnd();
	virtual ~CMainWnd();

	void OnTipsNotify(INT nCmd);
	
	afx_msg LRESULT OnHostMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSessionChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	BOOL CreatePcDlg(BOOL bCashPay);
	void DestroyPcDlg();

	INT OnDataArrival(INT nAppId, INT nMsg, INT nParam1, INT nParam2, 
		LPVOID lpBuf, size_t cbLen);

public:

	CString m_strParams;
	CPayCardDlg* m_lpPcDlg;
	BOOL m_bOldNetbar; // 是否为老的收银台

	NS_DataX::CDataXMReceiverT<CMainWnd>* m_lpDataXMReceiver;

};


