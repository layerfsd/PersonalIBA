#pragma once


// CXPEdit

class CXPEdit : public CEdit
{
	DECLARE_DYNAMIC(CXPEdit)

public:
	CXPEdit();
	virtual ~CXPEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNcPaint();
protected:
	BOOL m_bIsFocused;
	BOOL m_bIsMouseOver;
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void DrawEdge(void);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	BOOL bldo;

};


