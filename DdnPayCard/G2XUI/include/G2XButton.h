#pragma once
#include "g2xcontrol.h"
#include "G2XImageList.h"

G2XUI_BEGIN

class G2X_CLASSES CG2XButton : 	public CG2XControl
{
	DECLARE_DYNCREATE(CG2XButton)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XButton(void);
	virtual ~CG2XButton(void);

	BOOL Create(
		CG2XWnd* pParent, 
		LPCTSTR lpWndText, 
		LPCRECT lpRect, 
		UINT nID,
		G2XButtonStyle style=G2XButtonStylePushButton,
		G2XTextHAlign align=G2XTextHAlignCenter);
	
	void SetButtonStyle(G2XButtonStyle buttonStyle);

protected:
	virtual void DrawControl(Graphics* pGraphics,G2XControlState ctrlState);

	// ²»¸ø°´Å¥»­±³¾°
	virtual void DrawBackground(Graphics *pGraphics) { return; };

	virtual INT GetControlStateImage(G2XControlState ctrlState);

protected:
	G2XButtonStyle m_buttonStyle;
};

G2X_END
