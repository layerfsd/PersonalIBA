#pragma once
#include "G2XControl.h"

G2XUI_BEGIN

class G2X_CLASSES CG2XCheckBox : public CG2XControl
{
	DECLARE_DYNCREATE(CG2XCheckBox)
	DECLARE_GDI_MESSAGE_MAP()

public:
	CG2XCheckBox(void);
	virtual ~CG2XCheckBox(void);

	void SetCheck(BOOL bCheck);
	BOOL GetCheck();

protected:
	virtual void DrawControl(Graphics* pGraphics,G2XControlState ctrlState);

	g_msg void OnLButtonUp(UINT nFlags, CPoint point);
protected:
	BOOL m_bCheck;
};

G2X_END
