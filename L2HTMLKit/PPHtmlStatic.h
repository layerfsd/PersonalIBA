
#pragma once

#include "PPHtmlDrawer.h"


#define PPHTMLSTATIC_ALIGN_DEFAULT		0x00
#define PPHTMLSTATIC_ALIGN_LEFT    	    0x01
#define PPHTMLSTATIC_ALIGN_RIGHT  		0x02
#define PPHTMLSTATIC_ALIGN_CENTER 		0x03
#define PPHTMLSTATIC_ALIGN_TOP 		    0x04
#define PPHTMLSTATIC_ALIGN_BOTTOM  		0x08
#define PPHTMLSTATIC_ALIGN_VCENTER 		0x0C

/////////////////////////////////////////////////////////////////////////////
// CPPHtmlStatic window

class L2_EXT_CLASS CPPHtmlStatic : public CStatic
{
// Construction
public:
	CPPHtmlDrawer * GetHtmlDrawer() {return &m_drawer;};

	void SetCallbackHyperlink(UINT nMessage, LPARAM lParam = 0);
	void SetHyperlink(LPCTSTR lpszText, LPCTSTR lpszUrl, BOOL bParentNotify = FALSE);

	void SetIcon(DWORD dwIdIcon, LPSIZE lpSizeItem = NULL);
	void SetIcon(HICON hIcon);
	void SetBitmap(DWORD dwIdBitmap, COLORREF clrMask = RGB(255, 0, 255));
	void SetBitmap(HBITMAP hBitmap, COLORREF clrMask = RGB(255, 0, 255));
	void SetAnimation(DWORD dwIdBitmap, LPSIZE lpSizeItem = NULL, COLORREF clrMask = RGB(255, 0, 255));
	void SetAnimation(HBITMAP hBitmap, LPSIZE lpSizeItem = NULL, COLORREF clrMask = RGB(255, 0, 255));
	void SetImageList(DWORD dwIdBitmap, DWORD dwIndex, LPSIZE lpSizeItem = NULL, COLORREF clrMask = RGB(255, 0, 255));
	void SetImageList(HBITMAP hBitmap, DWORD dwIndex, LPSIZE lpSizeItem = NULL, COLORREF clrMask = RGB(255, 0, 255));

	//Regions
	void SetRgn(HRGN hRgn = NULL); //Sets a region
	void SetRectRgn(); //Creates and sets a rectangle region
	void SetRoundRectRgn(int nWidth, int nHeight); //Creates and sets a rounded rectangle region
	void SetBorder(COLORREF color, int nWidth = 1, int nHeight = 1); //Sets a static's border
	void SetBorder(HBRUSH hbr, int nWidth = 1, int nHeight = 1); //Sets a static's border

	void SetMargins(int nXMargin = 0, int nYMargin = 0); //Sets a text's margins 

	void  SetAlign(DWORD dwAlign = PPHTMLSTATIC_ALIGN_DEFAULT);
	DWORD GetAlign();

	void SetImageZoom(int nWidthPercent = 100, int nHeightPercent = 100);
	void SetImageZoom(int nPercent = 100);
	
	void SetFrameRate(DWORD dwFrameRate = 500);
	void ClearFrameRate();

	void SetHyperlinkStyle(COLORREF clrOrg, BOOL bOrgUnderline, COLORREF clrHover, BOOL bHoverUnderline);

	void SetTextColor(COLORREF clrText);
	void SetBkColor(COLORREF clrBk);
	void SetBkColor(COLORREF clrBegin, COLORREF clrEnd);
	void SetBkColor(COLORREF clrBegin, COLORREF clrMid, COLORREF clrEnd);
	void SetBkMode(int nBkMode = TRANSPARENT);
	void SetBkEffect(DWORD dwEffect, BYTE nGranularity = 5);

//	void SetFitOutput(BOOL bFitOutput = TRUE){m_bIsFitOutput = bFitOutput;};
	void SetWrapText(BOOL bWrapText);

	static short GetVersionI()		{return 0x12;}
	static LPCTSTR GetVersionC()	{return (LPCTSTR)_T("1.2");}

	CPPHtmlStatic();


public:
	virtual BOOL DestroyWindow();
protected:
	virtual void PreSubclassWindow();


// Implementation
public:
	virtual ~CPPHtmlStatic();

	// Generated message map functions
protected:
	void SetDefaultFont(HFONT hFont);
	void SetText(LPCTSTR lpszText);
	void UpdateSurface();
	void RepaintWindow(HDC hDC = NULL);
	HRGN CreateRegion();

	void FreeResources();

	HBITMAP m_hBitmapBk;

	HRGN m_hRgn;
	BOOL m_bRectRgn;
	int  m_nRoundX;
	int  m_nRoundY;

	//Border of the tooltip
	HBRUSH m_hbrBorder;
	SIZE m_szBorder;

	SIZE m_szTextMargin;

	CRect m_rcArea;
	CSize m_szOutput;
	CSize m_szImage;

	CPPHtmlDrawer m_drawer;
	DWORD	m_dwAlign;
	CString m_sText;
	CString m_sOrgText; //Original
	int m_nWidthPercent;
	int m_nHeightPercent;
	
	BOOL m_bIsEnabled;
//	BOOL m_bIsFitOutput;

	//Timer
	BOOL m_bIsRunning;
	DWORD m_dwFrameRate;

	BOOL m_bIsImage;
	CString m_sTemplate;

	HFONT m_hFont;

	COLORREF m_clrText;
	COLORREF m_clrBkBegin;
	COLORREF m_clrBkMid;
	COLORREF m_clrBkEnd;
	int m_nBkMode;
	DWORD m_dwBkEffect;
	BYTE m_nGranularity;

	CScrollBar m_pHScroll;
	CScrollBar m_pVScroll;

protected:

	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);

 	afx_msg LRESULT OnGetText(WPARAM, LPARAM);
	afx_msg LRESULT OnSetText(WPARAM, LPARAM);
	afx_msg LRESULT OnSetFont(WPARAM, LPARAM);
	afx_msg LRESULT OnGetFont(WPARAM, LPARAM);
    afx_msg LRESULT OnRepaintWindow(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

