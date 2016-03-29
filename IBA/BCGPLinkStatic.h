/**************************************************************************
file:  BCGPLinkStatic.h
class: CBCGPLinkStatic :public CBCGPStatic
for:   用于显示超链接样式的static,如果不设置颜色,static的背景颜色和文字颜色都采用
	BCGP库的文字颜色.
date:  2011-03-17
author:guoxuxing
**************************************************************************/

#pragma once


// CBCGPLinkStatic

class CBCGPLinkStatic : public CBCGPStatic
{
	DECLARE_DYNAMIC(CBCGPLinkStatic)

public:
	CBCGPLinkStatic();
	virtual ~CBCGPLinkStatic();

	enum emMouseState
	{
		emLinkNormal = 0,     // mouse beyond the link
		emLinkHover = 1,   // mouse above the link
		emLinkVisited = 2       // mouse click the link
	};

protected:
	DECLARE_MESSAGE_MAP()

public:
	void SetLinkAddress(CString strLink,BOOL bAutoOpen=FALSE);

public:
	void SetTransparent(BOOL bTransparent);  // 设置背景是否透明
	void SetVerticalCenter(BOOL bVCenter);   // 设置文本是否垂直居中
	void SetBkColor(COLORREF clr);
	void SetLinkColor(COLORREF clr);         // 设置链接的颜色
	void SetHoverColor(COLORREF clr);        // 设置鼠标越过是的颜色
	void SetVisitedColor(COLORREF clr);      // 设置链接被访问后的颜色

	void  SetFont(CFont *pFont);
	CFont* GetFont();
	void SetHoverFont(CFont *pFont);
	CFont* GetHoverFont();

protected:

	virtual void DrawLinkName(CDC* pDC,RECT rect,CString strName,UINT nMouseState);
	virtual void DrawLinkBk(CDC* pDC,RECT rect,BOOL bTransparent,UINT nMouseState);

public:
	afx_msg void OnPaint();
	afx_msg LRESULT OnBCGSetControlAero (WPARAM, LPARAM);

protected:
	CFont *m_pFont;
	CFont *m_pHoverFont;
	CString m_strLinkAddress;
	BOOL m_bAutoOpen;
	COLORREF m_clrBk;
	COLORREF m_clrLink;
	COLORREF m_clrHover;
	COLORREF m_clrVisited;
	BOOL m_bVCenter;
	BOOL m_bTransparent;
	BOOL m_bHaveVisited;

	UINT m_nMouseState;
public:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


