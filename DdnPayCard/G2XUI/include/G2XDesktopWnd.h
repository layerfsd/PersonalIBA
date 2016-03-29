#pragma once

G2XUI_BEGIN

class G2X_CLASSES CG2XDesktopWnd : public CWnd
{
	DECLARE_DYNCREATE(CG2XDesktopWnd)

public:
	CG2XDesktopWnd();
	virtual ~CG2XDesktopWnd();

	BOOL Create(const RECT& rect, CWnd* pParentWnd,CG2XWnd* pGdiParentWnd,CG2XPanel* pPanel);

protected:
	DECLARE_MESSAGE_MAP()

public:
	

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	CG2XWnd* m_pGdiParentWnd;
	CG2XPanel* m_pPanel;


public:
	CG2XPanel* GetPanel() { return m_pPanel; }
	CG2XWnd* GetGdiParentWnd() { return m_pGdiParentWnd; }
};

G2X_END