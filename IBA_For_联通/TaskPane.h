
#pragma once

#include "computerinfo.h"
#include "ActiveMember.h"
#include "WebBrowser.h"

class CIBATaskPane : public CBCGPTasksPane
{
	DECLARE_MESSAGE_MAP()
// Construction
public:
	CIBATaskPane();
	virtual ~CIBATaskPane();

// Attributes

protected:

	CFont		m_Font;
	CBCGPEdit	m_wndEdit;
	CWebBrowser m_WebBrowser;

	CString m_strQucikNoteFile;

	CActiveMember m_ActiveMember;

	INT m_nUserInfoGroup;
	INT m_nUserInfoTask;
	INT m_nUserInfoButton;
	INT m_nUserInfoList;
	BOOL m_bShowUseList;

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
};

