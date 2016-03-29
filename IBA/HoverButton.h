
#pragma once

// HoverButton.h : header file

class CHoverButton : public CButton
{
	DECLARE_DYNAMIC(CHoverButton);
	DECLARE_MESSAGE_MAP()
	// Construction
public:
	CHoverButton();
	virtual ~CHoverButton();

	void SetToolTipText(LPCTSTR lpszText, BOOL bActivate = TRUE);
	void SetToolTipText(int nId, BOOL bActivate = TRUE);

	BOOL LoadBitmap(HINSTANCE hRes, UINT bitmapid, UINT bmpCount = 3);
	BOOL LoadPng(HINSTANCE hRes, UINT pngid, UINT bmpCount /*= 3*/);

protected:
	void InitButtonData(UINT bmpCount);
	void InitToolTip();
	void ActivateTooltip(BOOL bActivate = TRUE);

	BOOL m_bHover;						// indicates if mouse is over the button
	CSize m_ButtonSize;					// width and height of the button
	CBitmap m_Bitmap;
	BOOL m_bTracking;
	UINT m_uHoverIndex;
	UINT m_uSelectedIndex;
	CToolTipCtrl m_ToolTip;

protected:

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam) ;
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};