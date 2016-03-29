#pragma once
#include "G2XFoundationWindow.h"

G2XUI_BEGIN

class G2X_CLASSES CG2XStaticEx;

class G2X_CLASSES CG2XToolTipCtrl : public G2XUIFrame::CG2XFoundationWindow
{
	DECLARE_DYNCREATE(CG2XToolTipCtrl)
	DECLARE_MESSAGE_MAP()
public:
	CG2XToolTipCtrl();
	virtual ~CG2XToolTipCtrl();

	BOOL Create(CWnd* pParent);

	void SetTipText(LPCTSTR lptStr);
	void SetPosition(CPoint point);

	void Active(BOOL bActive);

	void SetBkColor(const Color& clr);

protected:
	void OnTimer(UINT_PTR nIDEvent);
	virtual void OnInitialUpdate();

	// ms
	void SetAutoClose(DWORD uTime=5000); 

	CString m_strTipText;
	CColor m_clrTipBk;
	BOOL m_bActived;
	UINT m_nElapseTime; // 自动消失的时间长度(ms)

	CPoint m_ptTipPos;

public:
	CG2XStaticEx* m_pEx;
};

G2X_END