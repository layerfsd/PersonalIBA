#pragma once
#include "BCGPStaticTips.h"

class CBCGPStaticTips : public CBCGPStatic
{
	DECLARE_DYNAMIC(CBCGPStaticTips)
public:
	CBCGPStaticTips(void);
	~CBCGPStaticTips(void);
	void SetWindowText(LPCTSTR lpszString);
protected:
	DECLARE_MESSAGE_MAP()
private:
	HWND m_hwndTips;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	LRESULT SetToolTip(LPCTSTR pszText);
	LRESULT DeleteToolTip();
	HWND CreateToolTip();
protected:
	virtual void PreSubclassWindow();
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
