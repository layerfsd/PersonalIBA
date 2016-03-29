#pragma once

G2XUI_BEGIN

class G2X_CLASSES CG2XLinkButton :	public CG2XButton
{
	DECLARE_DYNCREATE(CG2XLinkButton)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XLinkButton(void);
	virtual ~CG2XLinkButton(void);

	BOOL Create(
		CG2XWnd* pParent, 
		LPCTSTR lpWndText, 
		LPCRECT lpRect, 
		UINT nID, 
		G2XTextHAlign hAlign=G2XTextHAlignLeft);

protected:
	virtual void DrawControl(Graphics* pGraphics,G2XControlState ctrlState);

	g_msg void OnMouseMove(UINT nFlags, CPoint point);
	g_msg void OnMouseLeave();

protected:
	CColor m_clrHover;
	HCURSOR m_hCursor;
};

G2X_END
