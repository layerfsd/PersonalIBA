#pragma once


class CIBAFormView : public CStringLoaderView<CBCGPFormView>
{
	DECLARE_MESSAGE_MAP()

protected :

	CIBAFormView(UINT nIDTemplate);           
	virtual ~CIBAFormView();

	CFont m_Font;
	CFont m_BigFont;
	void SetEditFont();

	CPPToolTip m_ToolTip;
	CRect m_ToolTipRect;

	void SetToolTipPos(UINT uID);

	void ShowToolTip(UINT uID, BOOL bErrorInfo = FALSE);
	void ShowToolTip(CString strInfo, BOOL bErrorInfo = FALSE);
	void ShowToolTip(UINT uResID, UINT nPosID, BOOL bErrorInfo = FALSE);
	void ShowToolTipFormat(UINT nFormatId, ...);

	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnClose();

public:

	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	INT_PTR MsgBox(LPCTSTR lpszText, UINT nType = MB_OK | MB_ICONINFORMATION, LPCTSTR lpszCaption = NULL);
	INT_PTR MsgBox(UINT nResId, UINT nType = MB_OK | MB_ICONINFORMATION, LPCTSTR lpszCaption = NULL);
	INT_PTR MsgBoxFormat(UINT nType, UINT nFormatId, ...);
	
};


