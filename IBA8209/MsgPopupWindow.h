#pragma once

// CMsgPopupWindow

class CMsgPopupWindow : public CBCGPPopupWindow
{
	DECLARE_DYNAMIC(CMsgPopupWindow)
	DECLARE_MESSAGE_MAP()

public:

	CMsgPopupWindow();

	virtual ~CMsgPopupWindow();

	virtual BOOL Create(CWnd* pWndOwner, CBCGPPopupWndParams& params, HMENU hMenu = NULL, CPoint ptPos = CPoint (-1, -1));

protected:
	
	CMenu m_Menu;

	CString m_strText;

	virtual BOOL OnClickLinkButton(UINT uiCmdID);

	afx_msg void OnCopyText();
};


