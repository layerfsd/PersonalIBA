#pragma once
#include "G2XCalendar.h"

G2XUI_BEGIN
// CG2XCalendarWnd

class G2X_CLASSES CG2XCalendarWnd : public CWnd
{
	DECLARE_DYNCREATE(CG2XCalendarWnd)

public:
	class G2X_CLASSES CGdiVirtualDateCalendarEx: public CG2XCalendar
	{
		DECLARE_GDI_MESSAGE_MAP()
	public:
		void OnLButtonDClick(UINT nFlags, CPoint point);
	};

public:
	CG2XCalendarWnd();
	virtual ~CG2XCalendarWnd();

	BOOL Create(const RECT& rect, CWnd* pParentWnd,CG2XWnd* pGdiParentWnd,
		const COleDateTime* lpDefaultDate=NULL);


	CG2XCalendar* GetCalendarCtrl();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

public:
	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClose();

	
protected:
	CG2XCalendar *m_pDate;

	COleDateTime m_dtDate;
	CG2XWnd* m_pGdiParentWnd;
	BOOL m_bHasDClick;


public:
	afx_msg void OnPaint();
};

G2X_END
