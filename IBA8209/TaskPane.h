
#pragma once

#include "computerinfo.h"
#include "ActiveMember.h"
#include "WebBrowser.h"
#include "DlgUserPhoto.h"

class CIBATaskPane : public CBCGPTasksPane
{
	DECLARE_MESSAGE_MAP()
// Construction
public:
	CIBATaskPane();
	virtual ~CIBATaskPane();

// Attributes

public:
	CDlgUserPhoto m_dlgUserPhoto;

protected:

	CFont		m_Font;
	CBCGPEdit	m_wndEdit;
	CBCGPEdit	m_wndEditNetbarNotice;
	CWebBrowser m_WebBrowser;


	CString m_strQucikNoteFile;

	CActiveMember m_ActiveMember;

	INT m_nUserInfoGroup;
	INT m_nUserInfoTask;
	INT m_nUserInfoButton;
	INT m_nUserInfoList;
	BOOL m_bShowUseList;
	
	const static UINT EDITIDOF_NETBAR_NOTICE = 1000;
protected :

	virtual void OnClickTask (int nGroupNumber, int nTaskNumber, 
		UINT uiCommandID, DWORD dwUserData);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

	
	BOOL CreateEditControl();
	BOOL CreateBroser();

	void ShowCashierInfo();

	void ShowUserInfo(CString strContent);

	CString QueryConsume();

public :

	void ShowUserInfo(CComputerInfo & ComputerInfo);
	void ShowUserInfo(CActiveMember & ActiveMember);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEnChangeEditNetbarNotice();

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnSetfocusEditNetbarNotice();
	afx_msg void OnEnKillfocusEditNetbarNotice();
};

