#pragma once
#include "G2XControl.h"

G2XUI_BEGIN

class G2X_CLASSES CG2XRadioBox :
	public CG2XControl
{
	DECLARE_DYNCREATE(CG2XRadioBox)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XRadioBox(void);
	virtual ~CG2XRadioBox(void);

	
public:
	BOOL GetCheck();
	
	void SetCheck(BOOL bCheck);
	
	// Group By RadioBox
	void SetGroup(CG2XRadioBox* pPreRadio); 

	// return the pointer of RadioBox's status is Checked, or return NULL
	CG2XRadioBox* GetCheckedGroupRadio();

protected:
	g_msg void OnLButtonDown(UINT nFlags, CPoint point);
	
	// override this method can repaint RadioBox
	virtual void DrawControl(Graphics* pGraphics,G2XControlState ctrlState);

protected:
	BOOL m_bCheck;
	CG2XRadioBox* m_pGroupPre;

};

G2X_END
