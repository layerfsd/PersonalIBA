#pragma once
#include "G2XWnd.h"

G2XUI_BEGIN

class G2X_CLASSES CG2XCalendarWnd;

class G2X_CLASSES CG2XDataCtrl : public CG2XWnd
{
	DECLARE_DYNCREATE(CG2XDataCtrl)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XDataCtrl(void);
	virtual ~CG2XDataCtrl(void);

	enum emFmts
	{
		year,
		month,
		day,
		hour,
		minute,
		second
	};

	// 设置为24小时制,否为12小时制
	void Set24(BOOL bSet24);

	// 设置显示格式
	// %y : year
	// %m : month
	// %d : day
	// %H : hour
	// %M : minute
	// %S : second
	void SetFormat(LPCTSTR lpFmt);

	void SetDate(COleDateTime& dt);
	void SetDate(INT nYear,INT nMonth, INT nDay);
	COleDateTime GetDate();

	virtual void DrawSelf(Graphics* pGraphics);

	g_msg void OnCreate();

	void OnMouseHover(UINT nFlags, CPoint point);
	void OnMouseLeave();
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnBtnYMD();
protected:
	CString ParseFmtDate();

	virtual void OnCalendarDropDown(CG2XCalendarWnd* pCalendar);
	
	g_msg LONG OnClosingCalendarWnd(WPARAM, LPARAM);
	g_msg void OnSize();
protected:
	CString m_strFmts;
	CString m_strFmtsHelper;

	COleDateTime m_dtDate;

	BOOL m_b24;

	INT m_nFmts[6];

	BOOL m_bMouseHover;

	CRect m_rcBtnYMD;

	Gdiplus::Color m_clrBtnYMD;

	void ReLayout();
	void DrawYMDBtn(Graphics* pGraphics ,LPRECT lpRect, BOOL bHover=FALSE);
};

G2X_END
