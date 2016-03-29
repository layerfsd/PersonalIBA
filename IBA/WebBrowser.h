#pragma once

#include <afxhtml.h>

class CWebBrowser : public CHtmlView
{
	DECLARE_DYNCREATE(CWebBrowser)
	DECLARE_MESSAGE_MAP()

public:

	CWebBrowser();          
	virtual ~CWebBrowser();

	BOOL IsContextMenuEnabled() const { return m_bContextMenuEnabled; }
	void EnableContextMenu(BOOL newVal = TRUE) { m_bContextMenuEnabled = newVal; }

protected:

	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();

protected:
	virtual void OnTitleChange(LPCTSTR lpszText);
	virtual void OnDocumentComplete(LPCTSTR lpszURL);

protected:

	BOOL m_bContextMenuEnabled;

};


