#pragma once

class CBCGPLabelButton : public CBCGPURLLinkButton
{
	DECLARE_MESSAGE_MAP()

public:

	CBCGPLabelButton(void);
	~CBCGPLabelButton(void);

	void OpenURL();

protected:

	CWnd* m_pLastActiveWnd;

	virtual void OnDrawFocusRect(CDC* pDC, const CRect& rectClient);

	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
