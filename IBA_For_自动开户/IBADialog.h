#pragma once

class CIBADialog : public CStringLoaderDlg<CBCGPDialog>
{
	DECLARE_MESSAGE_MAP()

protected:

	CIBADialog(UINT nIDTemplate, CWnd *pParent= NULL);

	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT nIDEvent);

protected:

	static const UINT TimerIdOfAutoClose = 1;
	INT m_nDialogResult;

	CFont m_Font;
	CFont m_NormalFont;
	CFont m_FontSmall;
	CFont m_FontSmallBold;
	CFont m_FontPlus1Bold;
	CFont m_FontUnderline;
	CFont m_BigFont;
	CFont m_BigFontUnderline;
	void SetEditFont();

	CPPToolTip m_ToolTip;
	CRect m_ToolTipRect;

	void SetToolTipPos(UINT uID);
	
	void ShowToolTip(UINT uID, BOOL bErrorInfo = FALSE);
	void ShowToolTip(CString strInfo, BOOL bErrorInfo = FALSE);
	void ShowToolTip(UINT uResID, UINT nPosID, BOOL bErrorInfo = FALSE);
	void ShowToolTipFormat(UINT nFormatId, ...);
	
	void AutoClose(INT nDialogResult = IDCANCEL);

public:

	INT_PTR MsgBox(LPCTSTR lpszText, UINT nType = MB_OK | MB_ICONINFORMATION, LPCTSTR lpszCaption = NULL);
	INT_PTR MsgBox(UINT nResId, UINT nType = MB_OK | MB_ICONINFORMATION, LPCTSTR lpszCaption = NULL);
	INT_PTR MsgBoxFormat(UINT nType, UINT nFormatId, ...);
};


