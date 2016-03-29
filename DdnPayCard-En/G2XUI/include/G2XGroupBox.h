#pragma once
#include "g2xstatic.h"

G2XUI_BEGIN

class G2X_CLASSES CG2XGroupBox : public CG2XStatic
{
	DECLARE_DYNCREATE(CG2XGroupBox)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XGroupBox(void);
	~CG2XGroupBox(void);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DrawControl(Graphics* pGraphics,G2XControlState ctrlState);
};

G2X_END