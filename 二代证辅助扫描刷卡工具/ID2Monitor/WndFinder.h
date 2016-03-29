#pragma once

class CWndFinder
{
public:
	CWndFinder();
	~CWndFinder(void);

	BOOL Init(LPCTSTR lpszClassName, LPCTSTR lpszWindowName);
	BOOL Init(LPCTSTR lpDialogCaption);

	HWND FindDialog(LPCTSTR lpszCaption);

	BOOL GetChildCtrlText(UINT nID, CString& strOut);
	BOOL GetChildCtrlTextEx(UINT nCtrlOrder, CString& strOut);

	HWND m_hBaseWnd;
};
